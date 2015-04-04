
#ifndef DISASM_H
#define DISASM_H


class Disasm {
public:
	Disasm(unsigned short addr, unsigned char code[], int len);
	~Disasm();
	struct dis_instr *GetList();
	//void Refresh();
private:
	int DisasmOp(unsigned short addr, unsigned char *code, int len, struct dis_instr *instr);

private:
	struct dis_instr *da_list;		// list head
};


// Opcode structure. The disasm module defines one structure for each opcode.
struct opcode {
	char *opc;			// mnemonic
	short bytes;		// instruction size in bytes
	short cycles;		// execution time in cycles
	int addr_mode;		// addressing mode
	int operands;		// number and bitwidth of operands
};

// Disassembled instruction structure. Maybe this should be an object instead,
// with dynamic mcode and instr strings.
struct dis_instr {
	unsigned short addr;
	char mcode[15];				// machine code (ASCII, hope 14 is enough)
	char instr[40];				// mnemonic and operands (also ASCII)
	unsigned short bytes;
	unsigned short cycles;
	struct dis_instr *next;
};

// these specify how verbose the disassembly is
#define DMODE_BYTES		1		// show number of bytes of instruction
#define DMODE_CYCLES	2		// show execution time (in cycles) of instr.
#define DMODE_CODE		4		// show machine code of instruction

// addressing modes, for addr_mode in opcode struct
#define AMODE_REL		0
#define AMODE_INH		1
#define AMODE_IMM		2
#define AMODE_DIR		3
#define AMODE_EXT		4
#define AMODE_INDX		5
#define AMODE_INDY		6

// operand format: bit-width and number of ops, for operands in opcode struct
#define OMODE_NONE		0x0000	// no operands
#define OMODE_8_1		0x0001	// 1 8-bit operand
#define OMODE_8_2		0x0002	// 2 8-bit operands
#define OMODE_8_3		0x0003	// 3 8-bit operands
#define OMODE_16_1		0x0101	// 1 16-bit operand


#endif // DISASM_H
