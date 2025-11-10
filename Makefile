CC=gcc
CFLAGS=-Wall -Wextra -g -Iinclude -pthread

SRC=src/main.c \
    src/sequential_compute.c \
    src/pipes_compute.c \
    src/mmap_compute.c \
    src/threads_compute.c \
    src/utils.c

OBJ=$(SRC:.c=.o)
TARGET=compute

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
