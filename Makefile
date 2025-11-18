CFLAGS         := -Wall -Wextra -Wpedantic -Werror -Wstrict-aliasing
DEBUG_FLAGS    := -g
RELEASE_FLAGS  := -O2 -DNDEBUG

MODE   ?= debug

ifeq ($(MODE),debug)
	CFLAGS += $(DEBUG_FLAGS)
	OBJ_DIR := obj/debug
else ifeq ($(MODE),release)
	CFLAGS += $(RELEASE_FLAGS)
	OBJ_DIR := obj/release
endif

SRC    := $(wildcard src/*.c)
OBJ    := $(SRC:src/%.c=$(OBJ_DIR)/%.o)
BUILD  := build/
TARGET := qb

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@printf "CC %-20s\n" $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@printf "LD %-20s\n" $@
	$(CC) $(OBJ) -o $@

run: $(TARGET)
	./$(TARGET) test.qb

clean:
	$(RM) -r $(OBJ_DIR) $(TARGET)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

help:
	@printf	"make all      : builds the exectuable to the current project\n"
	@printf	"make release  : builds the exectuable with release information\n"
	@printf	"make debug    : builds the executable with debug information\n"
	@printf	"make run      : runs the executable\n"
	@printf	"make clean    : removes the object and the executable\n"
