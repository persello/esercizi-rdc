#include "server_utilities.h"
#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <udpsocketlib.h>

#define PORT 25000
#define STARTING_PRICE 0.0

int main(int argc, char *argv[])
{
    int socket_handle;
    char buffer[BUFSIZ + 1];
    double current_offer = STARTING_PRICE;
    double max_offer = STARTING_PRICE;
    char current_offer_name[32];
    char max_offer_name[32];

    strcpy(current_offer_name, "");
    strcpy(max_offer_name, "");

    printf("Avvio server asta telematica.\n");
    socket_handle = create_udp_server("0.0.0.0", PORT);

    if (socket_handle > -1)
    {
        printf("Creato un server UDP con socket identifier: %d.\n", socket_handle);
    }
    else
    {
        printf("Errore durante la creazione del server.\n");
    }

    while (udp_receive(socket_handle, buffer) > 0)
    {
        if (parse_command(buffer, current_offer_name, &current_offer))
        {
            printf("Offerta ricevuta di %f da %s.\n", current_offer, current_offer_name);

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

    die(socket_handle, EXIT_SUCCESS);
}
