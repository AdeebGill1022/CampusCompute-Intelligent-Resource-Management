CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -D_POSIX_C_SOURCE=200809L
LDLIBS = -pthread -lrt
TARGET = campuscompute
SOURCES = main.c scheduler.c logger.c ipc.c banker.c worker.c resource_limits.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(TARGET)
