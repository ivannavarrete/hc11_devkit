
MAKE = make


all: txt

txt:
	@cd ./src; $(MAKE) txt;
	@cp ./src/devsys ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;

nc:
	@cd ./src; $(MAKE) nc;
	@cp ./src/devsys ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;

x:
	@cd ./src; $(MAKE) x;
	@cp ./src/devsys ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;

clean:
	@cd ./src; $(MAKE) clean;
	@cd ./bin; rm -f devsys hc11/monitor.s19 hc11/setup.s19;
