/**
 * @file utilities.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Useful functions for both server and client programs.
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

/**
 * @brief Closes the socket and then terminates the process.
 *
 * @param socket_handle Socket handle.
 * @param status Exit status code.
 */
void die(int socket_handle, int status);

/**
 * @brief Handler for udpsocketlib.
 *
 * @param message Error message
 */
void error_handler(const char *message);
