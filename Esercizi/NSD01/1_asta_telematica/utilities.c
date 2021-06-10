/**
 * @file utilities.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Useful functions for both server and client programs.
 * @version 0.1
 * @date 2021-06-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>

#include <udpsocketlib.h>
#include <liblog/log.h>

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
        LOG_INFO("Socket UDP %d chiuso.", socket_handle);
    }
    else
    {
        LOG_ERROR("Impossibile chiudere il socket UDP %d.", socket_handle);
    }

    LOG_INFO("Processo terminato.");
    exit(status);
}

/**
 * @brief Handler for udpsocketlib.
 * 
 * @param message Error message
 */
void error_handler(const char *message)
{
    LOG_ERROR("%s", message);
    exit(EXIT_FAILURE);
}
