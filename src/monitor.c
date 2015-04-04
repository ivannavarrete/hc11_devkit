
/*
 * monitor.c
 *
 * This module handles all the packet communication with the monitor in the
 * HC11. It uses SendData() and RecvData() for transmission of data. This
 * module should be the only one using SendData() and RecvData().
 *
 * main.c uses InstallMonitor(), CreatePacket(), DestroyPacket(), SendPacket(),
 *			RecvPacket().
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
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/in.h>			/* htons() */

#include "monitor.h"
#include "sfile.h"
#include "ui_txt.h"
#include "serial.h"


int com_port = 2;
int mcu_clock = 8;			/* 8Mhz */


/* Install the monitor at the hw end. */
int InstallMonitor(struct mcu_env *env) {
	struct sfile *sf;
	char accept_rate = 0xFF;
	int r1, r2;
	
	/* prepare the setup code, before uploading it */
	sf = CreateSFile("./hc11/setup.s19", SFILE_UNCOMPRESSED);
	if (sf == NULL)
		return -1;
	ConfigEnvOptions(env, sf->data);

	/* upload the MCU setup code */
	r1 = SendData(&accept_rate, 1);
	r2 = SendData(sf->data, sf->dsize);
	DestroySFile(sf);
	if (r1 == -1 || r2 == -1)
		return -1;

	/* set a higher baud rate */
	ConfigComm(9600);

	/* upload int vectors and monitor code */
	sf = CreateSFile("./hc11/monitor.s19", SFILE_UNCOMPRESSED);
	if (sf == NULL)
		return -1;
	r1 = SendData(sf->data+0x1FC0, 0x40);
	r2 = SendData(sf->data+0x100, 0x1EBF);
	DestroySFile(sf);
	if (r1 == -1 || r2 == -1)
		return -1;
	
	FlushDev();
	return 0;
}


/* Alter monitor setup code, to reflect user's choise of environment, and con-
 * figuration of time protected systems. */
int ConfigEnvOptions(struct mcu_env *env, char *buf) {
	buf[0xFA] = env->talker_page<<4;
	buf[0xFA] |= env->mode;
	buf[0xFB] = env->ram_page<<4 | env->reg_page;
	buf[0xFC] |= env->on_chip_eeprom;
	buf[0xFC] |= env->on_chip_rom<<1;
	buf[0xFD] |= env->irq_mode;
	buf[0xFD] |= env->osc_delay<<1;
	buf[0xFE] = env->timer_rate;
	buf[0xFF] = env->cop_rate;
		
	return 0;
}


/* Allocate memory for a packet and fill in the fields that are known. */
struct packet *CreatePacket(int data_size) {
	struct packet *p;
	
	p = malloc(sizeof(struct packet) + data_size);
	if (p == NULL)
		return p;

	p->id[0] = 'M';
	p->id[1] = 'C';
	p->id[2] = 'P';
	p->size = data_size;

	return p;
}


int DestroyPacket(struct packet *p) {
	free(p);
	return 0;
}


int SendPacket(struct packet *p) {
	unsigned short csum;
//	int i;

	p->csum = Csum(p);

/*
	printf("SendPacket()\nsending command:        ");
	for (i=0; i<PACKET_HDR_SIZE+p->size; i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	printf("\n");
*/

	p->size = htons(p->size);		/* convert size field */
	SendData((unsigned char *)p, PACKET_HDR_SIZE + ntohs(p->size));
	RecvData((unsigned char *)p, PACKET_HDR_SIZE);
	csum = Csum(p);

/*
	printf("receiving confirmation: ");
	for (i=0; i<PACKET_HDR_SIZE+ntohs(p->size); i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	printf("\n");
*/
	if (csum == p->csum)
//		printf("packet ok\n\n");
	;
	else
		printf("packet error\n\n");


	return 0;
}


struct packet *RecvPacket(void) {
	struct packet *p;
//	int i;

	p = malloc(PACKET_HDR_SIZE);

	if (RecvData((unsigned char *)p, PACKET_HDR_SIZE) == -1) {
		DestroyPacket(p);
		return NULL;
	}
	
	p->size = ntohs(p->size);

/*
	printf("RecvPacket()\nreceiving packet hdr:  ");
	for (i=0; i<PACKET_HDR_SIZE; i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	printf("\n");
*/
	
	/* resize packet to make room for incoming packet data */
	p = realloc(p, PACKET_HDR_SIZE + p->size);

	/* receive rest of packet */
	if (RecvData((unsigned char *)p + PACKET_HDR_SIZE, p->size) == -1) {
		DestroyPacket(p);
		return NULL;
	}

/*
	printf("receiving packet data: ");
	for (i=0; i<(p->size)%8; i++)
		printf("%02X ", ((unsigned char *)p)[PACKET_HDR_SIZE+i]);
	printf("\n");
*/

	return p;
}


/* Calculate checksum, on packet header only. */
int Csum(struct packet *p) {
	unsigned short csum, csum_tmp;
	int i;

	csum_tmp = p->csum;
	p->csum = 0;

	for (i=csum=0; i<=PACKET_HDR_SIZE-2+p->size; i++)
		csum += (((char *)p)[i])<<8 | ((char *)p)[i+1];
	
	p->csum = csum_tmp;
	return htons(csum);
}
