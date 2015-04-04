
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "serial.h"


int InitComm();
int CleanupComm(int dev);
int SaveComm(int dev);
int RestoreComm(int dev);
int ConfigComm(int dev, int baud);
int SendData(int dev, char *buf, int length);
int RecvData(int dev, char *buf, int length);

int DebugGetSpeed(speed_t speed);


struct termios org_mode;
int baud = 1200;			/* device speed */


int InitComm() {
	int dev;

	dev = open("/dev/ttyS1", O_RDWR);
	if (dev == -1) return -1;
	if (SaveComm(dev)) return -1;
	if (ConfigComm(dev, baud)) return -1;

	return dev;
}


int CleanupComm(int dev) {
	RestoreComm(dev);
	close(dev);
	return 0;
}


int SaveComm(int dev) {
	return tcgetattr(dev, &org_mode);
}


int RestoreComm(int dev) {
	return tcsetattr(dev, TCSANOW, &org_mode);
}


int ConfigComm(int dev, int baud) {
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

	if (tcgetattr(dev, &ser)) return -1;		/* get attributes */
	
	//printf("%i\n", DebugGetSpeed(cfgetispeed(&ser)));
	//printf("%i\n", DebugGetSpeed(cfgetospeed(&ser)));
	
	if (cfsetospeed(&ser, speed)) return -1;	/* set output baud rate */
	if (cfsetispeed(&ser, speed)) return -1;	/* set input baud rate */
	
	//printf("ispeed: %i\n", DebugGetSpeed(cfgetispeed(&ser)));
	//printf("ospeed: %i\n", DebugGetSpeed(cfgetospeed(&ser)));

	return 0;
}


int SendData(int dev, char *buf, int length) {
	int res;
	int i;

	for (i=res=0; i<length; i+=res) {
		res = write(dev, buf+i, length-i);
		if (res == -1) return -1;
	}
	
	return 0;
}


/* not implemented */
int RecvData(int dev, char *buf, int length) {
	return 0;
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
