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

#include <log/log.h>
#include <ringbuffer/ring_buffer.h>
#include <tcpsocketlib.h>

/**
 * @brief Closes the connection with the server. First sends the 'BYE' command, then closes the TCP socket.
 *
 * @param sk Socket handle.
 */
void close_connection(int sk);

/**
 * @brief Sends the shutdown command to the server and closes the connection.
 *
 * @param sk Socket handle.
 */
void shutdown_server(int sk);

/**
 * @brief Requests to the server the participants list, then prints it to stdout.
 *
 * @param sk Socket handle.
 */
void print_participants_list(int sk);

/**
 * @brief Requests to the server to add a participant with a specific name.
 *
 * @param sk Socket handle.
 * @param name Participant's name.
 * @return unsigned long The assigned number (>0). 0 on failure.
 */
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

  // Two times the single buffer's size.
  buffer = ring_buffer_create(BUFSIZ * 2);

  // Temporary buffer for read.
  char temp[BUFSIZ + 1];

  // A complete block (until \n).
  char *received_block = NULL;

  // Request the list.
  tcp_send(sk, "LISTA\n");
  LOG_INFO("Ricezione lista dal server...");

  // Blocking, receives a segment.
  while (tcp_receive(sk, temp)) {

    // Add to buffer and wait for a complete line.
    ring_buffer_append(buffer, temp);

    while (ring_buffer_read_line(buffer, &received_block) != (size_t)-1) {

      // A line is complete, and has been copied to received_block.

      // If a '.' is detected at the beginning of a line, it means that we reached the end of the list.
      if (strncmp(received_block, ".", 1) == 0) {

        // Free everything and return.
        free(received_block);
        ring_buffer_delete(buffer);
        return;
      }

      // If we're before the end of the list, print the line.
      printf("%s", received_block);

      // Free the block for the next line (gets allocated in ring_buffer_read_line, see docs).
      free(received_block);
      received_block = NULL;
    }
  }

  // Could get here due to an unexpected disconnection. In that case, free the buffer.
  ring_buffer_delete(buffer);
}

unsigned long add_runner(int sk, char *name) {
  ring_buffer_t *buffer;

  // Two times the single buffer's size.
  buffer = ring_buffer_create(BUFSIZ * 2);

  // Temporary buffer for read.
  char temp[BUFSIZ + 1];

  // A complete line (block, command?), up to \n.
  char *received_block = NULL;

  // Buffer for outgoing commands.
  char send_command[256 + 1];

  // Received number.
  unsigned long number;

  // Build outgoing command and send it.
  snprintf(send_command, 256, "ISCRIVI %s\n", name);
  tcp_send(sk, send_command);

  // Blocking, receives a segment.
  while (tcp_receive(sk, temp)) {

    // Add to buffer and wait for a complete line.
    ring_buffer_append(buffer, temp);

    while (ring_buffer_read_line(buffer, &received_block) != (size_t)-1) {

      // A line is complete, and has been copied to received_block.

      // If a number is available, then copy it in number and return it.
      if (sscanf(received_block, "%lu", &number) == 1) {
        free(received_block);
        ring_buffer_delete(buffer);
        return number;
      } else {
        // If a number is not detected, it means that the server sent an error string.
        LOG_WARNING("Errore durante l'aggiunta del partecipante: %s.", received_block);
        free(received_block);
        ring_buffer_delete(buffer);
        return 0;
      }
    }
  }

  // Executed only on unexpected disconnections.
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

  // Check for minimum IP length 1.1.1.1 (7 characters)
  if (strlen(server) < 7) {
    LOG_ERROR("Indicare un indirizzo IP valido.");
    parsing_fail_flag = true;
  }

  // If no operations have been specified, it makes no sense to connect with the server.
  if (!request_list_flag && !request_shutdown_flag && strlen(name) == 0) {
    LOG_ERROR("Indicare almeno un'operazione da eseguire.");
    parsing_fail_flag = true;
  }

  // Print help message.
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

  // Connect
  LOG_INFO("Connessione a %s:%d.", server, port);
  if ((socket_handle = create_tcp_client_connection(server, port)) == -1) {
    LOG_ERROR("Errore di connessione.");
    exit(EXIT_FAILURE);
  }
  
  // If a name has been specified, call the function that adds a runner to the DB. Print the received number.
  if (strlen(name) > 0) {
    unsigned long number;
    if ((number = add_runner(socket_handle, name)) != 0) {
      LOG_INFO("Il numero di pettorale di %s è %lu.", name, number);
    } else {
      LOG_WARNING("Impossibile ottenere un numero di pettorale dal server.");
    }
  }

  // If the list flag is on, request the list and print it.
  if (request_list_flag) {
    print_participants_list(socket_handle);
  }

  // If the shutdown flag is on, request shutdown, otherwise simply disconnect.
  if (request_shutdown_flag) {
    shutdown_server(socket_handle);
  } else {
    close_connection(socket_handle);
  }

  return EXIT_SUCCESS;
}

// Otherwise the compiler cries.
int server_handler() { return 0; }
