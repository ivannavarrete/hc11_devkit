
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui_txt.h"
#include "monitor.h"
#include "disasm.h"
#include "command.h"


/* exported functions, mainly used by main.c */
int InitUI(void);
int CleanupUI(void);
int ConfigUI(void);
int GetCommand(struct cmd *cmd);
int ShowCommand(struct cmd *cmd);
int GetEnvOptions(struct mcu_env *env);		/* xcept this one (for now) */
/* command routines */
int ShowState(struct cmd *scmd);
int ShowCode(struct cmd *scmd);
int ShowData(struct cmd *scmd);
//void ShowDisasmMode(struct cmd *cmd);
void Cls(struct cmd *scmd);
void Help(struct cmd *scmd);
void SyntaxError(struct cmd *scmd);
/* internal routine */
char GetHex(char c);


/* Initialize user interface. */
int InitUI(void) {
	int i;

	/* clear screen */
	for (i=0; i<28; i++)
		printf("\n");

	return 0;
}


/* not implemented */
/* Closes down UI and prepares for exit() call. exit() is done outside of this
 * routine. */
int CleanupUI(void) {
	return 0;
}


/* not implemented */
int ConfigUI(void) {
	return 0;
}


/* This routine gets a command structure which it fills in with the user's 
 * command and options. */
int GetCommand(struct cmd *cmd) {
	char cmdbuf[81];
	char *cmdptr;
	char *token;
	char *delim = " \t\n";
	char *end;
	int i;
	
	memset(cmd, 0, sizeof(struct cmd));

	/* get command and convert to lower case */
	printf(":");
	fgets(cmdbuf, 80, stdin);
	for (i=0; i<strlen(cmdbuf); i++)
		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);

	/* fill command structure */
	/* first parse the major command ... */
	cmdptr = cmdbuf;
	token = strsep(&cmdptr, delim);
	if (token[0]) {
		if (!strcmp(token, "db")) {
			cmd->cmd = CMD_DATA;
			cmd->mod = CMD_DATA_B;
		} else if (!strcmp(token, "dw")) {
			cmd->cmd = CMD_DATA;
			cmd->mod = CMD_DATA_W;
		} else if (!strcmp(token, "c"))
			cmd->cmd = CMD_CODE;
		else if (!strcmp(token, "r"))
			cmd->cmd = CMD_STATE;
		else if (!strcmp(token, "cls"))
			cmd->cmd = CMD_CLS;
		else if (!strcmp(token, "quit") || !strcmp(token, "q"))
			cmd->cmd = CMD_QUIT;
		else if (!strcmp(token, "help") || !strcmp(token, "h"))
			cmd->cmd = CMD_HELP;
		else
			cmd->cmd = CMD_SYNTAX_ERR;
	} else
		cmd->cmd = CMD_NOP;

	/* ... then get command parameters where necessary */
	switch (cmd->cmd) {
		case CMD_DATA:
		case CMD_CODE:
			token = strsep(&cmdptr, delim);		/* first addr is mandatory */
			if (token[0]) {
				cmd->addr1 = strtol(token, &end, 16);
				if (*end != 0) cmd->cmd = CMD_SYNTAX_ERR;
			} else {
				cmd->cmd = CMD_SYNTAX_ERR;
				break;
			}

			token = strsep(&cmdptr, delim);		/* second addr is optional */
			if (token[0]) {
				cmd->addr2 = strtol(token, &end, 16);
				if (*end != 0) cmd->cmd = CMD_SYNTAX_ERR;
			}

			break;
		case CMD_HELP:
			token = strsep(&cmdptr, delim);
			if (token[0]) {		/* get help on specific command */
				if (!strcmp(token, "c"))
					cmd->mod = CMD_CODE;
				else if (!strcmp(token, "db") || !strcmp(token, "dw"))
					cmd->mod = CMD_DATA;
				else if (!strcmp(token, "cls"))
					cmd->mod = CMD_CLS;
				else if (!strcmp(token, "help") || !strcmp(token, "h"))
					cmd->mod = CMD_HELP;
				else if (!strcmp(token, "quit") || !strcmp(token, "q"))
					cmd->mod = CMD_QUIT;
				else
					cmd->mod = CMD_SYNTAX_ERR;
			}

			break;
		default:
			break;			/* only commands with no params end up here */
	}



//	if (!strcmp(cmdbuf, "c code on\n"))
//		cmd->cmd = CMD_C_CODE_ON;
//	else if (!strcmp(cmdbuf, "c code off\n"))
//		cmd->cmd = CMD_C_CODE_OFF;
//	else if (!strcmp(cmdbuf, "c bytes on\n"))
//		cmd->cmd = CMD_C_BYTES_ON;
//	else if (!strcmp(cmdbuf, "c bytes off\n"))
//		cmd->cmd = CMD_C_BYTES_OFF;
//	else if (!strcmp(cmdbuf, "c cycles on\n"))
//		cmd->cmd = CMD_C_CYCLES_ON;
//	else if (!strcmp(cmdbuf, "c cycles off\n"))
//		cmd->cmd = CMD_C_CYCLES_OFF;
//	else if (!strcmp(cmdbuf, "c mode\n"))
//		cmd->cmd = CMD_DISASM_MODE;
//	else if (!strcmp(cmdbuf, "c\n")) {		/* incomplete */
//		cmd->cmd = CMD_CODE;
//		cmd->addr1 = 0x0000;
//		cmd->addr2 = 0x0100;
//	} else if (!strcmp(cmdbuf, "cls\n"))
//		cmd->cmd = CMD_CLS;
//	else if (!strcmp(cmdbuf, "help\n"))		/* incomplete */
//		cmd->cmd = CMD_HELP;
//	else if (!strcmp(cmdbuf, "q\n") || !strcmp(cmdbuf, "quit\n"))
//		cmd->cmd = CMD_QUIT;
//	else if (!strcmp(cmdbuf, "\n"))			/* incomplete */
//		cmd->cmd = CMD_NOP;
//	else
//		cmd->cmd = CMD_SYNTAX_ERR;
	
	return 0;
}


/* Takes a request from main program to display command-specific output. */
int ShowCommand(struct cmd *scmd) {
	switch (scmd->cmd) {
//		case CMD_DISASM_MODE: ShowDisasmMode(cmd); break;
		case CMD_CODE: ShowCode(scmd); break;
		case CMD_DATA: ShowData(scmd); break;
		case CMD_STATE: ShowState(scmd); break;
		case CMD_HELP: Help(scmd); break;
		case CMD_CLS: Cls(scmd); break;
		case CMD_SYNTAX_ERR: SyntaxError(scmd); break;
		default:
	}
	
	return 0;
}


/* Get the MCU environment options from user and fill the env struct. */
int GetEnvOptions(struct mcu_env *env) {
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
	
	do {
		printf("    talker page [0-F] [%01X]: ", env->talker_page);
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
			env->talker_page = c;
			break;
		}
	} while (1);
	
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

	printf("    timer prescaler rate [1/4/8/16] []: \n");
	printf("    COP prescaler rate [1/4/16/64] []: \n\n");
	return 0;
}


int ShowData(struct cmd *scmd) {
	int i;

	for (i=0; i<scmd->dsize; i++)
		printf("%02X ", ((char *)scmd->data)[i]);

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


int ShowCode(struct cmd *scmd) {
	struct dis_instr *da_list = (struct dis_instr *)scmd->data;
	int i;

	while (da_list != NULL) {
		printf("%s", da_list->mcode);
		for (i=strlen(da_list->mcode); i<15; i++)
			printf(" ");

		printf("%s", da_list->instr);
		for (i=strlen(da_list->instr); i<30; i++)
			printf(" ");

		printf("%s", da_list->bytes);
		for (i=strlen(da_list->bytes); i<8; i++)
			printf(" ");

		printf("%s\n", da_list->cycles);

		da_list = da_list->next;
	}
	
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


/* Display error message. */
void SyntaxError(struct cmd *scmd) {
	printf(": syntax error\n");
}


/* Display help. */
void Help(struct cmd *scmd) {
	switch (scmd->mod) {
		case CMD_CODE:
			printf(":\n:\tc <saddr> [eaddr]\n"
				": Disassemble a piece of code with start address saddr and\n"
				": end address eaddr. saddr is mandatory, eaddr is optional\n");
			break;
		case CMD_DATA:
			printf(":\n:\tdb <saddr> [eaddr]\n:\tdw <saddr> [eaddr]\n"
				": Dump a data reqion starting at saddr and ending at eaddr.\n"
				": saddr is mandatory, eaddr is optional. The data can be\n"
				": is displayd either as bytes or words.\n");
			break;
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
					":\tc <saddr> [eaddr]              disassemble code\n"
					":\tcls                            clear display\n"
					":\thelp [command]                 show help on commands\n"
					":\tquit                           exit program\n");
	}

	//printf(""
		//	"\tc code on\n"
		//	"\tc code off\n"
		//	"\tc bytes on\n"
		//	"\tc bytes off\n"
		//	"\tc cycles on\n"
		//	"\tc cycles off\n"
		//	"\tc mode\n"
}


/* Clear command screen. */
void Cls(struct cmd *scmd) {
	printf(":\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n"
			":\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n"
			":\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n"
			":\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n:\n");
}


/* Internal routine. Convert ASCII hex number ('0'-'F') to hex number
 * (0x0-0xF). */
char GetHex(char c) {
	c -= 0x30;
	if (c>=0 && c<=9) return c;
	c -= 7;
	if (c>=0xA && c<=0xF) return c;

	return -1;
}
