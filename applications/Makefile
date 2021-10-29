IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
include $(IMPERAS_HOME)/bin/Makefile.include
#VERBOSE=1
ifndef IMPERAS_HOME
	IMPERAS_ERROR := $(error "IMPERAS_HOME not defined")
endif

CROSS=RISCV32IM
#SRC?= $(wildcard *.c)
obj_files := $(patsubst %.c,%.o,$(wildcard *.c))
elf_files := $(patsubst %.c,%.elf,$(wildcard *.c))
lst_files := $(patsubst %.c,%.lst,$(wildcard *.c))
#SRCINC?=

$(CROSS)_LINKER_SCRIPT=-T ../Imperas_custom.ld

#-include $(IMPERAS_HOME)/lib/$(IMPERAS_ARCH)/CrossCompiler/$(CROSS).makefile.include

all: clean $(obj_files) $(elf_files) $(lst_files) lst

%.lst: %.elf
	$(V) echo "Objdump Application $@"
	$(V) riscv-none-embed-objdump --insn-width=4 -j .rodata -j .text -j .data -j .bss -dz $(patsubst %.lst,%.elf,$@) > $@

%.elf: %.o
	$(V) echo "Linking Application $@"
	$(V) riscv-none-embed-gcc -nostdlib -fno-builtin -march=rv32im -mabi=ilp32 -fPIC -T "../Imperas_custom.ld" -nostartfiles -o $@ $(patsubst %.elf,%.o,$@)

%.o: %.c
	$(V) echo "Compiling Application interrupt $@"
	$(V) riscv-none-embed-gcc -nostdlib -fno-builtin -march=rv32im -mabi=ilp32 -fPIC -T "../Imperas_custom.ld" -nostartfiles -w -c -D$(CROSS) -o $@ $(patsubst %.o,%.c,$@)

lst:
	$(V) echo "List Tasks in $(BIN_NAME).tsk"
	$(V) ls -1 *.lst > $(BIN_NAME).tsk

clean:
	- rm -f *.elf *.o *.lst *.tsk

print:
	lpr -p $?
	touch print
