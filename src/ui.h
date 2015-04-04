
//
// ui.h
//

#ifndef UI_H
#define UI_H


#include "cmd.h"
//#include "lex.h"
//#include "debugger.h"

class UI {
	// public functions
//	int GetCommand(struct cmd *cmd);
//	int ShowCommand(struct cmd *cmd);
	// command routines
//	int ShowState(struct cmd *scmd);
//	int ShowCode(struct cmd *scmd);
//	int ShowData(struct cmd *scmd);
//	int ShowBP(struct cmd *scmd);
//	void ShowDisasmMode(struct cmd *cmd);
//	void Cls(struct cmd *scmd);
//	void Help(struct cmd *scmd);
//	void SyntaxError(struct cmd *scmd);
	// internal help routines
//	int GetAddr(char **str, struct token *t, struct cmd *cmd, int opt);

public:		
	virtual int Config() {;}						// configure UI
	virtual int GetEnvOpt(struct mcu_env *env) {;}	// get env options
	virtual void ShowMsg(const char *msg) {;}		// show message

	virtual Cmd *GetCmd() {;}						// get command
	virtual void ShowCmd(CmdGetCode &cmd) {;}		// show 'get code' command
	virtual void ShowCmd(CmdSetCode &cmd) {;}		// show 'set code' command
	virtual void ShowCmd(CmdGetData &cmd) {;}		// show 'get data' command
	virtual void ShowCmd(CmdSetData &cmd) {;}		// show 'set data' command
	virtual void ShowCmd(CmdCls &cmd) {;}			// show 'clr screen' command
	virtual void ShowCmd(CmdHelp &cmd) {;}			// show 'help' command
	virtual void ShowCmd(CmdQuit &cmd) {;}			// show 'quit' command
	virtual void ShowCmd(CmdErr &cmd) {;}			// show 'error' command
};


#endif // UI_H
