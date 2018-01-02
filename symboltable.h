#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABLESIZE 16384

//类型定义
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Func_* Func;
typedef struct Symbol_* Symbol;

struct Type_{
	enum {BASIC, ARRAY, STRUCTURE} kind;
	union
	{
		enum{INT_TYPE, FLOAT_TYPE} basic; 
		struct { Type elem; int size;  } array;
		FieldList structure;
	} u;	
};

struct FieldList_{
	char *name;
	Type type;
	FieldList tail;
};

//函数体
struct Func_{
	Type retype;
	int num;
	FieldList params;    // 参数
};

//符号表
struct Symbol_{
	enum { VAR, STRU, FUNC } kind;
	char *name;
	union{
		Type type;       // 变量 结构体
		Func func;		 // 函数
	} t;	
	Symbol next;
};


void inittable();

int inserttable(Symbol Sym);

Symbol searchtable(char* name);


#endif
