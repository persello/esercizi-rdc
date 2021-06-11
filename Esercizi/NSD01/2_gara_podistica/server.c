/**
 * @file server.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief "Gara podistica" server.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "constants.h"
#include "runner.h"
#include "utilities.h"

#include <stdlib.h>
#include <unistd.h>

#include <liblog/log.h>
#include <libringbuffer/ring_buffer.h>
#include <tcpsocketlib.h>

int server_handler(int sk, char *ip_addr, int port) {
  ring_buffer_t *buffer;
  char received[BUFSIZ + 1];
  char *command = NULL;

  LOG_INFO("Connessione aperta con %s, porta %d.", ip_addr, port);

  buffer = ring_buffer_create(1024);

  while (tcp_receive(sk, received)) {
    ring_buffer_append(buffer, received);
    LOG_INFO("REC: %s", received);

    while (ring_buffer_read_line(buffer, command) != (size_t)-1) {

      LOG_INFO("COM: %s", command);
      if (strcmp(command, "BYE\n") == 0) {
        return 1;
      } else if (strcmp(command, "GOODBYE\n") == 0) {
        return 0;
      }

      free(command);
      command = NULL;
    }
  }

  LOG_INFO("Chiusura connessione con %s, porta %d.", ip_addr, port);

  ring_buffer_delete(buffer);
  return 1;
}

int main(int argc, char *argv[]) {

  // Server ariables
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
