/**
 * @file mastermind_solver.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Mastermind solver.
 * @version 0.1
 * @date 2021-06-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdbool.h>

#define MM_CODE_LENGTH 4
#define MM_DIGITS 4         // 2 <= MM_DIGITS <= 255
#define MM_COMBINATIONS 256 // MM_DIGITS^MM_CODE_LENGTH

typedef struct {
  int positions;
  int values;
} mm_guess_result_t;

typedef struct {
  unsigned char combinations[MM_COMBINATIONS][MM_CODE_LENGTH];
  bool dismissed[MM_COMBINATIONS];
  unsigned long dismissed_count;
} mm_game_t;

mm_game_t *mm_create_game(void);

void mm_print_game(mm_game_t *game);

unsigned long mm_make_guess(mm_game_t *game, unsigned char *guess);

bool mm_receive_guess_result(mm_game_t *game, mm_guess_result_t result, unsigned char *guess);
