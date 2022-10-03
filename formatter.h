/*
	C language grammar analyst
	formatter.h
*/

#ifndef FORMATTER_H
#define FORMATTER_H

typedef struct token{
	char *str;
	struct token *next;
} TOKEN;

void format(char *destfile);
extern TOKEN* readline(void);

#endif
