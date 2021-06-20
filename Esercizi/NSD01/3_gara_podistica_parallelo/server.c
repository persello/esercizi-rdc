/**
 * @file server.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief "Gara podistica" parallel server.
 * @version 0.1
 * @date 2021-06-20
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "constants.h"
#include "utilities.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <ringbuffer/ring_buffer.h>
#include <runnerdb/database.h>
#include <tcpsocketlib.h>

/**
 * @brief The child's server loop.
 *
 * @param sk Socket handle.
 * @param ip_addr IP address (for logging purposes).
 * @param port Port (for logging purposes).
 */
void child_server_loop(int sk, char *ip_addr, int port);

void child_server_loop(int sk, char *ip_addr, int port) {
  ring_buffer_t *buffer;
  char received[BUFSIZ + 1];
  char *command = NULL;

  pid_t pid = getpid();

  // Could contain two maximum-size reads.
  buffer = ring_buffer_create(BUFSIZ * 2);

  while (tcp_receive(sk, received)) {
    ring_buffer_append(buffer, received);

    // Wait for a complete line.
    while (ring_buffer_read_line(buffer, &command) != (size_t)-1) {

      if (strcmp(command, "BYE\n") == 0 || strcmp(command, "BYE\r\n") == 0) {

        // Disconnection command.

        LOG_INFO("PID %d: Chiusura connessione con %s, porta %d.", pid, ip_addr, port);

        tcp_send(sk, "Connessione chiusa.\n");
        ring_buffer_delete(buffer);
        return;

      } else if (strcmp(command, "SHUTDOWN\n") == 0 || strcmp(command, "SHUTDOWN\r\n") == 0) {

        // Shutdown command.

        LOG_WARNING("Shutdown non supportato: questo processo è effimero.");
        LOG_INFO("Chiusura connessione con %s, porta %d.", ip_addr, port);

        tcp_send(sk, "Connessione chiusa.\n");
        ring_buffer_delete(buffer);
        return;

      } else if (strcmp(command, "LISTA\n") == 0 || strcmp(command, "LISTA\r\n") == 0) {

        // List command.

        runner_t *runners;
        unsigned long count;
        char current_line[256 + 1];

        LOG_INFO("Invio lista dei partecipanti.");

        if ((count = database_get_all(&runners)) > 0) {

          LOG_INFO("Invio di %lu partecipanti.", count);

          for (unsigned long i = 0; i < count; i++) {
            snprintf(current_line, 256, "%lu: %s\n", runners[i].number, runners[i].name);
            tcp_send(sk, current_line);
          }
          tcp_send(sk, ".\n");
          free(runners);
        } else {
          LOG_INFO("Lista partecipanti vuota.");
          tcp_send(sk, "Lista vuota.\n.\n");
        }

      } else if (strncmp(command, "ISCRIVI ", 8) == 0) {

        // Add runner command.

        char name[128 + 1];
        strcpy(name, "");

        // Up to \r, \n.
        sscanf(command, "%*s %128[^\r\n]", name);

        // If a name has been supplies, add it to the database, get the number and check for errors.
        if (strlen(name) > 0) {
          LOG_INFO("Richiesta iscrizione per %s.", name);
          unsigned long number = database_add(name);

          if (number == 0) {
            tcp_send(sk, "Errore durante l'aggiunta al database.\n");
          } else {
            char answer[128 + 1];
            snprintf(answer, 128, "%lu\n", number);
            tcp_send(sk, answer);
          }
        }
      } else {

        // Unknown command.

        LOG_INFO("Ricevuto un comando sconosciuto: %s.", command);
        tcp_send(sk, "Comando sconosciuto.\n");
      }

      free(command);
      command = NULL;
    }
  }
}

int server_handler(int sk, char *ip_addr, int port) {
  pid_t pid;

  if ((pid = fork()) == -1) {
    LOG_ERROR("Unable to fork.");
    exit(EXIT_FAILURE);
  }

  if (pid != 0) {
    // Parent process.

    LOG_INFO("Processo principale: creato nuovo fork con PID %d.", pid);

    // Keep server alive.
    return 1;
  } else {
    // Child process.
    LOG_INFO("Server handler del processo figlio: connessione aperta con %s, porta %d.", ip_addr, port);

    // Void, returns when the client closes the connection.
    child_server_loop(sk, ip_addr, port);

    LOG_INFO("Server handler del processo figlio sta terminando.");

    // Kill server.
    return 0;
  }
}

int main(int argc, char *argv[]) {

  // Server variables
  char ip_addr[128];
  uint16_t port = DEFAULT_PORT;
  strncpy(ip_addr, DEFAULT_ADDRESS, 128);

  // Current option.
  int opt;
  // Flag that signals that a parsing error occurred.
  bool parsing_fail_flag = false;

  // Parameter parsing
  while ((opt = getopt(argc, argv, ":p:a:")) != -1) {
    switch (opt) {
      // Port parameter
    case 'p':
      if (sscanf(optarg, "%hu", &port) != 1) {
        LOG_ERROR("Formato porta non valido.");
        parsing_fail_flag = true;
      }
      break;

      // Address parameter
    case 'a':
      strncpy(ip_addr, optarg, 128);
      break;

      // Unknown parameters
    case '?':
      LOG_ERROR("Parametro sconosciuto: %c.", optopt);
      parsing_fail_flag = true;
      break;

      // Parameters without arguments
    case ':':
      LOG_ERROR("Argomento mancante per il parametro %c.", optopt);
      parsing_fail_flag = true;
      break;

      // Unknown errors
    default:
      LOG_ERROR("Errore sconosciuto durante il parsing dei parametri.");
      parsing_fail_flag = true;
      break;
    }
  }

  // Check IP address
  if (strlen(ip_addr) < 7) {
    LOG_ERROR("Indicare un indirizzo IP valido.");
  }

  // Abort execution if argument parsing failed
  if (parsing_fail_flag) {
    LOG_INFO("Uso: %s [-a <indirizzo>] [-p <porta>]", argv[0]);
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Avvio server su %s:%d.", ip_addr, port);
  if (create_tcp_server(ip_addr, port) < 0) {
    LOG_ERROR("Impossibile avviare il server su %s:%d.", ip_addr, port);
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
