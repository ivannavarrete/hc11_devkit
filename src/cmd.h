
#ifndef CMD_H
#define CMD_H


// Base command class for UI and main program communication.
class Cmd {
public:
	Cmd();						// constructor
	~Cmd();						// destructor
		
private:
	unsigned char cmd;			// main command
	unsigned char mod;			// command modifier
	unsigned short addr1;
	unsigned short addr2;
	unsigned short dsize;
	void *data;					// command specific data
};


//////////////////////////////////////////
class CmdGetCode : public Cmd {
public:
	CmdGetCode();				// constructor
	~CmdGetCode();				// destructor
	void Show();
};


//////////////////////////////////////////
class CmdSetCode : public Cmd {
public:
	CmdSetCode();				// constructor
	~CmdSetCode();				// destructor
	void Show();
};


//////////////////////////////////////////
class CmdGetData : public Cmd {
public:
	CmdGetData();				// constructor
	~CmdGetData();				// destructor
	void Show();
};


//////////////////////////////////////////
class CmdSetData : public Cmd {
public:
	CmdSetData();				// constructor
	~CmdSetData();				// destructor
	void Show();
};


//////////////////////////////////////////
class CmdCls : public Cmd {
public:
	CmdCls();					// constructor
	~CmdCls();					// destructor
	void Show();
};


//////////////////////////////////////////
class CmdHelp : public Cmd {
public:
	CmdHelp();					// constructor
	~CmdHelp();					// destructor
	void Show();
};


//////////////////////////////////////////
class CmdQuit : public Cmd {
public:
	CmdQuit();					// constructor
	~CmdQuit();					// destructor
	void Show();
};


//////////////////////////////////////////
class CmdErr : public Cmd {
public:
	CmdErr();					// constructor
	~CmdErr();					// destructor
	void Show();
};


// commands
// DON'T CHANGE THE NUMERICAL VALUES.
// They are synced with monitor.asm jump table.
#define CMD_NOP				0
#define CMD_GET_DATA		1	// get data from HC11
#define CMD_SET_DATA		2	// upload data to HC11
#define CMD_GET_STATE		3
#define CMD_SET_STATE		4
#define CMD_EXEC			5
#define CMD_BP				6
#define CMD_SET_CODE		7	// assemble a file and upload to HC11
#define CMD_SYNTAX_ERR		8	// syntax error

#define CMD_CLS				203	// clear command screen
#define CMD_GET_CODE		204	// disassemble code

#define CMD_QUIT			99	// quit program

#define CMD_HELP			200	// display help

// command modifiers
#define CMD_DATA_B			00	// CMD_GET_DATA: bytes
#define CMD_DATA_W			01	// CMD_GET_DATA: words
#define CMD_DATA_BF			02	// CMD_SET_DATA: block fill
#define CMD_BP_SET			03	// CMD_BP: set breakpoint
#define CMD_BP_CLEAR		04	// CMD_BP: clear breakpoint
#define CMD_BP_LIST			05	// CMD_BP: list breakpoints


#endif // CMD_H
