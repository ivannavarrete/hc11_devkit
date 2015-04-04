AS11 = as11

OBJFILES = setup.s19 monitor.s19 test.s19

all: $(OBJFILES)

setup.s19: setup.asm
	@$(AS11) setup.asm

monitor.s19: monitor.asm
	@$(AS11) monitor.asm

test.s19: test.asm
	@$(AS11) test.asm
	

clean:
	rm -f *.s19
