
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "disasm.h"


char *Disasm(unsigned char *data, int len, int mode);
int DisasmOp(unsigned char *data, int offset, char *buf, int mode);


int line = 80;


struct opcode ops[] = {
	/* 0x0 */
	{"test",1,0,AMODE_INH,OMODE_NONE}, {"nop",1,2,AMODE_INH,OMODE_NONE},
	{"idiv",1,41,AMODE_INH,OMODE_NONE}, {"fdiv",1,41,AMODE_INH,OMODE_NONE},
	{"lsrd",1,3,AMODE_INH,OMODE_NONE}, {"lsld",1,3,AMODE_INH,OMODE_NONE},
	{"tap",1,2,AMODE_INH,OMODE_NONE}, {"tpa",1,2,AMODE_INH,OMODE_NONE},
	{"inx",1,3,AMODE_INH,OMODE_NONE}, {"dex",1,3,AMODE_INH,OMODE_NONE},
	{"clv",1,2,AMODE_INH,OMODE_NONE}, {"sev",1,2,AMODE_INH,OMODE_NONE},
	{"clc",1,2,AMODE_INH,OMODE_NONE}, {"sec",1,2,AMODE_INH,OMODE_NONE},
	{"cli",1,2,AMODE_INH,OMODE_NONE}, {"sei",1,2,AMODE_INH,OMODE_NONE},
	/* 0x10 */
	{"sba",1,2,AMODE_INH,OMODE_NONE}, {"cba",1,2,AMODE_INH,OMODE_NONE},
	{"brset",4,6,AMODE_DIR,OMODE_8_3}, {"brclr",4,6,AMODE_DIR,OMODE_8_3},
	{"bset",3,6,AMODE_DIR,OMODE_8_2}, {"bclr",3,6,AMODE_DIR,OMODE_8_2},
	{"tab",1,2,AMODE_INH,OMODE_NONE}, {"tba",1,2,AMODE_INH,OMODE_NONE},
	{"mult_op",0,0,AMODE_INH,OMODE_NONE}, {"daa",1,2,AMODE_INH,OMODE_NONE},
	{"mult_op",0,0,AMODE_INH,OMODE_NONE}, {"aba",1,2,AMODE_INH,OMODE_NONE},
	{"bset",3,7,AMODE_INDX,OMODE_8_2}, {"bclr",3,7,AMODE_INDX,OMODE_8_2},
	{"brset",4,7,AMODE_INDX,OMODE_8_3}, {"brclr",4,7,AMODE_INDX,OMODE_8_3},
	/* 0x20 */
	{"bra",2,3,AMODE_REL,OMODE_8_1}, {"brn",2,3,AMODE_REL,OMODE_8_1},
	{"bhi",2,3,AMODE_REL,OMODE_8_1}, {"bls",2,3,AMODE_REL,OMODE_8_1},
	{"bcc",2,3,AMODE_REL,OMODE_8_1}, {"bcs",2,3,AMODE_REL,OMODE_8_1},
	{"bne",2,3,AMODE_REL,OMODE_8_1}, {"beq",2,3,AMODE_REL,OMODE_8_1},
	{"bvc",2,3,AMODE_REL,OMODE_8_1}, {"bvs",2,3,AMODE_REL,OMODE_8_1},
	{"bpl",2,3,AMODE_REL,OMODE_8_1}, {"bmi",2,3,AMODE_REL,OMODE_8_1},
	{"bge",2,3,AMODE_REL,OMODE_8_1}, {"blt",2,3,AMODE_REL,OMODE_8_1},
	{"bgt",2,3,AMODE_REL,OMODE_8_1}, {"ble",2,3,AMODE_REL,OMODE_8_1},
	/* 0x30 */
	{"tsx",1,3,AMODE_INH,OMODE_NONE}, {"ins",1,3,AMODE_INH,OMODE_NONE},
	{"pula",1,4,AMODE_INH,OMODE_NONE}, {"pulb",1,4,AMODE_INH,OMODE_NONE},
	{"des",1,3,AMODE_INH,OMODE_NONE}, {"txs",1,3,AMODE_INH,OMODE_NONE},
	{"psha",1,3,AMODE_INH,OMODE_NONE}, {"pshb",1,3,AMODE_INH,OMODE_NONE},
	{"pulx",1,5,AMODE_INH,OMODE_NONE}, {"rts",1,5,AMODE_INH,OMODE_NONE},
	{"abx",1,3,AMODE_INH,OMODE_NONE}, {"rti",1,12,AMODE_INH,OMODE_NONE},
	{"pshx",1,4,AMODE_INH,OMODE_NONE}, {"mul",1,10,AMODE_INH,OMODE_NONE},
	{"wai",1,14,AMODE_INH,OMODE_NONE}, {"swi",1,14,AMODE_INH,OMODE_NONE},
	/* 0x40 */
	{"nega",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"coma",1,2,AMODE_INH,OMODE_NONE},
	{"lsra",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"rora",1,2,AMODE_INH,OMODE_NONE}, {"asra",1,2,AMODE_INH,OMODE_NONE},
	{"asla",1,2,AMODE_INH,OMODE_NONE}, {"rola",1,2,AMODE_INH,OMODE_NONE},
	{"deca",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"inca",1,2,AMODE_INH,OMODE_NONE}, {"tsta",1,2,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"clra",1,2,AMODE_INH,OMODE_NONE},
	/* 0x50 */
	{"negb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"comb",1,2,AMODE_INH,OMODE_NONE},
	{"lsrb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"rorb",1,2,AMODE_INH,OMODE_NONE}, {"asrb",1,2,AMODE_INH,OMODE_NONE},
	{"aslb",1,2,AMODE_INH,OMODE_NONE}, {"rolb",1,2,AMODE_INH,OMODE_NONE},
	{"decb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"incb",1,2,AMODE_INH,OMODE_NONE}, {"tstb",1,2,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"clrb",1,2,AMODE_INH,OMODE_NONE},
	/* 0x60 */	
	{"neg",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"com",2,6,AMODE_INDX,OMODE_8_1},
	{"lsr",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"ror",2,6,AMODE_INDX,OMODE_8_1}, {"asr",2,6,AMODE_INDX,OMODE_8_1},
	{"asl",2,6,AMODE_INDX,OMODE_8_1}, {"rol",2,6,AMODE_INDX,OMODE_8_1},
	{"dec",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"inc",2,6,AMODE_INDX,OMODE_8_1}, {"tst",2,6,AMODE_INDX,OMODE_8_1},
	{"jmp",2,3,AMODE_INDX,OMODE_8_1}, {"clr",2,6,AMODE_INDX,OMODE_8_1},
	/* 0x70 */
	{"neg",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"invalid",0,0,AMODE_INH,OMODE_NONE}, {"com",3,6,AMODE_EXT,OMODE_8_2},
	{"lsr",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"ror",3,6,AMODE_EXT,OMODE_8_2}, {"asr",3,6,AMODE_EXT,OMODE_8_2},
	{"asl",3,6,AMODE_EXT,OMODE_8_2}, {"rol",3,6,AMODE_EXT,OMODE_8_2},
	{"dec",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",0,0,AMODE_INH,OMODE_NONE},
	{"inc",3,6,AMODE_EXT,OMODE_8_2}, {"tst",3,6,AMODE_EXT,OMODE_8_2},
	{"jmp",3,3,AMODE_EXT,OMODE_8_2}, {"clr",3,6,AMODE_EXT,OMODE_8_2},
	/* 0x80 */
	{"suba",2,2,AMODE_IMM,OMODE_8_1}, {"cmpa",2,2,AMODE_IMM,OMODE_8_1},
	{"sbca",2,2,AMODE_IMM,OMODE_8_1}, {"subd",3,4,AMODE_IMM,OMODE_16_1},
	{"anda",2,2,AMODE_IMM,OMODE_8_1}, {"bita",2,2,AMODE_IMM,OMODE_8_1},
	{"ldaa",2,2,AMODE_IMM,OMODE_8_1}, {"invalid",0,0,AMODE_IMM,OMODE_NONE},
	{"eora",2,2,AMODE_IMM,OMODE_8_1}, {"adca",2,2,AMODE_IMM,OMODE_8_1},
	{"oraa",2,2,AMODE_IMM,OMODE_8_1}, {"adda",2,2,AMODE_IMM,OMODE_8_1},
	{"cpx",3,4,AMODE_IMM,OMODE_16_1}, {"bsr",2,6,AMODE_REL,OMODE_8_1},
	{"lds",3,3,AMODE_IMM,OMODE_16_1}, {"xgdx",2,4,AMODE_INH,OMODE_NONE},
	/* 0x90 */
	{"suba",2,3,AMODE_DIR,OMODE_8_1}, {"cmpb",2,3,AMODE_DIR,OMODE_8_1},
	{"sbca",2,3,AMODE_DIR,OMODE_8_1}, {"subd",2,5,AMODE_DIR,OMODE_8_1},
	{"anda",2,3,AMODE_DIR,OMODE_8_1}, {"bita",2,3,AMODE_DIR,OMODE_8_1},
	{"ldaa",2,3,AMODE_DIR,OMODE_8_1}, {"staa",2,3,AMODE_DIR,OMODE_8_1},
	{"eora",2,3,AMODE_DIR,OMODE_8_1}, {"adca",2,3,AMODE_DIR,OMODE_8_1},
	{"oraa",2,3,AMODE_DIR,OMODE_8_1}, {"adda",2,3,AMODE_DIR,OMODE_8_1},
	{"cpx",2,5,AMODE_DIR,OMODE_8_1}, {"jsr",2,5,AMODE_DIR,OMODE_8_1},
	{"lds",2,4,AMODE_DIR,OMODE_8_1}, {"sts",2,4,AMODE_DIR,OMODE_8_1},
	/* 0xA0 */
	{"suba",2,4,AMODE_INDX,OMODE_8_1}, {"cmpa",2,4,AMODE_INDX,OMODE_8_1},
	{"sbca",2,4,AMODE_INDX,OMODE_8_1}, {"subd",2,6,AMODE_INDX,OMODE_8_1},
	{"anda",2,4,AMODE_INDX,OMODE_8_1}, {"bita",2,4,AMODE_INDX,OMODE_8_1},
	{"ldaa",2,4,AMODE_INDX,OMODE_8_1}, {"staa",2,4,AMODE_INDX,OMODE_8_1},
	{"eora",2,4,AMODE_INDX,OMODE_8_1}, {"adca",2,4,AMODE_INDX,OMODE_8_1},
	{"oraa",2,4,AMODE_INDX,OMODE_8_1}, {"adda",2,4,AMODE_INDX,OMODE_8_1},
	{"cpx",2,6,AMODE_INDX,OMODE_8_1}, {"jsr",2,6,AMODE_INDX,OMODE_8_1},
	{"lds",2,5,AMODE_INDX,OMODE_8_1}, {"sts",2,5,AMODE_INDX,OMODE_8_1},
	/* 0xB0 */
	{"suba",3,4,AMODE_EXT,OMODE_16_1}, {"cmpa",3,4,AMODE_EXT,OMODE_16_1},
	{"sbca",3,4,AMODE_EXT,OMODE_16_1}, {"subd",3,6,AMODE_EXT,OMODE_16_1},
	{"anda",3,4,AMODE_EXT,OMODE_16_1}, {"bita",3,4,AMODE_EXT,OMODE_16_1},
	{"ldaa",3,4,AMODE_EXT,OMODE_16_1}, {"staa",3,4,AMODE_EXT,OMODE_16_1},
	{"eora",3,4,AMODE_EXT,OMODE_16_1}, {"adca",3,4,AMODE_EXT,OMODE_16_1},
	{"oraa",3,4,AMODE_EXT,OMODE_16_1}, {"adda",3,4,AMODE_EXT,OMODE_16_1},
	{"cpx",3,6,AMODE_EXT,OMODE_16_1}, {"jsr",3,6,AMODE_EXT,OMODE_16_1},
	{"lds",3,5,AMODE_EXT,OMODE_16_1}, {"sts",3,5,AMODE_EXT,OMODE_16_1},
	/* 0xC0 */
	{"subb",2,2,AMODE_IMM,OMODE_8_1}, {"cmpb",2,2,AMODE_IMM,OMODE_8_1},
	{"sbcb",2,2,AMODE_IMM,OMODE_8_1}, {"addd",3,4,AMODE_IMM,OMODE_16_1},
	{"andb",2,2,AMODE_IMM,OMODE_8_1}, {"bitb",2,2,AMODE_IMM,OMODE_8_1},
	{"ldab",2,2,AMODE_IMM,OMODE_8_1}, {"invalid",0,0,AMODE_IMM,OMODE_NONE},
	{"eorb",2,2,AMODE_IMM,OMODE_8_1}, {"adcb",2,2,AMODE_IMM,OMODE_8_1},
	{"orab",2,2,AMODE_IMM,OMODE_8_1}, {"addb",2,2,AMODE_IMM,OMODE_8_1},
	{"ldd",3,3,AMODE_IMM,OMODE_16_1}, {"mult_op",0,0,AMODE_IMM,OMODE_NONE},
	{"ldx",3,3,AMODE_IMM,OMODE_16_1}, {"stop",1,2,AMODE_INH,OMODE_NONE},
	/* 0xD0 */
	{"subb",2,3,AMODE_DIR,OMODE_8_1}, {"cmpb",2,3,AMODE_DIR,OMODE_8_1},
	{"sbcb",2,3,AMODE_DIR,OMODE_8_1}, {"addd",2,5,AMODE_DIR,OMODE_8_1},
	{"andb",2,3,AMODE_DIR,OMODE_8_1}, {"bitb",2,3,AMODE_DIR,OMODE_8_1},
	{"ldab",2,3,AMODE_DIR,OMODE_8_1}, {"stab",2,3,AMODE_DIR,OMODE_8_1},
	{"eorb",2,3,AMODE_DIR,OMODE_8_1}, {"adcb",2,3,AMODE_DIR,OMODE_8_1},
	{"orab",2,3,AMODE_DIR,OMODE_8_1}, {"addb",2,3,AMODE_DIR,OMODE_8_1},
	{"ldd",2,4,AMODE_DIR,OMODE_8_1}, {"std",2,4,AMODE_DIR,OMODE_8_1},
	{"ldx",2,4,AMODE_DIR,OMODE_8_1}, {"stx",2,4,AMODE_DIR,OMODE_8_1},
	/* 0xE0 */
	{"subb",2,4,AMODE_INDX,OMODE_8_1}, {"cmpb",2,4,AMODE_INDX,OMODE_8_1},
	{"sbcb",2,4,AMODE_INDX,OMODE_8_1}, {"addd",2,6,AMODE_INDX,OMODE_8_1},
	{"andb",2,4,AMODE_INDX,OMODE_8_1}, {"bitb",2,4,AMODE_INDX,OMODE_8_1},
	{"ldab",2,4,AMODE_INDX,OMODE_8_1}, {"stab",2,4,AMODE_INDX,OMODE_8_1},
	{"eorb",2,4,AMODE_INDX,OMODE_8_1}, {"adcb",2,4,AMODE_INDX,OMODE_8_1},
	{"orab",2,4,AMODE_INDX,OMODE_8_1}, {"addb",2,4,AMODE_INDX,OMODE_8_1},
	{"ldd",2,5,AMODE_INDX,OMODE_8_1}, {"std",2,5,AMODE_INDX,OMODE_8_1},
	{"ldx",2,5,AMODE_INDX,OMODE_8_1}, {"stx",2,5,AMODE_INDX,OMODE_8_1},
	/* 0xF0 */
	{"subb",3,4,AMODE_EXT,OMODE_16_1}, {"cmpb",3,4,AMODE_EXT,OMODE_16_1},
	{"sbcb",3,4,AMODE_EXT,OMODE_16_1}, {"addd",3,6,AMODE_EXT,OMODE_16_1},
	{"andb",3,4,AMODE_EXT,OMODE_16_1}, {"bitb",3,4,AMODE_EXT,OMODE_16_1},
	{"ldab",3,4,AMODE_EXT,OMODE_16_1}, {"stab",3,4,AMODE_EXT,OMODE_16_1},
	{"eorb",3,4,AMODE_EXT,OMODE_16_1}, {"adcb",3,4,AMODE_EXT,OMODE_16_1},
	{"orab",3,4,AMODE_EXT,OMODE_16_1}, {"addb",3,4,AMODE_EXT,OMODE_16_1},
	{"ldd",3,5,AMODE_EXT,OMODE_16_1}, {"std",3,5,AMODE_EXT,OMODE_16_1},
	{"ldx",3,5,AMODE_EXT,OMODE_16_1}, {"stx",3,5,AMODE_EXT,OMODE_16_1}
};


int DisasmOp(unsigned char *code, int len, char *buf, int mode) {
	struct opcode op = ops[code[0]];
	char bytes[4], cycles[4], mcode[4], opbuf1[32], opbuf2[32], opbuf3[32];
	int operand1, operand2, operand3;
	int i, width = 0;

	/* check if the complete instruction is in the data array */
	if (op.bytes > len) return -1;

	/* flush buffers */
	buf[0] = 0;
	opbuf1[0] = 0;
	opbuf2[0] = 0;
	opbuf3[0] = 0;

	/* put machine code into buffer */
	if (mode & DMODE_CODE) {
		for (i=0; i<op.bytes;i++) {
			sprintf(mcode, "%02X ", code[i]);
			strcat(buf, mcode);
		}

		for (i=strlen(buf); i<line/4; i++)
			strcat(buf, " ");
		width = line / 4;
	}

	/* put mnemonic into buffer */
	strcat(buf, op.opcode);
	for (i=strlen(buf); i<width+8; i++)
		strcat(buf, " ");

	/* put operands into buffer */
	switch (op.operands) {				/* parse operands */
		case OMODE_8_3:
			operand3 = code[3];
			sprintf(opbuf3, "%02X", operand3);
		case OMODE_8_2:
			operand2 = code[2];
			sprintf(opbuf2, "%02X", operand2);
		case OMODE_8_1:
			operand1 = code[1];
			sprintf(opbuf1, "%02X", operand1); break;
		case OMODE_16_1:
			operand1 = code[1]<<8 | code[2];
			sprintf(opbuf1, "%04X", operand1);	break;
		default:
	}

	switch (op.addr_mode) {				/* put operands into buffer */
		case AMODE_INDX:
		case AMODE_INDY:
			strcat(buf, opbuf1);
			if (op.addr_mode == AMODE_INDX)	strcat(buf, ",X");
			else strcat(buf, ",Y");
			if (op.operands == OMODE_8_2 || op.operands == OMODE_8_3) {
				strcat(buf, " #$");
				strcat(buf, opbuf2);
			}
			if (op.operands == OMODE_8_3) {
				strcat(buf, " $");
				strcat(buf, opbuf3);
			}
			break;
		case AMODE_DIR:
			strcat(buf, "$");
			strcat(buf, opbuf1);
			if (op.operands == OMODE_8_2 || op.operands == OMODE_8_3) {
				strcat(buf, " #$");
				strcat(buf, opbuf2);
			}
			if (op.operands == OMODE_8_3) {
				strcat(buf, " $");
				strcat(buf, opbuf3);
			}
			break;
		case AMODE_IMM:
			strcat(buf, "#$");
			strcat(buf, opbuf1);
			break;
		case AMODE_REL:
		case AMODE_EXT:
			strcat(buf, "$");
			strcat(buf, opbuf1);
			break;
		case AMODE_INH:
		default:
	}

	/* put number of bytes for this instruction into buffer */
	if (mode & DMODE_BYTES) {
		sprintf(bytes, "%d", op.bytes);
		for (i=strlen(buf); i<line-15; i++)
			strcat(buf, " ");
		strcat(buf, bytes);
	}

	/* put number of cycles for this instruction into buffer */
	if (mode & DMODE_CYCLES) {
		sprintf(cycles, "%d", op.cycles);
		for (i=strlen(buf); i<line-10; i++)
			strcat(buf, " ");
		strcat(buf, cycles);
	}

	return op.bytes;
}

/* Disassemble code and place result in allocated buffer. The caller is
 * responsible for freeing the buffer. Unfinished instructions will not be
 * disassembled */
char *Disasm(unsigned char *code, int len, int mode) {
	char *buf;
	int buflen;
	int i, j;

	buf = malloc(1);
	if (!buf)
		return NULL;
	buf[0] = 0;

	/* disassemble instructions and place in buffer */
	for (i=j=0; i<len; i+=j) {
		buflen = strlen(buf);
		buf = realloc(buf, buflen+line); /* resize buffer to fit another line */
		if (!buf) {
			free(buf);
			return NULL;
		}
		
		j = DisasmOp(code+i, len-i, buf+buflen, mode);
		if (j == -1)		/* (j == -1) when the instruction wasn't complete */
			break;
		strcat(buf, "\n");
	}

	buflen = strlen(buf);
	realloc(buf, buflen);
	return buf;
}


/*
int main(void) {
	unsigned char data[] = {0xFE, 0x00, 0x4B, 0x1C, 0x07, 0xFF, 0x1C, 0x03,
							0xFF, 0x20, 0xFE, 0x10, 0x00, 0xFE, 0x00, 0x55};
	char *tmp;

	tmp = Disasm(data, 15, DMODE_CODE | DMODE_BYTES | DMODE_CYCLES);
	if (tmp != NULL)
		printf("%s", tmp);
	else
		printf("disassembly failure\n");

	printf("\n\n");

	tmp = Disasm(data, 16, DMODE_CODE | DMODE_BYTES | DMODE_CYCLES);
	if (tmp != NULL)
		printf("%s", tmp);
	else
		printf("disassembly failure\n");
	
	free(tmp);

	return 0;
}
*/
