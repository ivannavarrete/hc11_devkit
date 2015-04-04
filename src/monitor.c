
/* This module handles all the communication with the monitor in the HC11.
 * It uses SendData() and RecvData() for transmission of data. This module
 * should be the only one using SendData() and RecvData(). Other modules
 * should use the public routines.
 *
 * public routines:
 *		InstallMonitor() installs the monitor at the HC11.
 *		CreatePacket() allocates memory for a packet.
 *		DestroyPacket() destroys a previously created packet.
 *		SendPacket() sends a packet to the monitor.
 *		RecvPacket() receives a packet from the monitor.
 * private routines:
 *		ConfigEnvOptions() configures the setup program.
 *
 * todo: this module should not use GetEnvOptions() directly. It's better if
 * all access to UI is done by the main module... */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "monitor.h"
#include "sfile.h"


int InstallMonitor();
int ConfigEnvOptions(void);
struct packet *CreatePacket(int data_size);
int DestroyPacket(struct packet *p);
int SendPacket(struct packet *p);
struct packet *RecvPacket(void);

/* sfile.c */
extern int ParseSFile(char *file, char *buf, int options);
/* serial.c */
extern int ConfigComm(int baud);
extern int SendData(const unsigned char *buf, int len);
extern int RecvData(unsigned char *buf, int len);
/* ui*.c */
extern int GetEnvOptions(struct mcu_env *env);


unsigned char *send_buf;
int com_port = 2;
int mcu_clock = 8;			/* 8Mhz */

struct mcu_env env = {
	MCU_MODE_EXPANDED,	/* mcu mode */
	0,					/* on-chip ROM disabled */
	1,					/* on-chip EEPROM enabled */
	0,					/* registers page; mapped to 0x0000 */
	0,					/* on-chip RAM page; mapped to 0x0000 */
	0,					/* talker page; mapped to 0x0100-0x1FFF */
	0,					/* timer rate */
	0,					/* COP rate */
	0,					/* irq pin is level triggered */
	1					/* osc delay enabled */
};

//struct token tok = { 1, 0x55AA55AA, 0xAA55AA55 };
int r;


/* Install the monitor at the hw end. */
int InstallMonitor() {
	/* prepare the setup code, before uploading it */
	send_buf = (char *)malloc(257);
	ParseSFile("./hc11/setup.s19", send_buf+1, SFILE_UNCOMPRESSED);
	GetEnvOptions(&env);
	ConfigEnvOptions();

	/* upload the MCU setup code */
	if (SendData(send_buf, 257) == -1) {
		free(send_buf);
		return -1;
	}

	/* set a higher baud rate */
	ConfigComm(9600);

	/* upload int vectors and monitor code */
	send_buf = realloc(send_buf, 0x2000);
	if (send_buf == NULL) return -1;
	ParseSFile("./hc11/monitor.s19", send_buf, SFILE_UNCOMPRESSED);
	SendData(send_buf, 0x40);
	SendData(send_buf+0x100, 0x1EFF);

	free(send_buf);
	return 0;
}


/* Alter monitor setup code, to reflect user's choise of environment, and con-
 * figuration of time protected systems. */
int ConfigEnvOptions(void) {
	send_buf[1+0xFA] = env.talker_page<<4;
	send_buf[1+0xFA] |= env.mode;
	send_buf[1+0xFB] = env.ram_page<<4 | env.reg_page;
	send_buf[1+0xFC] |= env.on_chip_eeprom;
	send_buf[1+0xFC] |= env.on_chip_rom<<1;
	send_buf[1+0xFD] |= env.irq_mode;
	send_buf[1+0xFD] |= env.osc_delay<<1;
	send_buf[1+0xFE] = env.timer_rate;
	send_buf[1+0xFF] = env.cop_rate;
	send_buf[0] = 0xFF;				/* accept bootstrap code baud rate */
		
	return 0;
}


struct packet *CreatePacket(int data_size) {
	struct packet *p;
	
	p = malloc(sizeof(struct packet) + data_size);
	if (p == NULL) return p;

	p->id[0] = 'M';
	p->id[1] = 'C';
	p->id[2] = 'P';
	return p;
}


int DestroyPacket(struct packet *p) {
	free(p);
	return 0;
}


int SendPacket(struct packet *p) {
	SendData((unsigned char *)p, PACKET_HDR_SIZE + p->size);
	return 0;
}


struct packet *RecvPacket(void) {
	struct packet *p;
	//unsigned char launch[] = {0x55, 0xAA};

	p = malloc(PACKET_HDR_SIZE);
	
	/* tell HC11 to start transmission of header */
	//SendData(launch, 2);
	RecvData((unsigned char *)p, PACKET_HDR_SIZE);
	/* resize packet to make room for incoming packet data */
	p = realloc(p, PACKET_HDR_SIZE + p->size);
	/* receive rest of packet */
	//SendData(launch, 2);
	RecvData((unsigned char *)p + PACKET_HDR_SIZE, p->size);
	
	return p;
}
