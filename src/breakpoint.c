/*
 * breakpoint.c
 */


#include <unistd.h>
#include <stdio.h>

#include "breakpoint.h"
#include "ui.h"


struct breakpoint bp_list[MAX_BREAKPOINTS] = {};


/* Set a breakpoint. */
int SetBP(unsigned short addr, unsigned char byte) {
	int i;
	
	/* find an empty breakpoint slot */
	for (i=0; i<MAX_BREAKPOINTS; i++) {
		if (!bp_list[i].num)
			break;
	}
	if (i == MAX_BREAKPOINTS)
		return 0;
	
	/* insert breakpoint into slot */
	bp_list[i].num = i+1;
	bp_list[i].byte = byte;
	bp_list[i].addr = addr;

	return 1;
}


/* Clear a breakpoint. */
int ClearBP(int n) {
	/* if n is valid and there is a breakpoint, clear it */
	if (n > 0 && n <= MAX_BREAKPOINTS) {
		if (bp_list[n-1].num) {
			bp_list[n-1].num = 0;
			bp_list[n-1].addr = 0;
			return 1;
		}
	}

	ShowMsg("bad breakpoint number\n");
	return 0;
}


/* Get a breakpoint. */
struct breakpoint *GetBP(int n) {
	if (n >= 0 && n < MAX_BREAKPOINTS) {
		if (bp_list[n].num)
			return &bp_list[n];
	}
	
	return NULL;
}


/* Return an index into bp_list that contains a matching breakpoint, or -1 if no
 * matching breakpoint was found. */
int FindBP(unsigned short addr) {
	int i;

	for (i=0; i<MAX_BREAKPOINTS; i++) {
		if (bp_list[i].addr == addr && bp_list[i].num) {
			return i;
		}
	}
	
	return -1;
}
