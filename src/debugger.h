
//
// debugger.h
//
// This object handles all the packet communication with the debugger in the
// HC11. It uses a Serial object for transmission of data. This object should
// be the only one using a Serial object (xcept for main, which creates it).
//


#ifndef MONITOR_H
#define MONITOR_H

#include "serial.h"


class Debugger {
public:
	Debugger(Serial *serial, struct mcu_env *env); // concstructor
	struct packet *CreatePacket(int data_size);		// create packet
	int DestroyPacket(struct packet *p);			// destroy packet
	int SendPacket(struct packet *p);				// send packet to hc11
	struct packet *RecvPacket(void);				// receive packet from hc11
private:
	void ConfigEnvOptions(struct mcu_env *env, unsigned char *buf);
	int Csum(struct packet *p);			// calculate packet csum.

private:
	Serial *serial;						// object user for comm. with hc11
};


struct mcu_env {
	char mode;
	char on_chip_rom;
	char on_chip_eeprom;
	char reg_page;
	char ram_page;
	char talker_page;
	char timer_rate;
	char cop_rate;
	char irq_mode;
	char osc_delay;
};

// MCU modes of operation (don't change the numerical values)
#define MCU_MODE_SINGLECHIP	0
#define MCU_MODE_EXPANDED	1
#define MCU_MODE_BOOTSTRAP	2
#define MCU_MODE_TEST		3

#define MONITOR_START		0x100
#define MONITOR_SIZE		0x1EFF


#define PACKET_HDR_SIZE		8
struct packet {
	unsigned char id[3];
	unsigned char cmd;
	unsigned short size;		// size of packet data (not header)
	unsigned short csum;		// packet checksum
};


#define CMD_HW_OK			98
#define CMD_HW_ERR			99


#endif // MONITOR_H
