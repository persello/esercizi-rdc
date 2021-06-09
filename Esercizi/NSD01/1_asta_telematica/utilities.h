/**
 * @brief Closes the socket and then terminates the process.
 * 
 * @param socket_handle Socket handle.
 * @param status Exit status code.
 */
void die(int socket_handle, int status);

/**
 * @brief Handler for udpsocketlib.
 * 
 * @param message Error message
 */
void error_handler(const char *message);
