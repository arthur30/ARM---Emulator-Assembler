AS       = arm-none-eabi-as
CPP      = arm-none-eabi-cpp
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
RPI1     = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s \
           -nostartfiles
CSTD     = -std=c99
WARNINGS = -Wall -Wextra -Wformat=2 -pedantic \
           -Wfloat-equal -Wundef -Wcast-align \
           -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
           -Wstrict-overflow=5 -Wwrite-strings
ERRORS   = -Werror -pedantic-errors -Wno-error=unused-result -Wno-error=cast-align
DEFINES  = -D_DEFAULT_SOURCE
INCLUDES = -I libarmc
DEPSINFO = -MD -MP
OPTIMIZE = -O0 -DDEBUG
ifeq ($(BUILD),release)
OPTIMIZE = -O2 -D_FORTIFY_SOURCE=2 -pipe -fomit-frame-pointer
endif
ASFLAGS  = $(RPI1)
CFLAGS   = $(RPI1) $(CSTD) $(WARNINGS) $(ERRORS) $(DEFINES) $(INCLUDES) $(DEPSINFO) $(OPTIMIZE)
LDFLAGS  = $(RPI1) -Wl,-T,libarmc/rpi.x -L libarmc -L assets
LDLIBS   = -lc -larmc -lassets -lm
OBJCOPYFLAGS = -O binary
CHECKPATCH_IGNORE = VOLATILE,NEW_TYPEDEFS

BINARIES = kernel.img

.SUFFIXES: .c .o

.PHONY: all assets libarmc clean release

all: checksrc assets $(BINARIES)

kernel.elf: $(patsubst %.c,%.o,$(wildcard *.c)) libarmc/libarmc.a assets/assets.a libarmc/rpi.x
	$(CC) $(LDFLAGS) $(LOADLIBES) -o $@ $(filter %.o,$^) $(LDLIBS)

assets:
	$(MAKE) -C assets

libarmc:
	$(MAKE) -C libarmc

assets/assets.h: assets
assets/assets.a: assets

game.c: assets/assets.h
draw.c: assets/assets.h
text.c: assets/assets.h

libarmc/libarmc.a: libarmc

%.img: %.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

clean:
	$(RM) $(BINARIES) $(wildcard *.elf) $(wildcard *.o) $(wildcard *.d)
	$(MAKE) -C libarmc clean
	$(MAKE) -C assets clean

release:
	make "BUILD=release"

checksrc:
	scripts/checkpatch.pl --quiet --ignore $(CHECKPATCH_IGNORE) --no-tree -file $(wildcard *.c) $(wildcard *.h)

-include $(wildcard *.d)
