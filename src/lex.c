
/*
 * lex.c
 *
 * This module parses the options from a string. The line variable is only valid
 * if GetEnvOpt_RC() is used before any user commands are issued.
 *
 * public routines:
 *		Token() gets the next token from a string.
 * private routines:
 *		Next() switches to the next transition diagram to follow.
 *		Lookup() checks if a word is reserved.
 *		Debug() prints token info.
 *
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "lex.h"


/* array of possible rc file options */
struct token options[] = {
	{TOKEN_OPTION, OPT_MODE, 0, "mode"},
	{TOKEN_OPTION, OPT_TALKER_PAGE, 0, "talker"},
	{TOKEN_OPTION, OPT_RAM_PAGE, 0, "ram"},
	{TOKEN_OPTION, OPT_REG_PAGE, 0, "reg"},
	{TOKEN_OPTION, OPT_ON_CHIP_ROM, 0, "rom"},
	{TOKEN_OPTION, OPT_ON_CHIP_EEPROM, 0, "eeprom"},
	{TOKEN_OPTION, OPT_TIMER_RATE, 0, "tmr"},
	{TOKEN_OPTION, OPT_COP_RATE, 0, "cop"},
	{TOKEN_OPTION, OPT_IRQ_MODE, 0, "irq"},
	{TOKEN_OPTION, OPT_OSC_DELAY, 0, "osc"},

	//{TOKEN_COMMAND, CMD_NOP, 0, ""},			/* ??? */
	{TOKEN_COMMAND, CMD_GET_DATA, CMD_DATA_B, "d"},
	{TOKEN_COMMAND, CMD_GET_DATA, CMD_DATA_B, "db"},
	{TOKEN_COMMAND, CMD_GET_DATA, CMD_DATA_W, "dw"},
	{TOKEN_COMMAND, CMD_SET_DATA, 0, "bf"},
	{TOKEN_COMMAND, CMD_GET_CODE, 0, "c"},
	{TOKEN_COMMAND, CMD_SET_CODE, 0, "load"},
	{TOKEN_COMMAND, CMD_GET_STATE, 0, "r"},
	//{TOKEN_COMMAND, CMD_SET_STATE, "?"},
	{TOKEN_COMMAND, CMD_EXEC, 0, "g"},
	{TOKEN_COMMAND, CMD_BP, CMD_BP_SET, "bp"},
	{TOKEN_COMMAND, CMD_BP, CMD_BP_LIST, "bpl"},
	{TOKEN_COMMAND, CMD_BP, CMD_BP_CLEAR, "bpr"},
	{TOKEN_COMMAND, CMD_CLS, 0, "cls"},
	{TOKEN_COMMAND, CMD_QUIT, 0, "q"},
	{TOKEN_COMMAND, CMD_QUIT, 0, "quit"},
	{TOKEN_COMMAND, CMD_HELP, 0, "h"},
	{TOKEN_COMMAND, CMD_HELP, 0, "help"},
	
	{0, 0, 0, ""}
};


char lex[LEX_LEN];
int lex_ptr;
int str_ptr;
int line = 1;

int state, sstate;


/* These two macros only work within Token() and Next(). */
#define nextchar()\
	if (sscanf(str+str_ptr, "%c", &c) == EOF) return NULL;\
	if (lex_ptr < LEX_LEN-1) lex[lex_ptr++] = c;\
	str_ptr++;

#define ungetchar()\
	lex[--lex_ptr] = 0;\
	str_ptr--;

/* This works only in Token(). */
#define Return(a)\
	/*Debug(t);*/\
	return a;


/* Find the next token in str and fill in the token struct. Return a pointer
 * to the first character after the found token or return NULL and t.token=0
 * if no valid token was found. */
char *Token(char *str, struct token *t) {
	char c;

	bzero(lex, LEX_LEN);
	lex_ptr = 0;
	sstate = state = 1;
	str_ptr = 0;
	
	/* if only whitespace are found, return the CMD_NOP command */
	t->token = 0;
	t->attr = 0;
	t->attr2 = 0;
	t->lex = lex;

	if (str == NULL)
		return NULL;

	do {
		switch (state) {
			case 1:
				nextchar();

				/* strip blanks */
				if (c == ' ' || c == '\t' || c == '\n') {
					lex[--lex_ptr] = 0;
					if (c == '\n')
						line++;
					break;
				}

				if (isdigit(c)) state = 2;
				else state = Next();
				break;
			case 2:
				nextchar();

				if (isdigit(c)) break;
				else if (c == '.') state = 3;
				else state = 5;
				break;
			case 3:
				nextchar();

				if (isdigit(c)) state = 4;
				else state = Next();
				break;
			case 4:
				nextchar();

				if (isdigit(c)) break;
				else state = 5;
				break;
			case 5:
				ungetchar();
				t->token = TOKEN_NUM;
				t->attr = (unsigned int)atoi(lex);
				Return(str+str_ptr);
			
			case 6:
				nextchar();

				if (isalpha(c)) state = 7;
				else state = Next();
				break;
			case 7:
				nextchar();

				if (isalnum(c)) break;
				else state = 8;
				break;
			case 8:
				ungetchar();
				if (!Lookup(t))
					t->token = TOKEN_WORD;
				Return(str+str_ptr);
			
			case 9:
				nextchar();

				if (c == '#') state = 10;
				else state = Next();
				break;
			case 10:
				nextchar();

				if (c == '\n') state = 11;
				break;
			case 11:
				ungetchar();
				t->token = TOKEN_COMMENT;
				Return(str+str_ptr);
				
			default:
				printf("internal parser error: Token() in rc.c\n");
				exit(-1);
		}
	} while (1);
}


int Next() {
	ungetchar();

	switch (sstate) {
		case 1: sstate = 6; break;
		case 6: sstate = 9; break;
		case 9:
			printf("line %d: unrecognized token\n", line);
			sstate = 1;
			str_ptr--;
			break;
		default:
			printf("internal parser error: Next() in rc.c\n");
			exit(-1);
	}

	return sstate;
}


/* Check is t->lex is in the list of reserved keywords. If so, fill in the rest
 * of the token structure. */
int Lookup(struct token *t) {
	struct token *token_tmp = options;

	while (token_tmp->token) {
		if (strcmp(token_tmp->lex, t->lex) == 0) {
			t->token = token_tmp->token;
			t->attr = token_tmp->attr;
			t->attr2 = token_tmp->attr2;
			return 1;
		} else
			token_tmp++;
	}

	return 0;		/* no match found */
}


void Debug(struct token *t) {
	switch (t->token) {
		case TOKEN_NUM: printf("TOKEN_NUM"); break;
		case TOKEN_WORD: printf("TOKEN_WORD"); break;
		case TOKEN_COMMENT: printf("TOKEN_COMMENT"); break;
		case TOKEN_OPTION: printf("TOKEN_OPTION"); break;
		case TOKEN_COMMAND: printf("TOKEN_COMMAND"); break;
		default: printf("BAD_TOKEN");
	}
	printf(" %d %d %s\n", t->attr, t->attr2, t->lex);
}
