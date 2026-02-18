# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/03 23:03:35 by dlesieur          #+#    #+#              #
#    Updated: 2026/01/04 22:54:07 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := librt.a
CC := cc
CFLAGS := -std=c99 -I.
OPTFLAGS := -O3 -march=native -ffast-math -flto -DNDEBUG -pipe -fopenmp
CFLAGS += $(OPTFLAGS)
AR := ar
ARFLAGS := rcs

# Build directory structure
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
SRCDIR := .
TESTDIR := tests

# External dependency
PNG_WRITER_LIB := ../png_writer/libpnglode.a
LDFLAGS := -lm -flto -fopenmp

# Source and object files
LIB_SRCS := $(filter-out $(SRCDIR)/main.c,$(wildcard $(SRCDIR)/*.c))
LIB_OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(LIB_SRCS:.c=.o)))
LIB_PATH := $(BUILD_DIR)/$(NAME)

# Test sources and binaries
TEST_SRCS := $(wildcard $(TESTDIR)/*.c)
TEST_BINS := $(addprefix $(BIN_DIR)/,$(notdir $(TEST_SRCS:.c=)))

# Default target
all: $(LIB_PATH)

# Create build directories
$(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Check and build PNG writer dependency if needed
$(PNG_WRITER_LIB):
	@echo "Building PNG writer library..."
	$(MAKE) -C ../png_writer all

# Compile library object files
$(OBJ_DIR)/%.o: $(SRCDIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build library archive
$(LIB_PATH): $(PNG_WRITER_LIB) $(LIB_OBJS) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@echo "Library built: $@"

# Compile test binaries: pattern rule for build/bin/%
$(BIN_DIR)/%: $(TESTDIR)/%.c $(LIB_PATH) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LIB_PATH) $(PNG_WRITER_LIB) $(LDFLAGS)

# Build all test programs
test: $(TEST_BINS)
	@echo "All tests compiled into $(BIN_DIR)/"
	@ls -1 $(BIN_DIR)/

# Clean build artifacts (keep binaries)
clean:
	@rm -rf $(OBJ_DIR)
	@echo "Cleaned object files"

# Full clean (remove entire build directory)
fclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Rebuild library
re: fclean all

# Phony targets
.PHONY: all clean fclean re test