
CC = gcc
CFLAGS = -O2 -Wall -g
MAKE = make

OBJFILES = serial.o sfile.o monitor.o disasm.o ui_txt.o main.o

all: $(OBJFILES)
	gcc $(OBJFILES) -o devsys
	@cd ./hc11; $(MAKE) all;


debug: $(OBJFILES)
	gcc $(CFLAGS) $(OBJFILES) -o devsys
	@cd ./hc11; $(MAKE) all;


clean:
	rm -f *.o devsys;
	@cd ./hc11; $(MAKE) clean;
