
/*
 * main.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <linux/serial.h>

#include "command.h"
#include "monitor.h"
#include "disasm.h"


int Init(void);
void Cleanup(void);
void Usage(void);
void Error(char *err);
void Perror(char *err);
void SigDef(int signum);

//void CmdCChangeMode(int mode, int on);
//void CmdCShowMode(void);
void CmdGetCode(struct cmd *cmd);
void CmdSetCode(struct cmd *cmd);
void CmdGetData(struct cmd *cmd);
void CmdSetData(struct cmd *cmd);
void CmdGetState(struct cmd *cmd);
void CmdExec(struct cmd *cmd);
void CmdCls(struct cmd *cmd);
void CmdHelp(struct cmd *cmd);
void CmdQuit(struct cmd *cmd);
void CmdSyntaxErr(struct cmd *cmd);

/* serial.c */
extern int InitComm(void);
extern int CleanupComm();
/* monitor.c */
extern int InstallMonitor();
extern struct packet *CreatePacket(int packet_size);
extern int DestroyPacket(struct packet *p);
extern int SendPacket(struct packet *p);
extern struct packet *RecvPacket(void);
/* disasm.c */
extern struct dis_instr *CreateDisasm(unsigned short addr, unsigned char *code, int len);
extern int DestroyDisasm(struct dis_instr *da_list);
/* ui*.c */
extern int InitUI(void);
extern int CleanupUI(void);
extern int GetCommand(struct cmd *cmd);
extern int ShowCommand(struct cmd *cmd);


int disasm_mode;			/* disassembly output mode */

struct cmd cmd;				/* message struct for UI<-->main communication */
struct packet *sp, *rp;		/* packets for main<-->hc11 communication */


void main(int argc, char **argv) {
	if (Init() == -1)
		Error("Init()");

	/* command loop */
	do {
		memset(&cmd, 0, sizeof(struct cmd));
		if (GetCommand(&cmd) == -1)		/* get command */
			continue;					/* handle errors in UI module */

		switch (cmd.cmd) {		/* execute command */
			case CMD_GET_CODE: CmdGetCode(&cmd); break;
			case CMD_SET_CODE: CmdSetCode(&cmd); break;
			case CMD_GET_DATA: CmdGetData(&cmd); break;
			case CMD_SET_DATA: CmdSetData(&cmd); break;
			case CMD_GET_STATE: CmdGetState(&cmd); break;
			case CMD_EXEC: CmdExec(&cmd); break;
			case CMD_CLS: CmdCls(&cmd); break;
			case CMD_HELP: CmdHelp(&cmd); break;
			case CMD_QUIT: CmdQuit(&cmd); break;
			case CMD_SYNTAX_ERR: CmdSyntaxErr(&cmd); break;
			case CMD_NOP:
			default:
		}
	} while (1);
}


/*
void CmdCChangeMode(int mode, int on) {
	if (on)	disasm_mode |= mode;
	else disasm_mode &= ~mode;
}
*/


/*
void CmdCShowMode(void) {
	scmd.cmd = CMD_DISASM_MODE;
	scmd.data = &disasm_mode;
	ShowCommand(&scmd);
}
*/


void CmdGetCode(struct cmd *cmd) {
	/* get data from hardware */
	sp = CreatePacket(sizeof(struct cmd));
	sp->cmd = CMD_GET_DATA;
	memcpy((char *)sp+PACKET_HDR_SIZE, cmd, sizeof(struct cmd));
	
	SendPacket(sp);				/* send request for data */
	rp = RecvPacket();			/* receive data */

	/* show disassembly output */
	cmd->data = CreateDisasm(cmd->addr1, (unsigned char *)rp+PACKET_HDR_SIZE,
							rp->size);
	ShowCommand(cmd);

	/* destroy disassembly and packets */
	DestroyDisasm(cmd->data);
	DestroyPacket(sp);
	DestroyPacket(rp);
}


void CmdSetCode(struct cmd *cmd) {
	printf("%s\n", cmd->data);

	//ParseSFile(cmd->data, 
	
	free(cmd->data);
}


void CmdGetData(struct cmd *cmd) {
	/* get data from hardware */
	sp = CreatePacket(sizeof(struct cmd));
	sp->cmd = CMD_GET_DATA;
	memcpy((char *)sp+PACKET_HDR_SIZE, cmd, sizeof(struct cmd));
	
	SendPacket(sp);				/* send request for data */
	rp = RecvPacket();			/* receive data */

	/* show data output */
	cmd->dsize = rp->size;
	cmd->data = ((unsigned char *)rp)+PACKET_HDR_SIZE;
	ShowCommand(cmd);

	/* destroy packets */
	DestroyPacket(sp);
	DestroyPacket(rp);
}


void CmdSetData(struct cmd *cmd) {
	/* tell HC11 we'll be uploading data soon */
	sp = CreatePacket(sizeof(struct cmd));
	sp->cmd = CMD_SET_DATA;
	memcpy((char *)sp+PACKET_HDR_SIZE, cmd, sizeof(struct cmd));
	SendPacket(sp);
	DestroyPacket(sp);
	
	/* create packet with the data to upload and send it to HC11 */
	sp = CreatePacket(cmd->dsize);
	memcpy((char *)sp+PACKET_HDR_SIZE, cmd->data, cmd->dsize);
	SendPacket(sp);
	DestroyPacket(sp);

	/* free data */
	free(cmd->data);
}


void CmdGetState(struct cmd *cmd) {
	printf("not implemented\n");

	/* show state output */
	ShowCommand(cmd);
}


/* Change the program counter at HC11. */
void CmdExec(struct cmd *cmd) {
	sp = CreatePacket(sizeof(struct cmd));
	sp->cmd = CMD_EXEC;
	memcpy((char *)sp+PACKET_HDR_SIZE, cmd, sizeof(struct cmd));
	SendPacket(sp);
	DestroyPacket(sp);
}


/* Clear screen. */
void CmdCls(struct cmd *cmd) {
	ShowCommand(cmd);
}


/* Show help on different commands. */
void CmdHelp(struct cmd *cmd) {
	ShowCommand(cmd);
}


void CmdSyntaxErr(struct cmd *cmd) {
	ShowCommand(cmd);
}


void CmdQuit(struct cmd *cmd) {
	Cleanup();
	exit(0);
}


/* Init() initializes all the different parts of the program. If anything goes
 * wrong it cleans up and returns. */
int Init(void) {
	/* install signal handlers */
	signal(SIGHUP, SigDef);
	signal(SIGINT, SigDef);
	signal(SIGKILL, SigDef);

	/* initialize UI */
	if (InitUI() == -1) goto UI_fail;

	/* setup communications */
	if (InitComm() == -1) goto Comm_fail;

	/* install monitor */
	if (InstallMonitor() == -1)	goto Monitor_fail;
	
	return 0;

  Monitor_fail:
	CleanupComm();
  Comm_fail:
	CleanupUI();
  UI_fail:
	return -1;
}


/* Cleanup() resets everything so that we can exit cleanly. */
void Cleanup(void) {
	/* shut down communications */
	CleanupComm();

	/* cleanup UI */
	CleanupUI();
}


void SigDef(int signum) {
	printf("signal %i\n", signum);
	Cleanup();
	exit(-2);
}


void Usage(void) {
	printf("Usage: Not implemented\n");
	fflush(stdout);
	exit(1);
}


/* Error() displays errno and exits. Currently used only in main if Init() fails
 * since it is the only routine that doesn't need to do a Cleanup(). */
void Error(char *err) {
	perror(err);
	exit(2);
}


/* Perror displays errno, does a Cleanup() and exits. (pretty obvious!!) */
void Perror(char *err) {
	perror(err);
	Cleanup();
	exit(3);
}
