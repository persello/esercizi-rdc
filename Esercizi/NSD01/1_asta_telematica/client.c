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

#include <stdlib.h>

#include <udpsocketlib.h>

/**
 * @brief Usage: asta_telematica_client [offerta] [nome] [server] <port>
 */
int main(int argc, char *argv[])
{
    int socket_handle;
    uint16_t port = DEFAULT_PORT;

    if (argc == 2) {

    }
}
