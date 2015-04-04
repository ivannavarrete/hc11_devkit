
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
void CmdCode(struct cmd *gcmd);
void CmdData(struct cmd *gcmd);
void CmdState(struct cmd *gcmd);
void CmdCls(struct cmd *gcmd);
void CmdHelp(struct cmd *gcmd);
void CmdQuit(struct cmd *gcmd);
void CmdSyntaxErr(struct cmd *gcmd);

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
extern struct dis_instr *CreateDisasm(unsigned char *code, int len);
extern int DestroyDisasm(struct dis_instr *da_list);
/* ui*.c */
extern int InitUI(void);
extern int CleanupUI(void);
extern int GetCommand(struct cmd *cmd);
extern int ShowCommand(struct cmd *cmd);


int disasm_mode;			/* disassembly output mode */

struct cmd gcmd;			/* message struct, UI->main */
struct cmd scmd;			/* message struct, main->UI */
/* send and receive packet structs, for communication with hw */
struct packet *sp, *rp;


void main(int argc, char **argv) {
	if (Init() == -1) Error("Init()");

	/* command loop */
	do {
		GetCommand(&gcmd);		/* get command */

		switch (gcmd.cmd) {		/* execute command */
	//		case CMD_C_CODE_ON: CmdCChangeMode(DMODE_CODE, 1); break;
	//		case CMD_C_CODE_OFF: CmdCChangeMode(DMODE_CODE, 0); break;
	//		case CMD_C_BYTES_ON: CmdCChangeMode(DMODE_BYTES, 1); break;
	//		case CMD_C_BYTES_OFF: CmdCChangeMode(DMODE_BYTES, 0); break;
	//		case CMD_C_CYCLES_ON: CmdCChangeMode(DMODE_CYCLES, 1); break;
	//		case CMD_C_CYCLES_OFF: CmdCChangeMode(DMODE_CYCLES, 0); break;
	//		case CMD_DISASM_MODE: CmdCShowMode(); break;
			case CMD_CODE: CmdCode(&gcmd); break;
			case CMD_DATA: CmdData(&gcmd); break;
			case CMD_STATE: CmdState(&gcmd); break;
			case CMD_CLS: CmdCls(&gcmd); break;
			case CMD_HELP: CmdHelp(&gcmd); break;
			case CMD_QUIT: CmdQuit(&gcmd); break;
			case CMD_SYNTAX_ERR: CmdSyntaxErr(&gcmd); break;
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


void CmdCode(struct cmd *gcmd) {
	/* Get, say 100 bytes from hardware */
	sp = CreatePacket(0);
	sp->cmd = CMD_HW_GET_DATA;
	sp->size = 0;
	sp->csum = 0;
	SendPacket(sp);
	DestroyPacket(sp);
	rp = RecvPacket();

	/* show disassembly output */
	scmd.cmd = CMD_CODE;
	scmd.data = CreateDisasm((unsigned char *)(rp+PACKET_HDR_SIZE), rp->size);
	ShowCommand(&scmd);
	DestroyDisasm(scmd.data);
	DestroyPacket(rp);
}


void CmdData(struct cmd *gcmd) {
	/* get data from hardware */
	sp = CreatePacket(0);
	sp->cmd = CMD_HW_GET_DATA;
	sp->size = 0;
	sp->csum = 0;
	SendPacket(sp);
	DestroyPacket(sp);
	rp = RecvPacket();

	/* show data output */
	scmd.cmd = CMD_DATA;
	scmd.mod = gcmd->mod;
	scmd.addr1 = gcmd->addr1;
	scmd.addr2 = gcmd->addr2;
	scmd.dsize = rp->size;
	scmd.data = rp+PACKET_HDR_SIZE;
	ShowCommand(&scmd);
	DestroyPacket(rp);
}


void CmdState(struct cmd *gcmd) {
	/* show state output */
	ShowCommand(gcmd);
}


void CmdCls(struct cmd *gcmd) {
	scmd.cmd = CMD_CLS;
	ShowCommand(&scmd);
}


void CmdHelp(struct cmd *gcmd) {
	scmd.cmd = CMD_HELP;
	scmd.mod = gcmd->mod;
	ShowCommand(&scmd);
}


void CmdSyntaxErr(struct cmd *gcmd) {
	scmd.cmd = CMD_SYNTAX_ERR;
	ShowCommand(&scmd);
}


void CmdQuit(struct cmd *gcmd) {
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
	printf("devsys signal %i\n", signum);
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
