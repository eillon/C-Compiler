#ifndef _OBJECT_H
#define _OBJECT_H
#include"symboltable.h"
#include"intercode.h"
#include"treenode.h"

#define REG 18

typedef struct _reg_t* reg_t;
typedef struct _var_t* var_t;


struct _reg_t{
	char* var;
	struct _reg_t* next;
};

struct _var_t{
	char* name;
	int reg;
	int offset;
	struct _var_t *next;
};

reg_t regtable[REG];
var_t varaddr;
int offset;

void initreg();
void swreg(FILE* fp);
void add_var( var_t  v);
int choose() ;
var_t findvar(char *name);
int getReg(FILE *fp, Operand op);

#endif
