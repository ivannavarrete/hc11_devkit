
/*
 * ui_nc.c
 *
 * This is an ncurses user interface.
 *
 * public routines:
 * 		InitUI() initializes the user interface
 * 		CleanupUI() closes down the user interface
 * 		ConfigUI() reconfigures the user interface
 * 		GetCommand() gets a command structure from the user
 * 		ShowCommand() shows the result of a command
 * 		ShowMsg() shows a message
 * 		GetEnvOpt_UI() gets HC11 setup options from user
 * 	private routines:
 *
 */


#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "ui.h"
#include "command.h"
#include "monitor.h"
#include "disasm.h"


/* window pointers */
WINDOW *stdscr = NULL;
WINDOW *reg_w = NULL;
WINDOW *data_w = NULL;
WINDOW *code_w = NULL;
WINDOW *cmd_w = NULL;

int reg_w_rows;
int reg_w_cols;
int data_w_rows;
int data_w_cols;
int code_w_rows;
int code_w_cols;
int cmd_w_rows;
int cmd_w_cols;

int reg_l, data_l, code_l, cmd_l;

int cmd_w_scroll = 0;


int InitUI(void) {
	/* init ncurses stuff */
	stdscr = initscr();
	if (stdscr  == NULL)
		return -1;
	
	/* init color support */
	if (start_color() == ERR) {
		CleanupUI();
		return -1;
	}

	/* initialize window parameters */
	reg_w_rows = 3;
	data_w_rows = 5;
	code_w_rows = 8;
	cmd_w_rows = 4;
	reg_w_cols = data_w_cols = code_w_cols = cmd_w_cols = COLS-2;

	reg_l = 0;
	data_l = reg_w_rows+1;
	code_l = data_l+data_w_rows+1;
	cmd_l = code_l+code_w_rows+1;
	
	/* create windows */
	reg_w = newwin(reg_w_rows, reg_w_cols, reg_l+1, 1);
	data_w = newwin(data_w_rows, data_w_cols, data_l+1, 1);
	code_w = newwin(code_w_rows, code_w_cols, code_l+1, 1);
	cmd_w = newwin(cmd_w_rows, cmd_w_cols, cmd_l+1, 1);
	if (!reg_w || !data_w || !code_w || !cmd_w) {
		CleanupUI();
		return -1;
	}

	/* draw window borders */
	border(0, 0, 0, 0, 0, 0, 0, 0);
	move(data_l, 1);
	hline(0, COLS-2);
	move(code_l, 1);
	hline(0, COLS-2);
	move(cmd_l, 1);
	hline(0, COLS-2);
	//wborder(reg_w, 0, 0, 0, 0, 0, 0, 0, 0);
	//wborder(data_w, 0, 0, 0, 0, 0, 0, 0, 0);
	//wborder(code_w, 0, 0, 0, 0, 0, 0, 0, 0);
	//wborder(cmd_w, 0, 0, 0, 0, 0, 0, 0, 0);

	/* write window labels */
	mvaddstr(reg_l, 4, "[ reg ]");
	mvaddstr(data_l, 4, "[ data ]");
	mvaddstr(code_l, 4, "[ code ]");
	mvaddstr(cmd_l, 4, "[ cmd ]");

	/* set window attributes */
	scrollok(cmd_w, TRUE);
	scrollok(code_w, TRUE);

	/* refresh screen */
	refresh();
	wnoutrefresh(reg_w);
	wnoutrefresh(data_w);
	wnoutrefresh(code_w);
	wnoutrefresh(cmd_w);
	doupdate();

	return 0;
}


int CleanupUI(void) {
	/* delete windows */
	delwin(reg_w);
	delwin(data_w);
	delwin(code_w);
	delwin(cmd_w);
	
	if (endwin() == ERR);		/* reset terminal */
		return -1;
	refresh();

	return 0;
}


int ConfigUI(void) {

	return 0;
}


int GetCommand(struct cmd *cmd) {
	char cmdbuf[COLS+1];
	char *cmdptr = cmdbuf;
	struct token t;
	int i;

	char debugstr[80];

	memset(cmd, 0, sizeof(struct cmd));
	cmdbuf[COLS] = 0;
	
	/* get command and convert to lower case */
	wgetnstr(cmd_w, cmdbuf, COLS-3);
	wrefresh(cmd_w);
	for (i=0; i<strlen(cmdbuf); i++)
		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);

	/* fill command structure */
	/* first get the command ... */
	cmdptr = Token(cmdptr, &t);

	ShowMsg("cmdptr == ");
	ShowMsg(cmdptr);
	snprintf(debugstr, 79, "%d ", t.token);
	ShowMsg(", t.token == ");
	ShowMsg(debugstr);
	snprintf(debugstr, 79, "%d ", t.attr);
	ShowMsg(", cmd == ");
	ShowMsg(debugstr);
	ShowMsg("\n");

	if (!cmdptr)
		cmd->cmd = CMD_NOP;
	else if (t.token == TOKEN_COMMAND) {
		cmd->cmd = t.attr;
		cmd->mod = t.attr2;
	} else
		cmd->cmd = CMD_SYNTAX_ERR;

	/* ... then get command parameters, where necessary */
	switch (cmd->cmd) {
		case CMD_GET_DATA:
		case CMD_GET_CODE:
		case CMD_EXEC:
			cmd->addr1 = GetAddr(&cmdptr, &t, cmd, 0);
			/* this is ugly: we don't know how much code is needed to fill
			 * the entire code window.. */
			cmd->addr2 = cmd->addr1 + 100;
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
int ShowCommand(struct cmd *cmd) {
	switch (cmd->cmd) {
		case CMD_GET_CODE: ShowCode(cmd); break;
		case CMD_GET_DATA: ShowData(cmd); break;
		case CMD_GET_STATE: ShowState(cmd); break;
		case CMD_BP: ShowBP(cmd); break;
		case CMD_HELP: Help(cmd); break;
		case CMD_CLS: Cls(cmd); break;
		case CMD_SYNTAX_ERR: SyntaxError(cmd); break;
		default:
	}

	return 0;
}


/* Display disassembly output. */
int ShowCode(struct cmd *cmd) {
	struct dis_instr *da_list = (struct dis_instr *)cmd->data;
	char str[80];
	int i = 0;

	/* there *should* (I hope) be enough code to fill the code window */
	while (da_list != NULL && i < code_w_rows) {
		snprintf(str, 79, "%.04X", da_list->addr);			/* address */
		mvwaddnstr(code_w, i, 1, str, 5);
		
		mvwaddnstr(code_w, i, 6, da_list->mcode, 19);	/* machine code */
		
		mvwaddnstr(code_w, i, 20, da_list->instr, 20);	/* mnemonic */

		da_list = da_list->next;
		i++;
	}

	wrefresh(code_w);

	return 0;
}


/* Show data. */
int ShowData(struct cmd *cmd) {

	return 0;
}


int ShowState(struct cmd *cmd) {

	return 0;
}


int ShowBP(struct cmd *cmd) {

	return 0;
}


void Help(struct cmd *cmd) {

}


void Cls(struct cmd *cmd) {

}


void SyntaxError(struct cmd *cmd) {
	ShowMsg("syntax error\n");
}


int ShowMsg(char *msg) {
	waddnstr(cmd_w, msg, COLS-2);
	wrefresh(cmd_w);

	return 0;
}


int GetEnvOpt_UI(struct mcu_env *env) {

	return 0;
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
