IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
include $(IMPERAS_HOME)/bin/Makefile.include
#VERBOSE=1
ifndef IMPERAS_HOME
	IMPERAS_ERROR := $(error "IMPERAS_HOME not defined")
endif

CROSS=ARM_CORTEX_$(PROCESSOR)
SRC?= $(wildcard *.c)
outputs := $(patsubst %.c,%.o,$(wildcard *.c))
elf_files:= $(patsubst %.c,%.elf,$(wildcard *.c))
lst_files:= $(patsubst %.c,%.lst,$(wildcard *.c))
SRCINC?=

$(CROSS)_LINKER_SCRIPT=-T ../Imperas_custom.ld

-include $(IMPERAS_HOME)/lib/$(IMPERAS_ARCH)/CrossCompiler/$(CROSS).makefile.include

all: clean $(outputs) $(elf_files) $(lst_files) lst

%.lst: %.elf
	$(V) echo "Objdump Application $@"
	$(V) $(IMPERAS_OBJDUMP) --insn-width=4 -j .rodata -j .text -j .data -j .bss -dz $(patsubst %.lst,%.elf,$@) > $@

%.elf: %.o
	$(V) echo "Linking Application $@"
	$(V) $(IMPERAS_LINK) -nostartfiles -g -o $@ $(patsubst %.elf,%.o,$@)

%.o: %.c
	$(V) echo "Compiling Application interrupt $@"
	$(V) $(IMPERAS_CC) -nostartfiles -w -c $(IMPERAS_CFLAGS) -D$(CROSS) -o $@ $(patsubst %.o,%.c,$@)

lst:
	$(V) echo "List Tasks in $(BIN_NAME).tsk"
	$(V) ls -1 *.lst > $(BIN_NAME).tsk

clean:
	- rm -f *.elf *.o *.lst *.tsk

print:
	lpr -p $?
	touch print
