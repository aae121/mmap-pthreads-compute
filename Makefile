CC=gcc
CFLAGS=-Wall -Wextra -Iinclude -pthread
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TARGET=compute

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	del /Q src\*.o $(TARGET).exe 2>nul || true
