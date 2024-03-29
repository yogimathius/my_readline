# Makefile for a basic C program

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g3 -fsanitize=address

# Source file
SRC = my_readline.c

# Executable name
TARGET = my_readline

# Default target, which will be executed when you just type "make" without any arguments
all: $(TARGET)

# Rule to compile the C source file into an executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up the generated files
clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
