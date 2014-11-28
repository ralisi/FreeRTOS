CC=e-gcc
CFLAGS=-I.
FREERTOSSRC=../../Source
INCLUDES= -g -I$(FREERTOSSRC)/include -I$(FREERTOSSRC)/portable/GCC/Epiphany -I.
DEPS = $(FREERTOSSRC)/portable/GCC/Epiphany/portmacro.h Makefile FreeRTOSConfig.h c2c.h


ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=internal.ldf
EHDF=${EPIPHANY_HDF}

vpath %.c .:$(FREERTOSSRC)/:$(FREERTOSSRC)/portable/MemMang:$(FREERTOSSRC)/portable/GCC/Epiphany
vpath %.s $(FREERTOSSRC)/portable/GCC/Epiphany




run: armcode main.srec worker.srec
	sh -c "time sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./armcode"

%.srec: %.elf
	e-objcopy --srec-forceS3 --output-target srec $< $@

%.elf: $(ELDF) tasks.o queue.o list.o port.o portasm.o heap_1.o c2c.o %.o
	$(CC) -g -T$< -Wl,--gc-sections -o $@ $(filter-out $<,$^) -le-lib

armcode: armcode.c $(DEPS)
	gcc $< -o $@  -I ${EINCS} -L ${ELIBS} -le-hal

clean:
	rm -f *.o *.srec *.elf

.SECONDARY:
%.o: %.c  $(DEPS)
	$(CC) -fdata-sections -ffunction-sections -c -o $@ $< $(INCLUDES)

%.o: %.s $(DEPS)
	$(CC) -c -o $@ $< $(INCLUDES)
