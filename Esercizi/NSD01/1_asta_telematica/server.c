/**
 * @file server.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief "Asta Telematica" server main.
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "constants.h"
#include "server_utilities.h"
#include "utilities.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <udpsocketlib.h>

/**
 * @brief Manages the main server loop.
 *
 * @param socket_handle The socket handle.
 */
static void server_loop(int socket_handle) {

  char buffer[BUFSIZ + 1];

  // Offers
  double current_offer = MINIMUM_STARTING_PRICE;
  double max_offer = MINIMUM_STARTING_PRICE;

  // Names
  char current_offer_name[MAX_NAME_LENGTH];
  char max_offer_name[MAX_NAME_LENGTH];

  // String initialization
  strcpy(current_offer_name, "");
  strcpy(max_offer_name, "");

  // Blocking call
  while (udp_receive(socket_handle, buffer) > 0) {

    /**
     * If the command is valid, current_offer_name and current_offer will be
     * populated correctly. If the command is invalid, reply.
     *
     */
    if (parse_command(buffer, current_offer_name, &current_offer)) {
      LOG_INFO("Offerta di %.2lf ricevuta da %s.", current_offer, current_offer_name);

      /**
       * In case of best offer, save name and offer, then reply.
       * If not, only reply and discard it.
       *
       */
      if (current_offer > max_offer) {
        LOG_INFO("Offerta accettata.");

        max_offer = current_offer;
        strcpy(max_offer_name, current_offer_name);
        try_reply(socket_handle, OFFER_ACCEPTED_MSG);
      } else {
        LOG_INFO("Offerta troppo bassa.");
        try_reply(socket_handle, OFFER_NOT_ACCEPTED_MSG);
      }
    } else {
      LOG_WARNING("Ricevuto comando non valido.");
      try_reply(socket_handle, OFFER_INVALID_MSG);
    }
  }
}

/**
 * @brief Usage: asta_telematica_server [-p <porta>]
 */
int main(int argc, char *argv[]) {

  // Socket variables
  int socket_handle;
  uint16_t port = DEFAULT_PORT;

  // Current option.
  int opt;
  // Flag that signals that a parsing error occurred.
  bool parsing_fail_flag = false;

  // Parameter parsing
  while ((opt = getopt(argc, argv, ":p:")) != -1) {
    switch (opt) {

      // Port parameter
    case 'p':
      if (sscanf(optarg, "%hu", &port) != 1) {
        LOG_ERROR("Formato porta non valido.");
        parsing_fail_flag = true;
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

  // Abort execution if argument parsing failed
  if (parsing_fail_flag) {
    LOG_INFO("Uso: %s [-p <porta>]", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Server start
  LOG_INFO("Avvio server asta telematica sulla porta %u.", port);
  if ((socket_handle = create_udp_server("0.0.0.0", port)) > -1) {
    LOG_INFO("Creato un server UDP con handle %d.", socket_handle);
  } else {
    LOG_ERROR("Errore durante la creazione del server.");
    exit(EXIT_FAILURE);
  }

  // Interactive loop
  server_loop(socket_handle);

  // Exit
  die(socket_handle, EXIT_SUCCESS);
}
