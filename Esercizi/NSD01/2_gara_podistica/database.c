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

#include "database.h"

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

  fp = fopen("runners", "rb");

  if (fp != NULL) {
    fread(&count, sizeof(count), 1, fp);

    runners = (runner_t *)malloc(sizeof(runner_t) * (count + 1));
    unsigned long read_count = fread(runners, sizeof(runner_t), count, fp);

    fclose(fp);

    if (read_count != count) {
      free(runners);
      return 0;
    }

    count++;
  } else {
    count = 1;
    runners = (runner_t *)malloc(sizeof(runner_t));
  }

  // Add the new runner
  runner_t new_runner;

  // Start from 1
  new_runner.number = count;
  memset(new_runner.name, 0, 128 + 1);
  strcpy(new_runner.name, name);

  // Add to end of runners list
  runners[count - 1] = new_runner;

  // Write
  fp = fopen("runners", "wb");

  if (fp == NULL) {
    free(runners);
    return 0;
  }

  if (fwrite(&count, sizeof(count), 1, fp) == 1) {
    if (fwrite(runners, sizeof(runner_t), count, fp) == count) {
      fclose(fp);
      free(runners);
      return count;
    }
  }

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
