/**
 * @file database.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Runners database.
 * @version 0.1
 * @date 2021-06-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include "runner.h"

#include <stdbool.h>

#define DB_FILE_NAME "runners"

/**
 * @brief Adds a runner to the database.
 *
 * @param name The new runner's name.
 * @return unsigned long The runner's number, -1 on failure.
 */
unsigned long database_add(char *name);

/**
 * @brief Gets all the runners.
 *
 * @param runners Runners. Remember to free().
 * @return unsigned long Number of runners.
 */
unsigned long database_get_all(runner_t **runners);
