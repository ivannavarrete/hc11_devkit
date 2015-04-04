
#ifndef MESSAGE_H
#define MESSAGE_H


/* command structure for UI and main program communication. It is general enough
 * to be used in all request/reply commands. Not all fields are used in every
 * command. Those that are not should be zeroed. */
struct cmd {
	int cmd;			/* main command */
	int mod;			/* command modifier */
	int addr1;
	int addr2;
	int dsize;
	void *data;			/* command specific data */
};

/* commands: UI -> main */
//#define CMD_C_CODE_ON		0	/* change disassembly mode: machine code on */
//#define CMD_C_CODE_OFF		1	/*							machine code off */
//#define CMD_C_BYTES_ON		2	/*							num bytes on */
//#define CMD_C_BYTES_OFF		3	/*							num bytes off */
//#define CMD_C_CYCLES_ON		4	/*							exec time on */
//#define CMD_C_CYCLES_OFF	5	/*							exec time off */
#define CMD_QUIT			99	/* quit program */

/* commands: main <-> UI */
#define CMD_HELP			200	/* display help */
#define CMD_NOP				201	/* no command */
//#define CMD_DISASM_MODE		202	/* show current disassembly mode */
#define CMD_CLS				203	/* clear command screen */
#define CMD_CODE			204	/* disassemble code */
#define CMD_DATA			205 /* show data */
#define CMD_STATE			206
#define CMD_SYNTAX_ERR		208	/* syntax error */

/* command modifiers */
#define CMD_DATA_B			500
#define CMD_DATA_W			501


#endif MESSAGE_H
