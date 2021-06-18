/**
 * @file ring_buffer.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Ring buffer implementation.
 * @version 0.1
 * @date 2021-06-11
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "ringbuffer/ring_buffer.h"

#include <stdbool.h>
#include <string.h>

/**
 * @brief Creates a new ring buffer.
 *
 * @param size The size of the new buffer.
 * @return ring_buffer_t The resulting buffer. Remember to delete it. NULL if
 * failed.
 */
ring_buffer_t *ring_buffer_create(size_t size) {
  ring_buffer_t *buf = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
  buf->buffer = (char *)malloc(size);

  if (buf->buffer == NULL) {
    return NULL;
  }

  buf->size = size;
  buf->head = 0;
  buf->tail = 0;

  return buf;
}

/**
 * @brief Deallocates a ring buffer.
 *
 * @param buffer The buffer to delete.
 */
void ring_buffer_delete(ring_buffer_t *buffer) {
  free(buffer->buffer);
  free(buffer);
}

/**
 * @brief Gets the free available space on the specified buffer.
 *
 * @param buffer The buffer.
 * @return size_t The available space.
 */
size_t ring_buffer_available(ring_buffer_t *buffer) {
  size_t occupied;

  if (buffer->head >= buffer->tail) {
    occupied = buffer->head - buffer->tail;
  } else {
    occupied = buffer->size - buffer->tail + buffer->head;
  }

  return (buffer->size) - occupied;
}

/**
 * @brief Appends a character to the head of a ring buffer.
 *
 * @param buffer The buffer to use.
 * @param c The new character.
 * @return size_t The available space. -1 if failed.
 */
size_t ring_buffer_put(ring_buffer_t *buffer, char c) {
  if (ring_buffer_available(buffer) > 0) {
    buffer->buffer[buffer->head] = c;
    if (buffer->head < buffer->size - 1) {
      (buffer->head)++;
    } else {
      (buffer->head) = 0;
    }
    return ring_buffer_available(buffer);
  } else {
    return (size_t)-1;
  }
}

/**
 * @brief Reads a character from the tail of the buffer.
 *
 * @param buffer The buffer.
 * @return char The character read. Returns \0 if empty.
 */
char ring_buffer_get(ring_buffer_t *buffer) {
  if (ring_buffer_available(buffer) < buffer->size) {
    char c = buffer->buffer[buffer->tail];
    if (buffer->tail < buffer->size - 1) {
      (buffer->tail)++;
    } else {
      (buffer->tail) = 0;
    }
    return c;
  } else {
    return 0;
  }
}

/**
 * @brief Appends a string to the head of a ring buffer, without terminator.
 *
 * @param buffer The buffer.
 * @param string The string.
 * @return size_t The available space. -1 on failure due to buffer full.
 */
size_t ring_buffer_append(ring_buffer_t *buffer, char *string) {
  for (size_t i = 0; i <= strlen(string) - 1; i++) {
    if (ring_buffer_put(buffer, string[i]) == (size_t)-1) {
      return (size_t)-1;
    }
  }

  return ring_buffer_available(buffer);
}

/**
 * @brief Reads from the buffer until a specified character is found, or the end
 * of the buffer. Remember that strings are not terminated. \0 won't work as
 * expected.
 *
 * @param buffer The buffer.
 * @param output The output string pointer. Automatically allocated. Remember to
 * free it. Set it to NULL if not allocated previously.
 * @param delimiter The delimiter.
 * @return size_t Number of read characters.
 */
size_t ring_buffer_read_until(ring_buffer_t *buffer, char **output,
                              char delimiter) {
  char c;
  size_t size = 0;
  do {
    c = ring_buffer_get(buffer);
    char *new = (char *)realloc(*output, (size + 2) * sizeof(char));

    if (new == NULL) {
      return size + 1;
    } else {
      *output = new;
    }

    *(*output + size) = c;
    size++;
  } while (c != delimiter && c != 0);

  *(*output + size) = '\0';

  return size;
}

/**
 * @brief Reads into the string buffer until a \n, or the end of the buffer.
 *
 * @param buffer The buffer.
 * @param output The output string. Automatically allocated. Remember to free
 * it. Set it to NULL if not allocated previously.
 * @return size_t Number of read characters. -1 if a complete line couldn't be
 * found.
 */
size_t ring_buffer_read_line(ring_buffer_t *buffer, char **output) {
  if (buffer->head >= buffer->tail) {
    for (size_t i = buffer->tail; i < buffer->head; i++) {
      if (buffer->buffer[i] == '\n') {
        return ring_buffer_read_until(buffer, output, '\n');
      }
    }
  } else {
    for (size_t i = buffer->tail; i < (buffer->size) - 1; i++) {
      if (buffer->buffer[i] == '\n') {
        return ring_buffer_read_until(buffer, output, '\n');
      }
    }

    for (size_t i = 0; i < buffer->head; i++) {
      if (buffer->buffer[i] == '\n') {
        return ring_buffer_read_until((buffer), output, '\n');
      }
    }
  }

  return (size_t)-1;
}
