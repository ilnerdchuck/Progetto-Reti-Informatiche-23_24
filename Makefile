# Define your compiler and flags
CC := gcc
CFLAGS := -g
CFLAGS_DEP = -MD
CFLAGS_TOT = $(CFLAGS_DEP) $(CFLAGS)

# Define build directory
BUILD_DIR := build


# If the first argument is "run"... then set everything else as arguments
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

# List your source files
SRC := $(shell find pkg -name '*.c' ! -name '*_test.c')
CMD_SRC := $(shell find cmd -name '*.c')
TEST_SRC := /tmp/test.c

# Define corresponding object files in the build directory
OBJ := $(SRC:%.c=$(BUILD_DIR)/%.o)
CMD_OBJ := $(CMD_SRC:%.c=$(BUILD_DIR)/%.o)
CMD_EXE = $(CMD_SRC:cmd/%.c=$(BUILD_DIR)/%)
#TEST_OBJ := $(BUILD_DIR)/test.o # $(TEST_SRC:/%.c=$(BUILD_DIR)/%.o)

# Default target
all: test build run

# Test target
test: $(BUILD_DIR)/test
	@$(BUILD_DIR)/test

# Rule to generate test.c and compile it into an object file
$(BUILD_DIR)/test: $(TEST_SRC) $(BUILD_DIR)/libpkg.a
	$(CC) $(CFLAGS) -o $@ $^

# Rule to run test.sh and generate test.c
# @FIXME this will trigger a re-compilation each time
$(TEST_SRC):
	@./test.sh > $(TEST_SRC)

$(BUILD_DIR)/libpkg.a: $(OBJ)
	ar rcs $@ $^

$(BUILD_DIR)/%: cmd/%.o $(BUILD_DIR)/libpkg.a
	$(CC) $(CFLAGS_TOT) $^ -o $@

build: $(CMD_EXE) # $(BUILD_DIR)/test @FIXME TEST NOT WORKING ATM

# Run target
#run: build
#	@$(BUILD_DIR)/$(RUN_ARGS)

# General rule for object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_TOT) -c $< -o $@

# Clean target to remove generated files
clean:
	@rm -rf $(BUILD_DIR) $(TEST_SRC)

.PHONY: all test build run clean $(TEST_OBJ) $(BUILD_DIR)/test

-include $(OBJ:.o=.d)
