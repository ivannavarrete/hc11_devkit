
/*
 * ui_nc.c
 *
 * This is an ncurses user interface.
 *
 * public routines:
 * 		InitUI() initializes the user interface.
 * 		CleanupUI() closes down the user interface.
 * 		ConfigUI() reconfigures the user interface.
 * 		GetCommand() gets a command structure from the user.
 * 		ShowCommand() shows the result of a command.
 * 		ShowMsg() shows a message.
 * 		GetEnvOpt_UI() gets HC11 setup options from user.
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
#include "breakpoint.h"


/* window pointers */
WINDOW *stdscr = NULL;
WINDOW *reg_w = NULL;
WINDOW *data_w = NULL;
WINDOW *code_w = NULL;
WINDOW *cmd_w = NULL;

/* window sizes */
int reg_w_rows;
int reg_w_cols;
int data_w_rows;
int data_w_cols;
int code_w_rows;
int code_w_cols;
int cmd_w_rows;
int cmd_w_cols;
int reg_l, data_l, code_l, cmd_l;

/* colors, the COLOR_* are just indexes, not the actual future colors */
short color_main = 1;
short color_frame = 2;
short color_bp = 3;
short color_change = 4;
short f1=COLOR_WHITE, b1=COLOR_BLACK;
short f2=COLOR_CYAN, b2=COLOR_BLACK;
short f3=COLOR_BLUE, b3=COLOR_BLACK;
short f4=COLOR_CYAN, b4=COLOR_BLACK;


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

	//init_color(f1, 000, 0, 300);
	//init_color(b1, 0, 0, 0);
	//init_color(f2, 0, 1000, 0);
	//init_color(b2, 0, 0, 0);
	//init_color(f3, 0, 0, 1000);
	//init_color(b3, 0, 0, 0);
	//init_color(f4, 300, 0, 0);
	//init_color(b4, 0, 0, 0);

	init_pair(color_main, f1, b1);
	init_pair(color_frame, f2, b2);
	//init_pair(color_change, b4, f4);
	//init_pair(color_bp, f4, b4);

	/* initialize window parameters */
	if (LINES < 20 || COLS < 80) {
		CleanupUI();
		return -1;
	}
	reg_w_rows = 1;
	data_w_rows = (LINES-reg_w_rows-3)/4;
	code_w_rows = (LINES-reg_w_rows-3)/2;
	cmd_w_rows = LINES-reg_w_rows-data_w_rows-code_w_rows-5;
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

	/* set window colors */
	wattron(cmd_w, A_DIM);
	wattron(cmd_w, COLOR_PAIR(color_main));
	wattron(code_w, A_DIM);
	wattron(code_w, COLOR_PAIR(color_main));
	wattron(data_w, A_DIM);
	wattron(data_w, COLOR_PAIR(color_main));
	wattron(reg_w, A_DIM);
	wattron(reg_w, COLOR_PAIR(color_main));
	wattron(stdscr, A_DIM);
	wattron(stdscr, COLOR_PAIR(color_frame));

	/* draw window borders */
	border(0, 0, 0, 0, 0, 0, 0, 0);
	move(data_l, 1);
	hline(0, COLS-2);
	move(code_l, 1);
	hline(0, COLS-2);
	move(cmd_l, 1);
	hline(0, COLS-2);

	/* write window labels */
	mvaddstr(reg_l, 4, "[ reg ]");
	mvaddstr(data_l, 4, "[ data ]");
	mvaddstr(code_l, 4, "[ code ]");
	mvaddstr(cmd_l, 4, "[ cmd ]");

	/* set window attributes */
	scrollok(cmd_w, TRUE);
	scrollok(code_w, TRUE);

	/* put cursor into position */
	wmove(cmd_w, cmd_w_rows-1, 0);

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
	
	/* clear screen */
	werase(stdscr);
	wrefresh(stdscr);
	
	/* reset terminal */
	if (endwin() == ERR);
		return -1;
	refresh();

	return 0;
}


int ConfigUI(void) {

	return 0;
}


int GetCommand(struct cmd *cmd) {
	char cmdbuf[COLS];
	char *cmdptr = cmdbuf;
	struct token t;
	int i;

	memset(cmd, 0, sizeof(struct cmd));
	cmdbuf[COLS] = 0;
	
	/* get command and convert to lower case */
	wgetnstr(cmd_w, cmdbuf, COLS-3);
	wrefresh(cmd_w);
	for (i=0; i<strlen(cmdbuf); i++)
		cmdbuf[i] = (char)tolower((int)cmdbuf[i]);
	cmdbuf[i] = ' ';	/* insert whitespace or else lex. analyser won't work */
	cmdbuf[i+1] = 0;

	/* fill command structure */
	/* first get the command ... */
	cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);

	/*{
		char debugstr[80];

		ShowMsg("cmdptr == ");
		ShowMsg(cmdptr);
		snprintf(debugstr, 79, "%d ", t.token);
		ShowMsg(", t.token == ");
		ShowMsg(debugstr);
		snprintf(debugstr, 79, "%d ", t.attr);
		ShowMsg(", cmd == ");
		ShowMsg(debugstr);
		ShowMsg("\n");
	}*/

	//if (!cmdptr)
	//	cmd->cmd = CMD_NOP;
	if (t.token == TOKEN_COMMAND) {
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
			/* this is ugly: we don't know how much code/data is needed to
			 * fill the entire code/data window, so we just take a number
			 * and hope it is enough */
			cmd->addr2 = cmd->addr1 + 100;
			break;
		case CMD_SET_CODE:
			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);	/* get file name */
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

				cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
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
			cmdptr = Token(cmdptr, &t, TOKEN_COMMAND);
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

	/* clear window */
	werase(code_w);

	/* there *should* (I hope) be enough code to fill the code window */
	while (da_list != NULL && i < code_w_rows) {
		snprintf(str, 5, "%04X", da_list->addr);		/* address */
		mvwaddnstr(code_w, i, 1, str, 6);
		
		mvwaddnstr(code_w, i, 8, da_list->mcode, 25);	/* machine code */
		
		mvwaddnstr(code_w, i, 26, da_list->instr, 59);	/* mnemonic */

		snprintf(str, 4, "%i", da_list->bytes);			/* bytes */
		mvwaddnstr(code_w, i, 60, str, 5);
		
		snprintf(str, 4, "%i", da_list->cycles);		/* cycles */
		mvwaddnstr(code_w, i, 70, str, 5);

		da_list = da_list->next;
		i++;
	}

	/* move cursor and refresh window */
	wmove(cmd_w, cmd_w_rows-1, 0);
	wrefresh(code_w);

	return 0;
}


/* Show data. */
int ShowData(struct cmd *cmd) {
	unsigned char *data = (unsigned char *)cmd->data;
	char str[80];
	int r, c;

	/* clear window */
	werase(data_w);
	
	for (r=0; r<data_w_rows || r<(cmd->dsize)/0x10; r++) {
		/* address */
		snprintf(str, 5, "%04X", (cmd->addr1+r*0x10)&0xFFFF);
		mvwaddnstr(data_w, r, 1, str, 6);

		/* hex data */
		if (cmd->mod == CMD_DATA_B) {
			for (c=0; c<0x10; c++) {
				snprintf(str, 3, "%02X", data[r*0x10+c]);
				mvwaddnstr(data_w, r, 8+c*3+((c>7)?1:0), str, 3);
			}
		} else {
			for (c=0; c<0x10; c+=2) {
				snprintf(str, 5, "%02X%02X", data[r*0x10+c], data[r*0x10+c+1]);
				mvwaddnstr(data_w, r, 8+c*3+((c>6)?2:0), str, 5);
			}
		}
		
		/* ASCII data */
		for (c=0; c<0x10; c++) {
			snprintf(str, 2, "%c", (data[r*0x10+c]>=0x20 && data[r*0x10+c]<='Z')
					? data[r*0x10+c] : '.');
			mvwaddnstr(data_w, r, 60+c, str, 2);
		}
	}

	/* move cursor and refresh window */
	wmove(cmd_w, cmd_w_rows-1, 0);
	wrefresh(data_w);

	return 0;
}


int ShowState(struct cmd *cmd) {
	//char str[80];

	/* clear window */
	werase(reg_w);

	mvwaddstr(reg_w, 0, 1, "AccA: 00");
	mvwaddstr(reg_w, 0, 11, "AccB: 00");
	mvwaddstr(reg_w, 0, 21, "X: 0000");
	mvwaddstr(reg_w, 0, 30, "Y: 0000");
	mvwaddstr(reg_w, 0, 39, "SP: 0000");
	mvwaddstr(reg_w, 0, 49, "PC: 0000");
	mvwaddstr(reg_w, 0, 61, "flags: sxhinzvc");

	/* move cursor and refresh window */
	wmove(cmd_w, cmd_w_rows-1, 0);
	wrefresh(reg_w);

	return 0;
}


int ShowBP(struct cmd *cmd) {
	struct breakpoint *bp = (struct breakpoint *)cmd->data;
	char str[80];

	snprintf(str, 4, "%d", bp->num);
	waddstr(cmd_w, str);
	waddstr(cmd_w, ": ");
	snprintf(str, 6, "%04X", bp->addr);
	waddstr(cmd_w, str);
	waddstr(cmd_w, "\n");

	/* move cursor and refresh window */
	wmove(cmd_w, cmd_w_rows-1, 0);
	wrefresh(cmd_w);

	return 0;
}


void Help(struct cmd *cmd) {
	ShowMsg("Help() not implemented.\n");

}


void Cls(struct cmd *cmd) {
	werase(cmd_w);
	wrefresh(cmd_w);
	wmove(cmd_w, cmd_w_rows-1, 0);
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
	*str = Token(*str, t, TOKEN_COMMAND);

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
