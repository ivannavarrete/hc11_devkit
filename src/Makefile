
CC = gcc
CFLAGS = -g -Wall
MAKE = make

OBJFILES = serial.o sfile.o monitor.o disasm.o ui_nc.o main.o rc.o

all: $(OBJFILES)
	gcc -lncurses $(CFLAGS) $(OBJFILES) -o devsys
	@cd ./hc11; $(MAKE) all;


clean:
	rm -f *.o devsys;
	@cd ./hc11; $(MAKE) clean;
