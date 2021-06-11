/**
 * @file ring_buffer.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Ring buffer structure.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdlib.h>

typedef struct _ring_buffer_t {
  size_t size;
  size_t head;
  size_t tail;

  char *buffer;
} ring_buffer_t;

/**
 * @brief Creates a new ring buffer.
 *
 * @param size The size of the new buffer.
 * @return ring_buffer_t* The resulting buffer.
 */
ring_buffer_t *ring_buffer_create(size_t size);

/**
 * @brief Deallocates a ring buffer.
 *
 * @param buffer The buffer to delete.
 */
void ring_buffer_delete(ring_buffer_t *buffer);

/**
 * @brief Gets the free available space on the specified buffer.
 *
 * @param buffer The buffer.
 * @return size_t The available space.
 */
size_t ring_buffer_available(ring_buffer_t *buffer);

/**
 * @brief Appends a character to the head of a ring buffer.
 *
 * @param buffer The buffer to use.
 * @param c The new character.
 * @return size_t The available space. -1 if failed.
 */
size_t ring_buffer_put(ring_buffer_t *buffer, char c);

/**
 * @brief Reads a character from the tail of the buffer.
 *
 * @param buffer The buffer.
 * @return char The character read. Returns \0 if empty.
 */
char ring_buffer_get(ring_buffer_t *buffer);

/**
 * @brief Appends a string to the head of a ring buffer, without terminator.
 *
 * @param buffer The buffer.
 * @param string The string.
 * @return size_t The available space.
 */
size_t ring_buffer_append(ring_buffer_t *buffer, char *string);

/**
 * @brief Reads from the buffer until a specified character is found, or the end
 * of the buffer. Remember that strings are not terminated. \0 won't work as
 * expected.
 *
 * @param buffer The buffer.
 * @param output The output string.
 * @param delimiter The delimiter.
 * @return size_t Number of read characters.
 */
size_t ring_buffer_read_until(ring_buffer_t *buffer, char *output, char delimiter);

/**
 * @brief Reads into the string buffer until a \n, or the end of the buffer.
 *
 * @param buffer The buffer.
 * @param output The output string.
 * @return size_t Number of read characters.
 */
size_t ring_buffer_read_line(ring_buffer_t *buffer, char *output);

/**
 * @brief Tries to read n characters from the buffer.
 *
 * @param buffer The buffer.
 * @param output The output string.
 * @param n Target number of characters to read.
 * @return size_t Number of read characters.
 */
size_t ring_buffer_read_n(ring_buffer_t *buffer, char *output, unsigned long n);
