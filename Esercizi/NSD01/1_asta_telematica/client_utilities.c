/**
 * @file client_utilities.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Useful functions for "Asta Telematica" client.
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "client_utilities.h"
#include "constants.h"

#include <log/log.h>
#include <udpsocketlib.h>

/**
 * @brief Receives and parses a reply. Blocking. Ignores other servers' UDP
 * packets.
 *
 * @param socket_handle The socket handle.
 * @param expected_address The expected IP address.
 * @return reply_result_t Result value from the struct.
 */
reply_result_t receive_and_parse_reply(int socket_handle, char *expected_address) {
  char buffer[BUFSIZ + 1];
  char server_addr[128];
  int port;

  bool correct_server = false;
  reply_result_t result = INVALID_REPLY;

  // Loop until we receive something from the CORRECT server
  do {
    udp_receive_and_get_sender_info(socket_handle, buffer, server_addr, &port);

    // Check address correctness
    if (strcmp(server_addr, expected_address) == 0) {
      correct_server = true;

      // Parse reply
      if (strcmp(buffer, OFFER_ACCEPTED_MSG) == 0) {
        result = ACCEPTED;
      } else if (strcmp(buffer, OFFER_NOT_ACCEPTED_MSG) == 0) {
        result = NOT_ACCEPTED;
      } else if (strcmp(buffer, OFFER_INVALID_MSG) == 0) {
        result = INVALID_OFFER;
      } else {
        result = INVALID_REPLY;
      }
    }

  } while (!correct_server);

  return result;
}

/**
 * @brief Sends an offer to a server.
 *
 * @param handle The client handle.
 * @param address The server address.
 * @param port The server's port.
 * @param offer Offer value.
 * @param name Offerer name.
 * @return true If the offer is correctly sent.
 * @return false Otherwise.
 */
bool make_offer(int handle, char *address, uint16_t port, double offer, char *name) {

  char *command;

  int result;
  asprintf(&command, "OFFERTA NOME(%s) VALORE(%.2lf)", name, offer);

  result = udp_send(handle, command, address, port);
  free(command);
  return result;
}
