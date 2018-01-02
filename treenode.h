#ifndef _TREENODE_H_
#define _TREENODE_H_

#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include "symboltable.h"

#ifndef __BOOL__
#define __BOOL__
typedef enum {false,true} bool;
#endif

struct Node{
	char name[32];       //词素名称
	int linenum;         // 行数
	int int_value;       // int 
	float float_value;   // float 的值
	char id_value[32];   // id type 的值
	struct Node *child;  // 第一个子节点
	struct Node *sibling;// 兄弟节点
};

typedef struct Node Node;

Node *newnode(char* name,int linenum,int int_value,float float_value,char* id_value);      //新建节点

//Node *newnode(char* name,int linenum);

void addchild(Node *fathernode, Node* childnode);      //加子节点

void preorder(Node *root);   //先序遍历

void printtree(Node *root,int num);


//---语义分析---
void traveltree(Node *root); //语义分析
void ExtDefList(Node* n);
void ExtDef(Node *n);
Type Specifier(Node *n);
void ExtDecList(Node *n,Type type);
FieldList VarDec(Node* n,Type type);
void FunDec(Node *n, Type type);
Type StructSpecifier(Node *n);
FieldList DefList(Node* n,bool inStruct);
FieldList ParamDec(Node *n);
FieldList VarList(Node *n,int *num);
void CompSt(Node* n,Type type);
void StmtList(Node *n,Type type);
void Stmt(Node *n, Type type);
Type Exp(Node *n);
bool isTypeEqual(Type type1,Type type2);
FieldList Def(Node *n,bool inStruct);
FieldList DecList(Node *n,Type type,bool inStruct);
FieldList Dec(Node *n,Type type, bool inStruct);
bool isFieldListEqual(FieldList fl1,FieldList fl2);
bool isLeftVal(Node *n);
FieldList Args(Node* n);
#endif
