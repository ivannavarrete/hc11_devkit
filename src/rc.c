
/*
 * rc.c
 *
 * This module parses the options from a configuration file. The technique used
 * here is the one described in 'Compilers: principles, techniques and tools'
 * ch. 3 (and probably in many other places :).
 *
 * main.c uses GetEnvOpt_RC().
 *
 * public routines:
 *		GetEnvOpt_RC() parses the options from a configuration file.
 * private routines:
 *		Token() gets the next token from the configuration file.
 *		Next() switches to the next transition diagram to follow.
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

#include "monitor.h"
#include "rc.h"


int fd;

int token;
char lex[LEX_LEN];
char lex2[LEX_LEN];
int lex_ptr;
int line = 1;

int state, sstate;



#define nextchar()\
	if (read(fd, &c, 1) <= 0) return 0;\
	if (lex_ptr < LEX_LEN-1) lex[lex_ptr++] = c;

#define ungetchar()\
	lseek(fd, -1, SEEK_CUR);\
	lex[--lex_ptr] = 0;


int GetEnvOpt_RC(struct mcu_env *env) {
	int num;

	/* try to open an rc file */
	if ((fd = open(USER_RC_FILE, O_RDONLY)) == -1)
		if ((fd = open(GLOBAL_RC_FILE, O_RDONLY)) == -1)
			return -1;
	
	/* if the file is successfully opened, options won't be asked from the
	 * user, even if it is empty */
	while ((token = Token())) {
		switch (token) {
			/* ignore comments */
			case TOKEN_COMMENT:
				break;
			/* shouldn't be any numbers here */
			case TOKEN_NUM:
				printf("line %d: invalid option\n", line);
				break;
			/* parse an option */
			case TOKEN_WORD:
				strcpy(lex2, lex);
				
				token = Token();
				if (token != TOKEN_WORD && token != TOKEN_NUM) {
					printf("line %d: invalid option value\n", line);
					break;
				}

				if (!strcmp(lex2, "mode")) {			/* mode */
					if (!strcmp(lex, "s"))
						env->mode = MCU_MODE_SINGLECHIP;
					else if (!strcmp(lex, "e"))
						env->mode = MCU_MODE_EXPANDED;
					else if (!strcmp(lex, "b"))
						env->mode = MCU_MODE_BOOTSTRAP;
					else if (!strcmp(lex, "t"))
						env->mode = MCU_MODE_TEST;
					else
						printf("line %d: invalid option value\n", line);
				} else if (!strcmp(lex2, "talker"))	{	/* talker */
					num = (unsigned int)atoi(lex);
					if (num <= 15  && token == TOKEN_NUM)
						env->talker_page = num;
					else
						printf("line %d: invalide option value\n", line);
				} else if (!strcmp(lex2, "ram")) {		/* ram */
					num = (unsigned int)atoi(lex);
					if (num <= 15 && token == TOKEN_NUM)
						env->ram_page = num;
					else
						printf("line %d: invalide option value\n", line);
				} else if (!strcmp(lex2, "reg")) {		/* reg */
					num = (unsigned int)atoi(lex);
					if (num <= 15  && token == TOKEN_NUM)
						env->reg_page = num;
					else
						printf("line %d: invalide option value\n", line);
				} else if (!strcmp(lex2, "eeprom")) {	/* eeprom */
					if (!strcmp(lex, "on"))
						env->on_chip_eeprom = 1;
					else if (!strcmp(lex, "off"))
						env->on_chip_eeprom = 0;
					else
						printf("line %d: invalid option value\n", line);
				} else if (!strcmp(lex2, "rom")) {		/* rom */
					if (!strcmp(lex, "on"))
						env->on_chip_rom = 1;
					else if (!strcmp(lex, "off"))
						env->on_chip_rom = 0;
					else
						printf("line %d: invalid option value\n", line);
				} else if (!strcmp(lex2, "irq")) {		/* irq */
					if (!strcmp(lex, "edge"))
						env->on_chip_eeprom = 1;
					else if (!strcmp(lex, "level"))
						env->on_chip_eeprom = 0;
					else
						printf("line %d: invalid option value\n", line);
				} else if (!strcmp(lex2, "osc")) {		/* osc */
					if (!strcmp(lex, "on"))
						env->osc_delay = 1;
					else if (!strcmp(lex, "off"))
						env->osc_delay = 0;
					else
						printf("line %d: invalid option value\n", line);
				} else if (!strcmp(lex2, "tmr")) {		/* tmr */
					num = (unsigned int)atoi(lex);
					if ((num == 1 || num == 4 || num == 8 || num == 16) &&\
						token == TOKEN_NUM)
						env->timer_rate = num;
					else
						printf("line %d: invalide option value\n", line);
				} else if (!strcmp(lex2, "cop")) {		/* cop */
					num = (unsigned int)atoi(lex);
					if ((num == 1 || num == 4 || num == 16 || num == 64) &&\
						token==TOKEN_NUM)
						env->cop_rate = num;
					else
						printf("line %d: invalide option value\n", line);
				} else
					printf("line %d: invalid option\n", line);

				break;
			default:
				printf("internal rc.c error: illegal token\n");
		}
	}

	close(fd);
	return 0;
}


/* This routine recognizes decimal numbers. Probably unnecessary.
 * Note that this routine ignores read() errors and takes them for EOF. Can this
 * become a problem? */
int Token() {
	char c;

	bzero(lex, LEX_LEN);
	lex_ptr = 0;
	sstate = state = 1;

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
				return TOKEN_NUM;
			
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
				return TOKEN_WORD;
			
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
				return TOKEN_COMMENT;
			default:
				printf("internal parser error: Token() in rc.c\n");
				exit(-1);
		}
	} while (1);

	return 0;
}


int Next() {
	ungetchar();

	switch (sstate) {
		case 1: sstate = 6; break;
		case 6: sstate = 9; break;
		case 9:
			printf("line %d: unrecognized token\n", line);
			lseek(fd, 1, SEEK_CUR);
			sstate = 1;
			break;
		default:
			printf("internal parser error: Next() in rc.c\n");
			exit(-1);
	}

	return sstate;
}
