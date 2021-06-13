/**
 * @file client.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief "Gara podistica" client.
 * @version 0.1
 * @date 2021-06-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "constants.h"
#include "utilities.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <liblog/log.h>
#include <libringbuffer/ring_buffer.h>
#include <tcpsocketlib.h>

void close_connection(int sk);
void shutdown_server(int sk);
void print_participants_list(int sk);
unsigned long add_runner(int sk, char *name);

void close_connection(int sk) {
  LOG_INFO("Chiusura connessione.");
  tcp_send(sk, "BYE\n");
  close_tcp_connection(sk);
}

void shutdown_server(int sk) {
  LOG_INFO("Chiusura connessione e spegnimento del server.");
  tcp_send(sk, "SHUTDOWN\n");
  close_tcp_connection(sk);
}

void print_participants_list(int sk) {
  ring_buffer_t *buffer;

  // Two times a reasonably large TCP packet (usually smaller ~ 1500?).
  buffer = ring_buffer_create(4096 * 2);

  char temp[BUFSIZ + 1];
  char *received_block = NULL;

  tcp_send(sk, "LISTA\n");
  LOG_INFO("Ricezione lista dal server...");

  while (tcp_receive(sk, temp)) {
    ring_buffer_append(buffer, temp);

    while (ring_buffer_read_line(buffer, &received_block) != (size_t)-1) {

      if (strncmp(received_block, ".", 1) == 0) {
        free(received_block);
        ring_buffer_delete(buffer);
        return;
      }

      printf("%s", received_block);

      free(received_block);
      received_block = NULL;
    }
  }

  ring_buffer_delete(buffer);
}

/**
 * @brief Adds a new runner to the server.
 *
 * @param sk Socket handle.
 * @param name Runner name.
 * @return unsigned long Runner number. 0 on failure.
 */
unsigned long add_runner(int sk, char *name) {
  ring_buffer_t *buffer;
  buffer = ring_buffer_create(1024);

  char temp[BUFSIZ + 1];
  char *received_block = NULL;
  char send_command[256 + 1];
  unsigned long number;

  snprintf(send_command, 256, "ISCRIVI %s\n", name);
  tcp_send(sk, send_command);

  while (tcp_receive(sk, temp)) {
    ring_buffer_append(buffer, temp);

    while (ring_buffer_read_line(buffer, &received_block) != (size_t)-1) {
      if (sscanf(received_block, "%lu", &number) == 1) {
        free(received_block);
        ring_buffer_delete(buffer);
        return number;
      } else {
        LOG_WARNING("Errore durante l'aggiunta del partecipante: %s.", received_block);
        free(received_block);
        ring_buffer_delete(buffer);
        return 0;
      }
    }
  }

  ring_buffer_delete(buffer);
  return 0;
}

/**
 * @brief Usage: gara_podistica_client [-i <nome iscrizione>] [-l] [-s]
 * -l: Richiesta lista
 * -s: Richiesta spegnimento server
 *
 */
int main(int argc, char *argv[]) {

  // Getopt variables
  int opt;
  bool parsing_fail_flag = false;

  // User variables
  bool request_list_flag = false;
  bool request_shutdown_flag = false;
  char name[128 + 1];
  strcpy(name, "");

  // Socket variables
  int socket_handle;
  uint16_t port = DEFAULT_PORT;
  char server[128];
  strcpy(server, "");

  while ((opt = getopt(argc, argv, ":i:lxs:p:")) != -1) {
    switch (opt) {

    // Server
    case 's':
      if (strlen(optarg) > 128 - 1) {
        LOG_ERROR("Indirizzo server troppo lungo.");
        parsing_fail_flag = true;
      } else {
        strncpy(server, optarg, 128);
      }
      break;

    // Port
    case 'p':
      if (sscanf(optarg, "%hu", &port) != 1) {
        LOG_ERROR("Formato porta non valido.");
        parsing_fail_flag = true;
      }
      break;

    // Add runner
    case 'i':
      if (strlen(optarg) > 128 - 1) {
        LOG_ERROR("Nome troppo lungo.");
        parsing_fail_flag = true;
      } else {
        strncpy(name, optarg, 128);
      }
      break;

    // List request
    case 'l':
      request_list_flag = true;
      break;

    // Server shutdown
    case 'x':
      request_shutdown_flag = true;
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

  if (strlen(server) < 7) {
    LOG_ERROR("Indicare un indirizzo IP valido.");
    parsing_fail_flag = true;
  }

  if (!request_list_flag && !request_shutdown_flag && strlen(name) == 0) {
    LOG_ERROR("Indicare almeno un'operazione da eseguire.");
    parsing_fail_flag = true;
  }

  if (parsing_fail_flag) {
    LOG_ERROR("Uso: %s -s <server> [-p <porta>] [-i <nome>] [-l] [-x]\r\n\r\n"
              "         -s <server>:           Indirizzo del server.\r\n"
              "         -p <porta>:            Porta del server (default %d).\r\n"
              "         -i <nome>:             Iscrivi un nuovo partecipante.\r\n"
              "         -l:                    Richiedi lista partecipanti.\r\n"
              "         -x:                    Richiedi spegnimento server.\r\n",
              argv[0], DEFAULT_PORT);
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Connessione a %s:%d.", server, port);
  if ((socket_handle = create_tcp_client_connection(server, port)) == -1) {
    LOG_ERROR("Errore di connessione.");
    exit(EXIT_FAILURE);
  }

  if (strlen(name) > 0) {
    unsigned long number;
    if ((number = add_runner(socket_handle, name)) != 0) {
      LOG_INFO("Il numero di pettorale di %s è %lu.", name, number);
    } else {
      LOG_WARNING("Impossibile ottenere un numero di pettorale dal server.");
    }
  }

  if (request_list_flag) {
    print_participants_list(socket_handle);
  }

  if (request_shutdown_flag) {
    shutdown_server(socket_handle);
  } else {
    close_connection(socket_handle);
  }

  return EXIT_SUCCESS;
}

int server_handler() { return 0; }
