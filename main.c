#include<stdio.h>
#include "syntax.tab.h"
#include "treenode.h"
#include "irisim.h"
#include "assem.h"

extern FILE* yyin;
extern int yylineno;
extern int yyparse(void);
extern void yyrestart(FILE*);
extern Node* root;
extern int error_num;

int main(int argc,char **argv){
	
	if(argc<=2){
		printf("Need at least 2 parameters!\n");
		return 1;
	}
	
	FILE *fp1=fopen(argv[1],"r");
	if(fp1==NULL){
		perror(argv[1]);
		return 1;
	}
	
	FILE* fp2=fopen(argv[2],"w");
    if(fp2==NULL){
        perror(argv[2]);
        return 1;
    }

////////
    char tmp[99];
    sprintf(tmp,"%s.ir",argv[2]);
    FILE* fp3=fopen(tmp,"w");
    if(fp3==NULL){
        perror(tmp);
        return 1;
    }
////////

	yylineno=1;
	yyrestart(fp1);
	
	yyparse();

	if(error_num==0){
		traveltree(root);			//检查语义错误
		translateProgram(root);     //生成中间代码
		printf("Test: translateProgram over. \n");
		optimizeIR();				//中间代码优化
		printf("Test: optimizeIR over. \n");
	
		printInterCode(fp3);
		printf("Test: printInterCode over. \n");

		printAssem(fp2);	   	//输出汇编代码
		printf("Test: printAssem over. \n");
	}


	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	return 0;
}


