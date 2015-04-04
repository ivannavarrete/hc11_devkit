AS11 = asXX

OBJFILES = setup.s19 monitor.s19 test.s19

all: $(OBJFILES)

setup.s19: setup.asm
	@$(AS11) -lo setup.asm

monitor.s19: monitor.asm
	@$(AS11) -lo monitor.asm

test.s19: test.asm
	@$(AS11) -lo test.asm
	

clean:
	rm -f *.s19 *.lst
