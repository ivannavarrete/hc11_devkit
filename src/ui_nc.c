
/*
 * ui_nc.c
 *
 * This is an ncurses user interface.
 */


#include <curses.h>

#include "ui_nc.h"
#include "command.h"
#include "monitor.h"


WINDOW *stdscr;


int InitUI(void) {
	stdscr = initscr();		/* initinit ncurses stuff */
	if (stdscr  == NULL)
		return -1;
	
	return 0;
}


int CleanupUI(void) {
	if (endwin() == ERR);				/* reset terminal */
		return -1;
	
	return 0;
}


int ConfigUI(void) {

	return 0;
}


int GetCommand(struct cmd *cmd) {

	return 0;
}


int ShowCommand(struct cmd *cmd) {

	return 0;
}


int GetEnvOpt_UI(struct mcu_env *env) {

	return 0;
}
