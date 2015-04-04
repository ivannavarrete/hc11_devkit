AS11 = asXX

OBJFILES = setup.s19 debugger.s19 test.s19

all: $(OBJFILES)

setup.s19: setup.asm
	@$(AS11) -lo setup.asm

debugger.s19: debugger.asm
	@$(AS11) -lo debugger.asm

test.s19: test.asm
	@$(AS11) -lo test.asm
	

clean:
	rm -f *.s19 *.lst
