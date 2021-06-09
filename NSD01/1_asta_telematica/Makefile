INCLUDE_DIR = ../../lib
CFLAGS = -Wall -Wpedantic -I$(INCLUDE_DIR)
CLIENT_SRCS = server.c
SERVER_SRCS = server.C

.PHONY: clean all

all: client server

client: $(CLIENT_SRCS)
	$(CC) $^ -o $@ $(CFLAGS)

server: $(SERVER_SRCS)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f client server