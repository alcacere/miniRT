# Minimal Makefile to build library and test programs
CC := gcc
CFLAGS := -std=c99 -I.
AR := ar
ARFLAGS := rcs
LDFLAGS := -lm

SRCDIR := .
OBJDIR := obj
TESTDIR := tests
BINDIR := bin

# Library sources: all root .c except the smoke-test main.c
LIB_SRCS := $(filter-out $(SRCDIR)/main.c,$(wildcard $(SRCDIR)/*.c))
LIB_OBJS := $(addprefix $(OBJDIR)/,$(notdir $(LIB_SRCS:.c=.o)))
LIBNAME := libpnglode.a

TEST_SRC := $(TESTDIR)/test_png.c
TEST_OBJ := $(OBJDIR)/test_png.o
TEST_BIN := $(BINDIR)/test_png

MAIN_SRC := $(TESTDIR)/main.c
MAIN_OBJ := $(OBJDIR)/main.o
MAIN_BIN := $(BINDIR)/main

.PHONY: all dirs clean re test

all: dirs $(LIBNAME) $(MAIN_BIN) $(TEST_BIN)

dirs:
	@mkdir -p $(BINDIR) $(OBJDIR)

# Build library archive from object files (no main.o)
$(LIBNAME): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Link main with the library
$(MAIN_BIN): $(MAIN_OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(LIBNAME) $(LDFLAGS)

# Link test_png with the library
$(TEST_BIN): $(TEST_OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) $(LIBNAME) $(LDFLAGS)

# Compile root sources into obj/<basename>.o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test object
$(TEST_OBJ): $(TEST_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Build main object
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

re: clean all

clean:
	-rm -rf $(OBJDIR) $(BINDIR) $(LIBNAME)
