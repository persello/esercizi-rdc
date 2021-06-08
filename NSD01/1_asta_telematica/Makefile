INCLUDE_DIR = ../../lib
CFLAGS = -Wall -Wpedantic -I$(INCLUDE_DIR)
SRCS := $(wildcard *.c)

.PHONY: clean

asta_telematica: $(SRCS)
	gcc $^ -o $@ $(CFLAGS)

clean:
	rm -f asta_telematica