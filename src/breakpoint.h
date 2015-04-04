
#ifndef BREAKPOINT_H
#define BREAKPOINT_H


#define MAX_BREAKPOINTS		8

struct breakpoint {
	int num;				/* if num == 0, then breakpoint is clear */
	unsigned char byte;		/* the original byte */
	unsigned short addr;	/* address of breakpoint */
};


int SetBP(unsigned short addr, unsigned char byte);
int ClearBP(int num);
struct breakpoint *GetBP(int num);
int FindBP(unsigned short addr);


#endif BREAKPOINT_H
