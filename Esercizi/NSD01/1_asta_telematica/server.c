#include "server_utilities.h"
#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <udpsocketlib.h>

#define DEFAULT_PORT 25000
#define STARTING_PRICE 0.0

void server_loop(int socket_handle)
{
    char buffer[BUFSIZ + 1];

    // Offerte
    double current_offer = STARTING_PRICE;
    double max_offer = STARTING_PRICE;

    // Nomi
    char current_offer_name[32];
    char max_offer_name[32];

    // Preparazione delle stringhe
    strcpy(current_offer_name, "");
    strcpy(max_offer_name, "");

    // Bloccante
    while (udp_receive(socket_handle, buffer) > 0)
    {

        /**
         * Se il comando è valido, in current_offer_name ci sarà il nome dell'offerente,
         * mentre in current_offer l'offerta corrente.
         * Se non lo è, lo si indica al mittente.
         * 
         */
        if (parse_command(buffer, current_offer_name, &current_offer))
        {
            printf("Offerta ricevuta di %f da %s.\n", current_offer, current_offer_name);

            /**
             * Se l'offerta corrente è la migliore salvo offerta e nome (per eventuali usi successivi).
             * Se non lo è, si invia la risposta senza memorizzare nulla.
             * 
             */
            if (current_offer > max_offer)
            {
                printf("Offerta accettata.\n");

                max_offer = current_offer;
                strcpy(max_offer_name, current_offer_name);
                try_reply(socket_handle, "ACCETTATA");
            }
            else
            {
                printf("Offerta troppo bassa.\n");

                try_reply(socket_handle, "NON_ACCETTATA");
            }
        }
        else
        {
            printf("Ricevuto comando non valido.\n");

            try_reply(socket_handle, "INVALIDA");
        }
    }
}

int main(int argc, char *argv[])
{
    int socket_handle;
    uint16_t port = DEFAULT_PORT;

    if (argc == 1)
    {
        printf("Porta non specificata, si usa la porta di default (%d).\n", DEFAULT_PORT);
    }
    else if (argc == 2)
    {
        if (sscanf(argv[1], "%hu", &port) != 1)
        {
            printf("Uso: %s <port>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Uso: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Avvio del server
    printf("Avvio server asta telematica sulla porta %u.\n", port);
    if ((socket_handle = create_udp_server("0.0.0.0", port)) > -1)
    {
        printf("Creato un server UDP con socket identifier: %d.\n", socket_handle);
    }
    else
    {
        printf("Errore durante la creazione del server.\n");
        exit(EXIT_FAILURE);
    }

    // Programma interattivo
    server_loop(socket_handle);

    // Chiusura
    die(socket_handle, EXIT_SUCCESS);
}
