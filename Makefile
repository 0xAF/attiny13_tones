# --
# Copyright (c) 2015, Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
# --

# make MELODY=MELODY_1 && make flash
# make MELODY=MELODY_2 && make flash
# make TARGET=two && make TARGET=two flash

MCU=attiny13
FUSE_L=0x6A
FUSE_H=0xFF
F_CPU=1200000
CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.

#TARGET=two
TARGET=main
MELODY=MELODY_2


help:
	@echo "use one of:"
	@echo "make compile MELODY=MELODY_1 && make flash"
	@echo "make compile MELODY=MELODY_2 && make flash"
	@echo "make compile TARGET=two      && make TARGET=two flash"
	@echo "make compile TARGET=three    && make TARGET=three flash"

compile: clean ${TARGET}.hex

${TARGET}.hex:
	${CC} ${CFLAGS} -o ${TARGET}.o ${TARGET}.c -D${MELODY}
	${LD} -o ${TARGET}.elf ${TARGET}.o
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex
	${SIZE} -C --mcu=${MCU} ${TARGET}.elf


flash:
	${AVRDUDE} -p ${MCU} -c usbasp -B10 -U flash:w:${TARGET}.hex:i -F -P usb

fuse:
	$(AVRDUDE) -p ${MCU} -c usbasp -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m
	#$(AVRDUDE) -p ${MCU} -c usbasp -B10 -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m

clean:
	rm -f *.c~ *.o *.elf *.hex

