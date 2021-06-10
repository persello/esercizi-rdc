#include "server_utilities.h"
#include "utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <udpsocketlib.h>
#include <liblog/log.h>

/**
 * @brief Tries to reply to the latest command.
 * Dies on failure.
 * 
 * @param socket_handle Socket handle.
 * @param message Reply message.
 */
void try_reply(int socket_handle, char *message)
{
    if (!udp_reply(socket_handle, message))
    {
        LOG_ERROR("Impossibile rispondere al comando.");
        die(socket_handle, EXIT_FAILURE);
    }
}

/**
 * @brief Parses a command in the current format: `OFFERTA NOME(<name>) VALORE(<value>)`.
 * The maximum command length is limited to 128 bytes.
 * 
 * @param command The input string.
 * @param name Name output (32 bytes maximum).
 * @param value Value output.
 * @return true If the command was parsed correcty.
 * @return false If the command wasn't parsed correctly.
 */
bool parse_command(char *command, char *name, double *value)
{

    int name_index = 0;
    int value_length;

    // ----- Length check, set to 128 -----
    if (strlen(command) > 128)
    {
        return false;
    }

    // ----- Format check -----

    // Start of command "OFFERTA NOME("
    if (strncmp(command, "OFFERTA NOME(", 13) != 0)
    {
        return false;
    }

    command += 13;

    // Capture name (between parentheses)
    while (*command != ')' && *command != '\0')
    {
        *(name + name_index) = *command;
        command++;
        name_index++;
    }

    // Add name termination
    *(name + name_index) = '\0';

    // Check name length
    if (strlen(name) > 31)
    {
        return false;
    }

    // Check name whitespace
    if (isspace(*name) || isspace(*(name + strlen(name) - 1)))
    {
        return false;
    }

    // Check middle part ") VALORE("
    if (strncmp(command, ") VALORE(", 9) != 0)
    {
        return false;
    }

    command += 9;

    // Capture value
    if (sscanf(command, "%lf %n", value, &value_length) < 1)
    {
        return false;
    }

    // Check final parentheses
    if (*(command + value_length) != ')')
    {
        return false;
    }

    return true;
}
