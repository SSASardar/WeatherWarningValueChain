# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Source files and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target executable
TARGET = rain

# Default target
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

# Compile each .c file in src/ to build/*.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Optional: build all tests
tests: $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/test_%.o,$(wildcard $(TEST_DIR)/*.c))
	@for test_src in $(wildcard $(TEST_DIR)/*.c); do \
		test_exe="$(BUILD_DIR)/$$(basename $$test_src .c)"; \
		$(CC) $(CFLAGS) $$test_src -o $$test_exe; \
		echo "Built test: $$test_exe"; \
	done

# Run the app
run: all
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET) $(BUILD_DIR)/test_* 

.PHONY: all clean run tests

