
#ifndef LEX_H
#define LEX_H


struct token {
	int token;
	int attr;
	int attr2;
	char *lex;
};

#define LEX_LEN				256

#define TOKEN_NONE			0
#define TOKEN_NUM			1
#define TOKEN_WORD			2
#define TOKEN_COMMENT		3
#define TOKEN_OPTION		4
#define TOKEN_COMMAND		5

#define OPT_MODE			1
#define OPT_TALKER_PAGE		2
#define OPT_RAM_PAGE		3
#define OPT_REG_PAGE		4
#define OPT_ON_CHIP_ROM		5
#define OPT_ON_CHIP_EEPROM	6
#define OPT_TIMER_RATE		7
#define OPT_COP_RATE		8
#define OPT_IRQ_MODE		9
#define OPT_OSC_DELAY		10

//#define CMD_NOP				11
//#define CMD_GET_DATA		12
//#define CMD_SET_DATA		13
//#define CMD_GET_STATE		14
//#define CMD_SET_STATE		15
//#define CMD_EXEC			16
//#define CMD_GET_CODE		17
//#define CMD_SET_CODE		18
//#define CMD_CLS				19
//#define CMD_QUIT			20
//#define CMD_HELP			21


char *Token(char *str, struct token *t, int cmd);
int Next(void);
int Lookup(struct token *t, struct token *table);
void Debug(struct token *t);


#endif
