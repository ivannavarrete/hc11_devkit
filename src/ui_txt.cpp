
//
// ui_txt.cpp
//
// The UI_txt object handles the communication between the user and the PC.
// This is a text mode interface but it can be replaced with anything else.
//
// Only main.cpp should use this object. (??? ShowMsg is used in all objects
// for random UI output, for example when notifying user of program errors. ???)
//



#include <cstdio>			// XXX printf ...
#include <cctype>			// XXX tolower ...
#include <cstring>			// XXX memset ...
/*
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdlib.h>
*/

#include <iostream>

#include "debugger.h"		// XXX not converted
#include "disasm.h"
//#include "command.h"
//#include "breakpoint.h"
#include "lex.h"			// XXX not converted
#include "ui_txt.h"

using namespace std;


// Constructor.
UI_txt::UI_txt() {
	// XXX this should be found out at runtime
	rows = 30;
	cols = 80;

	// clear screen
	for (int i=0; i<rows; i++)
		cout << ":\n";
}


// Destructor. Not much to do in text mode.
UI_txt::~UI_txt() {
}


// Nothing here either. Should be different with other UI classes.
// Maybe I should support dynamic screen resizing somehow ?
int UI_txt::Config(void) {
	return 0;
}


// Return a command object of proper class. The caller is responsible for
// destroying the object.
Cmd *UI_txt::GetCmd() {
	char cmdbuf[81];
//	char *cmdptr = cmdbuf;
//	struct token t;
	int i;

	cmdbuf[80] = 0;

	// get command and convert to lower case
	cout << ":";
	//fgets(cmdbuf, 80, stdin);
	for (i=0; i<80; i++) {
		cin.get(cmdbuf[i]);
		if (cmdbuf[i] == '\n')
			break;
	}
	for (i=0; i<strlen(cmdbuf); i++)
		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);
	
	// fill command structure
	// first get the command ...
//	cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//	if (t.token == TOKEN_COMMAND) {
//		cmd->cmd = t.attr;
//		cmd->mod = t.attr2;
//	} else
//		cmd->cmd = CMD_SYNTAX_ERR;

//	// ... then get command parameters where necessary
//	switch (cmd->cmd) {
//		case CMD_GET_DATA:
//		case CMD_GET_CODE:
//		case CMD_EXEC:
//			cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//			cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 1);
//			break;
//		case CMD_SET_CODE:
//			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);	// get file name
//			if (cmdptr) {
//				cmd->dsize = strlen(t.lex) + 1;
//				cmd->data = malloc(cmd->dsize);
//				if (!cmd->data) {
//					perror(": GetCommand(): ");
//					return -1;
//				}
//				strcpy(cmd->data, t.lex);
//			} else
//				cmd->cmd = CMD_SYNTAX_ERR;

//			break;
//		case CMD_SET_DATA:
//			if (!strcmp(t.lex, "bf")) {
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//				cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 0);
//
//				cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//				if (t.token == TOKEN_NUM && t.attr < 256) {
//					cmd->dsize = cmd->addr2 - cmd->addr1 + 1;
//					cmd->data = malloc(cmd->dsize);
//					if (!cmd->data) {
//						perror(": GetCommand(): ");
//						return -1;
//					}

//					memset(cmd->data, t.attr, cmd->dsize);
//				} else
//					cmd->cmd = CMD_SYNTAX_ERR;
//			}

//			break;
//		case CMD_BP:
//			if (cmd->mod == CMD_BP_SET || cmd->mod == CMD_BP_CLEAR)
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//			else
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 1);

//			break;
//		case CMD_HELP:
//			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//			if (cmdptr && t.token == TOKEN_COMMAND)
//				cmd->mod = t.attr;
//			else if (cmdptr)
//				cmd->cmd = CMD_SYNTAX_ERR;

//			break;
//		case CMD_SYNTAX_ERR:
//		default:
//			break;			// only commands with no params end up here
//	}

	return 0;
}


//Cmd *UI_txt::GetCommand() {
//	char cmdbuf[81];
//	char *cmdptr = cmdbuf;
//	struct token t;
//	int i;
//
//	cmdbuf[80] = 0;
//
//	// get command and convert to lower case
//	cout << ":";
//	fgets(cmdbuf, 80, stdin);
//	for (i=0; i<strlen(cmdbuf); i++)
//		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);
//	
//	// fill command structure
//	// first get the command ...
//	cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//	if (t.token == TOKEN_COMMAND) {
//		cmd->cmd = t.attr;
//		cmd->mod = t.attr2;
//	} else
//		cmd->cmd = CMD_SYNTAX_ERR;
//
//	// ... then get command parameters where necessary
//	switch (cmd->cmd) {
//		case CMD_GET_DATA:
//		case CMD_GET_CODE:
//		case CMD_EXEC:
//			cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//			cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 1);
//			break;
//		case CMD_SET_CODE:
//			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);	// get file name
//			if (cmdptr) {
//				cmd->dsize = strlen(t.lex) + 1;
//				cmd->data = malloc(cmd->dsize);
//				if (!cmd->data) {
//					perror(": GetCommand(): ");
//					return -1;
//				}
//				strcpy(cmd->data, t.lex);
//			} else
//				cmd->cmd = CMD_SYNTAX_ERR;
//
//			break;
//		case CMD_SET_DATA:
//			if (!strcmp(t.lex, "bf")) {
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//				cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 0);
//
//				cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//				if (t.token == TOKEN_NUM && t.attr < 256) {
//					cmd->dsize = cmd->addr2 - cmd->addr1 + 1;
//					cmd->data = malloc(cmd->dsize);
//					if (!cmd->data) {
//						perror(": GetCommand(): ");
//						return -1;
//					}
//
//					memset(cmd->data, t.attr, cmd->dsize);
//				} else
//					cmd->cmd = CMD_SYNTAX_ERR;
//			}
//
//			break;
//		case CMD_BP:
//			if (cmd->mod == CMD_BP_SET || cmd->mod == CMD_BP_CLEAR)
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
//			else
//				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 1);
//			
//			break;
//		case CMD_HELP:
//			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
//			if (cmdptr && t.token == TOKEN_COMMAND)
//				cmd->mod = t.attr;
//			else if (cmdptr)
//				cmd->cmd = CMD_SYNTAX_ERR;
//				
//			break;
//		case CMD_SYNTAX_ERR:
//		default:
//			break;			// only commands with no params end up here
//	}
//
//	return 0;
//}


// XXX //
// Display disassembly output.
//void UI_txt::ShowCmd(CmdGetCode &cmd) {
//	struct dis_instr *da_list = (struct dis_instr *)scmd->data;
//	int i, j=2;
//
//	if (da_list != NULL)
//		cout << "addr | machine code     |    mnemonic           |          byt"
//				"es | cycles\n"
//				"-----|------------------|-----------------------|------------"
//				"----|---------\n";
//
//	while (da_list != NULL) {
//		printf("%04X:  ", da_list->addr);			// address
//	
//		printf("%s", da_list->mcode);				// machine code
//		for (i=strlen(da_list->mcode); i<22; i++)
//			cout << " ";
//
//		printf("%s", da_list->instr);				// mnemonic
//		for (i=strlen(da_list->instr); i<30; i++)
//			cout << " ";
//
//		printf("%02d", da_list->bytes);				// instruction size
//		for (i=2; i<8; i++)
//			cout << " ";
//
//		printf("%02d\n", da_list->cycles);			// instruction time
//
//		if (++j == ROWS-1) {
//			printf("press [enter] key to continue");
//			getchar();
//			j = 0;
//		}
//
//		da_list = da_list->next;
//	}
//}


// XXX //
// Show 'set code' command.
void UI_txt::ShowCmd(CmdSetCode &cmd) {
}


// XXX //
// Show 'get data' command
//void UI_txt::ShowCmd(CmdGetData &cmd) {
//	unsigned char *data = (unsigned char *)scmd->data;
//	int r, c;
//
//	for (r=0; r<(scmd->dsize)/0x10; r++)  {
//		// address
//		printf(": %04X:  ", (scmd->addr1+r*0x10) & 0xFFFF);
//
//		// hex data
//		if (scmd->mod == CMD_DATA_B) {
//			for (c=0; c<0x10; c++)
//				printf("%02X %s", data[r*0x10+c], (c==7) ? " ":"");
//		} else {
//			for (c=0; c<0x10; c+=2)
//				printf("%02X%02X  %s", data[r*0x10+c], data[r*0x10+c+1],
//						(c==6) ? " ":"");
//		}
//		cout << "  ";
//
//		// ascii data
//		for (c=0; c<0x10; c++)
//			printf("%c", (data[r*0x10+c]>=0x20 && data[r*0x10+c]<='Z') ?
//					data[r*0x10+c] : '.');
//		cout << endl;
//
//		if (r%(ROWS-2) == 0 && r != 0) {
//			cout << "press [enter] key to continue");
//			getchar();
//		}
//	}
//
//	// This if statement prints out the last incomplete line (if any).
//	// NOTE: Should incorporate it in the above loop instead.
//	if ((scmd->dsize)%0x10) {
//		// address
//		printf(": %04X:  ", (scmd->addr1+r*0x10) & 0xFFFF);
//
//		// hex data
//		if (scmd->mod == CMD_DATA_B) {
//			for (c=0; c<(scmd->dsize)%0x10; c++)
//				printf("%02X %s", data[r*0x10+c], (c==7) ? " ":"");
//		} else {
//			for (c=0; c<(scmd->dsize)%0x10; c++)
//				printf("%02X%02X  %s", data[r*0x10+c], data[r*0x10+c+1],
//						(c==6) ? " ":"");
//		}
//
//		// ascii data
//		for (c=0; c<(0x10-(scmd->dsize)%0x10+1)*3; c++)
//			cout << " ";
//
//		for (c=0; c<(scmd->dsize)%0x10; c++)
//			printf("%c", (data[r*0x10+c]>=0x20 && data[r*0x10+c]<='Z') ?
//					data[r*0x10+c] : '.');
//		cout << endl;
//	}
//}


// XXX //
// Show 'set data' command.
//void UI_txt::ShowCmd(CmdSetData &cmd) {
//}


// Clear command screen.
//void UI_txt::ShowCmd(CmdCls &cmd) {
//	for (int i=0; i<rows; i++)
//		cout << endl;
//}


// Display help.
//void UI_txt::ShowCmd(CmdHelp &cmd) {
//	switch (scmd->mod) {
//		case CMD_GET_CODE:
//			cout << ":\n:\tc <saddr> [eaddr]\n"
//				": Disassemble a piece of code with start address saddr and\n"
//				": end address eaddr. saddr is mandatory, eaddr is optional\n";
//			break;
//		case CMD_GET_DATA:
//			cout << ":\n:\tdb <saddr> [eaddr]\n:\tdw <saddr> [eaddr]\n"
//				": Dump a data reqion starting at saddr and ending at eaddr.\n"
//				": saddr is mandatory, eaddr is optional. The data can be\n"
//				": is displayd either as bytes or words.\n";
//			break;
//		case CMD_DATA_BF:
//			cout << ":\n:\tbf <saddr> <eaddr> <val>\n"
//				": Block fill a memory area with start address saddr and end\n"
//				": address eaddr, with the value val.\n";
//			break;
//		case CMD_EXEC:
//			cout << ":\n:\tg <saddr>\n"
//				": Execute code starting at address saddr.\n";
//			break;
//		case CMD_BP:
//			cout << ":\n:\tbp <addr>\n"
//				": Set a breakpoint at addr.\n";
//		case CMD_CLS:
//			cout << ":\n:\tcls\n: Clear display.\n";
//			break;
//		case CMD_HELP:
//			cout << ":\n:\thelp [command]\n:\th [command]\n"
//				": Display general help or help on a specific command.\n";
//			break;
//		case CMD_QUIT:
//			cout << ":\n:\tquit\n:\tq\n: Terminate program\n";
//			break;
//		case CMD_SYNTAX_ERR:
//			cout << ": no such command\n";
//			break;
//		default:
//			cout << ":\n: commands:\n"
//					":\tdb <saddr> [eaddr]             show data in bytes\n"
//					":\tdw <saddr> [eaddr]             show data in words\n"
//					":\tbf <saddr> <eaddr> <val>       block fill memory area\n"
//					":\tc <saddr> [eaddr]              disassemble code\n"
//					":\tg <saddr>                      execute code\n"
//					":\tbp <addr>                      set a breakpoint\n"
//					":\tcls                            clear display\n"
//					":\thelp [command]                 show help on commands\n"
//					":\tquit                           exit program\n";
//	}
//}


// XXX //
//void UI_txt::ShowCmd(CmdQuit &cmd) {
//}


// XXX //
// Show error message.
//void UI_txt::ShowCmd(CmdErr &cmd) {
//	cout << ": syntax error\n";
//}


// XXX This routine is to FAT! //
// Get the MCU environment options from user and fill the env struct.
int UI_txt::GetEnvOpt(struct mcu_env *env) {
	char buf[10];
	char c;
	
	cout << "*** environment options ***\n";

	do {
		cout << "    MCU mode [s/e/b/t] [";
		cout << (env->mode==MCU_MODE_BOOTSTRAP ? "b" :
				(env->mode==MCU_MODE_TEST ? "t" :
				(env->mode==MCU_MODE_EXPANDED ? "e" : "s"))) << "]: ";

		fgets(buf, 3, stdin);
		if (buf[0]=='\n') break;
		if (buf[1]!=0xA) continue;
		switch (buf[0]) {
			case 's': env->mode = MCU_MODE_SINGLECHIP; break;
			case 'e': env->mode = MCU_MODE_EXPANDED; break;
			case 'b': env->mode = MCU_MODE_BOOTSTRAP; break;
			case 't': env->mode = MCU_MODE_TEST; break;
			default: continue;
		}
		break;
	} while (1);
	
	//do {
		cout << "    talker page [0-F] [" << hex << (unsigned int)(env->talker_page);
		cout << "]: DOESN'T WORK!\n";
	//	fgets(buf, 3, stdin);
	//	c = GetHex(buf[0]);
	//	if (buf[0]=='\n') break;
	//	else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
	//		env->talker_page = c;
	//		break;
	//	}
	//} while (1);
	
	do {
		cout << "    RAM page [0-F] [" << hex << env->ram_page << "]: ";
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
			env->ram_page = c;
			break;
		}
	} while (1);
	
	do {
		cout << "    register page [0-F] [";
		cout << hex << (int)env->reg_page << "]: ";

		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((buf[0]>='0' && buf[0]<='F') && (buf[0]<='9' || buf[0]>='A'))
				&& buf[1]==0xA) {
			env->reg_page = c;
			break;
		}
	} while (1);
	
	do {
		cout << "    on-chip EEPROM [on/off] [";
		cout << (env->on_chip_eeprom ? "on" : "off") << "]: ";
		
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			env->on_chip_eeprom = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			env->on_chip_eeprom = 0;
			break;
		}
	} while (1);

	do {
		cout << "    on-chip ROM [on/off] [";
		cout << (env->on_chip_rom ? "on" : "off") << "]: ";
		
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			env->on_chip_rom = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			env->on_chip_rom = 0;
			break;
		}
	} while (1);

	do {
		cout << "    irq mode [edge/level] [";
		cout << (env->irq_mode ? "edge" : "level") << "]: ";
		
		fgets(buf, 7, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "edge\n")) {
			env->irq_mode = 1;
			break;
		}
		else if (!strcmp(buf, "level\n")) {
			env->irq_mode = 0;
			break;
		}
	} while (1);

	
	do {
		cout << "    osc delay [on/off] [";
		cout << (env->osc_delay ? "on" : "off") << "]: ";
		
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			env->osc_delay = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			env->osc_delay = 0;
			break;
		}
	} while (1);

	cout << "    timer prescaler rate [1/4/8/16] []: NOT IMPLEMENTED\n";
	cout << "    COP prescaler rate [1/4/16/64] []: NOT IMPLEMENTED\n\n";
	
	return 0;
}


// XXX //
//int UI_txt::ShowState(struct cmd *scmd) {
//	cout << ": AccA: \n";
//	cout << ": AccB: \n";
//	cout << ": X: \n";
//	cout << ": Y: \n";
//	cout << ": SP: \n";
//	cout << ": PC: \n";
//	cout << ": flags: \n";
//
//	return 0;
//}


// XXX //
//int UI_txt::ShowBP(struct cmd *scmd) {
//	struct breakpoint *bp = (struct breakpoint *)scmd->data;
//
//	printf(": %u: %04u\n", bp->num, bp->addr);
//
//	return 0;
//}


// XXX //
// Show current disassembly mode.
//void UI_txt::ShowDisasmMode(struct cmd *cmd) {
//	printf("code %s, bytes %s, cycles %s\n",
//			(*(int *)(cmd->data) & DMODE_CODE) ? "on" : "off",
//			(*(int *)(cmd->data) & DMODE_BYTES) ? "on" : "off",
//			(*(int *)(cmd->data) & DMODE_CYCLES) ? "on" : "off");
//}


// Show a message. This method is used by all other objects for general UI
// output.
// XXX make this function able to take unknown number of parameters (strings)
void UI_txt::ShowMsg(const char *msg) {
	cout << ":" << msg << flush;
}


// XXX //
// Internal routine for use in GetCommand(). Note that t gets changed, so
// don't call this if t is still needed in GetCommand(). (Or make another
// variable for this routine.)
// This routine returns a number on success. On failure it returns 0 AND sets
// cmd->cmd to CMD_SYNTAX_ERR.
/*
int UI_txt::GetAddr(char **str, struct token *t, struct cmd *cmd, int opt) {
	*str = Token(*str, t, TOKEN_COMMAND);

	if (opt) {								// optional
		if (*str && t->token == TOKEN_NUM)
			return t->attr;
		else if (*str)
			cmd->cmd = CMD_SYNTAX_ERR;
		else
			return 0;
	} else {								// mandatory
		if (t->token == TOKEN_NUM)
			return t->attr;
		else
			cmd->cmd = CMD_SYNTAX_ERR;
	}

	return 0;
}
*/


// XXX //
// Internal routine. Convert ASCII hex number ('0'-'F') to hex number
// (0x0-0xF).
char UI_txt::GetHex(char c) {
	c -= 0x30;
	if (c>=0 && c<=9)
		return c;
	c -= 7;
	if (c>=0xA && c<=0xF)
		return c;

	return -1;
}
