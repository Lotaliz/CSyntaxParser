/*
	C language grammar analyst
	syntexParser.c
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntaxParser.h"
#include "wordParser.h"

extern char tokenText[255];
extern char num[255];
extern int row;
int w, type;
int fatal = 0, arg = 0;
extern FILE *fp;
char text1[255];
VAR *vroot;
FUN *froot, *ftail;
int isVoid, hasReturn, isInCycle;

void analyze()
{
	row = 1;
	AST *root = program();
	if(!root || fatal){
		printf("程序语法出现错误！\n");
		return;
	}else{
		printf("代码处理完毕，未发现错误。\n");
		/* show AST tree by preorderTraverse */
		preOrderTraverse(root, 0);
	}
}

AST* program(void)
{
	w = getToken();
	while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	vroot = (VAR*)malloc(sizeof(VAR));
	vroot->size = 0;
	vroot->next = NULL;
	froot = (FUN*)malloc(sizeof(FUN));
	froot->name = NULL;
	froot->paranum = 0;
	froot->next = NULL;
	ftail = froot;
	AST *p = ExtDefList();
	if(p){
		if(!isVoid && !hasReturn){
			printf("错误：第%d行\n", row);
			printf("函数缺少返回值。\n");
			system("pause");
			exit(0);
		}
		AST *root = p;
		root->type = EXTDEFLIST;
		return root;
	}else{
		fatal = 1;
		return NULL;
	}
}

AST* ExtDefList(void)
{
	if(fatal) return NULL;
	if(w == EOF) return NULL; 
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: external definition; rc: external definition list root
	root->data.data = NULL;
	root->type = EXTDEFLIST;
	root->lc = ExtDef();
	w = getToken();
	while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	root->rc = ExtDefList();
	return root;
}

AST* ExtDef(void)
{
	if(fatal) return NULL;
		
	int a;
	if(w < INT || w > VOID){
		printf("错误：第%d行\n", row);
		printf("外部定义语法出现错误。\n");
		system("pause");
		exit(0);
	}
	type = w; // type is a data type
	w = getToken();
	while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	if(w != IDENT && w != ARRAY){
		printf("错误：第%d行\n", row);
		printf("外部定义语法出现错误。\n");
		system("pause");
		exit(0);
	}
	
	a = w; // a is an identifier or an array
	strcpy(text1, tokenText);
	AST *p;  // p is a definition of variable, array or function
	w = getToken();
	while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	strcpy(tokenText, text1);
	if(w == LBK) p = FuncDef();
	else if(a == ARRAY) p = ArrayDef();
	else p = ExtVarDef();
	return p;
}

AST* ExtVarDef(void)
{
	if(fatal) return NULL;
	int cnt = 0;
	if(type == VOID){
		printf("错误：第%d行\n", row);
		printf("变量类型不能是void。\n");
		system("pause");
		exit(0);
	}
	addName();
	
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: vartype p; rc: null
	root->lc = root->rc = NULL;
	root->data.data = NULL;
	root->type = EXTVARDEF;
	AST *p = (AST*)malloc(sizeof(AST));  // p: lc: var name; rc: var list q
	p->lc = p->rc = NULL;
	p->data.data = NULL;
	p->type = EXTVARTYPE;
	p->data.type = type;
	if(type == INT) p->data.data = "int";
    if(type == DOUBLE) p->data.data = "double";
    if(type == CHAR) p->data.data = "char";
    if(type == FLOAT) p->data.data = "float";
    if(type == LONG) p->data.data = "long";
    if(type == SHORT) p->data.data = "short";
    
    root->lc = p;
    p->lc = (AST*)malloc(sizeof(AST));
    p->lc->lc = p->lc->rc = NULL;
    p->lc->type = EXTVAR;
    char *text2 = (char*)malloc(sizeof(char)*25);
    strcpy(text2, tokenText);
    p->lc->data.data = text2;
    
    while(1){  // read respectively: initialize(if any) ,/; next variable(if any)
    	if(w == ASSIGN){
    		w = getToken();
    		while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	    	if(fatal) return NULL;
    		if(w < INTCONST || w >= STRINGCONST){
    	    	printf("错误：第%d行\n", row);
    			printf("外部变量初始化出现错误。\n");
    			system("pause");
				exit(0); 
			}			
    		AST *r = (AST*)malloc(sizeof(AST));  // p->lc: var def; lc: 
    		r->lc = r->rc = NULL;
    		r->type = EXTVARINIT;
    		char* text3 = (char*)malloc(25*sizeof(char));
    		strcpy(text3, tokenText);
    		r->data.data = text3;
    		p->lc->lc = r;
    	}else if(w != COMMA && w != SEMI){
    		if(row > cnt) row--;
    		printf("错误：第%d行\n", row);
    		printf("外部变量定义出现错误。\n");
    		system("pause");
			exit(0); 
		}
		if(w == SEMI) return root;
		w = getToken();
		if(w != IDENT){
			printf("错误：第%d行\n", row);
    		printf("外部变量定义出现错误。\n"); 
    		system("pause");
			exit(0); 
		}
		addName();
		
		AST* q = (AST*)malloc(sizeof(AST));  // q: lc: varname; rc: varlist q
		q->lc = q->rc = NULL;
		q->data.data = NULL;
		q->type = EXTVARLIST;
		p->rc = q;
		p = q;
		p->lc = (AST*)malloc(sizeof(AST));
		p->lc->lc = p->lc->rc = NULL;
		p->lc->data.data = NULL;
		p->lc->type = EXTVAR;
		char *text3 = (char*)malloc(25*sizeof(char));
		strcpy(text3, tokenText);
		p->lc->data.data = text3;
		cnt = row;
		w = getToken();
		while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	}
}

AST* ArrayDef(void)
{
	/* now tokenText includes the name of the array and [] */
	if(type == VOID){
		printf("错误：第%d行\n", row);
    	printf("数组类型不能是void。\n");
		system("pause"); 
		exit(0); 
	}
	AST* root = (AST*)malloc(sizeof(AST));  // root: lc: array type p1; rc: array name p2
	root->type = ARRAYDEF;
	root->lc = root->rc = NULL;
	root->data.data = NULL;
	AST* p = (AST*)malloc(sizeof(AST));  // p1: lc: null; rc: null
	p->type = ARRAYTYPE;
	p->lc = p->rc = NULL;
	if(type == INT) p->data.data = "int";
    if(type == DOUBLE) p->data.data = "double";
    if(type == CHAR) p->data.data = "char";
    if(type == FLOAT) p->data.data = "float";
    if(type == LONG) p->data.data = "long";
    if(type == SHORT) p->data.data = "short";
    
    root->lc = p;
	p = (AST*)malloc(sizeof(AST));  // p2: lc: array name q; rc: null
	p->type = ARRAYNAME;
	p->lc = p->rc = NULL;
	char *text2 = (char*)malloc(25*sizeof(char));
	strcpy(text2, tokenText);
	p->data.data = text2;
	root->rc = p;
	AST *q = (AST*)malloc(sizeof(AST));
	q->type = ARRAYSIZE;
	q->lc = q->rc = NULL;
	q->data.data = num;
	p->lc = q;
	return root;
}

AST* FuncDef(void)
{
	if(fatal) return NULL;
	isVoid = 0;
	
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: return value type p; rc: function name q
	root->data.data = NULL;
	root->type = FUNCDEF;
	AST* p = (AST*)malloc(sizeof(AST));  // p: lc & rc: null
    p->data.data = NULL;
    p->type = FUNCRETURNTYPE;
    p->data.type = type;
    if(type == INT) p->data.data = "int";
    else if(type == DOUBLE) p->data.data = "double";
    else if(type == CHAR) p->data.data = "char";
    else if(type == FLOAT) p->data.data = "float";
    else if(type == LONG) p->data.data = "long";
    else if(type == SHORT) p->data.data = "short";
    else if(type == VOID){
		p->data.data = "void";
		isVoid = 1;
	}
	p->lc = p->rc = NULL;
	root->lc = p;
	
	AST *q = (AST*)malloc(sizeof(AST));  // q: lc: formal parameter list; rc: function body (complex statements)
	q->lc = q->rc = NULL;
	q->type = FUNCTION;
	char *text2 = (char*)malloc(25*sizeof(char));
	strcpy(text2, tokenText);
	q->data.data = text2;
	int u = addFunc();
	if(u) fatal = 1;
	ftail->returnType = type;
	
	/* create nodes for variable link table */
	VAR *last = vroot;
	while(last->next) last = last->next;
	last->next = (VAR*)malloc(sizeof(VAR));
	last = last->next;
	last->next = NULL;
	last->size = 0;
	
	root->rc = q;
	q->lc = FormParaList();
	w = getToken();
	while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	if(w == SEMI){
		root->rc->rc = NULL;
		root->type = FUNCCLAIM;
	}else if(w == LBS){
		q->rc = CompState();
		q->rc->type = FUNCBODY;
	}else{
		printf("错误：第%d行\n", row);
    	printf("函数定义出现错误。\n"); 
    	system("pause");
		exit(0); 
	}
	return root;
}

AST* FormParaList(void)
{
	if(fatal) return NULL;
	/* now tokenText includes left bracket ('(') */
	w = getToken();
	while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	if(w == RBK) return NULL;
	if(w == COMMA){
		w = getToken();
		while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	}
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: formal parameter; rc: formal parameter list root
	root->data.data = NULL;
	root->type = FUNCFORMPARALIST;
	root->lc = FormParaDef();
	root->rc = FormParaList();
	return root;
}

AST* FormParaDef(void)
{
	if(fatal) return NULL;
    if(w < INT || w > SHORT){
        printf("错误：第%d行\n", row);
        printf("形参定义出现错误。\n");
        system("pause");
        exit(0);
    }
    
    type = w;
    w = getToken();
    while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
    if(w != IDENT){
        printf("错误：第%d行\n", row);
        printf("形参定义出现错误。\n");
        system("pause");
        exit(0);
	}
	
    AST* root = (AST*)malloc(sizeof(AST));  // root: lc: paremeter type p1; rc: parameter name p2
    root->data.data = NULL;
    root->type = FUNCFORMPARADEF;
    AST* p = (AST*)malloc(sizeof(AST));  // p1: lc & rc: null
    p->data.data = NULL;
    p->type = FUNCFORMPARATYPE;
    p->data.type = type;
    if(type == INT) p->data.data = "int";
    else if(type == DOUBLE) p->data.data = "double";
    else if(type == CHAR) p->data.data = "char";
    else if(type == FLOAT) p->data.data = "float";
    else if(type == LONG) p->data.data = "long";
    else if(type == SHORT) p->data.data = "short";
    else if(type == VOID) p->data.data = "void";
    p->lc = p->rc = NULL;
    root->lc = p;
    
    p = (AST*)malloc(sizeof(AST));  // p2: lc & rc: null
    p->data.data = NULL;
    p->type = FUNCFORMPARA;
    addName();
    
    char *text2 = (char*)malloc(25*sizeof(char));
    strcpy(text2, tokenText);
    p->data.data = text2;
    p->lc = p->rc = NULL;
    root->rc = p;
    
    ftail->para[ftail->paranum].name = text2;
    ftail->para[ftail->paranum++].type = type;
    return root;
}

AST* CompState(void)
{
	if(fatal) return NULL;
	
	/* now tokenText includes left brace ('{'), the complex statement closes when '}' is read */
	AST *root = (AST*)malloc(sizeof(AST));
	root->data.data = NULL;
	root->lc = root->rc = NULL;
	w = getToken();
	while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	if(w >= INT && w <= SHORT)
		root->lc = LocVarDefList();
	else root->lc = NULL;
	root->rc = StateList();
	if(w == RBS) return root;
	else{
		printf("错误：第%d行\n", row);
		printf("处理复合语句出错。\n");
		system("pause");
        exit(0);
	}
}

AST* LocVarDefList(void)
{
	if(fatal) return NULL;
	
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: local variable definition node p1; rc: lv def list root
	root->lc = root->rc = NULL;
	root->data.data = NULL;
	root->type = LOCVARDEFLIST;
	
	AST *p = (AST*)malloc(sizeof(AST));  // p1: lc: local variable type p2; rc: lv name list q
	p->lc = p->rc = NULL;
	p->data.data = NULL;
	p->type = LOCVARDEF;
	root->lc = p;
	
	p->lc = (AST*)malloc(sizeof(AST));  // p2: lc & rc: null
	p->lc->data.data = NULL;
	p->lc->type = LOCVARTYPE;
	char *text2 = (char*)malloc(25*sizeof(char));
    strcpy(text2, tokenText);
    p->lc->data.data = text2;
    p->lc->lc = p->lc->rc = NULL;
    
    w = getToken();
    while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	AST *q = (AST*)malloc(sizeof(AST));  // q: lc: local variable name q1; rc: lv name list q
	q->data.data = NULL;
	q->lc = q->rc = NULL;
	p->rc = q;
	q->type = LOCVARNAMELIST;
	
	q->lc = (AST*)malloc(sizeof(AST));  // q1: lc & rc: null
	q->lc->data.data = NULL;
	q->lc->type = LOCVAR;
	char *text3 = (char*)malloc(25*sizeof(char));
    strcpy(text3, tokenText);
    q->lc->data.data = text3;
    q->lc->lc = q->lc->rc = NULL;
	addName();
	
    while(1){  // parse following variables 
    	w = getToken();
    	while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
    	
    	if(w == SEMI){
    		q->rc = NULL;
    		w = getToken();
    		while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
    		break;
    	}else if(w == COMMA){
    		w = getToken();
    		while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
    		AST *r = (AST*)malloc(sizeof(AST));
    		r->data.data = NULL;
	    	q->rc = r;
    		q = q->rc;
    		q->type = LOCVARNAMELIST;
	    	q->lc = (AST*)malloc(sizeof(AST));
			q->lc->data.data = NULL;
			q->lc->type = LOCVAR;
			char *text4 = (char*)malloc(25*sizeof(char));
    		strcpy(text4, tokenText);
    		q->lc->data.data = text4;
    		q->lc->lc = q->lc->rc = NULL;
    		addName();
		}else if(w == ASSIGN){
    		w = getToken();
    		while(w == ANNO || w == INCLUDE || w == DEFINE) w = getToken();
	    	if(fatal) return NULL;
    		if(w < INTCONST || w >= STRINGCONST){
    	    	printf("错误：第%d行\n", row);
    			printf("局部变量初始化出现错误。\n");
    			system("pause");
				exit(0); 
			}			
    		AST *r = (AST*)malloc(sizeof(AST));  // p->lc: var def; lc: 
    		r->lc = r->rc = NULL;
    		r->type = LOCVARINIT;
    		char* text3 = (char*)malloc(25*sizeof(char));
    		strcpy(text3, tokenText);
    		r->data.data = text3;
    		q->lc->lc = r;
		}else{
			printf("错误：第%d行\n", row);
        	printf("局部变量定义出现错误。\n");
        	system("pause");
        	exit(0);
		}
	}
	
	/* if there is another row of local variable definitions */
	if(w >= INT && w <= SHORT)
		root->rc = LocVarDefList();
	else root->rc = NULL;
	root->rc = NULL;
	return root;  // returning with tokenText reading next token
}

AST* StateList(void)
{
	if(fatal) return NULL;
	AST *r1 = Statement();
	if(!r1) return NULL;  // statement list ends or fatal problems occur
	
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: statement node; rc: statement list root
	root->data.data = NULL;
	root->type = STATELIST;
	root->lc = r1;
	root->rc = NULL;
	w = getToken();
	while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	
	if(w != RBS) root->rc = StateList();  // when '}' is read, statement list ends
	return root;  // returning with tokenText reading next token
}

AST* Statement(){  // if, while, for, return, break, continue
    if(fatal) return NULL;
    
    /* now tokenText includes the first word of the statement */
    AST* root = (AST*)malloc(sizeof(AST));  // root: depending on the statement type
    root->lc = root->rc = NULL;
    root->data.data = NULL;
    switch(w){
        case IF: {  // "if" root: lc: if part p1; rc: else part (if any) p2
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != LBK) {
                printf("错误：第%d行\n", row);
        		printf("if条件语句出现错误。\n");
        		system("pause");
        		exit(0);
            }
            
        	w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            AST* p1 = (AST*)malloc(sizeof(AST));  // p1: lc: condition expression; rc: body statements
            p1->data.data = NULL;
            p1->type = IFPART;
            p1->lc = Expression(RBK);  //parse an expression
            if (p1->lc == NULL) {
                printf("错误：第%d行\n", row);
        		printf("if条件语句出现错误。\n");
        		system("pause");
        		exit(0);
            }
            
        	w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
			if (w == LBS) {
                /* statements are included in braces and end with '}' */
	        	w = getToken();
				while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                p1->rc = StateList();
            }else if(w >= INTCONST && w <= ULLCONST || w >= IF && w <= OKEY || w == IDENT){
                /* no  braces, only one statement */
                p1->rc = Statement();
                p1->rc->rc = NULL;
            }else{
            	printf("错误：第%d行\n", row);
                printf("if语句体出现错误。\n");
                fatal = 1;
                return NULL;
            }
            root->lc = p1;  // end of parsing "if"
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if (w == ELSE) {
                root->type = IFELSESTATEMENT;
                AST *p2 = (AST*)malloc(sizeof(AST));  // p2: lc: if part node (if any); rc: else statements
                p2->data.data = NULL;
                p2->type = ELSEPART;
                root->rc = p2;
                w = getToken();
				while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                if(w == LBS){
                    /* statements are included in braces and end with '}' */
                    w = getToken();
					while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                    p2->rc = StateList();
                }else if(w >= INTCONST && w <= ULLCONST || w >= IF && w <= OKEY || w == IDENT){
                    /* no  braces, only one statement */
                    p2->rc = Statement();
                    p2->rc->rc = NULL;
                }else if(w == IF) p2->lc = Statement();
				else{
					printf("错误：第%d行\n", row);
                    printf("else子句出现错误。\n");
                    fatal = 1;
                    return NULL;
                }
            }else{
                root->type = IFSTATEMENT;
                returnToken(w);
            }
            return root;
        }
        
        case WHILE: {  // "while" root: lc: condition node p1; rc: body statements p2
            isInCycle = 1;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if (w != LBK) {
                printf("错误：第%d行\n", row);
        		printf("while条件语句出现错误。\n");
        		system("pause");
        		exit(0);
                fatal = 1;
                return NULL;
            }
            
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            AST *p1 = (AST*)malloc(sizeof(AST));  // p1: lc: condition expression node; rc: null
            p1->data.data = NULL;
            p1->type = WHILEPART;
            p1->rc = NULL;
            p1->lc = Expression(RBK);
            if(!p1->lc) {
                printf("错误：第%d行\n", row);
        		printf("while条件语句出现错误。\n");
        		system("pause");
        		exit(0);
                fatal = 1;
                return NULL;
            }
            
            AST* p2 = (AST*)malloc(sizeof(AST));  // p2: lc: null; rc: statements
            p2->data.data = NULL;
            p2->type = WHILEBODY;
            p2->lc = p2->rc = NULL;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if (w == LBS) {
                /* statements are included in braces and end with '}' */
                w = getToken();
				while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                p2->rc = StateList();
            }else if(w >= INTCONST && w <= ULLCONST || w >= IF && w <= OKEY || w == IDENT){
                /* no  braces, only one statement */
                p2->rc = Statement();
                p2->rc->rc = NULL;
            }else{
                printf("错误：第%d行\n", row);
        		printf("while语句体出现错误。\n");
        		system("pause");
        		exit(0);
                fatal = 1;
                return NULL;
            }
            root->type = WHILESTATEMENT;
            root->lc = p1;
            root->rc = p2;
            isInCycle = 0;
            return root;
        }
        
        case FOR: {  // "for" root: lc: condition node p1; rc: body statements p2
            isInCycle = 1;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if (w != LBK) {
                printf("错误：第%d行\n", row);
        		printf("for语句出现错误。\n");
        		system("pause");
        		exit(0);
                fatal = 1;
                return NULL;
            }
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            AST *p1 = (AST*)malloc(sizeof(AST));  // p1: lc: condition 1 q1; rc: null
            p1->data.data = NULL;
            p1->type = FORPART;
            AST* q = (AST*)malloc(sizeof(AST));  // q1: lc: condition expression; rc: condition 2 q2
            p1->lc = q;
            q->type = FORPART1;
            q->data.data = NULL;
            q->lc = Expression(SEMI);
            if(!q->lc) q->data.data="无";
            
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            q->rc = (AST*)malloc(sizeof(AST));  // q2: lc: condition expression; rc: condition 3 q3
            q = q->rc;
            q->type = FORPART2;
            q->data.data = NULL;
            q->lc = Expression(SEMI);
            if(!q->lc) q->data.data="无";
            
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            q->rc = (AST*)malloc(sizeof(AST));  // q3: lc: condition expression; rc: null
            q = q->rc;
            q->rc = NULL;
            q->type = FORPART3;
            q->data.data = NULL;
            q->lc = Expression(RBK);
            if(!q->lc) q->data.data="无";
            /* end of the conditions of "for" */
            
            AST* p2=(AST*)malloc(sizeof(AST));  // p2: lc: null; rc: body statements
            p2->lc = p2->rc = NULL;
            p2->type = FORBODY;
            p2->data.data = NULL;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if (w == LBS) {
                /* statements are included in braces and end with '}' */
                w = getToken();
				while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                p2->rc = StateList();
            }else if(w >= INTCONST && w <= ULLCONST || w >= IF && w <= OKEY || w == IDENT){
                /* no  braces, only one statement */
                p2->rc = Statement();
                p2->rc->rc = NULL;
            }else{
                printf("错误：第%d行\n", row);
        		printf("for语句体出现错误。\n");
        		system("pause");
        		exit(0);
            }
            root->type = FORSTATEMENT;
            root->lc = p1;
            root->rc = p2;
            isInCycle = 0;
            return root;
        }
        
        case DO: {
            isInCycle = 1;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != LBS){
                printf("错误：第%d行\n", row);
        		printf("do后缺少大括号。\n");
        		system("pause");
        		exit(0);
            }
            AST* p1 = (AST*)malloc(sizeof(AST));
            p1->type = DOWHILEBODY;
            p1->lc = p1->rc = NULL; 
            AST* p2=(AST*)malloc(sizeof(AST));
            p2->type = DOWHILECONDITION;
            p2->lc = p2->rc = NULL;
            root->lc = p1;
            root->rc = p2;
            root->data.data = NULL;
            p1->data.data = NULL;
            p2->data.data = NULL;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            p1->lc = StateList();


            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != WHILE){
                printf("错误：第%d行\n", row);
        		printf("do-while结构缺少\"while\"。\n");
        		system("pause");
        		exit(0);
            }
            root->type = DOWHILESTATEMENT;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != LBK){
                printf("错误：第%d行\n", row);
        		printf("do-while的while条件出现错误。\n");
        		system("pause");
        		exit(0);
            }
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            p2->lc = Expression(RBK);
            if(!p2->lc){
                printf("错误：第%d行\n", row);
        		printf("do-while的while条件出现错误。\n");
        		system("pause");
        		exit(0);
            }
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != SEMI){
                printf("错误：第%d行\n", row);
        		printf("do-while缺少分号。\n");
        		system("pause");
        		exit(0);
            }
            isInCycle=0;
            return root;
        }
        
        case RETURN: {  // "return" root: lc: null; rc: expression returned
            hasReturn = 1;
            if(isVoid){
                printf("错误：第%d行\n", row);
        		printf("函数不应有返回值。\n");
        		system("pause");
        		exit(0);
            }
            root->type = RETURNSTATEMENT;
            root->lc = root->rc = NULL;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            root->rc = Expression(SEMI);
            return root;
        }
        
        case BREAK: {  // "break" root: lc: & rc: null
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != SEMI){
                printf("错误：第%d行\n", row);
        		printf("语句结尾缺少分号。\n");
        		system("pause");
        		exit(0);
            }
            if(!isInCycle){
                printf("错误：第%d行\n", row);
        		printf("非循环体中不应出现break。\n");
        		system("pause");
        		exit(0);
            }
            root->type = BREAKSTATEMENT;
            return root;
        }
        
        case CONTINUE: {  // "continue" root: lc: & rc: null
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
            if(w != SEMI){
                printf("错误：第%d行\n", row);
        		printf("语句结尾缺少分号。\n");
        		system("pause");
        		exit(0);
            }
            if(!isInCycle){
                printf("错误：第%d行\n", row);
        		printf("非循环体中不应出现continue。\n");
        		system("pause");
        		exit(0);
            }
            root->type = CONTINUESTATEMENT;
            return root;
        }
        /* other statements: no root, return expressions */
        case INTCONST:
        case FLOATCONST:
        case CHARCONST:
        case STRINGCONST:
        case LONGCONST:
        case UNSIGNEDCONST:
        case ULCONST:
        case ULLCONST:
        case IDENT:
        case ARRAY:
            return Expression(SEMI);

    }
    return root;
}  // returning without reading next token

AST* Expression(int endsym)  // the expression ends with endsym, including ')' and ';'
{
	if(fatal) return NULL;
	if(w == endsym) return NULL;
	int err = 0, m;
	AST *op[MAXSTACK], *num[MAXSTACK];  // stack for operators and operands
	int topo = 0, topn = 0;
	AST *p = (AST*)malloc(sizeof(AST));  // p: stack member, operator or operand
	p->data.data = NULL;
	p->type = OPERATOR;
	p->data.type = BOTTOM;  // stack bottom
	op[++topo] = p;
	AST *t, *t1, *t2, *root;
	
	while((w != endsym || op[topo]->data.type != BOTTOM) && !err){
		m = 0;
		if(op[topo]->data.type == RBK){  // remove brackets
            if(topo < 3){  // at least '(' and another operator in stack
                err++;
                break;
            }
            topo -= 2;
        }
        if(w == IDENT){
        	strcpy(text1, tokenText);
        	w = getToken();
        	if(w == LBK) m = 1;
        	else{
        		w = IDENT;
        		returnToken(w);
        		strcpy(tokenText, text1);
        		m = 2;
			}
		}
        if(m == 2)
			isDefined();
        if(w == IDENT && m == 2 || w >= INTCONST && w <= STRINGCONST ){
            p = (AST*)malloc(sizeof(AST));  // p: operand; lc & rc: null
            p->data.data = NULL;
            p->type = OPERAND;
            char *text2 = (char*)malloc(25*sizeof(char));
            strcpy(text2, tokenText);
            p->data.data = text2;
            num[++topn] = p;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
        }else if(m == 1){
        	p = FuncCall(text1);
            num[++topn] = p;
            w = getToken();
			while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
		}else if(w == endsym){
        	AST *p = (AST*)malloc(sizeof(AST));  //p: operator; lc & rc: null
        	p->data.data = NULL;
        	p->type = OPERATOR;
        	p->data.type = BOTTOM;
        	while(op[topo]->data.type != BOTTOM){
                t2 = num[topn];
                if(!t2){
                    err++;
                    break;
                }else topn--;
                if(topn == 0) t1 = NULL;
                else t1 = num[topn];   
                if(!t1){
                    err++;
                    break;
                }else topn--;
                t = op[topo];  // t: operator on top; lc: operand t1; rc: operand t2
                if(!t){
                    err++;
                    break;
                }
                topo--;
                t->lc = t1;
                t->rc = t2;
                num[++topn] = t;
            }
            if(topn!=1) err++;  // only one operand left in the stack
		}else if(w >= EQ && w <= BOTTOM){
            char *text2 = (char*)malloc(25*sizeof(char));
            switch(valuePrior(op[topo]->data.type, w)){  // value priority
                case '<':  // w > top, push w
                    p = (AST*) malloc(sizeof(AST));
                    p->data.data = NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(text2, tokenText);
                    p->data.data = text2;
                    op[++topo] = p;
                    w = getToken();
					while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                    break;
                case '=':  // only possible when '(' meets ')'
                    t = op[topo];
                    if(!t){
                        err++;
                        topo--;
                    }
                    w = getToken();
					while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                    break;
                case '>':  // w < top; calculate top, then push w
                    t2 = num[topn];
                    if(!t2){
                        err++;
                        break;
                    }
                    else topn--;
                    if (topn == 0) t1 = NULL;
                    else t1 = num[topn];
                    if(!t1){
                        err++;
                        break;
                    }
                    else topn--;
                    t = op[topo];
                    if(!t){
                        err++;
                        break;
                    }
                    topo--;
                    t->lc = t1;
                    t->rc = t2;
                    num[++topn] = t;

                    p = (AST*)malloc(sizeof(AST));
                    p->data.data = NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(text2, tokenText);
                    p->data.data = text2;
                    op[++topo] = p;

                    w = getToken();
					while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
                    break;
                case '\0':
                    printf("警告：第%d行\n", row);
                    printf("出现未知运算符。\n");
                    system("pause");
                    exit(0);
            }
        }else err = 1;
	}
	if((topn == 1 && op[topo]->data.type == BOTTOM) && !err){
        t = num[topn--];
        root = (AST*)malloc(sizeof(AST));  // root: lc: expression result t; rc: null
        root->data.data = NULL;
        root->lc = root->rc = NULL;
        root->type = EXPRESSION;
        root->lc = t;
        return root;
    }else{
        printf("错误：第%d行\n", row);
        printf("表达式出现错误。\n");
        system("pause");
    	exit(0);
    }
}

AST* FuncCall(char *name)
{
	if(fatal) return NULL;
	
	FUN *f = FindFunc(name);
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: return type p1; rc: actual parameter list p2
	root->lc = root->rc = NULL;
	root->type = FUNCTIONCALL;
	char *text2 = (char*)malloc(35*sizeof(char));
	strcpy(text2, name);
	root->data.data = text2;
	arg = 0; 
	
	AST *p1 = (AST*)malloc(sizeof(AST)); 
	p1->type = FUNCRETURNTYPE;
	int type = p1->data.type = f->returnType;
	if(type == INT) p1->data.data = "int";
    else if(type == DOUBLE) p1->data.data = "double";
    else if(type == CHAR) p1->data.data = "char";
    else if(type == FLOAT) p1->data.data = "float";
    else if(type == LONG) p1->data.data = "long";
    else if(type == SHORT) p1->data.data = "short";
    else if(type == VOID) p1->data.data = "void";
    
	p1->lc = p1->rc = NULL;
	
	AST* p2 = ActualParaList(f);
	
	root->lc = p1;
	root->rc = p2;
	return root;
}

AST* ActualParaList(FUN *f)
{
	/* now tokenText includes left bracket '(' */
	w = getToken();
	while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	if(w == RBK){
		if(arg < f->paranum){
			printf("错误：第%d行\n", row);
        	printf("实参数量过少。\n");
        	system("pause");
        	exit(0);
		}
		else return NULL;
	}
	if(w == COMMA){
		w = getToken();
		while(w == INCLUDE || w == DEFINE || w == ANNO) w = getToken();
	}
	AST *root = (AST*)malloc(sizeof(AST));  // root: lc: formal parameter; rc: formal parameter list root
	root->data.data = NULL;
	root->type = ACTUALPARALIST;
    
    if(w != IDENT && w < INTCONST && w > ULLCONST){
        printf("错误：第%d行\n", row);
        printf("参数传入出现错误。\n");
        system("pause");
        exit(0);
	}
	
    AST *p = (AST*)malloc(sizeof(AST));  // p2: lc & rc: null
    p->data.data = NULL;
    p->type = ACTUALPARA;

	if(w == IDENT)
		isDefined();
	if(arg >= f->paranum){
		printf("错误：第%d行\n", row);
        printf("实参数量过多。\n");
        system("pause");
        exit(0);
	}
    char *text2 = (char*)malloc(25*sizeof(char));
    strcpy(text2, tokenText);
    p->data.data = text2;
    p->lc = p->rc = NULL;
    root->lc = p;
    arg++;
	root->rc = ActualParaList(f);
	return root;
}


char valuePrior(int a, int b)
{
	if(fatal == 1) return '0';
    if(a == PLUS || a == MINUS){
        switch(b){
            case PLUS: case MINUS: case RBK: case BOTTOM:
            case LA: case RA: case LAE: case RAE: case EQ:
            case NEQ: case ASSIGN: case AND: case OR: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '>';
            case MULTI: case DIVIDE: case MOD: case LBK:
                return '<';
            default:
                return '\0'; 
        }
    }else if(a == MULTI || a == DIVIDE || a == MOD){
        switch(b){
            case PLUS: case MINUS: case RBK: case BOTTOM: case MODEQ:
			case MULTI: case DIVIDE: case RA: case LA: case MOD: 
			case RAE: case LAE: case EQ: case NEQ: case AND: case OR:				
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '>';
            case LBK: 
                return '<';
            default:
                return '\0';
        }
    }else if(a == LBK){
        switch(b){
            case PLUS: case MINUS: case MULTI: case MOD: 
			case DIVIDE: case LBK: case AND: case OR:
                return '<';
            case RBK:
                return '=';
            case RA: case LA: case RAE: case LAE: 
			case EQ: case NEQ: case BOTTOM: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '>';
            default:
                return '\0';
        }
    } else if(a == RBK){
        switch(b){
            case PLUS: case MINUS: case MULTI: case DIVIDE: case ASSIGN: 
            case LBK: case RA: case LA: case RAE: case LAE: case MOD: 
            case EQ: case NEQ: case BOTTOM: case AND: case OR: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '>';
            default:
                return '\0';
        }
    }else if(a == ASSIGN){
        switch(b){
            case PLUS: case MINUS: case MULTI: case ASSIGN: 
            case LBK: case RA: case LA: case RAE: case MOD: 
            case LAE: case EQ: case NEQ: case DIVIDE: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '<';
            case BOTTOM: case AND: case OR:
                return '>';
            default:
                return '\0';
        }
    }else if(a >= LA && a <= RAE){
        switch(b){
            case PLUS: case MINUS: case MULTI:
            case DIVIDE: case LBK: case MOD: 
                return '<';
            case RBK: case RA: case LA: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
            case RAE: case LAE: case EQ: case ASSIGN: 
            case NEQ: case BOTTOM: case AND: case OR:
                return '>';
            default:
                return '\0';
        }
    }else if(a == EQ || a == NEQ || a == AND){
        switch(b){
            case PLUS: case MINUS: case MULTI: case DIVIDE: case MOD: 
            case LBK: case RA: case LA: case RAE: case LAE: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
                return '<';
            case RBK: case EQ: case NEQ: case BOTTOM:
            case AND: case OR: case ASSIGN: 
                return '>';
            default:
                return '\0';
        }
    }else if(a == OR){
        switch(b){
            case PLUS: case MINUS: case MULTI: case DIVIDE: case MOD: 
            case LBK: case RA: case LA: case RAE: case LAE: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
            case RBK: case EQ: case NEQ: case ASSIGN: case AND: 
                return '<';
            case BOTTOM: case OR:
                return '>';
            default:
                return '\0';
        }
    }else if(a == BOTTOM){
        switch(b){
            case PLUS: case MINUS: case MULTI: case DIVIDE: case MOD: 
            case LBK: case RA: case LA: case RAE: case LAE: case MODEQ:
            case PLUSEQ: case MINUSEQ: case MULTIEQ: case DIVIDEEQ:
            case RBK: case EQ: case NEQ: case ASSIGN: case AND: case OR:
                return '<';
            case BOTTOM:
                return '=';
            default:
                return '\0';
        }
    }
}

void returnToken(int w)
{
    int digit = strlen(tokenText);
    int i;
    for(i = 0; i < digit; i++)
        ungetc(tokenText[digit-1-i], fp);
}

void display(int type){
    switch(type){
        case EXTDEFLIST:
            printf("外部定义序列\n");
            break;
        case EXTVARLIST:
            printf("外部变量名序列\n");
            break;
        case EXTVARTYPE:
            printf("外部变量类型\n");
            break;
        case EXTVAR:
            printf("外部变量名\n");
            break;
        case EXTVARDEF:
            printf("外部变量定义\n");
            break;
        case EXTVARINIT:
            printf("外部变量初始化\n");
            break;
        case ARRAYDEF:
            printf("数组定义\n");
            break;
        case ARRAY:
            printf("数组名\n");
            break;
        case ARRAYTYPE:
            printf("数组类型\n");
            break;
        case ARRAYSIZE:
            printf("数组大小\n");
            break;
        case FUNCDEF:
            printf("函数定义\n");
            break;
        case FUNCTION:
            printf("函数名\n");
            break;
        case FUNCRETURNTYPE:
            printf("函数返回值类型\n");
            break;
        case FUNCCLAIM:
            printf("函数声明\n");
            break;
        case FUNCBODY:
            printf("函数体\n");
            break;
        case FUNCFORMPARALIST:
            printf("函数形参序列\n");
            break;
        case FUNCFORMPARADEF:
            printf("函数形参定义\n");
            break;
        case FUNCFORMPARA:
            printf("函数形参名\n");
            break;
        case FUNCFORMPARATYPE:
            printf("函数形参类型\n");
            break;
        case LOCVARDEFLIST:
            printf("局部变量定义序列\n");
            break;
        case LOCVARDEF:
            printf("局部变量定义\n");
            break;
        case LOCVAR: 
        	printf("局部变量名\n");
            break;
        case LOCVARNAMELIST: 
        	printf("局部变量名序列\n");
            break;
        case LOCVARTYPE:
            printf("局部变量类型\n");
            break;
        case LOCVARINIT:
            printf("局部变量初始化\n");
            break;
        case STATELIST:
            printf("复合语句序列\n");
            break;
        case STATEMENT:
            printf("复合语句\n");
            break;
        case IFPART:
            printf("if条件语句\n");
            break;
        case IFELSESTATEMENT:
            printf("if-else语句\n");
            break;
        case ELSEPART:
            printf("else语句体\n");
            break;
        case IFSTATEMENT:
            printf("if语句\n");
            break;
        case WHILEPART:
            printf("while条件语句\n");
            break;
        case WHILEBODY:
            printf("while语句体\n");
            break;
        case WHILESTATEMENT:
            printf("while语句\n");
            break;
        case FORPART:
            printf("for条件语句\n");
            break;
        case FORPART1:
            printf("for语句条件一\n");
            break;
        case FORPART2:
            printf("for语句条件二\n");
            break;
        case FORPART3:
            printf("for语句条件三\n");
            break;
        case FORBODY:
            printf("for语句体\n");
            break;
        case FORSTATEMENT:
            printf("for语句\n");
            break;
        case DOWHILEBODY:
            printf("do-while语句体\n");
            break;
        case DOWHILECONDITION:
            printf("do-while语句条件\n");
            break;
        case DOWHILESTATEMENT:
            printf("do-while语句\n");
            break;
        case RETURNSTATEMENT:
            printf("return语句\n");
            break;
        case BREAKSTATEMENT:
            printf("break语句\n");
            break;
        case CONTINUESTATEMENT:
            printf("continue语句\n");
            break;
        case OPERATOR:
            printf("运算符\n");
            break;
        case OPERAND:
            printf("操作数\n");
            break;
        case EXPRESSION:
            printf("表达式\n");
            break;
        case FUNCTIONCALL:
            printf("函数调用\n");
            break;
        case ACTUALPARALIST:
            printf("函数实参序列\n");
            break;
        case ACTUALPARA:
            printf("函数实参名\n");
            break;
    }
}

void preOrderTraverse(AST* T, int depth)
{
    if(!T) printf("");
    else{
        // indent measurement
        if(!(T->type==1 || T->type==2 || T->type==16 || T->type==20 || T->type==23 || T->type==26 || T->type==37 || T->type==38 || T->type==51)){
        	depth++;
        	for(int i = 0; i < depth; i++) printf("\t");
        	display(T->type);
        	if(T->data.data){
            	for(int i = 0; i < depth; i++) printf("\t"); 
            	printf("%s\n", T->data.data);
        	}
		}else if(T->type==37 || T->type==38){
			for(int i = 0; i < depth; i++) printf("\t");
        	display(T->type);
        	if(T->data.data){
            	for(int i = 0; i < depth; i++) printf("\t"); 
            	printf("%s\n", T->data.data);
        	}
		}
        preOrderTraverse(T->lc, depth);
        preOrderTraverse(T->rc, depth);
    }
}

void addName(void)
{
	if(fatal) return;
	int flag = 0;
	VAR *p = vroot;
	while(p->next) p = p->next;
	for(int i = 0; i < p->size; i++){
		if(!strcmp(tokenText, p->var[i])){
			flag = 1;
			break;
		}
	}
	if(flag){
		printf("错误：第%d行\n", row);
        printf("变量重复定义。\n");
        system("pause");
        exit(0);
	}
	char *save = (char*)malloc(25*sizeof(char));
	strcpy(save, tokenText);
	p->var[p->size++] = save;
}

void isDefined(void)
{
	if(fatal) return;
	int flag = 0;
	VAR *p = vroot;
	while(p->next) p = p->next;
	for(int i = 0; i < p->size; i++){
		if(!strcmp(tokenText, p->var[i])){
			flag = 1;
			break;
		}
	}
    for(int i = 0; i < vroot->size; i++){
        if(!strcmp(tokenText, vroot->var[i])){
            flag = 1;
            break;
        }
    }
    if(!flag){
        printf("错误：第%d行\n", row);
        printf("变量未定义。\n");
        system("pause");
        exit(0);
    }
}

int addFunc(void)
{
	if(fatal) return 2;
	int flag = 0;
	FUN *f = ftail;
	
	char *save = (char*)malloc(25*sizeof(char));
	strcpy(save, tokenText);
	FUN *p = (FUN*)malloc(sizeof(FUN));
	p->name = save;
	p->paranum = 0;
	p->next = NULL;
	f->next = ftail = p;
	return flag;
}

FUN* FindFunc(char *name)
{
	if(fatal) return NULL;
	int flag = 0;
	FUN *f;
	for(f = froot; f->next; f = f->next){
		if(!strcmp(name, f->next->name)){
			flag = 1;
			break;
		}
	}
    if(!flag){
        printf("错误：第%d行\n", row);
        printf("函数未定义。\n");
        system("pause");
        exit(0);
    }
    return f->next;
}
