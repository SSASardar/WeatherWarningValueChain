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

# Run the app
run: all
	./$(TARGET)

# ------------------------------
# Testing support
# All source files except main.c
SRC_NO_MAIN := $(filter-out $(SRC_DIR)/main.c, $(SRC_FILES))

# Build and run all tests
tests: $(wildcard $(TEST_DIR)/*.c)
	@mkdir -p $(BUILD_DIR)
	@for test_src in $^; do \
		test_exe="$(BUILD_DIR)/$$(basename $$test_src .c)"; \
		echo "🔧 Building $$test_src"; \
		$(CC) $(CFLAGS) $$test_src $(SRC_NO_MAIN) -o $$test_exe || exit 1; \
		echo "✅ Running $$test_exe"; \
		./$$test_exe || exit 1; \
	done

# Run individual test by name or list available tests
test:
ifeq ($(strip $(TEST)),)
	@echo "🧪 Available tests:"
	@for f in $(wildcard $(TEST_DIR)/*.c); do \
		name=$$(basename $$f .c); \
		echo "  • $$name"; \
	done
	@echo ""
	@echo "Run a test with: make test TEST=test_name"
else
	@mkdir -p $(BUILD_DIR)
	@src_file="$(TEST_DIR)/$(TEST).c"; \
	out_file="$(BUILD_DIR)/$(TEST)"; \
	if [ ! -f "$$src_file" ]; then \
		echo "❌ Test $$src_file not found"; exit 1; \
	fi; \
	echo "🔧 Building $$src_file"; \
	$(CC) $(CFLAGS) $$src_file $(SRC_NO_MAIN) -o $$out_file && \
	echo "✅ Running $$out_file"; \
	./$$out_file
endif

# Clean everything
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET) $(BUILD_DIR)/*

.PHONY: all clean run tests test
