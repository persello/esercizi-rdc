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
#include <sys/file.h>
#include <unistd.h>

#include <log/log.h>

/**
 * @brief Adds a runner to the database.
 *
 * @param name The new runner's name.
 * @return unsigned long The runner's number, *0 on failure*.
 */
unsigned long database_add(char *name) {
  int fd;
  FILE *fp;
  runner_t *runners = NULL;
  unsigned long count = 0;

  LOG_INFO("Richiesta di aggiunta al database per %s.", name);

  // Open a file descriptor for the db file (rw-rw-rw-).
  fd = open(DB_FILE_NAME, O_RDWR | O_CREAT, 0666);

  // Quick check.
  if (fd == -1) {
    LOG_ERROR("Errore durante l'apertura del file descriptor del database.");
    return 0;
  }

  LOG_INFO("Controllo del blocco del file di database.");

  // Gets unlocked on fclose.
  while (flock(fd, LOCK_EX) != 0) {
    ;
  }

  LOG_INFO("Ottenuto il controllo del file.");

  // Open runners file in b mode from a duplicate of fd.
  fp = fdopen(dup(fd), "rb");

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
      LOG_INFO("Letti %lu partecipanti dal database (invece di %lu).", read_count, count);

      free(runners);
      return 0;
    }

    LOG_INFO("Letti %lu partecipanti dal database.", read_count);

    // Increment the counter that will be written at the beginning of the file.
    count++;
  } else {

    LOG_INFO("Il file di database non esiste, questo è il primo partecipante.");

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

  // Truncate file before writing (we can't reopen it with O_TRUNC, otherwise we may lose the lock in the meantime).
  ftruncate(fd, 0);

  // Sync changes.
  fsync(fd);

  LOG_INFO("Il file è stato cancellato ed è pronto per una nuova scrittura.");

  // Open runners file in b mode
  fp = fdopen(fd, "wb");

  // Seek start of file.
  fseek(fp, 0, SEEK_SET);

  // Error!
  if (fp == NULL) {
    LOG_ERROR("Errore durante l'apertura del file in scrittura.");
    free(runners);
    return 0;
  }

  // Write and check.
  if (fwrite(&count, sizeof(count), 1, fp) == 1) {
    if (fwrite(runners, sizeof(runner_t), count, fp) == count) {
      LOG_INFO("Scrittura di %lu partecipanti completata con successo.", count);
      fclose(fp);
      free(runners);
      return count;
    }

    LOG_ERROR("Non tutti i partecipanti sono stati aggiunti al database.");
  } else {
    LOG_ERROR("Errore durante la scrittura dell'indice.");
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

  // Unlocked on close
  while (flock(fileno(fp), LOCK_EX) != 0) {
    ;
  }

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
