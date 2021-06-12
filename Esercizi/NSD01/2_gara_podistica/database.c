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

#include <liblog/log.h>
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

    LOG_INFO("COUNT: %lu", count);

    runners = (runner_t *)malloc(sizeof(runner_t) * (count + 1));

    LOG_INFO("MALLOC OK");

    unsigned long read_count = fread(runners, sizeof(runner_t), count, fp);

    LOG_INFO("READ COUNT: %lu", read_count);

    fclose(fp);

    LOG_INFO("FCLOSE 1 OK");

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

  LOG_INFO("BEFORE STRCPY, POINTER=%p, NAME=%s", new_runner.name, name);
  memset(new_runner.name, 0, 128 + 1);
  strcpy(new_runner.name, name);

  LOG_INFO("NEW RUNNER INIT OK");

  runners[count - 1] = new_runner;

  // Write
  fp = fopen("runners", "wb");

  if (fp == NULL) {
    free(runners);
    return 0;
  }

  LOG_INFO("FOPEN 2 OK");

  if (fwrite(&count, sizeof(count), 1, fp) == 1) {
    LOG_INFO("COUNT WRITTEN OK");
    if (fwrite(runners, sizeof(runner_t), count, fp) == count) {
      LOG_INFO("RUNNERS WRITTEN OK");
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
 * @return runner_t* Array of runners. Remember to free().
 */
runner_t *database_get_all() {}
