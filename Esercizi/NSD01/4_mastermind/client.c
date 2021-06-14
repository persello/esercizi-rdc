/**
 * @file client.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Mastermind solver client.
 * @version 0.1
 * @date 2021-06-14
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
#include <libmastermind/mastermind_solver.h>
#include <libringbuffer/ring_buffer.h>
#include <tcpsocketlib.h>

/**
 * @brief Sends a new guess to the server.
 *
 * @param game The current game.
 * @param sk The socket handle.
 * @param guess The guess output.
 * @return unsigned long The number of remaining guesses. 0 in case of error.
 */
unsigned long send_new_guess(mm_game_t *game, int sk, unsigned char *guess) {
  unsigned long remaining_guesses;
  char send_buffer[256 + 1];

  if ((remaining_guesses = mm_make_guess(game, guess)) == 0) {
    LOG_WARNING("Tutti i possibili tentativi sono falliti.");
    return 0;
  } else {
    LOG_INFO("Provo il codice %d%d%d%d, ci sono ancora %lu tentativi.", guess[0], guess[1],
             guess[2], guess[3], remaining_guesses);
  }
  sprintf(send_buffer, "myguess: %d%d%d%d\n", guess[0], guess[1], guess[2], guess[3]);

  if (!tcp_send(sk, send_buffer)) {
    LOG_ERROR("Impossibile inviare il tentativo.");
    return 0;
  }

  return remaining_guesses;
}

/**
 * @brief Main loop.
 *
 * @param sk Socket handle.
 * @param name Player name.
 * @return int The number of tries. If something goes wrong, returns 0.
 */
int mastermind_client_loop(int sk, char *name) {
  ring_buffer_t *buffer;
  mm_game_t *game = mm_create_game();

  // Makes the loop return and exit the program.
  bool fail_flag = false;

  // Two times the single buffer's size.
  buffer = ring_buffer_create(BUFSIZ * 2);

  // Temporary buffer for a single TCP segment.
  char temp[BUFSIZ + 1];

  // A complete line (block, command?), up to \n.
  char *received_block = NULL;

  // Buffer for first outgoing command.
  char send_command[256 + 1];

  // Stores the current guess.
  unsigned char current_guess[MM_CODE_LENGTH];

  // --- Loop ---

  // Build outgoing command and send it.
  snprintf(send_command, 256, "player: %s\n", name);
  tcp_send(sk, send_command);

  // Blocking, receives a segment.
  while (tcp_receive(sk, temp)) {

    // Add to buffer and wait for a complete line.
    ring_buffer_append(buffer, temp);

    while (ring_buffer_read_line(buffer, &received_block) != (size_t)-1) {

      // A line is complete, and has been copied to received_block.

      if (strncmp(received_block, "PLAYERACCEPTED", 14) == 0) {

        // ----- Player accepted, do the first guess. -----

        fail_flag = send_new_guess(game, sk, current_guess) == 0;

      } else if (strncmp(received_block, "INVALIDFORMAT", 13) == 0) {

        // ----- Invalid format, tell user and fail. -----

        LOG_ERROR("Ricevuto dal server un messaggio di errore: %s.", received_block);
        fail_flag = true;

      } else if (strncmp(received_block, "CORRECT!!!", 10) == 0) {

        // ----- Correct, return number of tries. -----

        int tries = 0;

        sscanf(received_block, "%*s %*s %*s %*s %*d%*c %d", &tries);

        LOG_INFO("Tentativo #%d corretto!", tries);

        free(game);
        ring_buffer_delete(buffer);
        return tries;

      } else if (strncmp(received_block, "YOURSCORE", 9) == 0) {

        // ----- Generate next guess. -----

        mm_guess_result_t result;

        sscanf(received_block, "%*s %d %*s %d", &(result.positions), &(result.values));

        LOG_INFO("Il tentativo corrente ha un punteggio di %d posizioni e %d valori.", result.positions, result.values);

        mm_receive_guess_result(game, result, current_guess);

        send_new_guess(game, sk, current_guess);

      } else if (strncmp(received_block, "ERROR", 5) == 0) {

        // ----- Print error and quit. -----

        LOG_ERROR("Errore del server: %s.", received_block);
        fail_flag = true;

      } else {

        // ----- Unknown error, print and quit. -----

        LOG_ERROR("Messaggio sconosciuto: %s.", received_block);
        fail_flag = true;
      }

      if (fail_flag) {
        ring_buffer_delete(buffer);
        free(game);
        return 0;
      }
    }
  }

  // Executed only on unexpected disconnections.
  ring_buffer_delete(buffer);
  return 0;
}

/**
 * @brief Usage: mastermind_client -s <server> [-p <port>] -n <name>
 *
 */
int main(int argc, char *argv[]) {

  // Getopt variables
  int opt;
  bool parsing_fail_flag = false;

  // Player name
  char name[128];
  strcpy(name, "");

  // Number of guesses
  int guesses = 0;

  // Socket variables
  int socket_handle;
  uint16_t port = DEFAULT_PORT;
  char server[128];
  strcpy(server, "");

  while ((opt = getopt(argc, argv, ":s:p:n:")) != -1) {
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

    // Name
    case 'n':
      if (strlen(optarg) > 128 - 1) {
        LOG_ERROR("Nome troppo lungo.");
        parsing_fail_flag = true;
      } else {
        strncpy(name, optarg, 128);
      }
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

  // Check for minimum IP length 1.1.1.1 (7 characters).
  if (strlen(server) < 7) {
    LOG_ERROR("Indicare un indirizzo IP valido.");
    parsing_fail_flag = true;
  }

  // Check for name.
  if (strlen(name) < 1) {
    LOG_ERROR("Indicare il nome del giocatore.");
    parsing_fail_flag = true;
  }

  // Print help message.
  if (parsing_fail_flag) {
    LOG_ERROR("Uso: %s -s <server> [-p <porta>] -n <nome>", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Connect.
  LOG_INFO("Connessione a %s:%d.", server, port);
  if ((socket_handle = create_tcp_client_connection(server, port)) == -1) {
    LOG_ERROR("Errore di connessione.");
    exit(EXIT_FAILURE);
  }

  if ((guesses = mastermind_client_loop(socket_handle, name)) != 0) {
    LOG_INFO("Vittoria!");
    close_tcp_connection(socket_handle);
    return EXIT_SUCCESS;
  } else {
    LOG_WARNING("Qualcosa è andato storto e non è stato possibile trovare una soluzione valida.");
    close_tcp_connection(socket_handle);
    return EXIT_FAILURE;
  }
}

// Otherwise the compiler cries.
int server_handler() { return 0; }
