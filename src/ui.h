
#ifndef UI_H
#define UI_H


#include "command.h"
#include "lex.h"
#include "monitor.h"


/* UI defines.
 * as many as possible should be found out at runtime */
#define ROWS				30
#define COLUMNS				80

/* user interface commands */
//#define SHOW_DATA_WIN		0
//#define SHOW_CODE_WIN		1
//#define SHOW_MCU_STATE_WIN	2
//#define SHOW_ENV_OPT_WIN	3

#define SHOW_DATA			10
#define SHOW_CODE			11
#define SHOW_MCU_STATE		12
#define SHOW_ENV_OPT		13


/* exported functions, only used by main.c */
int InitUI(void);
int CleanupUI(void);
int ConfigUI(void);
int GetCommand(struct cmd *cmd);
int ShowCommand(struct cmd *cmd);
int ShowMsg(char *msg);
int GetEnvOpt_UI(struct mcu_env *env);
/* command routines */
int ShowState(struct cmd *scmd);
int ShowCode(struct cmd *scmd);
int ShowData(struct cmd *scmd);
int ShowBP(struct cmd *scmd);
//void ShowDisasmMode(struct cmd *cmd);
void Cls(struct cmd *scmd);
void Help(struct cmd *scmd);
void SyntaxError(struct cmd *scmd);
/* internal help routines */
int GetAddr(char **str, struct token *t, struct cmd *cmd, int opt);
char GetHex(char c);


#endif
