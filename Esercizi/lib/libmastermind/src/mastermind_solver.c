/**
 * @file mastermind_solver.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief
 * @version 0.1
 * @date 2021-06-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "libmastermind/mastermind_solver.h"

#include <liblog/log.h>
#include <memory.h>
#include <stdlib.h>

mm_guess_result_t mm_compute_hypothetical_response(unsigned char *target, unsigned char *guess);

mm_game_t *mm_create_game() {
  mm_game_t *game = (mm_game_t *)calloc(1, sizeof(mm_game_t));

  for (unsigned long i = 1; i < MM_COMBINATIONS; i++) {
    unsigned char unadjusted_sum[MM_CODE_LENGTH];
    memcpy(unadjusted_sum, game->combinations[i - 1], sizeof(unsigned char) * MM_CODE_LENGTH);

    // The previous combination, but with the last digit incremented
    unadjusted_sum[MM_CODE_LENGTH - 1] += 1;

    for (int j = MM_CODE_LENGTH - 1; j >= 0; j--) {
      if (unadjusted_sum[j] >= MM_DIGITS) {
        if (j > 0) {
          unadjusted_sum[j - 1] += unadjusted_sum[j] - MM_DIGITS + 1;
        }
        unadjusted_sum[j] = 0;
      }
    }

    memcpy(game->combinations[i], unadjusted_sum, sizeof(unsigned char) * MM_CODE_LENGTH);
  }

  return game;
}

void mm_print_game(mm_game_t *game) {
  LOG_INFO("Game information: %d combinations, %lu dismissed. Combinations:", MM_COMBINATIONS, game->dismissed_count);
  for (int i = 0; i < MM_COMBINATIONS; i++) {
    for (int j = 0; j < MM_CODE_LENGTH; j++) {
      printf("%d.", game->combinations[i][j]);
    }
    printf("\t%s\n", game->dismissed[i] ? "DISMISSED" : "OK");
  }
}

unsigned long mm_make_guess(mm_game_t *game, unsigned char *guess) {

  // First guess (1122)
  if (game->dismissed_count == 0) {
    for (int i = 0; i < MM_CODE_LENGTH; i++) {
      if (i < MM_CODE_LENGTH / 2) {
        guess[i] = 1;
      } else {
        guess[i] = 2;
      }
    }
  } else {
    for (unsigned long i = 0; i < MM_COMBINATIONS; i++) {
      if (game->dismissed[i] == false) {
        memcpy(guess, game->combinations[i], sizeof(unsigned char) * MM_CODE_LENGTH);
        return MM_COMBINATIONS - game->dismissed_count;
      }
    }
  }

  return MM_COMBINATIONS - game->dismissed_count;
}

bool mm_receive_guess_result(mm_game_t *game, mm_guess_result_t result, unsigned char *guess) {
  if (result.positions == MM_CODE_LENGTH) {
    // Win!
    return true;
  }

  for (unsigned long i = 0; i < MM_COMBINATIONS; i++) {
    if (game->dismissed[i] == false) {
      mm_guess_result_t current_test_guess_result = mm_compute_hypothetical_response(guess, game->combinations[i]);

      // If it generates a different result, dismiss the combination
      if (current_test_guess_result.positions != result.positions ||
          current_test_guess_result.values != result.values) {
        game->dismissed[i] = true;
        game->dismissed_count++;
      }
    }
  }

  return false;
}

// PRIVATE FUNCTIONS

mm_guess_result_t mm_compute_hypothetical_response(unsigned char *target, unsigned char *guess) {
  mm_guess_result_t result;
  result.positions = 0;
  result.values = 0;

  LOG_INFO("Computing hr from %u%u%u%u and %u%u%u%u", target[0], target[1], target[2], target[3], guess[0], guess[1],
           guess[2], guess[3]);

  bool already_considered[MM_CODE_LENGTH] = {0};

  // First pass: compute right positions
  for (int i = 0; i < MM_CODE_LENGTH; i++) {
    if (target[i] == guess[i]) {
      result.positions++;
      already_considered[i] = true;
    }
  }

  // Second pass: ignore right positions, search for correct values
  for (int i = 0; i < MM_CODE_LENGTH; i++) {
    for (int j = 0; j < MM_CODE_LENGTH; j++) {
      if ((!already_considered[i] && !already_considered[j]) && target[j] == guess[i]) {
        result.values++;
        already_considered[j] = true;
      }
    }
  }

  LOG_INFO("Result: %d %d", result.positions, result.values);

  return result;
}
