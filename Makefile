CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wno-deprecated-declarations -g -O2
INCLUDE = -Isrc

# Source and test files
SRC = src/malloc.c
HEADER = src/malloc.h

# Test targets
TEST_SPLIT = test_split_coalesce
TEST_SAMPLE = sample_c

.PHONY: all test test-valgrind clean help test_split_coalesce sample_c

all: $(TEST_SPLIT) $(TEST_SAMPLE)

# Compile test_split_coalesce
$(TEST_SPLIT): $(SRC) tests/test_split_coalesce.c $(HEADER)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) tests/test_split_coalesce.c -o $(TEST_SPLIT)

# Compile sample_c
$(TEST_SAMPLE): tests/sample_c.c
	$(CC) $(CFLAGS) tests/sample_c.c -o $(TEST_SAMPLE)

# Run all tests
test: $(TEST_SPLIT)
	./$(TEST_SPLIT)

# Run split/coalesce test with valgrind (memory checking)
test-valgrind: $(TEST_SPLIT)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_SPLIT)

# Clean build artifacts
clean:
	rm -f $(TEST_SPLIT) $(TEST_SAMPLE) *.o

# Display help
help:
	@echo "Memory Allocator Makefile targets:"
	@echo "  make              - Compile all tests"
	@echo "  make test         - Run the split/coalesce test"
	@echo "  make test-valgrind- Run tests with memory leak detection"
	@echo "  make clean        - Remove compiled binaries"
	@echo "  make help         - Show this help message"