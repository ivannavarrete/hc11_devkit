
#ifndef DISASM_H
#define DISASM_H


struct opcode {
	char *opcode;
	short bytes;
	short cycles;
	int addr_mode;
	int operands;
};

/* these specify how verbose the disassembly is */
#define DMODE_BYTES		1		/* show number of bytes of instruction */
#define DMODE_CYCLES	2		/* show execution time (in cycles) of instr. */
#define DMODE_CODE		4		/* show hex code of instruction */

/* addressing modes */
#define AMODE_REL		0
#define AMODE_INH		1
#define AMODE_IMM		2
#define AMODE_DIR		3
#define AMODE_EXT		4
#define AMODE_INDX		5
#define AMODE_INDY		6

/* operand format: bit-width and number of ops. */
#define OMODE_NONE		0x0000
#define OMODE_8_1		0x0001
#define OMODE_16_1		0x0101
#define OMODE_8_2		0x0002
#define OMODE_8_3		0x0003

#endif DISASM_H
