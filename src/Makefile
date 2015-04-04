
CC = gcc
CFLAGS = -O2 -Wall
MAKE = make

OBJFILES = serial.o sfile.o monitor.o main.o

all: $(OBJFILES)
	gcc $(OBJFILES) -o devsys
	@cd ./ui; $(MAKE) all;
	@cd ./hc11; $(MAKE) all;

clean:
	rm -f *.o devsys;
	@cd ./ui; $(MAKE) clean;
	@cd ./hc11; $(MAKE) clean;
