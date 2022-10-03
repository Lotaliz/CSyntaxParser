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
		printf("����Ҫ������ļ�����\n");
		scanf("%s", filename);
	}else strcpy(filename, argv[1]);
    int op;
    while(1) {
        system("cls");
        printf(" ��������������������������������������������\n");
        printf("     ���������ѡ����    \n");
        printf("     1.�ʷ�����          \n");
        printf("     2.�﷨����          \n");
        printf("     3.��ʽ��            \n");
        printf("     0.�˳�              \n");
        printf(" ��������������������������������������������\n");
        scanf("%d", &op);
        switch(op){
            case 1:
                fp = fopen(filename, "r");
                parseWord();
                printf("�����������\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 2:
                fp = fopen(filename, "r");
                analyze();
                printf("�����������\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 3:
                fp = fopen(filename, "r");
                char dest[255];
                printf("�����ʽ���ļ����ƣ�\n");
                scanf("%s", dest);
                format(dest);
                printf("�����������\n");
                fclose(fp);
                getchar();
                system("pause");
                break;
            case 0:
                system("cls");
                printf("��лʹ��\n");
                return 0;
            default:
                printf("������������������\n");
                printf("�����������\n");
                getchar();
                system("pause");
                break;
        }
    }
}
