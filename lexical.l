%{
#include "syntax.tab.h"
#include <stdio.h>
#include "treenode.h"
extern int error_num;

int yycolumn=1;
#define YY_USER_ACTION \
	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn; \
	yylloc.last_column=yycolumn+yyleng-1; \
	yycolumn+=yyleng;
%}

%option yylineno

letter_ [a-zA-Z_]
digit [0-9]
int8digit [0-7]
int16digit [0-9a-fA-F]
OPEXP [Ee][+-]?[0-9]+


%%
[ ]+|\t {
//	printf(" ");
}

\"[^\"]*\" {
	if(yytext[yyleng-2]=='\\'){
		yyless(yyleng-1);
		yymore();
	}
	else{
//		printf("STRING ");
	}
}
"//" {
	char c=input();
	while(c!='\n')
		c=input();
}

"/*" {
	char c=input();
	while(1){
		c=input();
		if(c=='*'){
			c=input();
			if(c=='/')
				break;
		}
	}
}

"*/" {
	error_num++;
	printf("Error: expected '/*' before.\n");
}

[1-9][0-9]*|0 {
//	printf("INT ");
	int t=0;
	sscanf(yytext,"%d",&t);
	yylval.node=newnode("INT",yylineno,t,0,"");
	return INT;
}

0[1-7][0-7]*  {
//	printf("INT8 ");
	int t=0;
	sscanf(yytext,"%o",&t);
	yylval.node=newnode("INT",yylineno,t,0,"");
	return INT;
}
0[x|X][1-9a-fA-F][0-9a-fA-F]* {
//	printf("INT16 ");
	int t=0;
	sscanf(yytext,"%x",&t);
	yylval.node=newnode("INT",yylineno,t,0,"");
	return INT;
}
"." {
//	printf("DOT ");
	yylval.node=newnode("DOT",yylineno,0,0,yytext);
	return DOT;
}

{digit}*"."{digit}*{OPEXP}? {
	//printf("FLOAT "); 
	yylval.node=newnode("FLOAT",yylineno,0,atof(yytext),"");
	return FLOAT;
}

";"		{
//	printf("SEMI "); 
	yylval.node=newnode("SEMI",yylineno,0,0,yytext);
	return SEMI;
}
"," {
/*	printf("COMMA "); */
	yylval.node=newnode("COMMA",yylineno,0,0,yytext);
	return COMMA;
}
"=" {
//	printf("ASSIGNOP ");
	yylval.node=newnode("ASSIGNOP",yylineno,0,0,yytext);
	return ASSIGNOP;
}
">"|"<"|">="|"<="|"=="|"!=" {
//	printf("RELOP ");
	yylval.node=newnode("RELOP",yylineno,0,0,yytext);
	return RELOP;
}
"+" {
//	printf("PLUS ");
	yylval.node=newnode("PLUS",yylineno,0,0,yytext);
	return PLUS;
}
"-" {
//	printf("MINUS ");
	yylval.node=newnode("MINUS",yylineno,0,0,yytext);
	return MINUS;
}
"*" {
//	printf("STAR ");
	yylval.node=newnode("STAR",yylineno,0,0,yytext);
	return STAR;
}
"/" {
//	printf("DIV ");
	yylval.node=newnode("DIV",yylineno,0,0,yytext);
	return DIV;
}
"&&" {
//	printf("AND ");
	yylval.node=newnode("AND",yylineno,0,0,yytext);
	return AND;
}
"||" {
//	printf("OR ");
	yylval.node=newnode("OR",yylineno,0,0,yytext);
	return OR;
}
"!" {
//	printf("NOT ");
	yylval.node=newnode("NOT",yylineno,0,0,yytext);
	return NOT;
}
"int"|"float" {
//	printf("TYPE ");
	yylval.node=newnode("TYPE",yylineno,0,0,yytext);
	return TYPE;
}
"(" {
//	printf("LP ");
	yylval.node=newnode("LP",yylineno,0,0,yytext);
	return LP;
}
")" {
//	printf("RP ");
	yylval.node=newnode("RP",yylineno,0,0,yytext);
	return RP;
}
"[" {
//	printf("LB ");
	yylval.node=newnode("LB",yylineno,0,0,yytext);
	return LB;
}
"]" {
//	printf("RB ");
	yylval.node=newnode("RB",yylineno,0,0,yytext);
	return RB;
}
"{" {
//	printf("LC ");
	yylval.node=newnode("LC",yylineno,0,0,yytext);
	return LC;
}
"}" {
//	printf("RC ");
	yylval.node=newnode("RC",yylineno,0,0,yytext);
	return RC;
}
"struct" {
//	printf("STRUCT ");
	yylval.node=newnode("STRUCT",yylineno,0,0,yytext);
	return STRUCT;
}
"return" {
//	printf("RETURN ");
	yylval.node=newnode("RETURN",yylineno,0,0,yytext);
	return RETURN;
}
"if" {
//	printf("IF ");
	yylval.node=newnode("IF",yylineno,0,0,yytext);
	return IF;
}
"else" {
//	printf("ELSE ");
	yylval.node=newnode("ELSE",yylineno,0,0,yytext);
	return ELSE;
}
"while" {
//	printf("WHILE ");
	yylval.node=newnode("WHILE",yylineno,0,0,yytext);
	return WHILE;
}

{letter_}({letter_}|{digit})*	{
//	printf("ID ");
	yylval.node=newnode("ID",yylineno,0,0,yytext);
	return ID;
}
\n {
//	printf("\n");
	yycolumn=1;
}	
. {
	error_num++;
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);
}


%%
//自定义函数
