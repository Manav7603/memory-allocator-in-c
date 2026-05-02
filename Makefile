CC = gcc
# Added -fvisibility=hidden to ensure our internal functions stay completely secure
CFLAGS = -Wall -Wextra -Werror -Wno-deprecated-declarations -g -O2 -fvisibility=hidden
# Look for headers in both the public 'include' folder and internal 'src' folder
INCLUDE = -Iinclude -Isrc

# All our new modular source files
SRC = src/malloc.c \
      src/block/block.c \
      src/arena/arena.c \
      src/strategy/strategy.c

# Test binaries
TEST_SPLIT = test_split_coalesce
TEST_SAMPLE = sample_c

.PHONY: all test test-valgrind clean help

all: $(TEST_SPLIT) $(TEST_SAMPLE)

# Compile the core allocator test
$(TEST_SPLIT): $(SRC) tests/test_split_coalesce.c
	@echo "🛠️ Compiling memory allocator and test suite..."
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) tests/test_split_coalesce.c -o $(TEST_SPLIT)

# Compile a sample user program
$(TEST_SAMPLE): tests/sample_c.c
	@echo "🛠️ Compiling sample user program..."
	$(CC) $(CFLAGS) $(INCLUDE) tests/sample_c.c -o $(TEST_SAMPLE)

# Run the split/coalesce test
test: $(TEST_SPLIT)
	@echo "🚀 Running Test Suite..."
	./$(TEST_SPLIT)

# Run tests with memory leak detection (Valgrind)
test-valgrind: $(TEST_SPLIT)
	@echo "🔍 Running Valgrind Memory Checks..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_SPLIT)

# Run tests using Apple/Google's modern AddressSanitizer (Best for macOS)
test-asan:
	@echo "🩻 Running AddressSanitizer Memory Checks..."
	$(CC) $(CFLAGS) -fsanitize=address $(INCLUDE) $(SRC) tests/test_split_coalesce.c -o test_split_coalesce_asan
	./test_split_coalesce_asan

# Clean build artifacts
clean:
	@echo "🧹 Cleaning up..."
	rm -f $(TEST_SPLIT) $(TEST_SAMPLE) *.o
	rm -rf *.dSYM

# Display help
help:
	@echo "Memory Allocator Makefile targets:"
	@echo "  make              - Compile all tests"
	@echo "  make test         - Run the split/coalesce test"
	@echo "  make test-valgrind- Run tests with memory leak detection"
	@echo "  make clean        - Remove compiled binaries"
	@echo "  make help         - Show this help message"