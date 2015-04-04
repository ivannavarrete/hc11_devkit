
//
// debugger.cpp
//


#include <iostream>
#include <cstring>


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/in.h>			// htons()

#include "debugger.h"
#include "error.h"
#include "sfile.h"
#include "ui.h"
#include "serial.h"


extern UI &ui;					// global user interface object


// Constructor.
// Install the monitor at the hw end.
Debugger::Debugger(Serial *serial, struct mcu_env *env) {
	unsigned char accept_rate = 0xFF;
	int r1, r2;

	this->serial = serial;

	ui.ShowMsg("installing debugger, please wait ...\n");
	
	try {
		// prepare the setup code, before uploading it
		SFile sf_setup = SFile("./hc11/setup.s19", SFILE_UNCOMPRESSED);
	//	if (sf == NULL)
	//		return -1;
	//	if (sf_setup.GetDataSize() != 256)
	//		// do somethin nasty
		ConfigEnvOptions(env, sf_setup.GetData());

		// upload the MCU setup code
		r1 = serial->SendData(&accept_rate, 1);
		r2 = serial->SendData(sf_setup.GetData(), sf_setup.GetDataSize());
	//	DestroySFile(sf);
	//	if (r1 == -1 || r2 == -1)
	//		return -1;

		// set a higher baud rate
		serial->ConfigComm(9600);

		// upload int vectors and monitor code
		SFile sf_monitor = SFile("./hc11/debugger.s19", SFILE_UNCOMPRESSED);
	//	if (sf == NULL)
	//		return -1;
		r1 = serial->SendData(sf_monitor.GetData()+0x1FC0, 0x40);// int vectors
		r2 = serial->SendData(sf_monitor.GetData()+0x100, 0x1EBF);// debugger
	//	DestroySFile(sf);
	//	if (r1 == -1 || r2 == -1)
	//		return -1;
	
		serial->FlushDev();
	} catch (Error &err) {
		throw;
	}
}


// Alter monitor setup code, to reflect user's choise of environment, and con-
// figuration of time protected systems. See setup.asm for more info.
void Debugger::ConfigEnvOptions(struct mcu_env *env, unsigned char *buf) {
	buf[0xFA] = env->talker_page<<4;
	buf[0xFA] |= env->mode;
	buf[0xFB] = env->ram_page<<4 | env->reg_page;
	buf[0xFC] |= env->on_chip_eeprom;
	buf[0xFC] |= env->on_chip_rom<<1;
	buf[0xFD] |= env->irq_mode;
	buf[0xFD] |= env->osc_delay<<1;
	buf[0xFE] = env->timer_rate;
	buf[0xFF] = env->cop_rate;
}


// Allocate memory for a packet and fill in the fields that are known.
struct packet *Debugger::CreatePacket(int data_size) {
	struct packet *p;

	p = new packet;
//	p = malloc(sizeof(struct packet) + data_size);
//	if (p == NULL)
//		return p;

//	p->id[0] = 'M';
//	p->id[1] = 'C';
//	p->id[2] = 'P';
//	p->size = data_size;

	return p;
}


int Debugger::DestroyPacket(struct packet *p) {
	free(p);
	return 0;
}


int Debugger::SendPacket(struct packet *p) {
	unsigned short csum;
//	int i;

	p->csum = Csum(p);

/*
	cout << "SendPacket()\nsending command:        ";
	for (i=0; i<PACKET_HDR_SIZE+p->size; i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	cout << endl;
*/

//	p->size = htons(p->size);		// convert size field
//	serial.SendData((unsigned char *)p, PACKET_HDR_SIZE + ntohs(p->size));
//	serial.RecvData((unsigned char *)p, PACKET_HDR_SIZE);
	csum = Csum(p);

/*
	cout << "receiving confirmation: ";
	for (i=0; i<PACKET_HDR_SIZE+ntohs(p->size); i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	cout << "\n";
*/
//	if (csum == p->csum)
//		cout << "packet ok\n\n";
//	;
//	else
//		cout << "packet error\n\n";


	return 0;
}


struct packet *Debugger::RecvPacket(void) {
	struct packet *p;
//	int i;

	p = (struct packet *)malloc(PACKET_HDR_SIZE);

//	if (serial.RecvData((unsigned char *)p, PACKET_HDR_SIZE) == -1) {
//		DestroyPacket(p);
//		return NULL;
//	}
	
	p->size = ntohs(p->size);

/*
	cout << "RecvPacket()\nreceiving packet hdr:  ";
	for (i=0; i<PACKET_HDR_SIZE; i++)
		printf("%02X ", ((unsigned char *)p)[i]);
	cout << "\n";
*/
	
	// resize packet to make room for incoming packet data
	p = (struct packet *)realloc(p, PACKET_HDR_SIZE + p->size);

	// receive rest of packet
//	if (serial.RecvData((unsigned char *)p + PACKET_HDR_SIZE, p->size) == -1) {
//		DestroyPacket(p);
//		return NULL;
//	}

/*
	cout << "receiving packet data: ";
	for (i=0; i<(p->size)%8; i++)
		printf("%02X ", ((unsigned char *)p)[PACKET_HDR_SIZE+i]);
	cout << "\n";
*/

	return p;
}


// Calculate checksum, on packet header only.
int Debugger::Csum(struct packet *p) {
	unsigned short csum, csum_tmp;
	int i;

	csum_tmp = p->csum;
	p->csum = 0;

	for (i=csum=0; i<=PACKET_HDR_SIZE-2+p->size; i++)
		csum += (((char *)p)[i])<<8 | ((char *)p)[i+1];
	
	p->csum = csum_tmp;
	return htons(csum);
}
