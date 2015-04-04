AS11 = as11

OBJFILES = setup.s19 monitor.s19

all: $(OBJFILES)

setup.s19: setup.asm
	@$(AS11) setup.asm

monitor.s19: monitor.asm
	@$(AS11) monitor.asm
	

clean:
	rm -f *.s19
