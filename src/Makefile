CSTD     = -std=c99
WARNINGS = -Wall -Wextra -Wformat=2 -pedantic \
           -Wfloat-equal -Wundef -Wcast-align \
           -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
           -Wstrict-overflow=5 -Wwrite-strings
ERRORS   = -Werror -pedantic-errors
DEFINES  = -D_DEFAULT_SOURCE -D_POSIX_SOURCE -D_BSD_SOURCE
DEPSINFO = -MD -MP
OPTIMIZE = -g -O0 -ggdb -DDEBUG
ifeq ($(BUILD),release)
OPTIMIZE = -O2 -D_FORTIFY_SOURCE=2 -pipe -fomit-frame-pointer
endif
CFLAGS   = $(CSTD) $(WARNINGS) $(ERRORS) $(DEFINES) $(DEPSINFO) $(OPTIMIZE)

BINARIES = assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean release

all: checksrc $(BINARIES)

assemble: $(patsubst %.c,%.o,$(wildcard assemble*.c))

emulate: $(patsubst %.c,%.o,$(wildcard emulate*.c))

clean:
	$(RM) $(BINARIES) $(wildcard *.o) $(wildcard *.d)

release:
	make "BUILD=release"

checksrc:
	scripts/checkpatch.pl --quiet --no-tree -file $(wildcard *.c) $(wildcard *.h)

-include $(wildcard *.d)
