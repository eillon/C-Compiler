#ifndef _IRISIM_H_
#define _IRISIM_H_

#include "symboltable.h"
#include "intercode.h"
#include "treenode.h"

void translateProgram(Node *node);
void translateExtDefList(Node *node);
void translateExtDef(Node *node);
void translateExtDecList(Node *node);


void translateExp(Node* node,Operand place);
void translateCond(Node* node,Operand op1,Operand op2);
void translateArgs(Node *node, Operands arglist);
void translateStmt(Node *node);
void translateCompSt(Node *node);
void translateStmtList(Node* node);
void translateDefList(Node *node);
void translateDef(Node *node);
void translateDecList(Node *node);
void translateDec(Node *node);
void translateVarDec(Node *node,Operand op);
void translateFunDec(Node *node);
void translateArray(Node *node,Operand place);
void translateStruct(Node *node,Operand place);

Node *findID(Node*);
void optimizeIR();

int getTypeSize(Type type);



#endif