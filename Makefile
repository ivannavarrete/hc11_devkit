
MAKE = make


all:
	@cd ./src; $(MAKE) all;
	@cp ./src/devsys ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;


debug:
	@cd ./src; $(MAKE) debug;
	@cp ./src/devsys ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;
	

clean:
	@cd ./src; $(MAKE) clean;
	@cd ./bin; rm -f devsys hc11/monitor.s19 hc11/setup.s19;
