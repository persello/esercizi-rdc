#pragma once
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "color.h"

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_ERROR(M, ...) fprintf(stderr, RED "[ERROR]" COLOR_X " (%s:%d:%s: errno: %s) " M "\n", __FILENAME__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)
#define LOG_WARNING(M, ...) fprintf(stderr, YELLOW "[WARN]" COLOR_X " (%s:%d:%s: errno: %s) " M "\n", __FILENAME__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)
#define LOG_INFO(M, ...) fprintf(stderr, WHITE "[INFO]" COLOR_X " (%s:%d:%s) " M "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
