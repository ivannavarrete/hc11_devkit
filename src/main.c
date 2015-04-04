
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

#include "ui/message.h"


int Init(void);
void Cleanup(void);

void Usage(void);
void Error(char *err);
void Perror(char *err);
void SigDef(int signum);
void SigUsr(int signum);

extern int InitComm();
extern int CleanupComm(int dev);
extern int InstallMonitor(int dev);
extern int Command();


char *send_buf;
char *recv_buf;
int shmid;

int dev;					/* comm device, for communication with hardware */
/* ui communications */
int ui_sendmsg, ui_recvmsg, ui_tmpsend;
struct ui_msg ui_msg;
int ui_pid;


int main(int argc, char **argv) {
	if (Init() == -1) Perror("Init");

	MSGSND(UI_GET_COMMAND);

//	ui_msg.datalen = 0x44;
//	ui_msg.addr = 0x20;
//	MSGSND(UI_SHOW_DATA);

//	MSGSND(UI_SHOW_CODE);

	Cleanup();
	return 0;
}


int Init(void) {
	char *ui_arg[] = { "./ui", NULL };
	//char *ui_env[] = { "PWD=./bin", NULL };
	key_t key;
	
	/* install signal handlers */
	signal(SIGHUP, SigDef);
	signal(SIGINT, SigDef);
	signal(SIGKILL, SigDef);

	/* make the send buffer shared among processes */
	//shmid = shmget(0, 257, IPC_CREAT|0600);
	//if (shmid == -1) Perror("shmget()");
	//send_buf = shmat(shmid, 0, 0);

	/* setup communications */
	dev = InitComm();
	if (dev == -1) Perror("InitComm");

	/* start user interface */
	ui_pid = fork();
	if (ui_pid == 0) {
		execve(ui_arg[0], ui_arg, NULL);
		Perror("Init");
	} else if (ui_pid == -1) Perror("ui");

	key = ftok("./ui", 'a');
	ui_sendmsg = ui_tmpsend = msgget(key, IPC_CREAT|0644);
	key = ftok("./devsys", 'b');
	ui_recvmsg = msgget(key, IPC_CREAT|0644);

	/* install monitor */
	InstallMonitor(dev);
	
	return 0;
}


void Cleanup(void) {
	struct msqid_ds msq_ds;

	/* shut down communications */
	CleanupComm(dev);

	/* delete shared data */
	//shmdt(send_buf);
	//shmctl(shmid, IPC_RMID, NULL);

	/* close ui communications */
	MSGSND(UI_SHUTDOWN);
	do {		/* wait for UI to receive SHUTDOWN message */
		msgctl(ui_sendmsg, IPC_STAT, &msq_ds);
	} while (msq_ds.msg_qnum != 0);
	msgctl(ui_sendmsg, IPC_RMID, NULL);
	msgctl(ui_recvmsg, IPC_RMID, NULL);
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


void Error(char *err) {
	printf("%s\n", err);
	Cleanup();
	exit(2);
}

void Perror(char *err) {
	perror(err);
	Cleanup();
	exit(3);
}
