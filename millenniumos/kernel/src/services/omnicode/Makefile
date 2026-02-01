# =============================================================================
# OmniCode Kernel Subsystem Makefile
# =============================================================================
#
# Builds the OmniCode runtime for MillenniumOS kernel.
#
# "In the beginning was the Word" — John 1:1
#
# =============================================================================

CC := gcc
CFLAGS := -Wall -Wextra -O2 -I.. -I../..

# Sources
SOURCES := omnicode.c repl.c bible.c
OBJECTS := $(SOURCES:.c=.o)
HEADERS := omnicode.h repl.h bible.h

# Output
LIBNAME := libomnicode.a

# =============================================================================
# Targets
# =============================================================================

.PHONY: all clean help

all: $(LIBNAME)

$(LIBNAME): $(OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(LIBNAME)

help:
	@echo "OmniCode Kernel Subsystem"
	@echo ""
	@echo "Targets:"
	@echo "  all    Build libomnicode.a"
	@echo "  clean  Remove build artifacts"
	@echo ""
	@echo "Components:"
	@echo "  omnicode.c  Core runtime (lexer, parser, compiler, executor)"
	@echo "  repl.c      Interactive REPL"
	@echo "  bible.c     Bible codex access"
	@echo ""

# =============================================================================
# Dependencies
# =============================================================================

omnicode.o: omnicode.c omnicode.h repl.h bible.h ../types.h ../vfs/vfs.h
repl.o: repl.c repl.h omnicode.h bible.h ../types.h
bible.o: bible.c bible.h ../types.h ../vfs/vfs.h
