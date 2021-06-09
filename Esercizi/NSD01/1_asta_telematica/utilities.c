#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <udpsocketlib.h>

/**
 * @brief Closes the socket and then terminates the process.
 * 
 * @param socket_handle Socket handle.
 * @param status Exit status code.
 */
void die(int socket_handle, int status)
{
    if (close_udp_socket(socket_handle))
    {
        printf("Socket UDP %d chiuso.\n", socket_handle);
    }
    else
    {
        printf("Impossibile chiudere il socket UDP %d.\n", socket_handle);
    }

    printf("Processo terminato.\n");
    exit(status);
}

/**
 * @brief Handler for udpsocketlib.
 * 
 * @param message Error message
 */
void error_handler(const char *message)
{
    printf("ERRORE: %s\n", message);
    exit(EXIT_FAILURE);
}
