
CC = g++
CFLAGS = -Wall
MAKE = make

OBJFILES_TXT = serial.o sfile.o debugger.o disasm.o ui_txt.o main.o rc.o lex.o breakpoint.o
OBJFILES_NC = serial.o sfile.o debugger.o disasm.o ui_nc.o main.o rc.o lex.o breakpoint.o
OBJFILES_X = serial.o sfile.o debugger.o disasm.o ui_x.o main.o rc.o lex.o breakpoint.o

all: txt

nc: $(OBJFILES_NC)
	gcc -lncurses $(CFLAGS) $(OBJFILES_NC) -o devsys
	@cd ./hc11; $(MAKE) all;

txt: $(OBJFILES_TXT)
	gcc $(CFLAGS) $(OBJFILES_TXT) -o devsys
	@cd ./hc11; $(MAKE) all;

x: $(OBJFILES_X)
	gcc $(CFLAGS) $(OBJFILES_X) -o devsys
	@cd ./hc11; $(MAKE) all;

clean:
	rm -f *.o devsys;
	@cd ./hc11; $(MAKE) clean;
