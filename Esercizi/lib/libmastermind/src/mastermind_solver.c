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

mm_guess_result_t mm_compute_hypothetical_result(unsigned char *target, unsigned char *guess);

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
  // for (int i = 0; i < MM_COMBINATIONS; i++) {
  //   for (int j = 0; j < MM_CODE_LENGTH; j++) {
  //     printf("%d.", game->combinations[i][j]);
  //   }
  //   printf("\t%s\n", game->dismissed[i] ? "DISMISSED" : "OK");
  // }
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
    unsigned long minimax = MM_COMBINATIONS;
    unsigned char best_guess[MM_CODE_LENGTH];

    for (unsigned long i = 0; i < MM_COMBINATIONS; i++) {
      if (game->dismissed[i] == false) {
        unsigned long max_frequency = 0;
        unsigned long minimax_map[MM_DIGITS][MM_DIGITS];
        memset(minimax_map, 0, sizeof(unsigned long) * MM_DIGITS * MM_DIGITS);

        for (unsigned long j = 0; j < MM_COMBINATIONS; j++) {
          if (game->dismissed[j] == false) {
            mm_guess_result_t result = mm_compute_hypothetical_result(game->combinations[i], game->combinations[j]);
            minimax_map[result.positions][result.values]++;
          }
        }

        LOG_INFO("Minimax map:\n\n");
        for (int i = 0; i < MM_DIGITS; i++) {
          for (int j = 0; j < MM_DIGITS; j++) {
            printf("%lu\t", minimax_map[i][j]);
            max_frequency = minimax_map[i][j] > max_frequency ? minimax_map[i][j] : max_frequency;
          }
          printf("\n");
        }

        if (max_frequency < minimax) {
          minimax = max_frequency;
          memcpy(best_guess, game->combinations[i], sizeof(unsigned char) * MM_CODE_LENGTH);
        }
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
      mm_guess_result_t current_test_guess_result = mm_compute_hypothetical_result(guess, game->combinations[i]);

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

mm_guess_result_t mm_compute_hypothetical_result(unsigned char *target, unsigned char *guess) {
  mm_guess_result_t result;
  result.positions = 0;
  result.values = 0;

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

  return result;
}
