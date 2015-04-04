
#ifndef MESSAGE_H
#define MESSAGE_H


/* request/responce structure */
struct ui_msg {
	long mtype;
	int saddr;
	int eaddr;
	unsigned char data[256];
};

/* environment options message format */
#define ENV_TALKER_PAGE		0
#define ENV_RAM_PAGE		1
#define ENV_REG_PAGE		2
#define ENV_EEPROM			3
#define ENV_ROM				4
#define ENV_IRQ_MODE		5
#define ENV_OSC_DELAY		6
#define ENV_TIMER_RATE		7
#define ENV_COP_RATE		8
#define ENV_MCU_MODE		9

/* requests to UI */
#define UI_REQ_ENV_OPT		1
#define UI_GET_COMMAND		2
#define UI_SHOW_CODE		96
#define UI_SHOW_DATA		97
#define UI_SHOW_MSG			98
#define UI_SHUTDOWN			99
/* responces from UI */
#define UI_RES_ENV_OPT		101


/* macros for communicating with UI */
#define MSGSND(type) \
	ui_msg.mtype=type;\
	msgsnd(ui_sendmsg, &ui_msg, sizeof(ui_msg), 0);\
	/*printf("send on %i: %i\n", ui_sendmsg, type);*/
#define MSGRCV(type) \
	msgrcv(ui_recvmsg, &ui_msg, sizeof(ui_msg), type, 0);\
	/*printf("recv on %i: %li\n", ui_recvmsg, ui_msg.mtype);*/


#endif MESSAGE_H
