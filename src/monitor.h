
#ifndef MONITOR_H
#define MONITOR_H


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

/* MCU modes of operation (don't change the numerical values) */
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
	unsigned short size;		/* size of packet data (not header) */
	unsigned short csum;		/* packet checksum */
};


#define CMD_HW_OK			98
#define CMD_HW_ERR			99


int InstallMonitor(struct mcu_env *env);
int ConfigEnvOptions(struct mcu_env *env, char *buf);
struct packet *CreatePacket(int data_size);
int DestroyPacket(struct packet *p);
int SendPacket(struct packet *p);
struct packet *RecvPacket(void);
int Csum(struct packet *p);


#endif MONITOR_H
