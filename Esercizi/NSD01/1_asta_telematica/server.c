#include "server_utilities.h"
#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <udpsocketlib.h>
#include <liblog/log.h>

#define DEFAULT_PORT 25000
#define STARTING_PRICE 0.0

#define OFFER_ACCEPTED_MSG "ACCETTATA"
#define OFFER_INVALID_MSG "INVALIDA"
#define OFFER_NOT_ACCEPTED_MSG "NON_ACCETTATA"

void server_loop(int socket_handle)
{
    char buffer[BUFSIZ + 1];

    // Offers
    double current_offer = STARTING_PRICE;
    double max_offer = STARTING_PRICE;

    // Names
    char current_offer_name[32];
    char max_offer_name[32];

    // String initialization
    strcpy(current_offer_name, "");
    strcpy(max_offer_name, "");

    // Blocking call
    while (udp_receive(socket_handle, buffer) > 0)
    {

        /**
         * If the command is valid, current_offer_name and current_offer will be populated correctly.
         * If the command is invalid, reply.
         * 
         */
        if (parse_command(buffer, current_offer_name, &current_offer))
        {
            LOG_INFO("Offerta ricevuta di %f da %s.", current_offer, current_offer_name);

            /**
             * In case of best offer, save name and offer, then reply.
             * If not, only reply and discard it.
             * 
             */
            if (current_offer > max_offer)
            {
                LOG_INFO("Offerta accettata.");

                max_offer = current_offer;
                strcpy(max_offer_name, current_offer_name);
                try_reply(socket_handle, OFFER_ACCEPTED_MSG);
            }
            else
            {
                LOG_INFO("Offerta troppo bassa.");
                try_reply(socket_handle, OFFER_NOT_ACCEPTED_MSG);
            }
        }
        else
        {
            LOG_WARNING("Ricevuto comando non valido.");
            try_reply(socket_handle, OFFER_INVALID_MSG);
        }
    }
}

/**
 * @brief Usage: asta_telematica_server <port>
 */
int main(int argc, char *argv[])
{
    // Socket variables
    int socket_handle;
    uint16_t port = DEFAULT_PORT;

    // Input parameters
    int opt;                        // Current option.
    bool parsing_fail_flag = false; // Flag that signals that a parsing error occurred.

    // Parameter parsing
    while ((opt = getopt(argc, argv, ":p:h")) != -1)
    {
        switch (opt)
        {
            // Port parameter
        case 'p':
            if (sscanf(optarg, "%hu", &port) != 1)
            {
                parsing_fail_flag = true;
            }
            break;

            // Unknown parameters
        case '?':
            LOG_ERROR("Parametro sconosciuto: %c.", optopt);
            parsing_fail_flag = true;
            break;

            // Parameters without arguments
        case ':':
            LOG_ERROR("Argomento mancante per il parametro %c.", optopt);
            parsing_fail_flag = true;
            break;

            // Unknown errors
        default:
            LOG_ERROR("Errore sconosciuto durante il parsing dei parametri.");
            parsing_fail_flag = true;
            break;
        }
    }

    // Abort execution if argument parsing failed
    if (parsing_fail_flag)
    {
        LOG_INFO("Uso: %s [-p porta]", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Server start
    LOG_INFO("Avvio server asta telematica sulla porta %u.", port);
    if ((socket_handle = create_udp_server("0.0.0.0", port)) > -1)
    {
        LOG_INFO("Creato un server UDP con socket identifier: %d.", socket_handle);
    }
    else
    {
        LOG_ERROR("Errore durante la creazione del server.");
        exit(EXIT_FAILURE);
    }

    // Interactive loop
    server_loop(socket_handle);

    // Exit
    die(socket_handle, EXIT_SUCCESS);
}
