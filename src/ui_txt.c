
/*
 * ui_txt.c
 *
 * This module handles the communication between the user and the PC. This is
 * a text mode interface but it can be replaced with anything else. Only main.c
 * should use this module.
 *
 * main.c uses InitUI(), CleanupUI(), GetCommand(), ShowCommand(),
 * 			GetEnvOpt_UI().
 *
 * public routines:
 *		InitUI() initializes the user interface.
 *		CleanupUI() closes down the user interface.
 *		ConfigUI() reconfigures the user interface.
 *		GetCommand() gets a command structure from the user.
 *		ShowCommand() shows the result of a command.
 *		ShowMsg() shows a message.
 *		GetEnvOpt_UI() gets HC11 setup options from user.
 * private routines:
 *		ShowState() shows the HC11 registers.
 *		ShowCode() shows a disassembly listing.
 *		ShowData() shows data.
 *		Cls() clears the screen.
 *		Help() shows help on commands.
 *		SyntaxError() prints an error string.
 *		GetHex() convers an ASCII hex byte into a real hex byte.
 *
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "monitor.h"
#include "disasm.h"
#include "command.h"
#include "breakpoint.h"
#include "lex.h"
#include "ui.h"


/* Initialize user interface. */
int InitUI(void) {
	int i;

	/* clear screen */
	for (i=0; i<ROWS; i++)
		printf("\n");

	return 0;
}


/* Closes down UI and prepares for exit() call. exit() is done outside of this
 * routine. Not much to do here in text mode though. */
int CleanupUI(void) {
	return 0;
}


/* Nothing here either. Should be different with other UI modules. */
int ConfigUI(void) {
	return 0;
}


/* This routine gets a command structure which it fills in with the user's 
 * command and options. */
int GetCommand(struct cmd *cmd) {
	char cmdbuf[81];
	char *cmdptr = cmdbuf;
	struct token t;
	int i;

	memset(cmd, 0, sizeof(struct cmd));
	cmdbuf[80] = 0;

	/* get command and convert to lower case */
	printf(":");
	fgets(cmdbuf, 80, stdin);
	for (i=0; i<strlen(cmdbuf); i++)
		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);
	
	/* fill command structure */
	/* first get the command ... */
	cmdptr = Token(cmdptr, &t);
	if (t.token == TOKEN_COMMAND) {
		cmd->cmd = t.attr;
		cmd->mod = t.attr2;
	} else
		cmd->cmd = CMD_SYNTAX_ERR;

	/* ... then get command parameters where necessary */
	switch (cmd->cmd) {
		case CMD_GET_DATA:
		case CMD_GET_CODE:
		case CMD_EXEC:
			cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
			cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 1);
			break;
		case CMD_SET_CODE:
			cmdptr = Token(cmdptr, &t);		/* get file name */
			if (cmdptr) {
				cmd->dsize = strlen(t.lex) + 1;
				cmd->data = malloc(cmd->dsize);
				if (!cmd->data) {
					perror(": GetCommand(): ");
					return -1;
				}
				strcpy(cmd->data, t.lex);
			} else
				cmd->cmd = CMD_SYNTAX_ERR;

			break;
		case CMD_SET_DATA:
			if (!strcmp(t.lex, "bf")) {
				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
				cmd->addr2 = GetAddr(&cmdptr, &t, cmd, 0);

				cmdptr = Token(cmdptr, &t);
				if (t.token == TOKEN_NUM && t.attr < 256) {
					cmd->dsize = cmd->addr2 - cmd->addr1 + 1;
					cmd->data = malloc(cmd->dsize);
					if (!cmd->data) {
						perror(": GetCommand(): ");
						return -1;
					}

					memset(cmd->data, t.attr, cmd->dsize);
				} else
					cmd->cmd = CMD_SYNTAX_ERR;
			}

			break;
		case CMD_BP:
			if (cmd->mod == CMD_BP_SET || cmd->mod == CMD_BP_CLEAR)
				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
			else
				cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 1);
			
			break;
		case CMD_HELP:
			cmdptr = Token(cmdptr, &t);
			if (cmdptr && t.token == TOKEN_COMMAND)
				cmd->mod = t.attr;
			else if (cmdptr)
				cmd->cmd = CMD_SYNTAX_ERR;
				
			break;
		case CMD_SYNTAX_ERR:
		default:
			break;			/* only commands with no params end up here */
	}

	return 0;
}


/* Takes a request from main program to display command-specific output. */
int ShowCommand(struct cmd *scmd) {
	switch (scmd->cmd) {
		case CMD_GET_CODE: ShowCode(scmd); break;
		case CMD_GET_DATA: ShowData(scmd); break;
		case CMD_GET_STATE: ShowState(scmd); break;
		case CMD_BP: ShowBP(scmd); break;
		case CMD_HELP: Help(scmd); break;
		case CMD_CLS: Cls(scmd); break;
		case CMD_SYNTAX_ERR: SyntaxError(scmd); break;
		default:
	}
	
	return 0;
}


/* Get the MCU environment options from user and fill the env struct. */
int GetEnvOpt_UI(struct mcu_env *env) {
	char c;
	char buf[10];
	
	printf("*** environment options ***\n");

	do {
		printf("    MCU mode [s/e/b/t] [%s]: ",
			env->mode==MCU_MODE_BOOTSTRAP ? "b" :
			(env->mode==MCU_MODE_TEST ? "t" :
			(env->mode==MCU_MODE_EXPANDED ? "e" : "s")));
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
		printf("    talker page [0-F] [%01X]: DOESN'T WORK!\n", env->talker_page);
	//	fgets(buf, 3, stdin);
	//	c = GetHex(buf[0]);
	//	if (buf[0]=='\n') break;
	//	else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
	//		env->talker_page = c;
	//		break;
	//	}
	//} while (1);
	
	do {
		printf("    RAM page [0-F] [%01X]: ", env->ram_page);
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
			env->ram_page = c;
			break;
		}
	} while (1);
	
	do {
		printf("    register page [0-F] [%01X]: ", env->reg_page);
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
		printf("    on-chip EEPROM [on/off] [%s]: ",
					env->on_chip_eeprom ? "on" : "off");
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
		printf("    on-chip ROM [on/off] [%s]: ",
				env->on_chip_rom ? "on" : "off");
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
		printf("    irq mode [edge/level] [%s]: ",
				env->irq_mode ? "edge" : "level");
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
		printf("    osc delay [on/off] [%s]: ",
				env->osc_delay ? "on" : "off");
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

	printf("    timer prescaler rate [1/4/8/16] []: NOT IMPLEMENTED\n");
	printf("    COP prescaler rate [1/4/16/64] []: NOT IMPLEMENTED\n\n");
	return 0;
}


int ShowData(struct cmd *scmd) {
	unsigned char *data = (unsigned char *)scmd->data;
	int r, c;

	for (r=0; r<(scmd->dsize)/0x10; r++)  {
		/* address */
		printf(": %04X:  ", (scmd->addr1+r*0x10) & 0xFFFF);

		/* hex data */
		if (scmd->mod == CMD_DATA_B) {
			for (c=0; c<0x10; c++)
				printf("%02X %s", data[r*0x10+c], (c==7) ? " ":"");
		} else {
			for (c=0; c<0x10; c+=2)
				printf("%02X%02X  %s", data[r*0x10+c], data[r*0x10+c+1],
						(c==6) ? " ":"");
		}
		printf("  ");

		/* ascii data */
		for (c=0; c<0x10; c++)
			printf("%c", (data[r*0x10+c]>=0x20 && data[r*0x10+c]<='Z') ?
					data[r*0x10+c] : '.');
		printf("\n");

		if (r%(ROWS-2) == 0 && r != 0) {
			printf("press [enter] key to continue");
			getchar();
		}
	}

	/* This if statement prints out the last incomplete line (if any).
	 * NOTE: Should incorporate it in the above loop instead. */
	if ((scmd->dsize)%0x10) {
		/* address */
		printf(": %04X:  ", (scmd->addr1+r*0x10) & 0xFFFF);

		/* hex data */
		if (scmd->mod == CMD_DATA_B) {
			for (c=0; c<(scmd->dsize)%0x10; c++)
				printf("%02X %s", data[r*0x10+c], (c==7) ? " ":"");
		} else {
			for (c=0; c<(scmd->dsize)%0x10; c++)
				printf("%02X%02X  %s", data[r*0x10+c], data[r*0x10+c+1],
						(c==6) ? " ":"");
		}

		/* ascii data */
		for (c=0; c<(0x10-(scmd->dsize)%0x10+1)*3; c++)
			printf(" ");

		for (c=0; c<(scmd->dsize)%0x10; c++)
			printf("%c", (data[r*0x10+c]>=0x20 && data[r*0x10+c]<='Z') ?
					data[r*0x10+c] : '.');
		printf("\n");
	}

	return 0;
}


int ShowState(struct cmd *scmd) {
	printf(": AccA: \n");
	printf(": AccB: \n");
	printf(": X: \n");
	printf(": Y: \n");
	printf(": SP: \n");
	printf(": PC: \n");
	printf(": flags: \n");

	return 0;
}


/* Display disassembly output. */
int ShowCode(struct cmd *scmd) {
	struct dis_instr *da_list = (struct dis_instr *)scmd->data;
	int i, j=2;

	if (da_list != NULL)
		printf("addr | machine code     |    mnemonic           |          byt"
				"es | cycles\n"
				"-----|------------------|-----------------------|------------"
				"----|---------\n");

	while (da_list != NULL) {
		printf("%04X:  ", da_list->addr);			/* address */
	
		printf("%s", da_list->mcode);				/* machine code */
		for (i=strlen(da_list->mcode); i<22; i++)
			printf(" ");

		printf("%s", da_list->instr);				/* mnemonic */
		for (i=strlen(da_list->instr); i<30; i++)
			printf(" ");

		printf("%02d", da_list->bytes);				/* instruction size */
		for (i=2; i<8; i++)
			printf(" ");

		printf("%02d\n", da_list->cycles);			/* instruction time */

		if (++j == ROWS-1) {
			printf("press [enter] key to continue");
			getchar();
			j = 0;
		}

		da_list = da_list->next;
	}
	
	return 0;
}


int ShowBP(struct cmd *scmd) {
	struct breakpoint *bp = (struct breakpoint *)scmd->data;

	printf(": %u: %04u\n", bp->num, bp->addr);

	return 0;
}


/* Show current disassembly mode. */
/*
void ShowDisasmMode(struct cmd *cmd) {
	printf("code %s, bytes %s, cycles %s\n",
			(*(int *)(cmd->data) & DMODE_CODE) ? "on" : "off",
			(*(int *)(cmd->data) & DMODE_BYTES) ? "on" : "off",
			(*(int *)(cmd->data) & DMODE_CYCLES) ? "on" : "off");
}
*/


int ShowMsg(char *msg) {
	printf("%s", msg);
	return 0;
}


/* Display error message. */
void SyntaxError(struct cmd *scmd) {
	printf(": syntax error\n");
}


/* Display help. */
void Help(struct cmd *scmd) {
	switch (scmd->mod) {
		case CMD_GET_CODE:
			printf(":\n:\tc <saddr> [eaddr]\n"
				": Disassemble a piece of code with start address saddr and\n"
				": end address eaddr. saddr is mandatory, eaddr is optional\n");
			break;
		case CMD_GET_DATA:
			printf(":\n:\tdb <saddr> [eaddr]\n:\tdw <saddr> [eaddr]\n"
				": Dump a data reqion starting at saddr and ending at eaddr.\n"
				": saddr is mandatory, eaddr is optional. The data can be\n"
				": is displayd either as bytes or words.\n");
			break;
		case CMD_DATA_BF:
			printf(":\n:\tbf <saddr> <eaddr> <val>\n"
				": Block fill a memory area with start address saddr and end\n"
				": address eaddr, with the value val.\n");
			break;
		case CMD_EXEC:
			printf(":\n:\tg <saddr>\n"
				": Execute code starting at address saddr.\n");
			break;
		case CMD_BP:
			printf(":\n:\tbp <addr>\n"
				": Set a breakpoint at addr.\n");
		case CMD_CLS:
			printf(":\n:\tcls\n: Clear display.\n");
			break;
		case CMD_HELP:
			printf(":\n:\thelp [command]\n:\th [command]\n"
				": Display general help or help on a specific command.\n");
			break;
		case CMD_QUIT:
			printf(":\n:\tquit\n:\tq\n: Terminate program\n");
			break;
		case CMD_SYNTAX_ERR:
			printf(": no such command\n");
			break;
		default:
			printf(":\n: commands:\n"
					":\tdb <saddr> [eaddr]             show data in bytes\n"
					":\tdw <saddr> [eaddr]             show data in words\n"
					":\tbf <saddr> <eaddr> <val>       block fill memory area\n"
					":\tc <saddr> [eaddr]              disassemble code\n"
					":\tg <saddr>                      execute code\n"
					":\tbp <addr>                      set a breakpoint\n"
					":\tcls                            clear display\n"
					":\thelp [command]                 show help on commands\n"
					":\tquit                           exit program\n");
	}
}


/* Clear command screen. */
void Cls(struct cmd *scmd) {
	int i;

	for (i=0; i<ROWS; i++)
		printf(":\n");
}


/* Internal routine for use in GetCommand(). Note that t gets changed, so
 * don't call this if t is still needed in GetCommand(). (Or make another
 * variable for this routine.)
 * This routine returns a number on success. On failure it returns 0 AND sets
 * cmd->cmd to CMD_SYNTAX_ERR. */
int GetAddr(char **str, struct token *t, struct cmd *cmd, int opt) {
	*str = Token(*str, t);

	if (opt) {								/* optional */
		if (*str && t->token == TOKEN_NUM)
			return t->attr;
		else if (*str)
			cmd->cmd = CMD_SYNTAX_ERR;
		else
			return 0;
	} else {								/* mandatory */
		if (t->token == TOKEN_NUM)
			return t->attr;
		else
			cmd->cmd = CMD_SYNTAX_ERR;
	}

	return 0;
}


/* Internal routine. Convert ASCII hex number ('0'-'F') to hex number
 * (0x0-0xF). */
char GetHex(char c) {
	c -= 0x30;
	if (c>=0 && c<=9)
		return c;
	c -= 7;
	if (c>=0xA && c<=0xF)
		return c;

	return -1;
}
