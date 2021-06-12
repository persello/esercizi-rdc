/**
 * @file runner.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Runner data type.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once

#include <stdbool.h>

typedef struct {
  unsigned long number;
  char name[128 + 1];
} runner_t;
