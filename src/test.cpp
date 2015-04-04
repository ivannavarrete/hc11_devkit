
// This code performs extensive tests of the other parts of the program in
// order to discover errors. Run this test after modifying some part of the
// program.


#include <iostream>

#include <unistd.h>					// sleep()

#include "disasm.h"
#include "serial.h"
#include "debugger.h"

using namespace std;


int DisasmTest();
int SerialTest();
int DebuggerTest();


int main() {
	//if (DisasmTest()) cout << "disasm test failed...\n";
	//if (SerialTest()) cout << "serial test failed...\n";
	if (DebuggerTest()) cout << "debugger test failed...\n";

	return 0;
}


int DisasmTest() {
	unsigned char code[100];

	// fill code buffer with nops
	for (int i=0; i<100; i++)
		code[i] = i;

	Disasm d = Disasm(0, code, 100);

	struct dis_instr *instr = d.GetList();
	while (instr) {
		cout << instr->addr << "   " << instr->mcode << instr->instr
			 << instr->bytes << "   " << instr->cycles << endl;
		instr = instr->next;
	}

	return 0;
}


int SerialTest() {
	unsigned char buf[257];

	Serial s = Serial("/dev/ttyS1", 1200);		// create Serial object
	s.ConfigComm(1200);							// configure baud rate

	buf[0] = 0xFF;
	s.SendData(buf, 1);
	s.SendData(buf+1, 256);

	s.FlushDev();

	return 0;
}


// This test routine needs the Serial object to fully functional.
int DebuggerTest() {
	struct mcu_env env = {
		MCU_MODE_EXPANDED,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		1
	};

	Serial serial = Serial("/dev/ttyS1", 1200);
	Debugger d = Debugger(&serial, &env);

	return 0;
}
