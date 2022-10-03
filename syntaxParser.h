/*
	C language grammar analyst
	syntaxParser.h
*/ 
#ifndef SYNTAX_PARSER_H
#define SYNTAX_PARSER_H

#define MAXSTACK 100

typedef struct data{
	int type;
	char *data;
} DATA;

typedef struct ASTTree{  // abstract syntax tree
	struct ASTTree *lc, *rc;
	int type;
	DATA data;
} AST;  

typedef struct variableNode{  // memorize variables
	int size;
	char *var[35];
	struct variableNode *next;
} VAR;

typedef struct parameter{
	int type;
	char *name;
} PARA;

typedef struct functionNode{
	int returnType;
	int paranum;
	PARA para[25];
	char *name;
	struct functionNode *next;
} FUN;

enum dataKind{
	EXTDEFLIST = 1,  // extern definition list
	EXTVARLIST, // extern variable definition list
	EXTVARTYPE,  // extern variable type
	EXTVAR,  // extern variable name
	EXTVARDEF,  // extern variable definition
	EXTVARINIT,  // extern variable initialization
	ARRAYDEF,  // array definition
	ARRAYNAME,  // array name
	ARRAYTYPE,  // array type
	ARRAYSIZE,  // array size 
	FUNCDEF,  // function type
	FUNCTION,  // function name
	FUNCRETURNTYPE,  // function return value type
	FUNCCLAIM,  // used when claiming a function without defining it
	FUNCBODY,  // function definition
	FUNCFORMPARALIST,  // formal parameter list
	FUNCFORMPARADEF,  // formal parameter definition
	FUNCFORMPARA,  // formal parameter name
	FUNCFORMPARATYPE,  // formal parameter type
	LOCVARDEFLIST,  // local variable definition list
	LOCVARDEF,  // local variable definition
	LOCVAR,  // local variable name
	LOCVARNAMELIST,  // local variable name list
	LOCVARTYPE,  // local variable type
	LOCVARINIT,  // local variable initialization
	STATELIST,  // statement list
	STATEMENT,  // statement
	IFPART,  // part following "if"
	IFELSESTATEMENT,  // an "if-else" statement set
	ELSEPART,  // part following "else"
	IFSTATEMENT,  // an "if" statement set without "else"
	WHILEPART,  // condition of "while"
	WHILEBODY,  // statements looping in a "while" set
	WHILESTATEMENT,  // a "while" statement set
	FORPART,  // condition of "for"
	FORPART1,  // condition 1 of "for"
	FORPART2,  // condition 2 of "for"
	FORPART3,  // condition 3 of "for"
	FORBODY,  // statements looping in a "for" set
	FORSTATEMENT,  // a "for" statement set
	DOWHILEBODY,  // statements looping in a "do-while" set
	DOWHILECONDITION,  // conditions of "do-while"
	DOWHILESTATEMENT,  // a "do-while" statement set 
	RETURNSTATEMENT,  // a "return" statement
	BREAKSTATEMENT,  // a "break" statement
	CONTINUESTATEMENT,  // a "continue" statement
	OPERATOR,  // operator
	OPERAND,  // operand
	EXPRESSION,  // expression
	FUNCTIONCALL,  // a call of function
	ACTUALPARALIST,  //  actual parameter list
	ACTUALPARA,  // actual parameter name
};

void analyze(void);
AST* program(void);
AST* ExtDefList(void);
AST* ExtDef(void);
AST* ExtVarDef(void);
AST* ArrayDef(void);
AST* FuncDef(void);
AST* FormParaList(void);
AST* FormParaDef(void);
AST* CompState(void);
AST* LocVarDefList(void);
AST* StateList(void);
AST* Statement(void);
AST* Expression(int endsym);
AST* FuncCall(char *name);
AST* ActualParaList(FUN *f);
char valuePrior(int a, int b);
void returnToken(int w);
void display(int type);
void preOrderTraverse(AST* T, int depth);
void addName(void);
void isDefined(void); 
int addFunc(void);
FUN* FindFunc(char *name);

#endif
