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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Creates a lock file.
 *
 * @return true If successful.
 * @return false In case of error.
 */
bool database_create_lock_file(void);

/**
 * @brief Checks whether the database is effectively locked.
 *
 * @return true A lockfile exists and the process that created it is alive.
 * @return false Safe to read database file.
 */
bool database_check_lock_file(void);

/**
 * @brief Deletes the lock file.
 *
 * @return true When successfully deleted.
 * @return false In case of error.
 */
bool database_release_lock_file(void);

bool database_create_lock_file() {
  pid_t current_pid = getpid();
  FILE *fp;
  fp = fopen("~" DB_FILE_NAME, "wb");

  if (fp != NULL) {

    // If successful, write the current PID in the lock file.
    fwrite(&current_pid, sizeof(pid_t), 1, fp);
    fclose(fp);

    printf("[LOCK] (%d) created lock file.\n", current_pid);

    return true;
  } else {

    printf("[LOCK] (%d) unable to create lock file.\n", current_pid);
    return false;
  }
}

bool database_check_lock_file() {
  pid_t current_pid = getpid();
  FILE *fp;
  fp = fopen("~" DB_FILE_NAME, "rb");

  if (fp == NULL) {

    printf("[LOCK] (%d) lock file does not exist. Safe to continue.\n", current_pid);

    // Database file is not locked: lock file does not exist.
    return false;
  } else {
    // Database file might be locked.
    pid_t read_pid;

    fread(&read_pid, sizeof(pid_t), 1, fp);
    fclose(fp);

    printf("[LOCK] (%d) a lock file exists. PID of the creator: %d.\n", current_pid, read_pid);

    // Check whether the read PID is still alive (prone to errors, but fail-safe).
    // Simulates a kill to the read PID. Signal is zero, so it doesn't kill it, but returns an error if the process does
    // not exist. In that case, the lock file can be ignored.
    // You can check errno for ESRCH = 3 = "No such process".
    if (kill(read_pid, 0) == -1) {

      printf(
          "[LOCK] (%d) the process whith the PID that created the file does not exist. Deleting lock and continuing.\n",
          current_pid);

      if (remove("~" DB_FILE_NAME) == 0) {
        printf("[LOCK] (%d) lock file deleted successfully.\n", current_pid);
      } else {
        printf("[LOCK] (%d) unable to delete lock file.\n", current_pid);
      }

      return false;
    } else {
      printf("[LOCK] (%d) process still exists. Database file is locked.\n", current_pid);
      return true;
    }
  }
}

bool database_release_lock_file() {
  pid_t current_pid = getpid();
  FILE *fp;
  fp = fopen("~" DB_FILE_NAME, "rb");

  if (fp != NULL) {
    pid_t read_pid;

    // Read lock file creator's PID.
    fread(&read_pid, sizeof(pid_t), 1, fp);
    fclose(fp);

    printf("[LOCK] (%d) found lock file with PID %d.\n", current_pid, read_pid);

    if (read_pid == current_pid) {
      printf("[LOCK] (%d) this lockfile is mine. Deleting it.\n", current_pid);
      if (remove("~" DB_FILE_NAME) == 0) {
        printf("[LOCK] (%d) deleted successfully.\n", current_pid);
        return true;
      } else {
        printf("[LOCK] (%d) Error during deletion.\n", current_pid);
        return false;
      }
    } else {
      printf("[LOCK] (%d) this lockfile is not mine. Can't delete it.\n", current_pid);
      return false;
    }

    return true;
  } else {

    printf("[LOCK] (%d) lock file not found.\n", current_pid);

    return false;
  }
}

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

  // For multiprocess: block until .lock file disappears
  while (database_check_lock_file()) {
    sleep(1);
  }

  database_create_lock_file();

  // Open runners file in b mode (for Windows?)
  fp = fopen(DB_FILE_NAME, "rb");

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
      database_release_lock_file();
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
  fp = fopen(DB_FILE_NAME, "wb");

  // Error!
  if (fp == NULL) {
    free(runners);
    database_release_lock_file();
    return 0;
  }

  // Write and check.
  if (fwrite(&count, sizeof(count), 1, fp) == 1) {
    if (fwrite(runners, sizeof(runner_t), count, fp) == count) {
      fclose(fp);
      free(runners);
      database_release_lock_file();
      return count;
    }
  }

  // Error!
  fclose(fp);
  free(runners);
  database_release_lock_file();
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

  while (database_check_lock_file()) {
    sleep(1);
  }

  database_create_lock_file();

  fp = fopen("runners", "rb");

  if (fp != NULL) {
    fread(&count, sizeof(count), 1, fp);

    *runners = (runner_t *)malloc(sizeof(runner_t) * (count));
    unsigned long read_count = fread(*runners, sizeof(runner_t), count, fp);

    fclose(fp);

    if (read_count != count) {
      database_release_lock_file();
      return 0;
    }

    database_release_lock_file();

    return count;
  } else {

    database_release_lock_file();
    return 0;
  }
}
