#ifndef _OBJECT_H
#define _OBJECT_H
#include"symboltable.h"
#include"intercode.h"
#include"treenode.h"

#define REG 18

typedef struct _reg_t* reg_t;
typedef struct _var_t* var_t;
int offset;

struct _reg_t{
	Operands op;
};

struct _var_t{
	Operand op;
	int reg;
	int offset;
	struct _var_t *next;
};

reg_t regtable[REG];
var_t varaddr;


void initreg();
void swreg();
void add_var( var_t  v);
int findreg(Operand op);
int choose() ;
var_t findvar(Operand op);
int getReg(FILE *fp, Operand op);

//void objectcode(InterCode ir, FILE * fp);
//void some(FILE *fp);

#endif
