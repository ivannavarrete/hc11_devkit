
//
// disasm.cpp
//
// This module handles the creation and destruction of a disassembly list.
//
// main.cpp uses Disasm() and ~Disasm().
//
// TODO: FIX THE TABLES!! IT'S HORRIBLE!! FIND A BETTER WAY TO DO THINGS!!!
//
//


#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "disasm.h"

using namespace std;


struct opcode ops[] = {
	// 0x0
	{"test",1,0,AMODE_INH,OMODE_NONE}, {"nop",1,2,AMODE_INH,OMODE_NONE},
	{"idiv",1,41,AMODE_INH,OMODE_NONE}, {"fdiv",1,41,AMODE_INH,OMODE_NONE},
	{"lsrd",1,3,AMODE_INH,OMODE_NONE}, {"lsld",1,3,AMODE_INH,OMODE_NONE},
	{"tap",1,2,AMODE_INH,OMODE_NONE}, {"tpa",1,2,AMODE_INH,OMODE_NONE},
	{"inx",1,3,AMODE_INH,OMODE_NONE}, {"dex",1,3,AMODE_INH,OMODE_NONE},
	{"clv",1,2,AMODE_INH,OMODE_NONE}, {"sev",1,2,AMODE_INH,OMODE_NONE},
	{"clc",1,2,AMODE_INH,OMODE_NONE}, {"sec",1,2,AMODE_INH,OMODE_NONE},
	{"cli",1,2,AMODE_INH,OMODE_NONE}, {"sei",1,2,AMODE_INH,OMODE_NONE},
	// 0x10
	{"sba",1,2,AMODE_INH,OMODE_NONE}, {"cba",1,2,AMODE_INH,OMODE_NONE},
	{"brset",4,6,AMODE_DIR,OMODE_8_3}, {"brclr",4,6,AMODE_DIR,OMODE_8_3},
	{"bset",3,6,AMODE_DIR,OMODE_8_2}, {"bclr",3,6,AMODE_DIR,OMODE_8_2},
	{"tab",1,2,AMODE_INH,OMODE_NONE}, {"tba",1,2,AMODE_INH,OMODE_NONE},
	{"mult_op",1,0,AMODE_INH,OMODE_NONE}, {"daa",1,2,AMODE_INH,OMODE_NONE},
	{"mult_op",1,0,AMODE_INH,OMODE_NONE}, {"aba",1,2,AMODE_INH,OMODE_NONE},
	{"bset",3,7,AMODE_INDX,OMODE_8_2}, {"bclr",3,7,AMODE_INDX,OMODE_8_2},
	{"brset",4,7,AMODE_INDX,OMODE_8_3}, {"brclr",4,7,AMODE_INDX,OMODE_8_3},
	// 0x20
	{"bra",2,3,AMODE_REL,OMODE_8_1}, {"brn",2,3,AMODE_REL,OMODE_8_1},
	{"bhi",2,3,AMODE_REL,OMODE_8_1}, {"bls",2,3,AMODE_REL,OMODE_8_1},
	{"bcc",2,3,AMODE_REL,OMODE_8_1}, {"bcs",2,3,AMODE_REL,OMODE_8_1},
	{"bne",2,3,AMODE_REL,OMODE_8_1}, {"beq",2,3,AMODE_REL,OMODE_8_1},
	{"bvc",2,3,AMODE_REL,OMODE_8_1}, {"bvs",2,3,AMODE_REL,OMODE_8_1},
	{"bpl",2,3,AMODE_REL,OMODE_8_1}, {"bmi",2,3,AMODE_REL,OMODE_8_1},
	{"bge",2,3,AMODE_REL,OMODE_8_1}, {"blt",2,3,AMODE_REL,OMODE_8_1},
	{"bgt",2,3,AMODE_REL,OMODE_8_1}, {"ble",2,3,AMODE_REL,OMODE_8_1},
	// 0x30
	{"tsx",1,3,AMODE_INH,OMODE_NONE}, {"ins",1,3,AMODE_INH,OMODE_NONE},
	{"pula",1,4,AMODE_INH,OMODE_NONE}, {"pulb",1,4,AMODE_INH,OMODE_NONE},
	{"des",1,3,AMODE_INH,OMODE_NONE}, {"txs",1,3,AMODE_INH,OMODE_NONE},
	{"psha",1,3,AMODE_INH,OMODE_NONE}, {"pshb",1,3,AMODE_INH,OMODE_NONE},
	{"pulx",1,5,AMODE_INH,OMODE_NONE}, {"rts",1,5,AMODE_INH,OMODE_NONE},
	{"abx",1,3,AMODE_INH,OMODE_NONE}, {"rti",1,12,AMODE_INH,OMODE_NONE},
	{"pshx",1,4,AMODE_INH,OMODE_NONE}, {"mul",1,10,AMODE_INH,OMODE_NONE},
	{"wai",1,14,AMODE_INH,OMODE_NONE}, {"swi",1,14,AMODE_INH,OMODE_NONE},
	// 0x40
	{"nega",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"coma",1,2,AMODE_INH,OMODE_NONE},
	{"lsra",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"rora",1,2,AMODE_INH,OMODE_NONE}, {"asra",1,2,AMODE_INH,OMODE_NONE},
	{"asla",1,2,AMODE_INH,OMODE_NONE}, {"rola",1,2,AMODE_INH,OMODE_NONE},
	{"deca",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"inca",1,2,AMODE_INH,OMODE_NONE}, {"tsta",1,2,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"clra",1,2,AMODE_INH,OMODE_NONE},
	// 0x50
	{"negb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"comb",1,2,AMODE_INH,OMODE_NONE},
	{"lsrb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"rorb",1,2,AMODE_INH,OMODE_NONE}, {"asrb",1,2,AMODE_INH,OMODE_NONE},
	{"aslb",1,2,AMODE_INH,OMODE_NONE}, {"rolb",1,2,AMODE_INH,OMODE_NONE},
	{"decb",1,2,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"incb",1,2,AMODE_INH,OMODE_NONE}, {"tstb",1,2,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"clrb",1,2,AMODE_INH,OMODE_NONE},
	// 0x60
	{"neg",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"com",2,6,AMODE_INDX,OMODE_8_1},
	{"lsr",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ror",2,6,AMODE_INDX,OMODE_8_1}, {"asr",2,6,AMODE_INDX,OMODE_8_1},
	{"asl",2,6,AMODE_INDX,OMODE_8_1}, {"rol",2,6,AMODE_INDX,OMODE_8_1},
	{"dec",2,6,AMODE_INDX,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"inc",2,6,AMODE_INDX,OMODE_8_1}, {"tst",2,6,AMODE_INDX,OMODE_8_1},
	{"jmp",2,3,AMODE_INDX,OMODE_8_1}, {"clr",2,6,AMODE_INDX,OMODE_8_1},
	// 0x70
	{"neg",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"com",3,6,AMODE_EXT,OMODE_8_2},
	{"lsr",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ror",3,6,AMODE_EXT,OMODE_8_2}, {"asr",3,6,AMODE_EXT,OMODE_8_2},
	{"asl",3,6,AMODE_EXT,OMODE_8_2}, {"rol",3,6,AMODE_EXT,OMODE_8_2},
	{"dec",3,6,AMODE_EXT,OMODE_8_2}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"inc",3,6,AMODE_EXT,OMODE_8_2}, {"tst",3,6,AMODE_EXT,OMODE_8_2},
	{"jmp",3,3,AMODE_EXT,OMODE_8_2}, {"clr",3,6,AMODE_EXT,OMODE_8_2},
	// 0x80
	{"suba",2,2,AMODE_IMM,OMODE_8_1}, {"cmpa",2,2,AMODE_IMM,OMODE_8_1},
	{"sbca",2,2,AMODE_IMM,OMODE_8_1}, {"subd",3,4,AMODE_IMM,OMODE_16_1},
	{"anda",2,2,AMODE_IMM,OMODE_8_1}, {"bita",2,2,AMODE_IMM,OMODE_8_1},
	{"ldaa",2,2,AMODE_IMM,OMODE_8_1}, {"invalid",1,0,AMODE_IMM,OMODE_NONE},
	{"eora",2,2,AMODE_IMM,OMODE_8_1}, {"adca",2,2,AMODE_IMM,OMODE_8_1},
	{"oraa",2,2,AMODE_IMM,OMODE_8_1}, {"adda",2,2,AMODE_IMM,OMODE_8_1},
	{"cpx",3,4,AMODE_IMM,OMODE_16_1}, {"bsr",2,6,AMODE_REL,OMODE_8_1},
	{"lds",3,3,AMODE_IMM,OMODE_16_1}, {"xgdx",1,3,AMODE_INH,OMODE_NONE},
	// 0x90
	{"suba",2,3,AMODE_DIR,OMODE_8_1}, {"cmpb",2,3,AMODE_DIR,OMODE_8_1},
	{"sbca",2,3,AMODE_DIR,OMODE_8_1}, {"subd",2,5,AMODE_DIR,OMODE_8_1},
	{"anda",2,3,AMODE_DIR,OMODE_8_1}, {"bita",2,3,AMODE_DIR,OMODE_8_1},
	{"ldaa",2,3,AMODE_DIR,OMODE_8_1}, {"staa",2,3,AMODE_DIR,OMODE_8_1},
	{"eora",2,3,AMODE_DIR,OMODE_8_1}, {"adca",2,3,AMODE_DIR,OMODE_8_1},
	{"oraa",2,3,AMODE_DIR,OMODE_8_1}, {"adda",2,3,AMODE_DIR,OMODE_8_1},
	{"cpx",2,5,AMODE_DIR,OMODE_8_1}, {"jsr",2,5,AMODE_DIR,OMODE_8_1},
	{"lds",2,4,AMODE_DIR,OMODE_8_1}, {"sts",2,4,AMODE_DIR,OMODE_8_1},
	// 0xA0
	{"suba",2,4,AMODE_INDX,OMODE_8_1}, {"cmpa",2,4,AMODE_INDX,OMODE_8_1},
	{"sbca",2,4,AMODE_INDX,OMODE_8_1}, {"subd",2,6,AMODE_INDX,OMODE_8_1},
	{"anda",2,4,AMODE_INDX,OMODE_8_1}, {"bita",2,4,AMODE_INDX,OMODE_8_1},
	{"ldaa",2,4,AMODE_INDX,OMODE_8_1}, {"staa",2,4,AMODE_INDX,OMODE_8_1},
	{"eora",2,4,AMODE_INDX,OMODE_8_1}, {"adca",2,4,AMODE_INDX,OMODE_8_1},
	{"oraa",2,4,AMODE_INDX,OMODE_8_1}, {"adda",2,4,AMODE_INDX,OMODE_8_1},
	{"cpx",2,6,AMODE_INDX,OMODE_8_1}, {"jsr",2,6,AMODE_INDX,OMODE_8_1},
	{"lds",2,5,AMODE_INDX,OMODE_8_1}, {"sts",2,5,AMODE_INDX,OMODE_8_1},
	// 0xB0
	{"suba",3,4,AMODE_EXT,OMODE_16_1}, {"cmpa",3,4,AMODE_EXT,OMODE_16_1},
	{"sbca",3,4,AMODE_EXT,OMODE_16_1}, {"subd",3,6,AMODE_EXT,OMODE_16_1},
	{"anda",3,4,AMODE_EXT,OMODE_16_1}, {"bita",3,4,AMODE_EXT,OMODE_16_1},
	{"ldaa",3,4,AMODE_EXT,OMODE_16_1}, {"staa",3,4,AMODE_EXT,OMODE_16_1},
	{"eora",3,4,AMODE_EXT,OMODE_16_1}, {"adca",3,4,AMODE_EXT,OMODE_16_1},
	{"oraa",3,4,AMODE_EXT,OMODE_16_1}, {"adda",3,4,AMODE_EXT,OMODE_16_1},
	{"cpx",3,6,AMODE_EXT,OMODE_16_1}, {"jsr",3,6,AMODE_EXT,OMODE_16_1},
	{"lds",3,5,AMODE_EXT,OMODE_16_1}, {"sts",3,5,AMODE_EXT,OMODE_16_1},
	// 0xC0
	{"subb",2,2,AMODE_IMM,OMODE_8_1}, {"cmpb",2,2,AMODE_IMM,OMODE_8_1},
	{"sbcb",2,2,AMODE_IMM,OMODE_8_1}, {"addd",3,4,AMODE_IMM,OMODE_16_1},
	{"andb",2,2,AMODE_IMM,OMODE_8_1}, {"bitb",2,2,AMODE_IMM,OMODE_8_1},
	{"ldab",2,2,AMODE_IMM,OMODE_8_1}, {"invalid",1,0,AMODE_IMM,OMODE_NONE},
	{"eorb",2,2,AMODE_IMM,OMODE_8_1}, {"adcb",2,2,AMODE_IMM,OMODE_8_1},
	{"orab",2,2,AMODE_IMM,OMODE_8_1}, {"addb",2,2,AMODE_IMM,OMODE_8_1},
	{"ldd",3,3,AMODE_IMM,OMODE_16_1}, {"mult_op",1,0,AMODE_IMM,OMODE_NONE},
	{"ldx",3,3,AMODE_IMM,OMODE_16_1}, {"stop",1,2,AMODE_INH,OMODE_NONE},
	// 0xD0
	{"subb",2,3,AMODE_DIR,OMODE_8_1}, {"cmpb",2,3,AMODE_DIR,OMODE_8_1},
	{"sbcb",2,3,AMODE_DIR,OMODE_8_1}, {"addd",2,5,AMODE_DIR,OMODE_8_1},
	{"andb",2,3,AMODE_DIR,OMODE_8_1}, {"bitb",2,3,AMODE_DIR,OMODE_8_1},
	{"ldab",2,3,AMODE_DIR,OMODE_8_1}, {"stab",2,3,AMODE_DIR,OMODE_8_1},
	{"eorb",2,3,AMODE_DIR,OMODE_8_1}, {"adcb",2,3,AMODE_DIR,OMODE_8_1},
	{"orab",2,3,AMODE_DIR,OMODE_8_1}, {"addb",2,3,AMODE_DIR,OMODE_8_1},
	{"ldd",2,4,AMODE_DIR,OMODE_8_1}, {"std",2,4,AMODE_DIR,OMODE_8_1},
	{"ldx",2,4,AMODE_DIR,OMODE_8_1}, {"stx",2,4,AMODE_DIR,OMODE_8_1},
	// 0xE0
	{"subb",2,4,AMODE_INDX,OMODE_8_1}, {"cmpb",2,4,AMODE_INDX,OMODE_8_1},
	{"sbcb",2,4,AMODE_INDX,OMODE_8_1}, {"addd",2,6,AMODE_INDX,OMODE_8_1},
	{"andb",2,4,AMODE_INDX,OMODE_8_1}, {"bitb",2,4,AMODE_INDX,OMODE_8_1},
	{"ldab",2,4,AMODE_INDX,OMODE_8_1}, {"stab",2,4,AMODE_INDX,OMODE_8_1},
	{"eorb",2,4,AMODE_INDX,OMODE_8_1}, {"adcb",2,4,AMODE_INDX,OMODE_8_1},
	{"orab",2,4,AMODE_INDX,OMODE_8_1}, {"addb",2,4,AMODE_INDX,OMODE_8_1},
	{"ldd",2,5,AMODE_INDX,OMODE_8_1}, {"std",2,5,AMODE_INDX,OMODE_8_1},
	{"ldx",2,5,AMODE_INDX,OMODE_8_1}, {"stx",2,5,AMODE_INDX,OMODE_8_1},
	// 0xF0
	{"subb",3,4,AMODE_EXT,OMODE_16_1}, {"cmpb",3,4,AMODE_EXT,OMODE_16_1},
	{"sbcb",3,4,AMODE_EXT,OMODE_16_1}, {"addd",3,6,AMODE_EXT,OMODE_16_1},
	{"andb",3,4,AMODE_EXT,OMODE_16_1}, {"bitb",3,4,AMODE_EXT,OMODE_16_1},
	{"ldab",3,4,AMODE_EXT,OMODE_16_1}, {"stab",3,4,AMODE_EXT,OMODE_16_1},
	{"eorb",3,4,AMODE_EXT,OMODE_16_1}, {"adcb",3,4,AMODE_EXT,OMODE_16_1},
	{"orab",3,4,AMODE_EXT,OMODE_16_1}, {"addb",3,4,AMODE_EXT,OMODE_16_1},
	{"ldd",3,5,AMODE_EXT,OMODE_16_1}, {"std",3,5,AMODE_EXT,OMODE_16_1},
	{"ldx",3,5,AMODE_EXT,OMODE_16_1}, {"stx",3,5,AMODE_EXT,OMODE_16_1}
};

struct opcode ops2[] = {
	// 0x0
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"iny",2,4,AMODE_INH,OMODE_NONE}, {"dey",2,4,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x10
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"bset",4,8,AMODE_INDY,OMODE_8_2}, {"bclr",4,8,AMODE_INDY,OMODE_8_2},
	{"brset",5,8,AMODE_INDY,OMODE_8_3}, {"brclr",5,8,AMODE_INDY,OMODE_8_3},
	// 0x20
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x30
	{"tsy",2,4,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"tys",2,4,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"puly",2,6,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"aby",2,4,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"pshy",2,5,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x40
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x50
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x60	
	{"neg",3,7,AMODE_INDY,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"com",3,7,AMODE_INDY,OMODE_8_1},
	{"lsr",3,7,AMODE_INDY,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ror",3,7,AMODE_INDY,OMODE_8_1}, {"asr",3,7,AMODE_INDY,OMODE_8_1},
	{"asl",3,7,AMODE_INDY,OMODE_8_1}, {"rol",3,7,AMODE_INDY,OMODE_8_1},
	{"dec",3,7,AMODE_INDY,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"inc",3,7,AMODE_INDY,OMODE_8_1}, {"tst",3,7,AMODE_INDY,OMODE_8_1},
	{"jmp",3,4,AMODE_INDY,OMODE_8_1}, {"clr",3,7,AMODE_INDY,OMODE_8_1},
	// 0x70
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x80
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"cpd",4,5,AMODE_IMM,OMODE_16_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpy",4,5,AMODE_IMM,OMODE_16_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"xgdy",2,4,AMODE_INH,OMODE_NONE},
	// 0x90
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"cpd",3,6,AMODE_DIR,OMODE_8_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpy",3,6,AMODE_DIR,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0xA0
	{"suba",3,5,AMODE_INDY,OMODE_8_1}, {"cmpa",3,5,AMODE_INDY,OMODE_8_1},
	{"sbca",3,5,AMODE_INDY,OMODE_8_1}, {"subd",3,7,AMODE_INDY,OMODE_8_1},
	{"anda",3,5,AMODE_INDY,OMODE_8_1}, {"bita",3,5,AMODE_INDY,OMODE_8_1},
	{"ldaa",3,5,AMODE_INDY,OMODE_8_1}, {"staa",3,5,AMODE_INDY,OMODE_8_1},
	{"eora",3,5,AMODE_INDY,OMODE_8_1}, {"adca",3,5,AMODE_INDY,OMODE_8_1},
	{"oraa",3,5,AMODE_INDY,OMODE_8_1}, {"adda",3,5,AMODE_INDY,OMODE_8_1},
	{"cpy",3,7,AMODE_INDY,OMODE_8_1}, {"jsr",3,7,AMODE_INDY,OMODE_8_1},
	{"lds",3,6,AMODE_INDY,OMODE_8_1}, {"sts",3,6,AMODE_INDY,OMODE_8_1},
	// 0xB0
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"cpd",4,7,AMODE_EXT,OMODE_16_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpy",4,7,AMODE_EXT,OMODE_16_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0xC0
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ldy",4,4,AMODE_IMM,OMODE_16_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0xD0
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ldy",3,5,AMODE_DIR,OMODE_8_1}, {"sty",3,5,AMODE_DIR,OMODE_8_1},
	// 0xE0
	{"subb",3,5,AMODE_INDY,OMODE_8_1}, {"cmpb",3,5,AMODE_INDY,OMODE_8_1},
	{"sbcb",3,5,AMODE_INDY,OMODE_8_1}, {"addd",3,7,AMODE_INDY,OMODE_8_1},
	{"andb",3,5,AMODE_INDY,OMODE_8_1}, {"bitb",3,5,AMODE_INDY,OMODE_8_1},
	{"ldab",3,5,AMODE_INDY,OMODE_8_1}, {"stab",3,5,AMODE_INDY,OMODE_8_1},
	{"eorb",3,5,AMODE_INDY,OMODE_8_1}, {"adcb",3,5,AMODE_INDY,OMODE_8_1},
	{"orab",3,5,AMODE_INDY,OMODE_8_1}, {"addb",3,5,AMODE_INDY,OMODE_8_1},
	{"ldd",3,6,AMODE_INDY,OMODE_8_1}, {"std",3,6,AMODE_INDY,OMODE_8_1},
	{"ldy",3,6,AMODE_INDY,OMODE_8_1}, {"sty",3,6,AMODE_INDY,OMODE_8_1},
	// 0xF0
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"ldy",4,6,AMODE_EXT,OMODE_16_1}, {"sty",4,6,AMODE_EXT,OMODE_16_1},
};

struct opcode ops3[] = {
	// 0x00
	{"ldy",3,6,AMODE_INDX,OMODE_8_1}, {"sty",3,6,AMODE_INDX,OMODE_8_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpy",3,7,AMODE_INDX,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpd",4,7,AMODE_EXT,OMODE_16_1}, {"cpd",3,7,AMODE_INDX,OMODE_8_1},
	{"cpd",3,6,AMODE_DIR,OMODE_8_1}, {"cpd",4,5,AMODE_IMM,OMODE_16_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	// 0x10
	{"ldx",3,6,AMODE_INDY,OMODE_8_1}, {"stx",3,6,AMODE_INDY,OMODE_8_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"cpx",3,7,AMODE_INDY,OMODE_8_1}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"cpd",3,7,AMODE_INDY,OMODE_8_1},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE},
	{"invalid",1,0,AMODE_INH,OMODE_NONE}, {"invalid",1,0,AMODE_INH,OMODE_NONE}
};

	

// Constructor.
// Disassemble code and place result in allocated linked list. Unfinished
// instructions will not be disassembled.
Disasm::Disasm(unsigned short addr, unsigned char code[], int len) {
	struct dis_instr *instr1, *instr2;
	struct dis_instr dummy_head;
	int i, j;

	da_list = instr1 = &dummy_head;
	// each run creates another dis_instr structure appended to the list
	for (i=j=0; i<len; i+=j) {
		// create structure
		instr2 = new struct dis_instr;
		if (instr2 == NULL) {
			perror(": Disasm(): ");		// XXX take care of this error, L00ZER!
			break;
		}

		// fill structure
		j = DisasmOp(addr+i, code+i, len-i, instr2);
		if (j == -1) {	// j==-1 when instr. size is larger than remaining len
			delete instr2;				// delete incomplete structure
			break;						// break out of for loop
		}

		// link structure into list
		instr1->next = instr2;
		instr1 = instr2;
	}

	instr1->next = NULL;		// terminate list
	da_list = da_list->next;	// ignore dummy head
}


// Destructor.
// Free da_list.
Disasm::~Disasm() {
	struct dis_instr *instr = da_list;

	// free allocated memories
	while (instr != NULL) {
		instr = instr->next;
		delete da_list;
		da_list = instr;
	}
}


// Get the list of disassembled instructions.
struct dis_instr *Disasm::GetList() {
	return da_list;
}


// Disassembles the next instruction in code array. If instruction is longer
// than len bytes, it is not disassembled. Returns the number of bytes in
// disassembled instruction or -1 if the instruction wasn't completely in 
// the code array.
// FOR THE LOVE OF GOD, there's got to be a better way to translate machine
// code into into asm. Is there some logic behind the mcode assignments ??
int Disasm::DisasmOp(unsigned short addr, unsigned char *code, int len, struct dis_instr *instr) {
	struct opcode op;
	char mcode[4], opbuf1[32], opbuf2[32], opbuf3[32];
	unsigned char *_code = code;
	int operand1, operand2, operand3;
	int i, j;

	// flush buffers
	opbuf1[0] =	opbuf2[0] = opbuf3[0] = 0;

	// select opcode table
	// this SUX ..
	if (code[0] != 0x18 && code[0] != 0x1A && code[0] != 0xCD)
		op = ops[code[0]];				// normal 1-byte opcodes
	else {
		if (len < 2)
			return -1;

		if (code[0] == 0x18)			// normal 2-byte opcodes
			op = ops2[code[1]];
		else {							// special case 2-byte opcodes
			i = (((code[1]>>4)&0xF)^(code[1]&0xF)) | (code[0]&1);	// get index
			op = ops3[i];
		}

		code++;
	}
	
	// check if the complete instruction is in the data array
	if (op.bytes > len)
		return -1;
	
	// put address into structure
	instr->addr = addr;

	// put machine code into structure
	for (i=0; i<op.bytes; i++) {
		sprintf(mcode, "%02X ", _code[i]);
		strcat(instr->mcode, mcode);
	}

	// put mnemonic into structure.
	// mnemonic and operands must not be more than 39 chars (coz there's no
	// room in the dis_instr structure).
	strncpy(instr->instr, op.opc, 9);

	// insert some whatespace
	j = strlen(instr->instr);
	for (i=0; i<8-j; i++)
		strcat(instr->instr, " ");
	
	// put operands into structure
	switch (op.operands) {				// parse operands
		case OMODE_8_3:
			operand3 = code[3];
			sprintf(opbuf3, "%02X", operand3);
		case OMODE_8_2:
			operand2 = code[2];
			sprintf(opbuf2, "%02X", operand2);
		case OMODE_8_1:
			operand1 = code[1];
			sprintf(opbuf1, "%02X", operand1);
			break;
		case OMODE_16_1:
			operand1 = code[1]<<8 | code[2];
			sprintf(opbuf1, "%04X", operand1);
			break;
		default:
			break;
	}

	switch (op.addr_mode) {				// put operands into structure
		case AMODE_INDX:
		case AMODE_INDY:
			strcat(instr->instr, opbuf1);
			if (op.addr_mode == AMODE_INDX)
				strcat(instr->instr, ",X");
			else
				strcat(instr->instr, ",Y");
			if (op.operands == OMODE_8_2 || op.operands == OMODE_8_3) {
				strcat(instr->instr, " #$");
				strcat(instr->instr, opbuf2);
			}
			if (op.operands == OMODE_8_3) {
				strcat(instr->instr, " $");
				strcat(instr->instr, opbuf3);
			}
			break;
		case AMODE_DIR:
			strcat(instr->instr, "$");
			strcat(instr->instr, opbuf1);
			if (op.operands == OMODE_8_2 || op.operands == OMODE_8_3) {
				strcat(instr->instr, " #$");
				strcat(instr->instr, opbuf2);
			}
			if (op.operands == OMODE_8_3) {
				strcat(instr->instr, " $");
				strcat(instr->instr, opbuf3);
			}
			break;
		case AMODE_IMM:
			strcat(instr->instr, "#$");
			strcat(instr->instr, opbuf1);
			break;
		case AMODE_REL:
		case AMODE_EXT:
			strcat(instr->instr, "$");
			strcat(instr->instr, opbuf1);
			break;
		case AMODE_INH:
		default:
			break;
	}

	// put number of bytes for this instruction into structure
	instr->bytes = op.bytes;

	// put number of cycles for this instruction into structure
	instr->cycles = op.cycles;

	return op.bytes;
}
