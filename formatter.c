/*
	C language grammar analyst
	formatter.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordParser.h"
#include "formatter.h"

extern FILE *fp;
extern char tokenText[255];

void format(char *destfile)
{
	FILE *fp1 = fopen(destfile, "w");
	if(!fp1){
		printf("创建格式化文件失败。");
		return;
	}
	int indent = 0;
    int i;
    int flag = 0;  // marking situations where indent is needed but without '{'
    TOKEN *head, *tail, *p;
    while(1){
        head = readline();
        if(!head) break;
        tail = head;
        while(tail->next) tail = tail->next;
        if(head == tail && !strcmp(tail->str, "}")) indent--;
        if(head != tail && !strcmp(head->str, "}")) indent--;
        for(i = 0; i < indent; i++) fprintf(fp1, "\t");
        
        if(flag){
            indent--;
            flag = 0;
        }
        p = head;
        while(p){
        	if(p->next && (!strcmp(p->next->str, ";") || !strcmp(p->next->str, ",")))
        		fprintf(fp1, "%s", p->str);
            else fprintf(fp1, "%s ", p->str);
            p = p->next;
        }
        fprintf(fp1, "\n");
        if(!strcmp(tail->str, "{")) indent++;
        p = head;
        while(p){
            if(!strcmp(p->str, "if") || !strcmp(p->str, "for") || !strcmp(p->str, "while")){
                if(strcmp(tail->str, "{") != 0 && strcmp(tail->str, ";") != 0){
                    indent++;
                    flag = 1;
                }
            }
            p = p->next;
        }
    }
    fclose(fp1);
}

TOKEN* readline(void)
{
	char c;
    int w;
    TOKEN *root = NULL;
    TOKEN *tail = root;
    w = getToken();
    if(w == EOF) return NULL;
    root = (TOKEN*)malloc(sizeof(TOKEN));
    char *text2 = (char*)malloc(255*sizeof(char));
    strcpy(text2, tokenText);
    root->str = text2;
    root->next = NULL;
    tail = root;
    
    while((c=fgetc(fp)) != '\n'){
        ungetc(c, fp);
        getToken();
        tail->next = (TOKEN*)malloc(sizeof(TOKEN));
        tail = tail->next;
        text2 = (char*)malloc(255* sizeof(char));
        strcpy(text2, tokenText);
        tail->str = text2;
        tail->next = NULL;
    }
    return root;
}
