
//
// serial.cpp
//
// This object handles the physical communication. It can be replaced if the
// communication interface between the PC and HC11 changes. Only the Debugger
// object should use this (xcept for main.cpp, which creates it).
//
// TODO: change the open/read/write way of communication to proper c++ streams
// TODO: ConfigComm doesn't work properly. I currently start up minicom so it
// configures the serial port and then exit without reset. Check the minicom
// source..
// Note: baud rate selection in ConfigComm seem to work, so there's probably
// a problem in the data format.
//


#include <iostream>
#include <unistd.h>						// open/read/write stuff...
#include <fcntl.h>						// open/read/write stuff...
#include <cstdio>
#include <cstring>

#include "serial.h"
#include "error.h"
#include "ui.h"


extern UI &ui;							// global user interface object


// Constructor.
Serial::Serial(char *iface, int baud) {
	// open device
	dev = open(iface, O_RDWR | O_SYNC);
	if (dev == -1)
		throw *(new Error("Serial::Serial()", "can't open() interface"));

	// save interface configuration for later restoral
	if (int err = SaveComm()) {
		ui.ShowMsg("Serial::Serial(): can't save interface configuration: ");
		ui.ShowMsg(strerror(err));
		ui.ShowMsg("\n");
	}

	// configure interface
	ConfigComm(baud);

	return;
}


// Destructor.
Serial::~Serial() {
	RestoreComm();
	close(dev);
}


// Save device parameters.
int Serial::SaveComm() {
	return tcgetattr(dev, &org_mode);
}


// Restore device parameters.
int Serial::RestoreComm() {
	return tcsetattr(dev, TCSANOW, &org_mode);
}


// Configure device parameters; baud rate and data format.
void Serial::ConfigComm(int baud) {
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

	// change input and output baud rate
	if (tcgetattr(dev, &ser))
		throw *(new Error("Serial::ConfigComm", "can't get iface attributes"));
	if (cfsetospeed(&ser, speed))
		throw *(new Error("Serial::ConfigComm", "can't change iface ospeed"));
	if (cfsetispeed(&ser, speed))
		throw *(new Error("Serial::ConfigComm", "can't change iface ispeed"));
	if (tcsetattr(dev, TCSADRAIN, &ser))
		throw *(new Error("Serial::ConfigComm", "can't set iface attributes"));
	
	// 1 start bit, 8 data bits, 1 stop bit, no parity
	// TODO: Fix this ugly cfmakeraw() hack ASAP!!
	cfmakeraw(&ser);
	if (tcsetattr(dev, TCSADRAIN, &ser))
		throw *(new Error("Serial::ConfigComm",
							"can't change iface data format"));
}


// Send data to interface.
int Serial::SendData(const unsigned char *buf, int length) {
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


// Receive data from interface
int Serial::RecvData(unsigned char *buf, int length) {
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


// Flush interface. Find a better way, maybe tcflush() ?
int Serial::FlushDev() {
	int res;
	char c;
	
	fcntl(dev, F_SETFL, O_NONBLOCK);
	do {
		res = read(dev, &c, 1);
	} while (res != 0 && res != -1);
	fcntl(dev, F_SETFL, ~O_NONBLOCK);

	return res;
}


// debug routine
/*
int Serial::DebugGetSpeed(speed_t spd) {
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
}*/
