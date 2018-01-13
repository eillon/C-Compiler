#include "irisim.h"

void translateExp(Node* node,Operand place){
    //printf("Test: Exp\n");
    Node* p=node->child;
    if(strcmp(p->name,"Exp")==0){
        //printf("Test: -->Exp \n");
        Node* pp=p->sibling;
        if(strcmp(pp->name,"ASSIGNOP")==0){
            Node *pc=p->child;
            if(strcmp(pc->name,"ID")==0){
                Operand t1=newTemp();
                Operand v1=newVar(pc->id_value);
                translateExp(pp->sibling,t1);
                if(t1->isAddr==true) t1=chAddr(t1);
                if(v1->isAddr==true) v1=chAddr(v1);
                genAssign(v1,t1);
                genAssign(place,v1);
                return;
            }
            else{
                Operand t1=newTemp();
                Operand t2=newTemp();
                translateExp(p,t1);
                translateExp(pp->sibling,t2);
                if(t1->isAddr==true) t1=chAddr(t1);
                if(t2->isAddr==true) t2=chAddr(t2);
                genAssign(t1,t2);
                genAssign(place,t1);
                return;
            }
        }
        else if(strcmp(pp->name,"AND")==0){
            Operand l1=newLabel();
            Operand l2=newLabel();
            Operand c1=newConst(0);
            genAssign(place,c1);
            translateCond(node,l1,l2);
            genLabel(l1);
            Operand c2=newConst(1);
            genAssign(place,c2);
            genLabel(l2);
            return;
        }
        else if(strcmp(pp->name,"OR")==0){
            Operand l1=newLabel();
            Operand l2=newLabel();
            Operand c1=newConst(0);
            genAssign(place,c1);
            translateCond(node,l1,l2);
            genLabel(l1);
            Operand c2=newConst(1);
            genAssign(place,c2);
            genLabel(l2);
            return;
        }
        else if(strcmp(pp->name,"RELOP")==0){
            Operand l1=newLabel();
            Operand l2=newLabel();
            Operand c1=newConst(0);
            genAssign(place,c1);
            translateCond(node,l1,l2);
            genLabel(l1);
            Operand c2=newConst(1);
            genAssign(place,c2);
            genLabel(l2);
            return;
        }
        else if(strcmp(pp->name,"PLUS")==0){
            Operand t1=newTemp();
            Operand t2=newTemp();
            translateExp(p,t1);
            translateExp(pp->sibling,t2);
            if(t1->isAddr==true) t1=chAddr(t1);
            if(t2->isAddr==true) t2=chAddr(t2);
            genAdd(place,t1,t2);
            return ;
        }
        else if(strcmp(pp->name,"MINUS")==0){
            Operand t1=newTemp();
            Operand t2=newTemp();
            translateExp(p,t1);
            translateExp(pp->sibling,t2);
            genSub(place,t1,t2);
            return ;
        }
        else if(strcmp(pp->name,"STAR")==0){
            Operand t1=newTemp();
            Operand t2=newTemp();
            translateExp(p,t1);
            translateExp(pp->sibling,t2);
            genMul(place,t1,t2);
            return ;
        }
        else if(strcmp(pp->name,"DIV")==0){
            Operand t1=newTemp();
            Operand t2=newTemp();
            translateExp(p,t1);
            translateExp(pp->sibling,t2);
            genDiv(place,t1,t2);
            return ;
        }
        else if(strcmp(pp->name,"LB")==0){
            translateArray(node,place);
        }
        else if(strcmp(pp->name,"DOT")==0){
            translateStruct(node,place);
        }
        else{
            printf("Unknown Error in Exp->Exp\n");
            return;
        }
    }
    else if(strcmp(p->name,"LP")==0){
        //printf("Test: -->LP \n");
        translateExp(p->sibling,place);
    }
    else if(strcmp(p->name,"MINUS")==0){
        //printf("Test: -->MINUS \n");
        Operand t1=newTemp();
        translateExp(p->sibling,t1);
        Operand c1=newConst(0);
        genSub(place,c1,t1);
        return;
    }
    else if(strcmp(p->name,"NOT")==0){
        //printf("Test: -->NOT \n");
        Operand l1=newLabel();
        Operand l2=newLabel();
        Operand c1=newConst(0);
        genAssign(place,c1);
        translateCond(node,l1,l2);
        genLabel(l1);
        Operand c2=newConst(1);
        genAssign(place,c2);
        genLabel(l2);
        return;
    }
    else if(strcmp(p->name,"ID")==0){
        //printf("Test: -->ID \n");
        Node *pp=p->sibling;
        if(pp==NULL){
            Symbol s=searchtable(p->id_value);
            
            //新增，为结构体和数组设计
            if(s->kind==VAR || s->kind==STRU){
                if(s->t.type->kind==ARRAY || s->t.type->kind==STRUCTURE ){
                    Operand v1=newVar(s->name);
                    Operand r1=chRefe(v1);
                    genAssign(place,r1);
                    return;
                }
            }

            Operand op=newVar(s->name);
            genAssign(place,op);
            return;
        }
        else{
            pp=pp->sibling;
            if(strcmp(pp->name,"RP")==0){
                if(strcmp(p->id_value,"read")==0){
                    genRead(place);
                    return;
                }
                else{
                    Operand v1=newVar(p->id_value);
                    genCall(place,v1);
                    return;
                }
            }
            else if(strcmp(pp->name,"Args")==0){
                Operands argList=malloc(sizeof(struct Operands_));   //头节点，不存储任何内容
                translateArgs(pp,argList);
                if(argList==NULL){
                    printf("Error in prefer program.\n");
                }
                //printf("Test: -->ID LP Args RP:  Program get there. \n");
                if(strcmp(p->id_value,"write")==0){
                    
                    genWrite(argList->next->op);
                    //printf("Test: -->ID LP Args RP: Program get there? \n");
                    return;
                }
                else{
                    for(Operands t=argList->next;t!=NULL;t=t->next){
                        genArg(t->op);
                    }
                    Operand v1=newVar(p->id_value);
                    genCall(place,v1);
                }
            }
            else{
                printf("Unknown Error in Exp->ID LP \n");
                return;
            }
        } 
    }
    else if(strcmp(p->name,"INT")==0){
        //printf("Test: -->INT \n");
        Operand op=newConst(p->int_value);
        genAssign(place,op);
        return;
    }
    else if(strcmp(p->name,"FLOAT")==0){
        //printf("Test: -->FLOAT \n");
        //do nothing
    }
    else {
        printf("Unknown Error in Exp \n");
        return ;
    }
}

void translateCond(Node *node, Operand l1, Operand l2){
    //printf("Test: Cond\n");
    Node *p=node->child;
    if(strcmp(p->name,"NOT")==0){
        translateCond(p->sibling,l1,l2);
        return;
    }
    else if(strcmp(p->name,"Exp")==0){
        Node *pp=p->sibling;
        if(strcmp(pp->name,"RELOP")==0){
            Operand t1=newTemp();
            Operand t2=newTemp();
            translateExp(p,t1);
            translateExp(pp->sibling,t2);
            genIfGoto(t1,t2,l1,pp->id_value);
            genGoto(l2);
            return;
        }
        else if(strcmp(pp->name,"AND")==0){
            Operand label1=newLabel();
            translateCond(p,label1,l2);
            genLabel(label1);        
            translateCond(pp->sibling,l1,l2);
            return;
        }
        else if(strcmp(pp->name,"OR")==0){
            Operand label1=newLabel();
            translateCond(p,l1,label1);
            genLabel(label1);
            translateCond(pp->sibling,l1,l2);
            return;
        }
        else{
            printf("\n Unknown Error in translateCond \n");
            return;
        }
    }
    else{
        Operand t1=newTemp();
        translateExp(node,t1);
        Operand c1=newConst(0);
        genIfGoto(t1,c1,l1,"!=");
        genGoto(l2);
        return;
    }
}

void translateArgs(Node *node,Operands argList){
    //printf("Test: Args\n");
    Node *p=node->child,*pp=p->sibling;
    if(pp==NULL){
        Operand t1=newTemp();
        translateExp(p,t1);
        if(t1->isAddr==true) t1=chAddr(t1);
        insertOperand(argList,t1);
        return;
    }
    else{
        Operand t1=newTemp();
        translateExp(p,t1);
        if(t1->isAddr==true) t1=chAddr(t1);
        insertOperand(argList,t1);
        translateArgs(pp->sibling,argList);
        return;
    }
}

void translateStmt(Node *node){
    //printf("Test: Stmt\n");
    Node *p=node->child;
    if(strcmp(p->name,"Exp")==0){
        Operand t1=newTemp();
        translateExp(p,t1);          //place 本来是NULL， 这里用 t1 代替， 无影响
        return;
    }
    else if(strcmp(p->name,"CompSt")==0){
        translateCompSt(p);
        return;
    }
    else if(strcmp(p->name,"RETURN")==0){
        Operand t1=newTemp();
        translateExp(p->sibling,t1);
        genReturn(t1);
        return;
    }
    else if(strcmp(p->name,"WHILE")==0){
        Operand l1=newLabel();
        Operand l2=newLabel();
        Operand l3=newLabel();
        genLabel(l1);
        translateCond(p->sibling->sibling,l2,l3);
        genLabel(l2);
        translateStmt(p->sibling->sibling->sibling->sibling);
        genGoto(l1);
        genLabel(l3);
        return;
    }
    else if(strcmp(p->name,"IF")==0){
        Node *pp=p->sibling->sibling->sibling->sibling->sibling;
        if(pp==NULL){
            Operand l1=newLabel();
            Operand l2=newLabel();
            translateCond(p->sibling->sibling,l1,l2);
            genLabel(l1);
            translateStmt(p->sibling->sibling->sibling->sibling);
            genLabel(l2);
            return;
        }
        else{
            Operand l1=newLabel();
            Operand l2=newLabel();
            Operand l3=newLabel();
            p=p->sibling->sibling;
            translateCond(p,l1,l2);
            genLabel(l1);
            p=p->sibling->sibling;
            translateStmt(p);
            genGoto(l3);
            genLabel(l2);
            p=p->sibling->sibling;
            translateStmt(p);
            genLabel(l3);
            return;
        }
    }
}

void translateCompSt(Node *node){
    //printf("Test: CompSt\n");
    Node *p=node->child->sibling;
    translateDefList(p);
    p=p->sibling;
    translateStmtList(p);
}

void translateStmtList(Node* node){
    //printf("Test: StmtList\n");
    Node *p=node->child;
    if(p==NULL){
        return;
    }
    else{
        translateStmt(p);
        translateStmtList(p->sibling);
    }
}

void translateDefList(Node *node){
    //printf("Test: DefList\n");
    Node *p=node->child;
    if(p==NULL) return;
    else{
        translateDef(p);
        translateDefList(p->sibling);
    }
}

void translateDef(Node *node){
    //printf("Test: Def\n");
    Node *p=node->child->sibling;   //Specifier 不用翻译
    translateDecList(p);
}

void translateDecList(Node *node){
    //printf("Test: DecList\n");
    Node *p=node->child;
    translateDec(p);
    p=p->sibling;
    if(p==NULL) return;
    p=p->sibling;
    translateDecList(p);
}

void translateDec(Node *node){
    //printf("Test: Dec\n");
    Node *p=node->child;
    Node *pp=p->sibling;
    Operand tmp=malloc(sizeof(struct Operand_));;
    if(pp==NULL){
        translateVarDec(p,tmp);
    }
    else{
        pp=pp->sibling;
        translateVarDec(p,tmp);        //对tmp赋值
        translateExp(pp,tmp);
        return;
    }
}

void translateVarDec(Node *node,Operand op){     // 为数组或结构体 生成 DEC [size]
    //printf("Test: VarDec\n");
    Node *p=node->child;
    if(strcmp(p->name,"ID")==0){
        Symbol s=searchtable(p->id_value);
        op->kind=VAR_OP;
        op->u.val_name=p->id_value;
        
        Operand v1=newVar(p->id_value);
        if(s==NULL){
            printf("\n Error in grammar \n");
            exit(0);
        }

        if(s->t.type->kind==BASIC) return;

        int size=getTypeSize(s->t.type);
        genDec(v1,size);
    }
    else{
        translateVarDec(p,op);
    }
}

void translateProgram(Node *node){
    //printf("Test: Program\n");
    initInterCode();
    translateExtDefList(node->child);
}

void translateExtDefList(Node *node){
    //printf("Test: ExtDefList\n");
    Node *p=node->child;
    if(p!=NULL){
        translateExtDef(p);
        translateExtDefList(p->sibling);
    }
}

void translateExtDef(Node *node){
    //printf("Test: ExtDef\n");
    Node *p=node->child->sibling;
    if(strcmp(p->name,"ExtDecList")==0){
        translateExtDecList(p);
    }
    else if(strcmp(p->name,"SEMI")==0){
        return;
    }
    else{
        translateFunDec(p);
        translateCompSt(p->sibling);
    }
}

void translateExtDecList(Node *node){
    //printf("Test: ExtDecList\n");
    Node *p=node->child;
    Operand tmp;
    translateVarDec(p,tmp);
    p=p->sibling;
    if(p!=NULL){
        p=p->sibling;
        translateExtDecList(p);
    }
}

void translateFunDec(Node *node){
    //printf("Test: FunDec\n");
    Node *p=node->child;
    Operand f1=newVar(p->id_value);  
    Symbol s=searchtable(p->id_value);
    genFunction(f1);
    p=p->sibling->sibling;
    if(strcmp(p->name,"VarList")==0){
        //printf("Test: -->VarList \n");
        if(s==NULL){
            printf("translateFunDec: Error in grammar\n");
            exit(0);
        }
        FieldList param=s->t.func->params;
        while(param!=NULL){
            Operand p1=newVar(param->name);
            genParam(p1);
            param=param->tail;
        }
    }
    else{
        return;
    }
}

void translateArray(Node *node,Operand place){
    Node *p=node->child;
    Node *pc=p->child;
    if(strcmp(pc->name,"ID")==0){
        Symbol s=searchtable(pc->id_value);
        Type type=s->t.type->u.array.elem;
        Operand v1=newVar(pc->id_value);
        Operand f1=chRefe(v1);
        int typeSize=getTypeSize(type);
        Node *pp=p->sibling->sibling->child;  //一定是INT

        Operand c1=newConst(typeSize*pp->int_value);  //offset
        genAdd(place,f1,c1);
        return;
    }
    else if(strcmp(pc->name,"Exp")==0 && strcmp(pc->sibling->name,"LB")==0){
        printf("Can not translate the code: Contain multidimensional array and function parameters of array type!\n");
		exit(0);
    }
    else if(strcmp(pc->name,"Exp")==0 && strcmp(pc->sibling->name,"DOT")==0){
        translateStruct(pc,place);
    }
    else {
        printf("Error in grammar.\n");
        return;
    }
}

void translateStruct(Node *node,Operand place){
    Node *p=node->child;
    Operand t1=newTemp();
    translateExp(p,t1);

    char *name=p->sibling->sibling->id_value;
    Node *id=findID(p);
    Symbol s=searchtable(id->id_value);
    if(s==NULL) return ;
    if(s->kind==FUNC) return;

    Type type=s->t.type;   
    FieldList fl=type->u.structure;
    int offset=0;
    while(fl!=NULL && strcmp(fl->name,name)!=0){
        offset+=getTypeSize(fl->type);
        fl=fl->tail;
    }
    Operand c1=newConst(offset);
    genAdd(place,t1,c1);    
}

Node* findID(Node *node){
    if(node==NULL) return NULL;
    Node *p=node->child;
    if(strcmp(p->name,"Exp")==0){
        Node *pp=p->sibling;
        if(strcmp(pp->name,"LB")==0){
            return findID(p);
        }
        else if(strcmp(pp->name,"DOT")==0){
            return findID(p);
        }
        else{
            return NULL;
        }
    }
    else if(strcmp(p->name,"ID")==0){
        return p;
    }
    else{
        return NULL;
    }
}


int getTypeSize(Type type){
    //printf("Test: getTypeSize \n");
    if(type->kind==BASIC){
        //printf("Test: --> BASIC \n");
        if(type->u.basic==INT_TYPE) return 4;
        else if(type->u.basic==FLOAT_TYPE) return 8;
    }
    else if(type->kind==ARRAY){
        //printf("Test: --> ARRAY \n");
        return type->u.array.size*getTypeSize(type->u.array.elem);
    }
    else if(type->kind==STRUCTURE){
        //printf("Test: --> STRUCTURE \n");
        FieldList fl=type->u.structure;
        int size=0;
        while(fl!=NULL) {
            size+=getTypeSize(fl->type);
            fl=fl->tail;
        }
        return size;
    }
}

void optimizeIR(){
    getBlock();
    InterCodes ic=codeHead->next;
    while(ic!=codeHead){
        switch(ic->code->kind){
            case LABEL:    //消除连续的LABEL
                if(ic->next->code->kind==LABEL) {
                    replaceOperand(ic->next->code->u.oneOp.op,ic->code->u.oneOp.op);
                    removeInterCode(ic->next);
                }
                else {
                    ic=ic->next;
                } 
                break;

            case ASSIGN:
                ic=ic->next;
                break;

            case ADD:
            case SUB:
            case MUL:
            case DIV_IC:
                ic=ic->next;
                break;


            default:
                ic=ic->next;
                break;
        }
    }

}


void getBlock()
{
	for(InterCodes code = codeHead ->next;code != codeHead ;code = code ->next)
    	code->block =0;

	int i = 2;
	
	for(InterCodes ic = codeHead->next;ic!= codeHead ;ic = ic ->next)
	{
		if(ic == codeHead->next)
			ic->block = 1;
		else if(ic ->code ->kind == LABEL)
		{
			ic->block =i;
			i ++;
		}
		else if(ic ->code->kind ==GOTO || ic->code->kind ==IFGOTO)
		{
            ic->next->block =i; 
            i++; 
		}
	}
}


int change(InterCodes code,int i , Operand Op)
{
	int num = code->code->kind;
	if(num>= 0&& num<= 7)
	{
		if(code->code->u.oneOp.op->kind == 4 && code->code->u.oneOp.op->u.val_temp ==i)
		{
			code->code->u.oneOp.op = Op;
		}
	}
	else if(num >= 8 && num <= 9)
	{
		if(code->code->u.assignOp.right->kind == 4 && code->code->u.assignOp.right->u.val_temp ==i)
			code->code->u.assignOp.right = Op;
		
		if(code->code->u.assignOp.left->kind == 4 && code->code->u.assignOp.left->u.val_temp ==i)
			return -1;
		
	}
	else if(num>= 10 && num<= 13)
	{
		if(code->code->u.binOp.op1->kind == 4 && code->code->u.binOp.op1->u.val_temp ==i)
			code->code->u.binOp.op1 = Op;
	
		if(code->code->u.binOp.op2->kind == 4 && code->code->u.binOp.op2->u.val_temp ==i)
			code->code->u.binOp.op2 = Op;
		
		
		if(code->code->u.binOp.result->kind == 4 && code->code->u.binOp.result->u.val_temp ==i)
			return -1;
	}
	else if(num == 14 )
	{
		
		if(code->code->u.ifGotoOp.op1->kind == 4 && code->code->u.ifGotoOp.op1->u.val_temp ==i)
			code->code->u.ifGotoOp.op1 = Op;
		
		if(code->code->u.ifGotoOp.op2->kind == 4 && code->code->u.ifGotoOp.op2->u.val_temp ==i)
			code->code->u.ifGotoOp.op2 = Op;
	}
	else
	{
		if(code->code->u.decOp.op->kind == 4 && code->code->u.decOp.op->u.val_temp ==i)
			return -1;
	}
	return 1;
}

void Opt()
{
	InterCodes code = codeHead ->next;
	for(;code != codeHead;)
	{
		if(code ->code->kind == 8)
		{
			if(code->code->u.assignOp.left->kind ==4)
			{
				int i= code->code->u.assignOp.left->u.val_temp;
				Operand rOp = code->code->u.assignOp.right;
				
				InterCodes code1 = code->next;
				
				for(;code1!= codeHead&&code1 ->block ==0;code1 = code1->next)
				{
					int j;
					j = change(code1,i,rOp);
					if(j == -1)
						break;
				}
				code1= code;
				code = code->next;
				removeInterCode(code1);
			}
			else
				code = code->next;
		}
		else
			code = code->next;
	}
}

void Opt2()
{
	InterCodes code = codeHead->next;
	for(;code != codeHead;code = code->next)
	{
		if(code->code->kind == 14 && code->next->code->kind ==2)
		{
			int i = code->next->code->u.oneOp.op->u.val_label;
			removeInterCode(code->next);
			InterCodes code1 = code->next;
			for(;code1!= codeHead;code1 = code1->next)
			{
				if(code1->code->kind == 0 &&code1->code->u.oneOp.op->u.val_label ==i)
					break;
			}
			code1 = code1 ->prev;
			removeInterCode(code1->next);
			code1 = code1->next;
			InterCodes code2 = code1->next;
			for(;code2!= codeHead /*&&code2->block==0*/;code2 = code2->next);
			code2 = code2->prev;
			code1->prev->next = code2->next;
			code2->next->prev = code1->prev;
			code->next->prev = code2;
			code2->next = code->next;
			code->next = code1;
			code1->prev = code;
		}
	}
	printf("Optend\n");	
}

