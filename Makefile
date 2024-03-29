# compiler
CC=gcc
#flags
CLFAGS= -Wall -Wextra -pedantic

# target executable
TARGET = shellA

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
