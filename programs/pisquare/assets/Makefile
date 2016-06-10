ORIGCC   := $(CC)
AS       = arm-none-eabi-as
CPP      = arm-none-eabi-cpp
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
RPI1     = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s \
           -nostartfiles
CFLAGS   = $(RPI1)

MAKEHEADERS = makeheaders
IMGS = $(wildcard *.png) $(wildcard font/*.png)
ARCHIVE = libassets.a
HEADER = assets.h

all: $(HEADER) $(ARCHIVE) $(patsubst %.png,%.o,$(IMGS))

.PHONY: clean

$(HEADER): $(sort $(patsubst %.png,%.h,$(IMGS)))
	echo $^ > $@
	sed -i 's/ /\n/g' $@
	sed -i 's/^\(.*\)$$/#include "\1"/' $@
	sed -i '1 i\#define ASSETS_H' $@
	sed -i '1 i\#ifndef ASSETS_H' $@
	sed -i -e '$$a\#endif' $@

$(ARCHIVE): $(patsubst %.png,%.o,$(IMGS))
	$(AR) $(ARFLAGS) $@ $^

%.bin: %.png
	./imager.py -o $@ $^

%.c: %.bin
	xxd -i $^ $@

%.h: %.c $(MAKEHEADERS)
	$(RM) $@
	./$(MAKEHEADERS) $<:$@

$(MAKEHEADERS): makeheaders.c
	$(ORIGCC) -o $@ $^

clean:
	$(RM) $(HEADER) $(ARCHIVE) $(MAKEHEADERS) $(patsubst %.png,%.h,$(IMGS)) $(patsubst %.png,%.c,$(IMGS)) $(patsubst %.png,%.o,$(IMGS))
