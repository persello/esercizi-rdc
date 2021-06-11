/**
 * @file server_utilities.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Useful functions for "Asta Telematica" server.
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <stdbool.h>

/**
 * @brief Tries to reply to the latest command.
 * Dies on failure.
 *
 * @param socket_handle Socket handle.
 * @param message Reply message.
 */
void try_reply(int socket_handle, char *message);

/**
 * @brief Parses a command in the current format: `OFFERTA NOME(<name>)
 * VALORE(<value>)`. The maximum command length is limited to 128 bytes.
 *
 * @param command The input string.
 * @param name Name output (32 bytes maximum).
 * @param value Value output.
 * @return true If the command was parsed correcty.
 * @return false If the command wasn't parsed correctly.
 */
bool parse_command(char *command, char *name, double *value);
