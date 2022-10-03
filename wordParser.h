/*
	C language grammar analyst
	wordParser.h
*/
#ifndef WORD_PARSER_H
#define WORD_PARSER_H

enum tokenKind{
	ERROR_TOKEN, IDENT,  // 0 1 
	INTCONST, FLOATCONST, CHARCONST, LONGCONST, UNSIGNEDCONST,  // 2 - 6
	LLCONST, ULCONST, ULLCONST, LDCONST, STRINGCONST,  // 7 - 11
	INT, FLOAT, CHAR, LONG, UNSIGNED, DOUBLE, SHORT, VOID,  // 12 - 19
	IF, ELSE, WHILE, FOR, DO, BREAK, CONTINUE, RETURN, OKEY,   // 20 - 28
	INCLUDE, DEFINE,  // 29 30
	EQ, NEQ, ASSIGN, LBK, RBK, PLUS, MINUS, AND, OR,  // 31 - 39
	PLUSEQ, MINUSEQ, MULTIEQ, DIVIDEEQ, MODEQ, // 40 - 44
	MULTI, DIVIDE, MOD, LA, RA, LAE, RAE, BOTTOM,   // 45 - 52
	LBS, RBS, LB, RB, SEMI, COMMA,   // 53 - 58
	ARRAY, ANNO,   // 59 60
};

int isLetter(char c);
int isNum(char c);
int isKey(void);
int getToken(void);
void parseWord(void);

#endif
