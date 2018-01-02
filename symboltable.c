#include "symboltable.h"


Symbol hashtable[TABLESIZE];

unsigned int hash_pjw(char *name)
{
	unsigned int val = 0, i;
	for(;*name;++name)
	{
		val = (val << 2) + *name;
		if(i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val;
}

void inittable()
{
	int i ; 
	for(i = 0; i < TABLESIZE; i++)
		hashtable[i] = NULL ;
	
	//预设函数 read() write()
	Type intType=malloc(sizeof(struct Type_));
	intType->kind=BASIC;
	intType->u.basic=INT_TYPE;
	
	//int read()
	Symbol read=malloc(sizeof(struct Symbol_));
	read->kind=FUNC;
	read->name=malloc(sizeof("read"));
	strcpy(read->name,"read");
	read->t.func=malloc(sizeof(struct Func_));
	read->t.func->retype=intType;
	read->t.func->num=0;
	read->t.func->params=NULL;
	read->next=NULL;
	inserttable(read);

	//int write(int)
	Symbol write=malloc(sizeof(struct Symbol_));
	write->kind=FUNC;
	write->name=malloc(sizeof("write"));
	strcpy(write->name,"write");
	write->t.func=malloc(sizeof(struct Func_));
	write->t.func->retype=intType;
	write->t.func->num=1;
	write->t.func->params=malloc(sizeof(struct FieldList_));
	write->t.func->params->type=intType;
	write->t.func->params->tail=NULL;
	write->next=NULL;
	inserttable(write);
//	printf("init over\n");	
}

// 返回 1 表示名重复
// 返回 0 表示插入成功
int inserttable(Symbol sym)
{
	if(sym == NULL) return 0;
	unsigned int num = hash_pjw(sym -> name);
	if(hashtable[num] == NULL)
		hashtable[num] = sym;
	else	
	{
		Symbol p = hashtable[num];
		for(;p != NULL; p = p -> next)
		{
			if(strcmp(p -> name, sym -> name) == 0) return 1; // redefine 3 4
			
		}
        //插入到头
		p = hashtable[num];
		hashtable[num] = sym;
		sym -> next = p;
	
	}
	return 0;
}

// 返回 NULL 当符号表里没有该值
Symbol searchtable(char * name)
{
	unsigned int num = hash_pjw(name);
	if(hashtable[num] == NULL)
		return NULL;     //undefined 1 2

	else
	{
		Symbol p = hashtable[num];
		for(;p != NULL;p = p->next)
		{
			if(strcmp(p->name, name)== 0) return p;
		}
		return NULL;    //undefine 1 2
	}

}