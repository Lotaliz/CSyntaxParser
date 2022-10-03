/*
	C language grammar analyst
	wordParser.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordParser.h"

#define ISHEXLETTER(x) ((x)>='a'&&(x)<='f'||(x)>='A'&&(x)<='F')
#define ISOCTNUM(x) ((x)>='0'&&(x)<='7')
#define KEYS 16

extern FILE *fp;
extern char tokenText[255];
extern char num[255];
extern int row;

char keywords[32][10] = {
	"auto", "case", "const", "default", "enum", "extern", 
	"goto", "register", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "volatile"
};

int isLetter(char c) 
{
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
}

int isNum(char c)
{
	return (c >= '0' && c <= '9');
}

int isKey()
{
	for(int i = 0; i < KEYS; i++){
		if(!strcmp(tokenText, keywords[i])) return 1;
	}
	return 0;
}

int getToken(void)
{
	memset(tokenText, 0, sizeof(tokenText));
	int textp = 0, nump = 0;
	char c;
	do{
        c = fgetc(fp);
        if(c == '\n') row++;
    }while(c == ' ' || c == '\n' || c == '\t');
	if(isLetter(c)){
    	do{
			tokenText[textp++] = c;	
			c = fgetc(fp);
		}while(isLetter(c) || isNum(c));   // recognize identifiers, arrays, tokens
        ungetc(c, fp);
        if(isKey()) return OKEY;
        else if(!strcmp("int", tokenText)) return INT; 
		else if(!strcmp("double", tokenText)) return DOUBLE; 
		else if(!strcmp("float", tokenText)) return FLOAT;
		else if(!strcmp("char", tokenText)) return CHAR;
		else if(!strcmp("long", tokenText)) return LONG;
		else if(!strcmp("unsigned", tokenText)) return UNSIGNED;
		else if(!strcmp("void", tokenText)) return VOID;
		else if(!strcmp("if", tokenText)) return IF;
		else if(!strcmp("else", tokenText)) return ELSE;
		else if(!strcmp("return", tokenText)) return RETURN;
		else if(!strcmp("while", tokenText)) return WHILE;
		else if(!strcmp("for", tokenText)) return FOR;
		else if(!strcmp("do", tokenText)) return DO;
		else if(!strcmp("break", tokenText)) return BREAK;
		else if(!strcmp("continue", tokenText)) return CONTINUE;
		else{
			c = fgetc(fp);
			if(c == '['){ // recognize arrays
				tokenText[textp++] = c;
				c = fgetc(fp);
				while(isNum(c)){
					tokenText[textp++] = c;
					num[nump++] = c;
					c = fgetc(fp);
				}
				if(c != ']') return ERROR_TOKEN;
				tokenText[textp++] = c;
				return ARRAY;
			}else{
				ungetc(c, fp);
				return IDENT;
			}
		}
    }else if(isNum(c)){ // recognize integar and float constants
    	int base = 10;
    	if(c == '0'){
    		tokenText[textp++] = c;	
    		c = fgetc(fp);
    		if(c == 'x' || c == 'X'){
    			base = 16;
    			tokenText[textp++] = c;	
    			c = fgetc(fp);
    			if(isNum(c) || ISHEXLETTER(c)){
    				do{
						tokenText[textp++] = c;	
						c = fgetc(fp);
					}while(isNum(c) || ISHEXLETTER(c));
				}
			}else if(ISOCTNUM(c)){
				base = 8;
				tokenText[textp] = c;
				textp++;	
    			c = fgetc(fp);
    			if(ISOCTNUM(c)){
    				do{
						tokenText[textp] = c;
						textp++;	
						c = fgetc(fp);
					}while(ISOCTNUM(c));
				}
				if(c == '8' || c == '9') return ERROR_TOKEN;
			}else if(c == '.'){
                tokenText[textp] = c;
				textp++;	
                c = fgetc(fp);
                while(isNum(c)){
                    tokenText[textp++] = c;
                    c = fgetc(fp);
                }
                if(c == 'f' || c == 'F'){
                    tokenText[textp++] = c;
                    return FLOATCONST;
                }else if(c == 'l' || c == 'L'){
                	tokenText[textp++] = c;
                    return LDCONST;
				}else if(c == 'e' || c == 'E'){
					c = fgetc(fp);
					if(isNum(c))
						do{
							tokenText[textp++] = c;	
							c = fgetc(fp);
						}while(isNum(c));
					else return ERROR_TOKEN;
					ungetc(c, fp);
					return FLOATCONST; 
				}else ungetc(c,fp);
                return FLOATCONST;
			}else if(c == '8' || c == '9') return ERROR_TOKEN;
			else{
				ungetc(c, fp);
				return INTCONST;
			}
		}else{
        	do{
				tokenText[textp++] = c;	
				c = fgetc(fp);
			}while(isNum(c)); 
		}
		if(c != '.' && c != 'u' && c != 'l' && c != 'e' && c != 'U' && c != 'L' && c != 'E'){
            if(c!=' ' && c!=';' && c!=')' && c!=',' && c!='+' && c!='-' && c!='*' 
			&& c!='/' && c!='%' && c!='&' && c!='>' && c!='<' && c!='=' && c!='|')
                return ERROR_TOKEN;
        	ungetc(c, fp);  
        	return INTCONST;
		}else if(c == '.'){
            tokenText[textp] = c;
			textp++;	
            c = fgetc(fp);
            while(isNum(c)){
                tokenText[textp++] = c;
                c = fgetc(fp);
            }
        	if(c == 'f'){
                tokenText[textp++] = c;
                return FLOATCONST;
            }else if(c == 'l'){
            	tokenText[textp++] = c;
                return LDCONST;
			}else if(c == 'e'){
				tokenText[textp++] = c;
				c = fgetc(fp);
				if(isNum(c))
					do{
						tokenText[textp++] = c;	
						c = fgetc(fp);
					}while(isNum(c));
				else return ERROR_TOKEN;
				ungetc(c, fp);
				return FLOATCONST; 
			}else ungetc(c, fp);
            return FLOATCONST;
		}else if(c == 'u' || c == 'U'){
            tokenText[textp++] = c;	
            c = fgetc(fp);
            if(c == 'l' || c == 'L'){
                tokenText[textp++] = c;	
                c = fgetc(fp);
                if(c == 'l' || c == 'L'){
                    tokenText[textp++] = c;	
                    return ULLCONST;
                }else{
                    ungetc(c, fp);
                    return ULCONST;
                }
            }else{
                ungetc(c,fp);
                return UNSIGNEDCONST;
            }
        }else if(c == 'l' || c == 'L'){
            tokenText[textp++] = c;	
            return LONGCONST;
        }else if(c == 'e' || c == 'E'){
			c = fgetc(fp);
			if(isNum(c))
				do{
					tokenText[textp++] = c;	
					c = fgetc(fp);
				}while(isNum(c));
			else return ERROR_TOKEN;
			return FLOATCONST; 
        }else return ERROR_TOKEN;
	}else if(c=='.'){ // recognize float constants starting with a dot
        do{
			tokenText[textp++] = c;	
			c = fgetc(fp);
		}while(isNum(c)); 
        ungetc(c,fp);
        return FLOATCONST;
    }else if(c == '\''){ // recognize character constants
        tokenText[textp++] = c;	
        if((c=fgetc(fp)) != '\\'){
            tokenText[textp++] = c;	
            if((c=fgetc(fp)) == '\''){
                tokenText[textp++] = c;	
                return CHARCONST;
            }else return ERROR_TOKEN;
        }else{
            tokenText[textp++] = c;	
            c = fgetc(fp);
            if(c=='n' || c=='t' || c=='\\' || c=='\'' || c=='\"' || c=='a' || c=='b' || c=='f' || c=='r' || c=='v' || c=='?'){
                tokenText[textp++] = c;	
                if((c=fgetc(fp)) == '\''){ // common escapes
                    tokenText[textp++] = c;	
                    return CHARCONST;
                }else return ERROR_TOKEN;
            }else if(c == 'x'){ // HEX escapes
                tokenText[textp++] = c;	
                c = fgetc(fp);
                if(ISHEXLETTER(c) || isNum(c)){
                    tokenText[textp++] = c;	
                    c = fgetc(fp);
                    if(ISHEXLETTER(c) || isNum(c))
                        tokenText[textp++] = c;
                    else ungetc(c,fp);
                    c = fgetc(fp);
                    if(c == '\''){
                        tokenText[textp++] = c;
                        return CHARCONST;
                    }else return ERROR_TOKEN;
                }else return ERROR_TOKEN;
            }else if(ISOCTNUM(c)){ // OCT escapes
                tokenText[textp++] = c;
                c = fgetc(fp);
                if(ISOCTNUM(c)){
                    tokenText[textp++] = c;
                    c = fgetc(fp);
                    if(ISOCTNUM(c)){
                        tokenText[textp++] = c;
                    	c = fgetc(fp);
                        if(c == '\''){
                            tokenText[textp++] = c;
                            return CHARCONST;
                        }else return ERROR_TOKEN;
                    }else if(c == '\''){
                        tokenText[textp++] = c;
                        return CHARCONST;
                    }else return ERROR_TOKEN;
                }else{
                    if(c=='\''){
                        tokenText[textp++] = c;
                        return CHARCONST;
                    }else{
                        ungetc(c, fp);
                        return ERROR_TOKEN;
                    }
                }
            }else return ERROR_TOKEN;
        }
    }else if(c == '"'){ // recognize string constants
        do{
            if(c != '\\')
                tokenText[textp++] = c;
            if(c == '\\'){
                c = fgetc(fp);
                tokenText[textp++] = c;
            }
        }while((c=fgetc(fp)) != '"' &&c != '\n');
        if(c == '"'){
            tokenText[textp++] = c;
            return STRINGCONST;
        }else return ERROR_TOKEN;
    }else if(c == '/'){ // recognize derives and annotations
        tokenText[textp] = c;
        textp++;
        if((c=fgetc(fp)) == '/'){
            do{
                tokenText[textp] = c;
                textp++;
                c = fgetc(fp);
            }while (c != '\n');
            row++;
            return ANNO;
        }else if(c == '*'){
            while(1){
                tokenText[textp] = c;
                textp++;
                c = fgetc(fp);
                if(c == '*'){
                    tokenText[textp] = c;
                	textp++;
                    c = fgetc(fp);
                    if(c == '/'){
                        tokenText[textp] = c;
                		textp++;
                        return ANNO;
                    }else if(c == '*') ungetc(c, fp);
                }
                if(c == '\n'){
                    row++;
                }
            }
        }else if(c == '='){
            tokenText[textp] = c;
            textp++;
            return DIVIDEEQ;
		}else{
            ungetc(c, fp);
            return DIVIDE;
        }
    }else if(c == '#'){ // recognize headers and macro definitions
        tokenText[textp] = c;
        textp++;
        c = fgetc(fp);
        if(isLetter(c)){
            do{
                tokenText[textp] = c;
                textp++;
                c = fgetc(fp);
            }while(isLetter(c));
            if(!strcmp(tokenText,"#include")){
                do{
                    tokenText[textp] = c;
                    textp++;
                    c = fgetc(fp);
                }while (c != '\n');
                ungetc(c, fp);
                return INCLUDE;
            } else if(!strcmp(tokenText,"#define")){
                do{
                    tokenText[textp] = c;
                    textp++;
                    c = fgetc(fp);
                }while (c != '\n');
                ungetc(c, fp);
                return DEFINE;
            }else return ERROR_TOKEN;
        }else return ERROR_TOKEN;
    }
 	switch(c){
    	case  '=':  
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                tokenText[textp] = c;
                textp++;
                return EQ;
            }
            ungetc(c,fp);
            tokenText[textp] = '=';
            textp++;
            return ASSIGN;
        case '!':
        	c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = '!';
                textp++;
                tokenText[textp] = '=';
                textp++;
                return NEQ;
            } else return ERROR_TOKEN;
		case '+':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return PLUSEQ;
            } else{
                ungetc(c, fp);
                return PLUS;
            }
		case '-':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return MINUSEQ;
            } else{
                ungetc(c, fp);
                return MINUS;
            }
		case '*':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return MULTIEQ;
            } else{
                ungetc(c, fp);
                return MULTI;
            }
		case '%':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return MODEQ;
            } else{
                ungetc(c, fp);
                return MOD;
            }
		case '&':
			c = fgetc(fp);
            if(c == '&'){
            	tokenText[textp] = c;
            	textp++;
                tokenText[textp] = c;
                textp++;
                return AND;
			}
            ungetc(c, fp);
			return ERROR_TOKEN;
		case '|':
			c = fgetc(fp);
            if(c == '|'){
            	tokenText[textp] = c;
            	textp++;
                tokenText[textp] = c;
                textp++;
                return OR;
			}
            ungetc(c, fp);
			return ERROR_TOKEN;
		case '{':
			tokenText[textp] = c;
			textp++;
			return LBS;
		case '}':
			tokenText[textp] = c;
			textp++;
			return RBS;
		case '(':
			tokenText[textp] = c;
			textp++;
			return LBK;
		case ')':
			tokenText[textp] = c;
			textp++;
			return RBK;
		case '<':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return LAE;
            } else{
                ungetc(c, fp);
                return LA;
            }
		case '>':
			tokenText[textp] = c;
			textp++;
			c = fgetc(fp);
            if(c == '='){
                tokenText[textp] = c;
                textp++;
                return RAE;
            } else{
                ungetc(c, fp);
                return RA;
            }
		case ';':
			tokenText[textp] = c;
			textp++;
			return SEMI;
		case ',':
			tokenText[textp] = c;
			textp++;
			return COMMA;
    	default:  
			if(feof(fp)) return EOF;
			else return ERROR_TOKEN; 
    } 
}

void parseWord(void)
{
	int w;
	row = 1;
    if(!fp){
    	printf("文件打开失败\n");
    	return;
	}
    printf("文件打开成功\n");
    printf("\n");
    printf("        单词类别");
    printf("\t单词值\n");
    do{
        w = getToken();
        if(w != ERROR_TOKEN) {
            switch(w){
                case IDENT:
                    printf("          标识符");
                    break;
                case INTCONST:
                    printf("        整型常量");
                    break;
                case FLOATCONST:
                    printf("      浮点型常量");
                    break;
                case CHARCONST:
                    printf("        字符常量");
                    break;
                case STRINGCONST:
                    printf("      字符串常量");
                    break;
                case UNSIGNEDCONST:
                    printf("  无符号整型常量");
                    break;
                case LONGCONST:
                    printf("      长整型常量");
                    break;
                case ULCONST:
                    printf("无符号长整型常量");
                    break;
                case ULLCONST:
                    printf("       ull型常量");
                    break;
                case LDCONST:
                    printf("    长浮点型常量");
                    break;
                case OKEY:
                    printf("          关键字");
                    break;
                case INT:
                    printf("      类型关键字");
                    break;
                case DOUBLE:
                    printf("      类型关键字");
                    break;
                case FLOAT:
                    printf("      类型关键字");
                    break;
                case CHAR:
                    printf("      类型关键字");
                    break;
                case SHORT:
                    printf("      类型关键字");
                    break;
                case LONG:
                    printf("      类型关键字");
                    break;
                case UNSIGNED:
                    printf("      类型关键字");
                    break;
                case IF:
                    printf("          关键字");
                    break;
                case ELSE:
                    printf("          关键字");
                    break;
                case WHILE:
                    printf("          关键字");
                    break;
                case FOR:
                    printf("          关键字");
                    break;
                case DO:
                    printf("          关键字");
                    break;
                case BREAK:
                    printf("          关键字");
                    break;
                case RETURN:
                    printf("          关键字");
                    break;
                case CONTINUE:
                    printf("          关键字");
                    break;
                case VOID:
                    printf("          关键字");
                    break;
                case EQ:
                    printf("          等于号");
                    break;
                case NEQ:
                    printf("        不等于号");
                    break;
                case ASSIGN:
                    printf("          赋值号");
                    break;
                case LBK:
                    printf("          左括号");
                    break;
                case RBK:
                    printf("          右括号");
                    break;
                case LBS:
                    printf("        左大括号");
                    break;
                case RBS:
                    printf("        右大括号");
                    break;
                case LB:
                    printf("        左中括号");
                    break;
                case RB:
                    printf("        右中括号");
                    break;
                case AND:
                    printf("          逻辑与");
                    break;
                case PLUSEQ:
                    printf("          加等于");
                    break;
                case MINUSEQ:
                    printf("          减等于");
                    break;
                case MULTIEQ:
                    printf("          乘等于");
                    break;
                case DIVIDEEQ:
                    printf("          除等于");
                    break;
                case MODEQ:
                    printf("          模等于");
                    break;
                case OR:
                    printf("          逻辑或");
                    break;
                case SEMI:
                    printf("            分号");
                    break;
                case COMMA:
                    printf("            逗号");
                    break;
                case MULTI:
                    printf("            乘号");
                    break;
                case DIVIDE:
                    printf("            除号");
                    break;
                case MOD:
                    printf("           取余数");
                    break;
                case ANNO:
                    printf("            注释");
                    break;
                case PLUS:
                    printf("            加法");
                    break;
                case MINUS:
                    printf("            减法");
                    break;
                case RA:
                    printf("          大于号");
                    break;
                case RAE:
                    printf("      大于等于号");
                    break;
                case LA:
                    printf("          小于号");
                    break;
                case LAE:
                    printf("      小于等于号");
                    break;
                case INCLUDE:
                    printf("      头文件引用");
                    break;
                case DEFINE:
                    printf("          宏定义");
                    break;
                case ARRAY:
                    printf("            数组");
                    break;
            }
            printf("\t%s\n", tokenText);
        } else{
            printf("\t第%d行出现错误\n", row);
            break;
        }
    } while(w != EOF);
}
