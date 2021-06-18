/**
 * @file test.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief libmastermind test.
 * @version 0.1
 * @date 2021-06-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdlib.h>

#include <log/log.h>
#include <mastermind/mastermind_solver.h>

bool ask_guess_to_user(unsigned char *guess, mm_guess_result_t *user_result);

bool ask_guess_to_user(unsigned char *guess, mm_guess_result_t *user_result) {
  LOG_INFO("My guess is %d%d%d%d.", guess[0], guess[1], guess[2], guess[3]);
  LOG_INFO("Please tell how many positions are correct and how many values are correct. Separate them with a space.");

  if ((scanf("%d %d", &(user_result->positions), &(user_result->values)) != 2)) {
    return false;
  }

  return true;
}

int main(void) {
  mm_game_t *game = mm_create_game();

  while (true) {
    unsigned char guess[MM_CODE_LENGTH];
    mm_guess_result_t result;
    mm_print_game(game);

    if (mm_make_guess(game, guess) == 0) {
      LOG_WARNING("No more guesses available. Something went wrong.");
      free(game);
      return 1;
    }
    while (!ask_guess_to_user(guess, &result)) {
      LOG_WARNING("Please use the correct format: <positions> <values>.");
    }

    if (mm_receive_guess_result(game, result, guess)) {
      LOG_INFO("Win!");
      free(game);
      return 0;
    }
  }
}
