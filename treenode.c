#include "treenode.h"

Node *newnode(char* name,int linenum,int int_value,float float_value,char* id_value){      //新建节点
	Node *p=malloc(sizeof(Node));
	strcpy(p->name,name);
	p->linenum=linenum;
	p->int_value=int_value;
	p->float_value=float_value;
	strcpy(p->id_value,id_value);
	p->child=NULL;
	p->sibling=NULL;
	return p;
}

//Node *newnode(char *name,int linenum){
//	newnode(name,linenum,0,0.0,"");

void addchild(Node *fathernode, Node* childnode){      //加子节点
	if(fathernode!=NULL && childnode!=NULL){
		childnode->sibling = fathernode->child;
		fathernode->child=childnode;
	}
}

void preorder(Node *root){   //先序遍历
	if(root==NULL) return ;
	printf("%s \n",root->name);
	preorder(root->child);
	preorder(root->sibling);
}

void printtree(Node *root,int num){
	if(root==NULL) return ;
	
	if(root->child==NULL){        //产生 NULL 的表达式
		if(	!strcmp(root->name,"ExtDefList")||
			!strcmp(root->name,"OptTag")||
			!strcmp(root->name,"StmtList")){
				//dose nothing
			return;
		}
	}

	for(int i=0;i<num;i++)
		printf("  ");

	if(root->child==NULL){      //词法单元
		if(!strcmp(root->name,"TYPE"))
			printf("%s: %s\n",root->name,root->id_value);
		else if(!strcmp(root->name,"INT"))
			printf("%s: %d\n",root->name,root->int_value);
		else if(!strcmp(root->name,"FLOAT"))
			printf("%s: %.6f\n",root->name,root->float_value);
		else if(!strcmp(root->name,"ID"))
			printf("%s: %s\n",root->name,root->id_value);
		else
			printf("%s\n",root->name);
	}
	else{
		printf("%s (%d)\n",root->name,root->linenum);
	}

	
	printtree(root->child,num+1);
	printtree(root->sibling,num);

}


//-----------------------------------------------
//-------------------语义分析 ---------------------

void traveltree(Node *n){
	//printf("Test:Program\n");
	inittable();
	//Program -> ExtDefList
	ExtDefList(n->child);
	//printf("Test:travel over\n");
}

void ExtDefList(Node* n){
	//printf("Test:ExtDefList\n");
	//ExtDefList -> e
	if(n->child==NULL) return;     
	
	//ExtDefList -> ExtDef ExtDefList
	ExtDef(n->child);
	ExtDefList(n->child->sibling);
}

void ExtDef(Node *n){
	//printf("Test:ExtDef\n");
	Type type=Specifier(n->child);
	Node *sibling=n->child->sibling;

	if(strcmp(sibling->name,"ExtDecList")==0){
		//ExtDef -> Specifier ExtDecList SEMI
		ExtDecList(sibling,type);      //将type继承下去
	}
	else if(strcmp(sibling->name,"SEMI")==0){
		//ExtDef -> Specifier SEMI
		// does nothing
	}
	else if(strcmp(sibling->name,"FunDec")==0){
		//ExtDef -> Specifier FunDec CompSt
		FunDec(sibling,type);
		CompSt(sibling->sibling,type);  //type用来检查返回值是否匹配
	}
	else{
		//error
		printf("Unknown Error\n");
	}
}

Type Specifier(Node *n){
	//printf("Test:Specifier\n");
	Type type;
	
	Node *child=n->child;
	if(strcmp(child->name,"TYPE")==0){
		type=malloc(sizeof(struct Type_));
		type->kind=BASIC;
		if(strcmp(child->id_value,"int")==0)
			type->u.basic=INT_TYPE;
		else if(strcmp(child->id_value,"float")==0)
			type->u.basic=FLOAT_TYPE;
	}
	else if(strcmp(child->name,"StructSpecifier")==0){
		type=StructSpecifier(child);
	//	printf("%d\n",type->kind);
	}
	else{
		//error
		printf("Unknown Error\n");
		type=NULL;
	}
	return type;
}

void ExtDecList(Node *n,Type type){
	//printf("Test:ExtDecList\n");

	Node* p=n->child;
	VarDec(p,type);
	if(p->sibling!=NULL){
		ExtDecList(p->sibling->sibling,type);
	}
}

FieldList VarDec(Node* n,Type type){
	//printf("Test:VarDec\n");	
	Node *p=n->child;
	FieldList fl;

	if(strcmp(p->name,"ID")==0){
		Symbol t=searchtable(p->id_value);
		if(t!=NULL){                                                                    //Error 3
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",p->linenum,p->id_value);
		} 
		else{
			t=malloc(sizeof(struct Symbol_));
			t->kind=VAR;
			t->name=p->id_value;
			t->t.type=type;
			t->next=NULL;
			inserttable(t);

			fl=malloc(sizeof(struct FieldList_));
			fl->name=p->id_value;
			fl->type=type;
			fl->tail=NULL;
		}
	}
	else if(strcmp(p->name,"VarDec")==0){
		Type newtype=malloc(sizeof(struct Type_));
		newtype->kind=ARRAY;
		newtype->u.array.elem=type;
		newtype->u.array.size=p->sibling->sibling->int_value;
		
	//	fl=malloc(sizeof(struct FieldList_));
	//	fl->type=newtype;
		fl=VarDec(p,newtype);		
	}
	else {
		printf("Unknown Error\n");
	}
	
	return fl;
}

void FunDec(Node *n, Type type){
	//printf("Test:FunDec\n");
	Node* p=n->child;
	Symbol t=searchtable(p->id_value);
	if(t!=NULL){                                                                    //ERROR 4
		printf("Error type 4 at Line %d: Redefined function \"%s\".\n",p->linenum,p->id_value);
	}
	else{
		t=malloc(sizeof(struct Symbol_));
		t->kind=FUNC;
		t->name=p->id_value;
		t->t.func=malloc(sizeof(struct Func_));
		t->t.func->retype=type;
		Node *pp=p->sibling->sibling;
		int paramsnum;
		FieldList params;
		if(strcmp(pp->name,"RP")==0){
			paramsnum=0;
			params=NULL;
		}
		else{
			params=VarList(pp,&paramsnum);
		}
		t->t.func->num=paramsnum;
		t->t.func->params=params;
		inserttable(t);
	}
}

FieldList VarList(Node *n,int *num){
	//printf("Test:VarList\n");
	Node *p=n->child;
	FieldList fl;
	
	fl=ParamDec(p);
	*num++;

	if(p->sibling!=NULL){
		fl->tail=VarList(p->sibling->sibling,num);
	}

	return fl;
}

FieldList ParamDec(Node *n){
	//printf("Test:ParamDec\n");
	return VarDec(n->child->sibling,Specifier(n->child));
}

Type StructSpecifier(Node *n){
	//printf("Test:StructSpecifier\n");
	Type type;

	Node *p=n->child->sibling;  //第二个子节点
	if(strcmp(p->name,"Tag")==0){
		// 结构体 使用
		Node *pp=p->child;
		Symbol t=searchtable(pp->id_value);
		if(t==NULL){  //结构体未定义                                                        //  ERROR 17
			printf("Error type 17 at Line %d: Undefined struct \" %s \".\n",pp->linenum,pp->id_value);
			type=NULL;
		}
		else{
			type=t->t.type;
		}
	}
	else if(strcmp(p->name,"OptTag")==0){
		// 结构体 定义
		type=malloc(sizeof(struct Type_));
		Node* pp=p->child;
		if(pp==NULL){
			//does nothing      因为以后不可能用到
			//printf("Unknown \n");
			type=NULL; 
		}
		else{
			Symbol t=searchtable(pp->id_value);
			if(t!=NULL){
				printf("Error type 16 at Line %d: Duplicated defined \" %s \".\n",pp->linenum,pp->id_value);
				type=NULL;
			}
			else{
				type->kind=STRUCTURE;
				type->u.structure=DefList(p->sibling->sibling,true);   
							
				//将该结构体存放进符号表中
				t=malloc(sizeof(struct Symbol_));
				t->kind=STRU;
				t->name=pp->id_value;
				t->t.type=type;
				t->next=NULL;
				inserttable(t);     //插入必然成功
			}
		}
	}
	else {
		printf("Unknown error\n");
	}

	return type;
}

FieldList DefList(Node* n,bool inStruct){       //isStruct 表示是否在结构体中
	//printf("Test:DefList\n");
	Node *p=n->child;
	if(p==NULL) return NULL;
	else{
		FieldList fl;
		fl=Def(p,inStruct);
		if(p->sibling!=NULL){
			fl->tail=DefList(p->sibling,inStruct);
		}
		return fl;
	}
}

FieldList Def(Node *n,bool inStruct){
	//printf("Test:Def\n");
	return DecList(n->child->sibling,Specifier(n->child),inStruct);
}

FieldList DecList(Node *n,Type type,bool inStruct){
	//printf("Test:DecList\n");
	Node *p=n->child;
	FieldList fl;
	fl=Dec(p,type,inStruct);
	if(p->sibling!=NULL){
		fl->tail=DecList(p->sibling->sibling,type,inStruct);
	}
	return fl;
}

FieldList Dec(Node *n,Type type, bool inStruct){
	//printf("Test:Dec\n");
	Node* p=n->child;
	if(p->sibling!=NULL){
		if(inStruct==true){                                      //ERROR 15
			printf("Error type 15 at Line %d: Initialized vars in STRUCT.\n",p->linenum);
		}
		Type e=Exp(p->sibling->sibling);
		if(!isTypeEqual(type,e)){
			printf("Error type 5 at line %d: The type mismatched\n",p->linenum);
		}
	}
	FieldList fl;
	fl=VarDec(p,type);
	return fl;
}

void CompSt(Node* n,Type type){
	//printf("Test:CompSt\n");
	Node *p=n->child->sibling;
	DefList(p,false);
	StmtList(p->sibling,type);
}

void StmtList(Node *n,Type type){
	//printf("Test:StmtList\n");
	Node *p=n->child;
	if(p==NULL) return ;
	else{
		Stmt(p,type);
		StmtList(p->sibling,type);
	}
}

void Stmt(Node *n, Type type){
	//printf("Test:Stmt\n");
	Node *p=n->child;
	if(strcmp(p->name,"Exp")==0){
		Exp(p);
	}
	else if(strcmp(p->name,"CompSt")==0){
		CompSt(p,type);
	}
	else if(strcmp(p->name,"RETURN")==0){
		Node *pp=p->sibling;
		Type e=Exp(pp);
		if(!isTypeEqual(e,type)){                                           //ERROR 8
			printf("Error type 8 at Line %d: Type mismatched for return.\n",p->linenum);
		}
	}
	else if(strcmp(p->name,"IF")==0){
		Node *pp=p->sibling->sibling;

		Type typeInt=malloc(sizeof(struct Type_));
		typeInt->kind=BASIC;
		typeInt->u.basic=INT_TYPE;

		Type e=Exp(pp);
		if(!isTypeEqual(typeInt,e)){                        //ERROR 0---INT 作为if/while的条件
			printf("Error type 0 at Line %d: Type mismatched for IF.\n",p->linenum);
		}

		pp=pp->sibling->sibling;
		Stmt(pp,type);

		pp=pp->sibling;
		if(pp!=NULL){
			pp=pp->sibling;
			Stmt(pp,type);
		}
	}
	else if(strcmp(p->name,"WHILE")==0){
		Node *pp=p->sibling->sibling;

		Type typeInt=malloc(sizeof(struct Type_));
		typeInt->kind=BASIC;
		typeInt->u.basic=INT_TYPE;
		
		Type e=Exp(pp);
		if(!isTypeEqual(typeInt,e)){                         //ERROR 0---INT 作为if/while的条件
			printf("Error type 0 at Line %d: Type mismatched for WHILE.\n",p->linenum);
		}

		pp=pp->sibling->sibling;
		Stmt(pp,type);
	}
}

Type Exp(Node *n){
	//printf("Test:Exp\n");
	Node *p=n->child;
	Type type=NULL;

	Type typeInt=malloc(sizeof(struct Type_));
	typeInt->kind=BASIC;
	typeInt->u.basic=INT_TYPE;

	Type typeFloat=malloc(sizeof(struct Type_));
	typeFloat->kind=BASIC;
	typeFloat->u.basic=FLOAT_TYPE;

	if(strcmp(p->name,"Exp")==0){
		Node *pp=p->sibling;
		if(strcmp(pp->name,"ASSIGNOP")==0){
			if(!isLeftVal(p)){                                                                      //ERROR 6
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",pp->linenum);
			}
			Type e1=Exp(p),e2=Exp(pp->sibling);
			if(isTypeEqual(e1,e2)){
				type=e1;
			}
			else{                                                  //ERROR  5
				printf("Error type 5 at line %d: Type mismatched for assignment.\n",p->linenum);
			}
		}
		else if(strcmp(pp->name,"AND")==0 ||
				strcmp(pp->name,"OR")==0 ||
				strcmp(pp->name,"RELOP")==0				
				){
			Type e1=Exp(p),e2=Exp(pp->sibling);
			if(isTypeEqual(typeInt,e1)&&isTypeEqual(typeInt,e2)){
				type=typeInt;
			}
			else{
				printf("Error type 7 at line %d: Type mismatched for operands.\n",p->linenum);
			}
		}
		else if(strcmp(p->name,"NOT")==0){
			Type e=Exp(p->sibling);
			if(isTypeEqual(typeInt,e)){
				type=typeInt;
			}
			else{
				printf("Error type 7 at line %d: Type mismatched for operands.\n",p->linenum);
			}
		}
		else if(strcmp(pp->name,"PLUS")==0||
				strcmp(pp->name,"MINUS")==0||
				strcmp(pp->name,"STAR")==0||
				strcmp(pp->name,"DIV")==0
				){
			Type e1=Exp(p),e2=Exp(pp->sibling);		
			if(!isTypeEqual(e1,e2)){                       //ERROR 7
				printf("Error type 7 at line %d: Type mismatched for operands.\n",p->linenum);
			}
			else if(!isTypeEqual(e1,typeInt)&&!isTypeEqual(e1,typeFloat)){
				printf("Error type 7 at line %d: Type mismatched for operands.\n",p->linenum);
			}
			else{
				type=e1;
			}
		}
		else if(strcmp(pp->name,"LB")==0){
			Type t=Exp(p);
			if(t==NULL) return NULL;
			if(t->kind!=ARRAY){
				printf("Error type 10 at Line %d: is not an array.\n",p->linenum);
			}
			Type tt=Exp(pp->sibling);
			if(tt==NULL) return NULL;
			if(tt->kind==BASIC&&tt->u.basic==INT_TYPE){
				type=t->u.array.elem;           //数据类型变了
			}
			else{
				printf("Error type 12 at Line %d: is not an integer.\n",p->linenum);
			}
		}
		else if(strcmp(pp->name,"DOT")==0){
			Type t=Exp(p);
			if(t==NULL) return NULL;
			if(t->kind!=STRUCTURE){
				printf("Error type 13 at line %d: Illegal use of '.'\n",p->linenum);
			}
			else{
				FieldList fl=t->u.structure;
				Node *ppp=pp->sibling;
				while(fl!=NULL){
					if(strcmp(fl->name,ppp->id_value)==0){
						type= fl->type;
						break;
					}
					fl=fl->tail;
				}
				if(fl==NULL){
					printf("Error type 14 at line %d: Non-existent field \"%s\".\n",ppp->linenum,ppp->id_value);
				}
			}
		}
	}
	else if(strcmp(p->name,"LP")==0){
		type=Exp(p->sibling);
	}
	else if(strcmp(p->name,"MINUS")==0){
		type=Exp(p->sibling);
	}
	else if(strcmp(p->name,"ID")==0){
		Node *pp=p->sibling;
		if(pp==NULL){
			Symbol s=searchtable(p->id_value);
			if(s==NULL){
				printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",p->linenum,p->id_value);
			}
			else{

				type=s->t.type;
			}
		}
		else{
			Symbol s=searchtable(p->id_value);
			if(s==NULL){
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",p->linenum,p->id_value);
			}
			else {
				if(s->kind!=FUNC){
					printf("Error type 11 at line %d: \"%s\" is not a function\n",p->linenum,p->id_value);
				}
				else{
					pp=pp->sibling;
					FieldList fl1;
					FieldList fl2=s->t.func->params;
					if(strcmp(pp->name,"Args")==0){
						fl1=Args(pp);
					}
					else if(strcmp(pp->name,"RP")==0){
						fl1=NULL;
					}

					if(!isFieldListEqual(fl1,fl2)){
						printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n",p->linenum,p->id_value);
					}

					type=s->t.func->retype;
				}
			}
		}	
	}
	else if(strcmp(p->name,"INT")==0){
		type=typeInt;		
	}
	else if(strcmp(p->name,"FLOAT")==0){
		type=typeFloat;
	}

	return type;
}

FieldList Args(Node* n){
	//printf("Test:Arags\n");
	Node *p=n->child;
	FieldList fl=malloc(sizeof(struct FieldList_));
	fl->type=Exp(p);
	fl->tail=NULL;
	if(p->sibling!=NULL)
		fl->tail=Args(p->sibling->sibling);
	
	return fl;
}

bool isFieldListEqual(FieldList fl1,FieldList fl2){
	while(fl1!=NULL && fl2!=NULL){
		if(!isTypeEqual(fl1->type,fl2->type)) return false;
		fl1=fl1->tail;
		fl2=fl2->tail;	
	}
	
	if(fl1==NULL&&fl2==NULL)
		return true;
	
	return false;
}

bool isTypeEqual(Type t1,Type t2){
	if(t1==NULL && t2==NULL) return true;
	if(t1==NULL || t2==NULL) return false;

	if(t1->kind!=t2->kind) return false;
	if(t1->kind==BASIC){
		if(t1->u.basic!=t2->u.basic) return false;
	}
	else if(t1->kind==ARRAY){
		if(!isTypeEqual(t1->u.array.elem,t2->u.array.elem)) return false;
	}
	else if(t1->kind==STRUCTURE){
		if(strcmp(t1->u.structure->name,t2->u.structure->name)) return false;
	}

	return true;
}

bool isLeftVal(Node *n){
	Node *p=n->child;
	if(strcmp(p->name,"ID")==0&&p->sibling==NULL) return true;
	if(strcmp(p->name,"Exp")==0&&strcmp(p->sibling->name,"DOT")==0) return true;
	if(strcmp(p->name,"Exp")==0&&strcmp(p->sibling->name,"LB")==0) return true;
	return false;
}
