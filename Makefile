CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wno-deprecated-declarations -g

TARGET = test_split_coalesce

all: $(TARGET)

$(TARGET): src/malloc.c tests/test_split_coalesce.c
	$(CC) $(CFLAGS) -Isrc src/malloc.c tests/test_split_coalesce.c -o $(TARGET)

clean:
	rm -f $(TARGET)