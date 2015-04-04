
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "monitor.h"
#include "sfile.h"
#include "ui/message.h"


int InstallMonitor(int dev);
int GetEnvOptions(void);
int ConfigEnvOptions(void);

extern int ParseSFile(char *file, char *buf, int options);
extern int SendData(int dev, char *buf, int len);
extern int RecvData(int dev, char *buf, int len);

/* ui communication */
extern int ui_sendmsg, ui_recvmsg, ui_tmpsend;
extern struct ui_msg ui_msg;


int com_port = 2;
int mcu_clock = 8;			/* 8Mhz */

char mcu_mode = MCU_MODE_EXPANDED;
char on_chip_rom = 0;		/* on-chip ROM disabled */
char on_chip_eeprom = 1;	/* on-chip EEPROM enabled */
char reg_page = 0;			/* registers mapped to 0x0000 */
char ram_page = 0;			/* on-chip RAM mapped to 0x0000 */
char talker_page = 0;		/* talker mapped to 0x0100-0x1FFF */
char timer_rate = 0;
char cop_rate = 0;
char irq_mode = 0;			/* irq pin is level triggered */
char osc_delay = 1;			/* osc delay enabled */

unsigned char *send_buf;


/* Install the monitor at the hw end. */
int InstallMonitor(int dev) {
	int pid;
	
	/* prepare the setup code, before uploading it */
	send_buf = (char *)malloc(257);
	ParseSFile("./hc11/setup.s19", send_buf+1, SFILE_UNCOMPRESSED);
	GetEnvOptions();
	ConfigEnvOptions();

	/* show message */
	strcpy(ui_msg.data, "uploading monitor...");
	MSGSND(UI_SHOW_MSG);

	/* upload the MCU setup code */
	pid = fork();
	if (pid > 0) {
		/* parent uploads code to the MCU */
		if (SendData(dev, send_buf, 257) == -1) {
			free(send_buf);
			return -1;
		}
	} else if (pid == 0) {
		/* child verifies that code was successfully uploaded */
		/* Does not work yet. */
		/*
		for (i=res=0; i<256; i+=res) {
			res += read(dev, recv_buf+i, 256-i);
			if (res == -1) Perror("read()");
		}
		
		res = memcmp(send_buf+1, recv_buf, 256);
		if (res != 0) Error("Verification error");
		*/
		exit(0);
	} else if (pid == -1) {
		free(send_buf);
		return -1;
	}

	send_buf = realloc(send_buf, 0x1F40);
	if (send_buf == NULL) return -1;
	ParseSFile("./hc11/monitor.s19", send_buf, SFILE_UNCOMPRESSED);
	SendData(dev, send_buf, 0x40);			/* upload vectors */
	SendData(dev, send_buf+0x40, 0x1EFF);	/* upload monitor code */

	free(send_buf);
	return 0;
}


/* Send message to ui to let user input environment options. Return the options
 * to us and init the associated variables. */
int GetEnvOptions(void) {
	/* send old environment values to UI */
	ui_msg.data[ENV_MCU_MODE] = mcu_mode;
	ui_msg.data[ENV_TALKER_PAGE] = talker_page;
	ui_msg.data[ENV_RAM_PAGE] = ram_page;
	ui_msg.data[ENV_REG_PAGE] = reg_page;
	ui_msg.data[ENV_EEPROM] = on_chip_eeprom;
	ui_msg.data[ENV_ROM] = on_chip_rom;
	ui_msg.data[ENV_IRQ_MODE] = irq_mode;
	ui_msg.data[ENV_OSC_DELAY] = osc_delay;
	ui_msg.data[ENV_TIMER_RATE] = timer_rate;
	ui_msg.data[ENV_COP_RATE] = cop_rate;

	MSGSND(UI_REQ_ENV_OPT);
	/* the following call sets ui_sendmsg = ui_recvmsg for some reason */
	MSGRCV(UI_RES_ENV_OPT);
	/* BUG WARNING! Restore self-modifying variable */
	ui_sendmsg = ui_tmpsend;
	
	/*
	printf("mcu mode: %02X\n", ui_msg.data[ENV_MCU_MODE]);
	printf("talker page: %02X\n", ui_msg.data[ENV_TALKER_PAGE]);
	printf("RAM page: %02X\n", ui_msg.data[ENV_RAM_PAGE]);
	printf("register page: %02X\n", ui_msg.data[ENV_REG_PAGE]);
	printf("on-chip EEPROM: %s\n", ui_msg.data[ENV_EEPROM]?"on":"off");
	printf("on-chip ROM: %s\n", ui_msg.data[ENV_ROM]?"on":"off");
	printf("irq mode: %s\n", ui_msg.data[ENV_IRQ_MODE]?"edge":"level");
	printf("osc delay: %s\n", ui_msg.data[ENV_OSC_DELAY]?"on":"off");
	*/

	talker_page = ui_msg.data[ENV_TALKER_PAGE];
	ram_page = ui_msg.data[ENV_RAM_PAGE];
	reg_page = ui_msg.data[ENV_REG_PAGE];
	on_chip_eeprom = ui_msg.data[ENV_EEPROM];
	on_chip_rom = ui_msg.data[ENV_ROM];
	irq_mode = ui_msg.data[ENV_IRQ_MODE];
	osc_delay = ui_msg.data[ENV_OSC_DELAY];
	mcu_mode = ui_msg.data[ENV_MCU_MODE];
/*
	timer_rate = ui_msg.data[ENV_TIMER_RATE];
	cop_rate = ui_msg.data[ENV_COP_RATE];
*/

	return 0;
}


/* Alter monitor setup code, to reflect user's choise of environment, and con-
 * figuration of time protected systems. */
int ConfigEnvOptions(void) {
	send_buf[1+0xFA] = talker_page<<4;
	send_buf[1+0xFA] |= mcu_mode;
	send_buf[1+0xFB] = ram_page<<4 | reg_page;
	send_buf[1+0xFC] |= on_chip_eeprom;
	send_buf[1+0xFC] |= on_chip_rom<<1;
	send_buf[1+0xFD] |= irq_mode;
	send_buf[1+0xFD] |= osc_delay<<1;
	send_buf[1+0xFE] = timer_rate;
	send_buf[1+0xFF] = cop_rate;
	send_buf[0] = 0xFF;
		
	return 0;
}
