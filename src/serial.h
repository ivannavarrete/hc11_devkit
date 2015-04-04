
#ifndef SERIAL_H
#define SERIAL_H


#include <termios.h>		// termios struct


class Serial {
public:
	Serial(char *iface, int baud);
	~Serial();
	void ConfigComm(int baud);
	int SendData(const unsigned char *buf, int length);
	int RecvData(unsigned char *buf, int length);
	int FlushDev();
private:
	int SaveComm();
	int RestoreComm();
	//int Serial::DebugGetSpeed(speed_t speed);		// debug routine

	int dev;						// interface handler
	struct termios org_mode;		// original interface mode
};

#endif // SERIAL_H
