PROG = process_pool

CC = gcc

CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic

TARGET = $(PROG).out

all: $(TARGET)

$(TARGET): $(PROG).c
				$(CC) $(CFLAGS) $(PROG).c -o $(TARGET)

clean:
		rm -f $(PROG).out *.o

debug: $(PROG).c
			$(CC) $(CFLAGS) -g $(PROG).c -o $(TARGET)