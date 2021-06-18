/**
 * @file database.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Runners database.
 * @version 0.1
 * @date 2021-06-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "runnerdb/database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Adds a runner to the database.
 *
 * @param name The new runner's name.
 * @return unsigned long The runner's number, *0 on failure*.
 */
unsigned long database_add(char *name) {
  FILE *fp;
  runner_t *runners = NULL;
  unsigned long count = 0;

  // Open runners file in b mode (for Windows?)
  fp = fopen("runners", "rb");

  if (fp != NULL) {
    // If the file exists

    // Read the runner count in the first bytes.
    fread(&count, sizeof(count), 1, fp);

    // Then allocate enough space for all the existing runners and one more.
    runners = (runner_t *)malloc(sizeof(runner_t) * (count + 1));

    // Then read the existing runners from file.
    unsigned long read_count = fread(runners, sizeof(runner_t), count, fp);

    // No need to read the file anymore.
    fclose(fp);

    // Check for reading errors.
    if (read_count != count) {
      free(runners);
      return 0;
    }

    // Increment the counter that will be written at the beginning of the file.
    count++;
  } else {

    // If the file does not exist, we have only one runner (the one we're adding).
    // Start the runner numbers from 1.
    count = 1;
    runners = (runner_t *)malloc(sizeof(runner_t));
  }

  // The new runner.
  runner_t new_runner;

  // Set the number and clear the name field, in order not to write garbage on file. Then set the name.
  new_runner.number = count;
  memset(new_runner.name, 0, 128 + 1);
  strcpy(new_runner.name, name);

  // Add current runner to end of runners list.
  runners[count - 1] = new_runner;

  // Open for overwriting.
  fp = fopen("runners", "wb");

  // Error!
  if (fp == NULL) {
    free(runners);
    return 0;
  }

  // Write and check.
  if (fwrite(&count, sizeof(count), 1, fp) == 1) {
    if (fwrite(runners, sizeof(runner_t), count, fp) == count) {
      fclose(fp);
      free(runners);
      return count;
    }
  }

  // Error!
  fclose(fp);
  free(runners);
  return 0;
}

/**
 * @brief Gets all the runners.
 *
 * @param runners Runners. Remember to free().
 * @return unsigned long Number of runners.
 */
unsigned long database_get_all(runner_t **runners) {
  FILE *fp;
  unsigned long count = 0;

  fp = fopen("runners", "rb");

  if (fp != NULL) {
    fread(&count, sizeof(count), 1, fp);

    *runners = (runner_t *)malloc(sizeof(runner_t) * (count));
    unsigned long read_count = fread(*runners, sizeof(runner_t), count, fp);

    fclose(fp);

    if (read_count != count) {
      return 0;
    }

    return count;
  } else {

    return 0;
  }
}
