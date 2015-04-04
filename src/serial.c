
/*
 * This module handles the physical communication. It can be replaced if the
 * communication interface between PC and HC11 changes. Only the monitor.c
 * and main.c modules should use the functions in this module.
 *
 * main.c uses InitComm(), ConfigComm() and CleanupComm().
 * monitor.c uses SendData() and RecvData().
 *
 * TODO: ConfigComm doesn't work properly. I currently start up minicom so it
 * configures the serial port and then exit without reset. Check the minicom
 * source..
 * Note: baud rate selection in ConfigComm seem to work, so ther's probably
 * a problem in the data format.
 *
 * public routines:
 * 		InitComm() sets up the communication interface on the PC side.
 *		ConfigComm() reconfigures the communication interface.
 * 		CleanupComm() shuts down the communication interface on the PC side.
 *		SendData() sends a buffer to the HC11.
 *		RecvData() receives a byte stream from the HC11.
 * private routines:
 * 		SaveComm() saves the current configuration of the interface.
 *		RestoreComm() restores the previous configuration of the interface.
 *
 */


#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "serial.h"


int InitComm(void);
int CleanupComm(void);
int SaveComm(void);
int RestoreComm(void);
int ConfigComm(int baud);
int SendData(const unsigned char *buf, int length);
int RecvData(unsigned char *buf, int length);
int FlushDev(void);

int DebugGetSpeed(speed_t speed);		/* debug routine */


struct termios org_mode;
int dev;								/* interface handler */
const char *iface = "/dev/ttyS1";		/* default interface */
int baud = 1200;						/* default initial interface speed */


int InitComm(void) {
	dev = open(iface, O_RDWR | O_SYNC);
	if (dev == -1)
		return -1;
	if (SaveComm())
		return -1;
	if (ConfigComm(baud))
		return -1;

	return dev;
}


int CleanupComm(void) {
	RestoreComm();
	close(dev);
	return 0;
}


int SaveComm(void) {
	return tcgetattr(dev, &org_mode);
}


int RestoreComm(void) {
	return tcsetattr(dev, TCSANOW, &org_mode);
}


/* Currently this only changes the baud rate. */
int ConfigComm(int baud) {
	struct termios ser;
	speed_t speed;

	switch (baud) {
		case 50: speed = B50; break;
		case 75: speed = B75; break;
		case 110: speed = B110; break;
		case 134: speed = B134; break;
		case 150: speed = B150; break;
		case 200: speed = B200; break;
		case 300: speed = B300; break;
		case 600: speed = B600; break;
		case 1200: speed = B1200; break;
		case 1800: speed = B1800; break;
		case 2400: speed = B2400; break;
		case 4800: speed = B4800; break;
		case 9600: speed = B9600; break;
		default: speed = B1200; break;
	}

	/* change input and output baud rate */
	if (tcgetattr(dev, &ser)) return -1;
	if (cfsetospeed(&ser, speed)) return -1;
	if (cfsetispeed(&ser, speed)) return -1;
	if (tcsetattr(dev, TCSADRAIN, &ser)) return -1;

	return 0;
}


int SendData(const unsigned char *buf, int length) {
	int res;
	int i;

	for (i=res=0; i<length; i+=res) {
		res = write(dev, buf+i, length-i);
		if (res == -1) {
			perror(": SendData(): ");
			return -1;
		}
	}

	return 0;
}


int RecvData(unsigned char *buf, int length) {
	int res;
	int i;

	for (i=res=0; i<length; i+=res) {
		res = read(dev, buf+i, length-i);
		if (res == -1) {
			perror(": RecvData(): ");
			return -1;
		}
	}

	return 0;
}


/* Find a better way to flush the device */
int FlushDev(void) {
	int res;
	char c;
	
	fcntl(dev, F_SETFL, O_NONBLOCK);
	do {
		res = read(dev, &c, 1);
	} while (res != 0 && res != -1);
	fcntl(dev, F_SETFL, ~O_NONBLOCK);

	return res;
}


/* debug routine */
/*
int DebugGetSpeed(speed_t spd) {
	switch (spd) {
		case B50: return 50;
		case B75: return 75;
		case B110: return 110;
		case B134: return 134;
		case B150: return 150;
		case B200: return 200;
		case B300: return 300;
		case B600: return 600;
		case B1200: return 1200;
		case B1800: return 1800;
		case B2400: return 2400;
		case B4800: return 4800;
		case B9600: return 9600;
		default: return  0;
	}
}
*/
