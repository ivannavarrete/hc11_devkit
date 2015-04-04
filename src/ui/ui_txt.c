
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui_txt.h"
#include "message.h"
#include "../monitor.h"
#include "disasm.h"


int InitUI();
int CleanupUI();
int ConfigUI();
int GetCommand();
int GetEnvOptions();
int ShowMCUState();
int ShowData();
int ShowCode();
int ShowMsg();
int BadMsg();

char GetHex(char c);
void SigDef(int signum);

extern char *Disasm(unsigned char *code, int len, int mode);


/* ui communications */
int ui_recvmsg, ui_sendmsg, ui_tmpsend;
struct ui_msg ui_msg;

int codemode;
int res;


void main(void) {
	InitUI();

	/* handle incoming messages */
	while (1) {
		/* BUG WARNING 1.1 */
		MSGRCV(0);
		ui_sendmsg = ui_tmpsend;

		switch (ui_msg.mtype) {
			case UI_GET_COMMAND: GetCommand(); break;
			case UI_REQ_ENV_OPT: GetEnvOptions(); break;
			case UI_SHOW_DATA: ShowData(); break;
			case UI_SHOW_CODE: ShowCode(); break;
			case UI_SHOW_MSG: ShowMsg(); break;
			case UI_SHUTDOWN: CleanupUI(); exit(-1);
			default: BadMsg();
		}
	}
}


int GetCommand() {
	char cmd[81];
	int i;
	
	/* get command */
	while (1) {
		fgets(cmd, 80, stdin);
		for (i=0; i<strlen(cmd); i++)
			tolower((int)cmd[i]);

		if (!strcmp(cmd, "code on\n"))
			codemode |= DMODE_CODE;
		else if (!strcmp(cmd, "code off\n"))
			codemode &= ~DMODE_CODE;
		else if (!strcmp(cmd, "d\n"))
			ShowCode();
		else if (!strcmp(cmd, "c\n"))
			ShowData();
		else if (!strcmp(cmd, "q\n")) {
			CleanupUI();
			exit(-1);
		} else
			printf("syntax error\n");
	}
	
	return 0;
}


int InitUI() {
	key_t key;
	int i;

	/* install signal handlers */
	signal(SIGHUP, SigDef);
	signal(SIGINT, SigDef);
	signal(SIGKILL, SigDef);

	/* get msg queue id */
	key = ftok("./devsys", 'b');
	ui_sendmsg = ui_tmpsend = msgget(key, 0);		/* BUG WARNING 1.2 */
	key = ftok("./ui", 'a');
	ui_recvmsg = msgget(key, 0);

	/* clear screen */
	for (i=0; i<28; i++)
		printf("\n");

	return 0;
}


/* not implemented */
/* Closes down UI and prepares for exit() call. exit() is done outside of this
 * routine. */
int CleanupUI() {
	return 0;
}


/* Gets the MCU environment options from user and sends them back to main
 * program. */
int GetEnvOptions() {
	char c;
	char buf[10];
	
	printf("*** environment options ***\n");

	do {
		printf("    MCU mode [s/e/b/t] [%s]: ",
			ui_msg.data[ENV_MCU_MODE]==MCU_MODE_BOOTSTRAP?"b":
			(ui_msg.data[ENV_MCU_MODE]==MCU_MODE_TEST?"t":
			(ui_msg.data[ENV_MCU_MODE]==MCU_MODE_EXPANDED?"e":"s")));
		fgets(buf, 3, stdin);
		if (buf[0]=='\n') break;
		if (buf[1]!=0xA) continue;
		switch (buf[0]) {
			case 's': ui_msg.data[ENV_MCU_MODE] = MCU_MODE_SINGLECHIP;break;
			case 'e': ui_msg.data[ENV_MCU_MODE] = MCU_MODE_EXPANDED; break;
			case 'b': ui_msg.data[ENV_MCU_MODE] = MCU_MODE_BOOTSTRAP; break;
			case 't': ui_msg.data[ENV_MCU_MODE] = MCU_MODE_TEST; break;
			default: continue;
		}
		break;
	} while (1);
	
	do {
		printf("    talker page [0-F] [%01X]: ", ui_msg.data[ENV_TALKER_PAGE]);
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
			ui_msg.data[ENV_TALKER_PAGE] = c;
			break;
		}
	} while (1);
	
	do {
		printf("    RAM page [0-F] [%01X]: ", ui_msg.data[ENV_RAM_PAGE]);
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((c>=0 && c<=0xF) && (c<=9 || c>=0xA))	&& buf[1]==0xA) {
			ui_msg.data[ENV_RAM_PAGE] = c;
			break;
		}
	} while (1);
	
	do {
		printf("    register page [0-F] [%01X]: ", ui_msg.data[ENV_REG_PAGE]);
		fgets(buf, 3, stdin);
		c = GetHex(buf[0]);
		if (buf[0]=='\n') break;
		else if (((buf[0]>='0' && buf[0]<='F') && (buf[0]<='9' || buf[0]>='A'))
				&& buf[1]==0xA) {
			ui_msg.data[ENV_REG_PAGE] = c;
			break;
		}
	} while (1);
	
	do {
		printf("    on-chip EEPROM [on/off] [%s]: ",
				ui_msg.data[ENV_EEPROM]?"on":"off");
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			ui_msg.data[ENV_EEPROM] = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			ui_msg.data[ENV_EEPROM] = 0;
			break;
		}
	} while (1);

	do {
		printf("    on-chip ROM [on/off] [%s]: ",
				ui_msg.data[ENV_ROM]?"on":"off");
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			ui_msg.data[ENV_ROM] = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			ui_msg.data[ENV_ROM] = 0;
			break;
		}
	} while (1);

	do {
		printf("    irq mode [edge/level] [%s]: ",
				ui_msg.data[ENV_IRQ_MODE]?"edge":"level");
		fgets(buf, 7, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "edge\n")) {
			ui_msg.data[ENV_IRQ_MODE] = 1;
			break;
		}
		else if (!strcmp(buf, "level\n")) {
			ui_msg.data[ENV_IRQ_MODE] = 0;
			break;
		}
	} while (1);

	
	do {
		printf("    osc delay [on/off] [%s]: ",
				ui_msg.data[ENV_OSC_DELAY]?"on":"off");
		fgets(buf, 5, stdin);
		if (buf[0]=='\n') break;
		else if (!strcmp(buf, "on\n")) {
			ui_msg.data[ENV_OSC_DELAY] = 1;
			break;
		}
		else if (!strcmp(buf, "off\n")) {
			ui_msg.data[ENV_OSC_DELAY] = 0;
			break;
		}
	} while (1);

	printf("    timer prescaler rate [1/4/8/16] []: \n");
	
	printf("    COP prescaler rate [1/4/16/64] []: \n\n");

	MSGSND(UI_RES_ENV_OPT);
	return 0;
}


/* not implemented */
int ConfigUI() {
	return 0;
}


int ShowData() {
	int i, j, k=0;

	ui_msg.eaddr>256 ? (k=256) : (k=(ui_msg.eaddr%0x10)*0x10);

	for (i=j=0; i<k; i+=0x10) {
		printf("   %04X  ", ui_msg.saddr+i);
		for (j=i; j<i+0x10; j++)
			printf("%02X ", ui_msg.data[j]);
		printf("   ");
		for (j=i; j<i+0x10; j++)
			printf("%c", ui_msg.data[j]);
		printf("\n");
	}
	fflush(stdout);

	return 0;
}


int ShowCode() {
	char *buf;
	int len;

	len = ui_msg.eaddr - ui_msg.saddr;

	buf = Disasm(ui_msg.data, len, codemode);
	printf("%s", buf);
	free(buf);
	
	return 0;
}


int ShowMsg() {
	printf("%s\n", ui_msg.data);
	fflush(stdout);
	return 0;
}


/* not implemented */
/* Invoked when UI receives a message with incorrect message type. */
int BadMsg() {
	printf("very very bad...\n");
	fflush(stdout);
	
	return 0;
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


/* Default signal handler. Shuts down UI. */
void SigDef(int signum) {
	printf("UI signal %i\n", signum);
	fflush(stdout);
	
	MSGRCV(UI_SHUTDOWN);
	CleanupUI();
	exit(-2);
}
