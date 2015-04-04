
//
// ui_txt.h
//


#ifndef UI_TXT_H
#define UI_TXT_H

#include "ui.h"
#include "cmd.h"


// Text user interface class.
class UI_txt : public UI {
public:
	UI_txt();								// constructor
	~UI_txt();								// destructor

	int Config();							// configure UI
	int GetEnvOpt(struct mcu_env *env);		// get env options
	void ShowMsg(const char *msg);			// show message

	Cmd *GetCmd();							// get command
	//void ShowCmd(CmdGetCode &cmd);			// show 'get code' command
	void ShowCmd(CmdSetCode &cmd);			// show 'set code' command
	/*
	void ShowCmd(CmdGetData &cmd);			// show 'get data' command
	void ShowCmd(CmdSetData &cmd);			// show 'set data' command
	void ShowCmd(CmdCls &cmd);				// show 'clear screen' command
	void ShowCmd(CmdHelp &cmd);				// show 'help' command
	void ShowCmd(CmdQuit &cmd);				// show 'quit' command
	void ShowCmd(CmdErr &cmd);				// show 'error' command
	*/
private:
	//int GetAddr(char **str, struct token t, struct cmd *cmd, int opt);
	char GetHex(char c);

private:
	int rows, cols;							// screen size
};


#endif // UI_TXT_H
