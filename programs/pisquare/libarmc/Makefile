AS       = arm-none-eabi-as
AR       = arm-none-eabi-ar
CPP      = arm-none-eabi-cpp
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
RPI1     = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s \
           -nostartfiles
CSTD     = -std=c99
WARNINGS = -Wall -Wformat=2
DEFINES  = -D_DEFAULT_SOURCE
DEPSINFO = -MD -MP
OPTIMIZE = -O0 -DDEBUG
ifeq ($(BUILD),release)
OPTIMIZE = -O2 -D_FORTIFY_SOURCE=2 -pipe -fomit-frame-pointer
endif
ASFLAGS  = $(RPI1)
CFLAGS   = $(RPI1) $(CSTD) $(WARNINGS) $(DEFINES) $(DEPSINFO) $(OPTIMIZE)

LIBRARY = libarmc.a

.SUFFIXES: .a .c .o

.PHONY: all clean release

all: $(LIBRARY)

$(LIBRARY): $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.S,%.o,$(wildcard *.S))
	$(AR) $(ARFLAGS) $@ $^

clean:
	$(RM) $(LIBRARY) $(wildcard *.o) $(wildcard *.d)

release:
	make "BUILD=release"

-include $(wildcard *.d)
