/**
 * @file client_utilities.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Useful functions for "Asta Telematica" client.
 * @version 0.1
 * @date 2021-06-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Types of possible replies.
 *
 */
typedef enum {
  /**
   * @brief The server's reply is not valid.
   *
   */
  INVALID_REPLY,

  /**
   * @brief The server received an invalid command.
   *
   */
  INVALID_OFFER,

  /**
   * @brief The server accepted this offer as the best offer.
   *
   */
  ACCEPTED,

  /**
   * @brief This offer is not the best offer.
   *
   */
  NOT_ACCEPTED,
} reply_result_t;

/**
 * @brief Receives and parses a reply. Blocking. Ignores other servers' UDP
 * packets.
 *
 * @param socket_handle The socket handle.
 * @param expected_address The expected IP address.
 * @return reply_result_t Result value from the struct.
 */
reply_result_t receive_and_parse_reply(int socket_handle, char *expected_address);

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
bool make_offer(int handle, char *address, uint16_t port, double offer, char *name);
