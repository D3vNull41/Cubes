# Determine the compiler and linker
CC     = $(shell which gcc)
CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c99 -Iinclude -I/usr/include/freetype2
LDFLAGS = -Wl,-z,relro,-z,now
LIBS = -lX11 -lXft

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BASENAME = Cubes
BINDIR = bin
OUTPUT = $(BINDIR)/$(BASENAME)

STRIP = $(shell which strip)
STRIP_FLAGS = --strip-all --remove-section=.comment --remove-section=.note # make the binary smaller

# Source and Object files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Default target
.PHONY: all
all: release

# Release build settings
release: CFLAGS += -O3
release: LDFLAGS = -s $(LIBS)
release: $(BINDIR)/$(BASENAME)

# Debug build settings
debug: CFLAGS += -g -DDEBUG
debug: LDFLAGS = $(LIBS)
debug: $(BINDIR)/$(BASENAME)

# Link the final binary
$(OUTPUT): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $(STRIP_FLAGS) $(OUTPUT)

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/*.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Run the application
.PHONY: run
run: $(BINDIR)/$(BASENAME)
	./$(BINDIR)/$(BASENAME)

# Build release target
.PHONY: build-release
build-release: clean release

# Build debug target
.PHONY: build-debug
build-debug: clean debug