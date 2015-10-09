TUC		= ../../../tuc
AS		= nasm
LD		= ld
RM		= rm

ASFORMAT= elf32
ASFLAGS	= -f $(ASFORMAT)

TARGET	= functions_test

all: $(TARGET)

.SECONDARY:
%_test: %.o
	$(LD) $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.asm: %.ul $(TUC)
	$(TUC) $< $@

clean:
	$(RM) *.asm *.o $(TARGET)
