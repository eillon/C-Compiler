%{
#include<stdio.h>
#include "treenode.h"
#include "lex.yy.c"
int yyerror(char *msg);
int error_num=0;
Node *root;
%}


%union {
	struct Node* node;
};

%token <node> INT FLOAT ID
%token <node> SEMI COMMA 
%token <node> ASSIGNOP RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR 
%token <node> DOT
%token <node> NOT
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF ELSE WHILE

%type <node> Program ExtDefList ExtDef ExtDecList Specifier
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args


%right ASSIGNOP
%left OR 
%left AND 
%left RELOP 
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left LB RB LP RP
%left DOT 
%nonassoc ELSE STRUCT RETURN WHILE

%%
Program : ExtDefList {
	$$=newnode("Program",@$.first_line,0,0,"");
//	printf("Program linenum: %d\n",$$->linenum);
	addchild($$,$1);
	root=$$;}
	;
ExtDefList : ExtDef ExtDefList{
		$$=newnode("ExtDefList",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	| {
		$$=newnode("ExtDefList",@$.first_line,0,0,"");
		}
 	;
ExtDef : Specifier ExtDecList SEMI {
		$$=newnode("ExtDef",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		 }
	| Specifier SEMI{
		$$=newnode("ExtDef",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	| Specifier FunDec CompSt{
		$$=newnode("ExtDef",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| error SEMI{error_num++;}
	;
ExtDecList : VarDec{
		$$=newnode("ExtDecList",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| VarDec COMMA ExtDecList{
		$$=newnode("ExtDecList",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	;

Specifier : TYPE{
		$$=newnode("Specifier",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| StructSpecifier{
		$$=newnode("Specifier",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	;
StructSpecifier : STRUCT OptTag LC DefList RC{
		$$=newnode("StructSpecifier",@$.first_line,0,0,"");
		addchild($$,$5);
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| STRUCT Tag{
		$$=newnode("StructSpecifier",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	;
OptTag : ID{
		$$=newnode("OptTag",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| {	
		$$=newnode("OptTag",@$.first_line,0,0,"");
		}
	;

Tag : ID{
		$$=newnode("Tag",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	;

VarDec : ID{
		$$=newnode("VarDec",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| VarDec LB INT RB{
		$$=newnode("VarDec",@$.first_line,0,0,"");
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	;
FunDec : ID LP VarList RP{
		$$=newnode("FunDec",@$.first_line,0,0,"");
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| ID LP RP{
		$$=newnode("FunDec",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| error RP {error_num++;}
	;
VarList : ParamDec COMMA VarList{
		$$=newnode("VarList",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| ParamDec{
		$$=newnode("VarList",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	;
ParamDec : Specifier VarDec{
		$$=newnode("ParamDec",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	;	

CompSt : LC DefList StmtList RC{
		$$=newnode("CompSt",@$.first_line,0,0,"");
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| error RC {error_num++;}
	;
StmtList : Stmt StmtList{
		$$=newnode("StmtList",@$.first_line,0,0,"Stmt StmtList");
		addchild($$,$2);
		addchild($$,$1);
		}
	|{
		$$=newnode("StmtList",@$.first_line,0,0,"Stmt StmtList");
		}  
	;
Stmt : Exp SEMI{
		$$=newnode("Stmt",@$.first_line,0,0,"Exp SEMI");
		addchild($$,$2);
		addchild($$,$1);
		}
	| CompSt{
		$$=newnode("Stmt",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| RETURN Exp SEMI{
		$$=newnode("Stmt",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| IF LP Exp RP Stmt{
		$$=newnode("Stmt",@$.first_line,0,0,"");
		addchild($$,$5);
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| IF LP Exp RP Stmt ELSE Stmt{
		$$=newnode("Stmt",@$.first_line,0,0,"");
		addchild($$,$7);
		addchild($$,$6);
		addchild($$,$5);
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| WHILE LP Exp RP Stmt{
		$$=newnode("Stmt",@$.first_line,0,0,"");
		addchild($$,$5);
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
 	| error RP{error_num++;}  
	| error SEMI{error_num++;}  
	;

DefList : Def DefList{
		$$=newnode("DefList",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	| {
		$$=newnode("DefList",@$.first_line,0,0,"");
		}
	;
Def : Specifier DecList SEMI{
		$$=newnode("Def",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| error SEMI{error_num++;}        
	;
DecList : Dec{
		$$=newnode("DecList",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| Dec COMMA DecList{
		$$=newnode("DecList",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	;
Dec : VarDec{
		$$=newnode("Dec",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| VarDec ASSIGNOP Exp{
		$$=newnode("Dec",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	;

Exp : Exp ASSIGNOP Exp{
		$$=newnode("Exp",@$.first_line,0,0,"Exp ASSIGNOP Exp");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp AND Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp OR Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp RELOP Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp PLUS Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp MINUS Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp STAR Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp DIV Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| LP Exp RP{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| MINUS Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	| NOT Exp{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$2);
		addchild($$,$1);
		}
	| ID LP Args RP{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| ID LP RP{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp LB Exp RB{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$4);
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
 	| Exp DOT ID{
		$$=newnode("Exp",@$.first_line,0,0,"Exp DOT ID");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| ID{
		$$=newnode("Exp",@$.first_line,0,0,"Exp->ID");
		addchild($$,$1);
		}
	| INT{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	| FLOAT{
		$$=newnode("Exp",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	;
Args : Exp COMMA Args{
		$$=newnode("Args",@$.first_line,0,0,"");
		addchild($$,$3);
		addchild($$,$2);
		addchild($$,$1);
		}
	| Exp{
		$$=newnode("Args",@$.first_line,0,0,"");
		addchild($$,$1);
		}
	;


%%

int yyerror(char *msg){
	printf("Error type B at line %d:unexpected near '%s'\n",yylineno,yylval.node->id_value);
}

