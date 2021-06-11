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

#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include <liblog/log.h>
#include <tcpsocketlib.h>

/**
 * @brief Handler for tcpsocketlib.
 *
 * @param message Error message
 */
noreturn void error_handler(const char *message) {
  LOG_ERROR("%s", message);
  exit(EXIT_FAILURE);
}
