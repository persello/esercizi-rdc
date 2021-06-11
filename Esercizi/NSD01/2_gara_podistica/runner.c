/**
 * @file runner.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Runner data type.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a string describing a runner.
 *
 * @param stringp The output string pointer, not allocated.
 * @param runner The runner struct.
 * @return true On success.
 * @return false On failure.
 * 
 * @note Remember to free the string afterwards.
 */
bool runner_to_string(char **stringp, runner_t runner) {
  if (strlen(runner.name) > 0 && runner.number > 0) {
    asprintf(stringp, "%lu\t%s", runner.number, runner.name);
    return true;
  }

  return false;
}

/**
 * @brief Creates a runner struct instance from a string.
 *
 * @param runner The output runner.
 * @param string The input string.
 * @return true On success.
 * @return false On failure.
 */
bool runner_from_string(runner_t *runner, char *string) {

  unsigned long number;
  char *name = NULL;

  if (sscanf(string, "%lu\t%s", &number, name) == 2) {
    strcpy(runner->name, name);
    runner->number = number;

    free(name);

    return true;
  }

  return false;
}
