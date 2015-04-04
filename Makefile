
MAKE = make


all:
	@cd ./src; $(MAKE) all;
	@cp ./src/devsys ./bin;
	@cp ./src/ui/ui ./bin;
	@cp ./src/hc11/monitor.s19 ./bin/hc11;
	@cp ./src/hc11/setup.s19 ./bin/hc11;

clean:
	@cd ./src; $(MAKE) clean;
	@cd ./bin; rm -f devsys ui hc11/monitor.s19 hc11/setup.s19;
