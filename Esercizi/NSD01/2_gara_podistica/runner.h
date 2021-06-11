/**
 * @file runner.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Runner data type.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdbool.h>

typedef struct _runner_t {
  char *name;
  unsigned long number;
} runner_t;

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
bool runner_to_string(char **stringp, runner_t runner);

/**
 * @brief Creates a runner struct instance from a string.
 *
 * @param runner The output runner.
 * @param string The input string.
 * @return true On success.
 * @return false On failure.
 */
bool runner_from_string(runner_t *runner, char *string);
