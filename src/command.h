
#ifndef COMMAND_H
#define COMMAND_H


/* Command structure for UI and main program communication. It is general enough
 * to be used in all request/reply commands. Not all fields are used in every
 * command. Those that are not should be zeroed. */
struct cmd {
	unsigned char cmd;			/* main command */
	unsigned char mod;			/* command modifier */
	unsigned short addr1;
	unsigned short addr2;
	unsigned short dsize;
	void *data;					/* command specific data */
};


/* commands */
/* DON'T CHANGE THE NUMERICAL VALUES.
 * They are synced with monitor.asm jump table. */
#define CMD_NOP				0
#define CMD_GET_DATA		1	/* get data from HC11 */
#define CMD_SET_DATA		2	/* upload data to HC11 */
#define CMD_GET_STATE		3
#define CMD_SET_STATE		4
#define CMD_EXEC			5
#define CMD_BP				6
#define CMD_SET_CODE		7	/* assemble a file and upload to HC11 */
#define CMD_SYNTAX_ERR		8	/* syntax error */

#define CMD_CLS				203	/* clear command screen */
#define CMD_GET_CODE		204	/* disassemble code */

#define CMD_QUIT			99	/* quit program */

#define CMD_HELP			200	/* display help */

/* command modifiers */
#define CMD_DATA_B			00	/* CMD_GET_DATA: bytes */
#define CMD_DATA_W			01	/* CMD_GET_DATA: words */
#define CMD_DATA_BF			02	/* CMD_SET_DATA: block fill */
#define CMD_BP_SET			03	/* CMD_BP: set breakpoint */
#define CMD_BP_CLEAR		04	/* CMD_BP: clear breakpoint */
#define CMD_BP_LIST			05	/* CMD_BP: list breakpoints */


#endif
