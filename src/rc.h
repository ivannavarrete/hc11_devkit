
#ifndef RC_H
#define RC_H


#define GLOBAL_RC_FILE		"/etc/hc11rc"
#define USER_RC_FILE		"~/.hc11rc"

#define LEX_LEN				256

#define TOKEN_NUM			1
#define TOKEN_WORD			2
#define TOKEN_COMMENT		3


int GetEnvOpt_RC(struct mcu_env *env);
int Token(void);
int Next(void);


#endif RC_H
