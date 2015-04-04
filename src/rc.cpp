
//
// rc.c
//
// This module parses the options from a configuration file.
//
// main.c uses GetEnvOpt_RC().
//
// public routines:
//		GetEnvOpt_RC() parses the options from a configuration file.
//


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugger.h"
#include "lex.h"
#include "rc.h"


int GetEnvOpt_RC(struct mcu_env *env) {
	struct token t, t2;
	char buf[BUF_SIZE];	// rc file must not be larger than BUF_SIZE bytes
	char *str = buf;
	int fd;

	// try to open an rc file
	if ((fd = open(USER_RC_FILE, O_RDONLY)) == -1)
		if ((fd = open(GLOBAL_RC_FILE, O_RDONLY)) == -1)
			return -1;

	// read file
	if (read(fd, buf, BUF_SIZE-1) == -1)
		return -1;
	close(fd);

	// if the file is successfully opened and read , options won't be asked
	// from the user, even if it is empty
	while ((str = Token(str, &t, TOKEN_OPTION))) {
		switch (t.token) {
			// ignore comments
			case TOKEN_COMMENT:
				break;
			// shouldn't be any of theese here
			case TOKEN_NUM:
			case TOKEN_WORD:
			case TOKEN_COMMAND:
				printf("line ?: invalid option\n");
				break;
			// parse an option
			case TOKEN_OPTION:
				// get next token (option value)
				str = Token(str, &t2, TOKEN_OPTION);
				if (!str || (t2.token != TOKEN_WORD && t2.token != TOKEN_NUM)) {
					printf("line ?: invalid option value\n");
					break;
				}

				switch (t.attr) {
					case OPT_MODE:						// mode
						if (!strcmp(t2.lex, "s"))
							env->mode = MCU_MODE_SINGLECHIP;
						else if (!strcmp(t2.lex, "e"))
							env->mode = MCU_MODE_EXPANDED;
						else if (!strcmp(t2.lex, "b"))
							env->mode = MCU_MODE_BOOTSTRAP;
						else if (!strcmp(t2.lex, "t"))
							env->mode = MCU_MODE_TEST;
						else
							printf("line ?: invalid option value\n");
						break;
					case OPT_TALKER_PAGE:				// talker page
						if (t2.attr <= 15  && t2.token == TOKEN_NUM)
							env->talker_page = t2.attr;
						else
							printf("line ?: invalide option value\n");
						break;
					case OPT_RAM_PAGE:					// ram page
						if (t2.attr <= 15 && t2.token == TOKEN_NUM)
							env->ram_page = t2.attr;
						else
							printf("line ?: invalide option value\n");
						break;
					case OPT_REG_PAGE:					// reg page
						if (t2.attr <= 15  && t2.token == TOKEN_NUM)
							env->reg_page = t2.attr;
						else
							printf("line ?: invalide option value\n");
						break;
					case OPT_ON_CHIP_EEPROM:			// eeprom
						if (!strcmp(t2.lex, "on"))
							env->on_chip_eeprom = 1;
						else if (!strcmp(t2.lex, "off"))
							env->on_chip_eeprom = 0;
						else
							printf("line ?: invalid option value\n");
						break;
					case OPT_ON_CHIP_ROM:				// rom
						if (!strcmp(t2.lex, "on"))
							env->on_chip_rom = 1;
						else if (!strcmp(t2.lex, "off"))
							env->on_chip_rom = 0;
						else
							printf("line ?: invalid option value\n");
						break;
					case OPT_IRQ_MODE:					// irq mode
						if (!strcmp(t2.lex, "edge"))
							env->irq_mode = 1;
						else if (!strcmp(t2.lex, "level"))
							env->irq_mode = 0;
						else
							printf("line ?: invalid option value\n");
						break;
					case OPT_OSC_DELAY:					// osc delay
						if (!strcmp(t2.lex, "on"))
							env->osc_delay = 1;
						else if (!strcmp(t2.lex, "off"))
							env->osc_delay = 0;
						else
							printf("line ?: invalid option value\n");
						break;
					case OPT_TIMER_RATE:				// timer rate
						if ((t2.attr == 1 || t2.attr == 4 || t2.attr == 8 ||\
									t2.attr == 16) && t2.token == TOKEN_NUM)
							env->timer_rate = t2.attr;
						else
							printf("line ?: invalide option value\n");
						break;
					case OPT_COP_RATE:					// cop rate
						if ((t2.attr == 1 || t2.attr == 4 ||t2.attr == 16 ||\
								t2.attr == 64) && t2.token==TOKEN_NUM)
							env->cop_rate = t2.attr;
						else
							printf("line ?: invalide option value\n");
						break;
					default:
						printf("line ?: invalid option\n");
				}

				break;
			default:
				printf("internal rc.c error: illegal token\n");
		}
	}

	return 0;
}
