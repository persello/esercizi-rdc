/**
 * @file client.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief "Asta Telematica" client main.
 * @version 0.1
 * @date 2021-06-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "constants.h"
#include "utilities.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <liblog/log.h>
#include <udpsocketlib.h>

/**
 * @brief Types of possible replies.
 * 
 */
typedef enum
{
    /**
     * @brief The server's reply is not valid.
     * 
     */
    INVALID_REPLY,

    /**
     * @brief The server received an invalid command.
     * 
     */
    INVALID_OFFER,

    /**
     * @brief The server accepted this offer as the best offer.
     * 
     */
    ACCEPTED,

    /**
     * @brief This offer is not the best offer.
     * 
     */
    NOT_ACCEPTED,
} reply_result_t;

/**
 * @brief Receives and parses a reply. Blocking. Ignores other servers' UDP packets.
 * 
 * @param socket_handle The socket handle.
 * @param expected_address The expected IP address.
 * @return reply_result_t Result value from the struct.
 */
static reply_result_t receive_and_parse_reply(int socket_handle, char *expected_address)
{
    char buffer[BUFSIZ + 1];
    char server_addr[128];
    int port;

    bool correct_server = false;
    reply_result_t result = INVALID_REPLY;

    do
    {
        udp_receive_and_get_sender_info(socket_handle, buffer, server_addr, &port);

        if (strcmp(server_addr, expected_address) == 0)
        {
            correct_server = true;
            if (strcmp(buffer, OFFER_ACCEPTED_MSG) == 0)
            {
                result = ACCEPTED;
            }
            else if (strcmp(buffer, OFFER_NOT_ACCEPTED_MSG) == 0)
            {
                result = NOT_ACCEPTED;
            }
            else if (strcmp(buffer, OFFER_INVALID_MSG) == 0)
            {
                result = INVALID_OFFER;
            }
            else
            {
                result = INVALID_REPLY;
            }
        }
    } while (!correct_server);

    return result;
}

/**
 * @brief Sends an offer to a server.
 * 
 * @param handle The client handle.
 * @param address The server address.
 * @param port The server's port.
 * @param offer Offer value.
 * @param name Offerer name.
 * @return true If the offer is correctly sent.
 * @return false Otherwise.
 */
static bool make_offer(int handle, char *address, uint16_t port, double offer, char *name)
{
    char *command;

    int result;
    asprintf(&command, "OFFERTA NOME(%s) VALORE(%.2lf)", name, offer);

    result = udp_send(handle, command, address, port);
    free(command);
    return result;
}

/**
 * @brief Usage: asta_telematica_client -o <offerta> -n <nome> -s <indirizzo server> [-p <porta>]
 */
int main(int argc, char *argv[])
{
    // Socket variables
    int socket_handle;
    uint16_t port = DEFAULT_PORT;
    char server[128];
    strcpy(server, "");
    bool answer_successful = false;

    // Offer
    double offer = MINIMUM_STARTING_PRICE;
    char name[MAX_NAME_LENGTH];
    strcpy(name, "");

    // Input parameters
    int opt;                        // Current option.
    bool parsing_fail_flag = false; // Flag that signals that a parsing error occurred.

    // Parameter parsing
    while ((opt = getopt(argc, argv, ":o:n:s:p:")) != -1)
    {
        switch (opt)
        {

            // Offer
        case 'o':
            if (sscanf(optarg, "%lf", &offer) != 1)
            {
                LOG_ERROR("Formato offerta non valido.");
                parsing_fail_flag = true;
            }
            break;

            // Name
        case 'n':
            if (strlen(optarg) > MAX_NAME_LENGTH - 1)
            {
                LOG_WARNING("Nome troppo lungo, sarà troncato.");
            }

            strncpy(name, optarg, MAX_NAME_LENGTH);
            break;

            // Server address
        case 's':
            if (strlen(optarg) > 128 - 1)
            {
                LOG_ERROR("Indirizzo server troppo lungo.");
                parsing_fail_flag = true;
            }
            else
            {
                strncpy(server, optarg, 128);
            }
            break;

            // Port
        case 'p':
            if (sscanf(optarg, "%hu", &port) != 1)
            {
                LOG_ERROR("Formato porta non valido.");
                parsing_fail_flag = true;
            }
            break;

        default:
            break;
        }
    }

    // Abort execution if some data is missing
    if (strlen(name) < 1)
    {
        LOG_ERROR("Il nome dell'offerente è vuoto.");
        parsing_fail_flag = true;
    }

    if (strlen(server) < 1)
    {
        LOG_ERROR("L'indirizzo del server è vuoto.");
        parsing_fail_flag = true;
    }

    if (offer <= MINIMUM_STARTING_PRICE)
    {
        LOG_ERROR("Offerta troppo bassa. Minimo: %.2f.", MINIMUM_STARTING_PRICE);
        parsing_fail_flag = true;
    }

    // Abort execution if argument parsing failed
    if (parsing_fail_flag)
    {
        LOG_INFO("Uso: %s -o <offerta> -n <nome> -s <indirizzo server> [-p <porta>]", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create client
    socket_handle = create_udp_client();
    if (socket_handle > -1)
    {
        LOG_INFO("Creato un client UDP con socket identifier: %d.", socket_handle);
    }
    else
    {
        LOG_ERROR("Errore durante la creazione del client.");
        exit(EXIT_FAILURE);
    }

    // Offer
    if (make_offer(socket_handle, server, port, offer, name))
    {
        LOG_INFO("Offerta inoltrata al server %s:%d.", server, port);
    }
    else
    {
        LOG_ERROR("Invio offerta non riuscito.");
        die(socket_handle, EXIT_FAILURE);
    }

    LOG_INFO("In attesa di risposta dal server...");

    // Answer
    switch (receive_and_parse_reply(socket_handle, server))
    {
    case ACCEPTED:
        LOG_INFO("La tua offerta è attualmente la migliore.");
        answer_successful = true;
        break;
    case NOT_ACCEPTED:
        LOG_INFO("La tua offerta non è sufficientemente alta.");
        answer_successful = true;
        break;
    case INVALID_OFFER:
        LOG_WARNING("La tua offerta non è valida.");
        break;
    case INVALID_REPLY:
        LOG_WARNING("Il server ha inviato una risposta non valida.");
        break;
    }

    die(socket_handle, answer_successful ? EXIT_SUCCESS : EXIT_FAILURE);
}
