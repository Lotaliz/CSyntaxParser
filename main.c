/*
	C language grammar analyst
	main.c
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordParser.h"
#include "syntaxParser.h"
#include "formatter.h"

char tokenText[255], num[255], filename[255];
FILE *fp;
int row = 1;

int main(int argc, char* argv[])
{
	if(argc != 2){
		printf("输入要处理的文件名：\n");
		scanf("%s", filename);
	}else strcpy(filename, argv[1]);
    int op;
    while(1) {
        system("cls");
        printf(" ——————————————————————\n");
        printf("     输入序号以选择功能    \n");
        printf("     1.词法分析          \n");
        printf("     2.语法分析          \n");
        printf("     3.格式化            \n");
        printf("     0.退出              \n");
        printf(" ——————————————————————\n");
        scanf("%d", &op);
        switch(op){
            case 1:
                fp = fopen(filename, "r");
                parseWord();
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 2:
                fp = fopen(filename, "r");
                analyze();
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 3:
                fp = fopen(filename, "r");
                char dest[255];
                printf("输入格式化文件名称：\n");
                scanf("%s", dest);
                format(dest);
                printf("按任意键继续\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 0:
                system("cls");
                printf("感谢使用\n");
                return 0;
            default:
                printf("输入有误，请重新输入\n");
                printf("按任意键继续\n");
                getchar();
                system("pause");
                break;
        }
    }
}
