#include "intercode.h"

int tempNum=0;
int labelNum=0;

void insertOperand(Operands head,Operand op){   //自带头节点
    Operands tmp=head;
    while(tmp->next!=NULL) tmp=tmp->next;
    tmp->next=malloc(sizeof(struct Operands_));
    tmp->next->op=op;
    tmp->next->next=NULL;
}

void initInterCode(){
    codeHead=malloc(sizeof(struct InterCodes_));  //附加头结点
    codeHead->next=codeHead;
    codeHead->prev=codeHead;
    codeHead->code=NULL;
}

void insertInterCode(InterCodes ic){
        ic->next=codeHead;
        ic->prev=codeHead->prev;
        codeHead->prev->next=ic;
        codeHead->prev=ic;
}

void removeInterCode(InterCodes ic){
    if(ic==codeHead) return;

    ic->prev->next=ic->next;
    ic->next->prev=ic->prev;
    free(ic);
}

Operand newConst(int value){
    Operand op=malloc(sizeof(struct Operand_));
    op->kind=CONST_OP;
    op->u.val_const=value;
    op->isAddr=false;
    return op;
}

Operand newVar(char *value){
    Operand op=malloc(sizeof(struct Operand_));
    op->kind=VAR_OP;
    op->u.val_name=value;
    op->isAddr=false;
    return op;
}

Operand newTemp(){
    Operand op=malloc(sizeof(struct Operand_));
    op->kind=TEMP_OP;
    op->u.val_temp=tempNum;
    op->isAddr=false;
    tempNum++;
    return op;
}

Operand newLabel(){
    Operand op=malloc(sizeof(struct Operand_));
    op->kind=LABEL_OP;
    op->u.val_label=labelNum;
    op->isAddr=false;
    labelNum++;
    return op;
}

Operand chAddr(Operand op){
    Operand t=malloc(sizeof(struct Operand_));
    if(op->kind==TEMP_OP){
        t->kind=TADDR_OP;
        t->u.val_temp=op->u.val_temp;
    }
    if(op->kind==VAR_OP){
        t->kind=VADDR_OP;
        t->u.val_name=op->u.val_name;
    }
    t->isAddr=true;
    return t;
}

Operand chRefe(Operand op){
    Operand t=malloc(sizeof(struct Operand_));
    if(op->kind==TEMP_OP){
        t->kind=TREFE_OP;
        t->u.val_temp=op->u.val_temp;
    }
    if(op->kind==VAR_OP){
        t->kind=VREFE_OP;
        t->u.val_name=op->u.val_name;
    }
    t->isAddr=true;
    return t;
}

void genLabel(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=LABEL;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genFunction(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=FUNCTION;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
} 

void genGoto(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=GOTO;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genReturn(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=RETURN_IC;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genArg(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=ARG;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genParam(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=PARAM;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genRead(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=READ;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genWrite(Operand op){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=WRITE;
    ic->code->u.oneOp.op=op;
    insertInterCode(ic);
}

void genAssign(Operand left,Operand right){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=ASSIGN;
    left->isAddr=right->isAddr;
    ic->code->u.assignOp.left=left;
    ic->code->u.assignOp.right=right;
    
    insertInterCode(ic);
}

void genCall(Operand left,Operand right){          //函数不会返回结构体或数组类型的值。
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=CALL;
    ic->code->u.assignOp.left=left;
    ic->code->u.assignOp.right=right;
    insertInterCode(ic);
}

void genAdd(Operand result,Operand op1,Operand op2){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=ADD;
    result->isAddr=op1->isAddr & op2->isAddr;
    ic->code->u.binOp.result=result;
    ic->code->u.binOp.op1=op1;
    ic->code->u.binOp.op2=op2;
    insertInterCode(ic);
}

void genSub(Operand result,Operand op1,Operand op2){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=SUB;
    result->isAddr=op1->isAddr & op2->isAddr;
    ic->code->u.binOp.result=result;
    ic->code->u.binOp.op1=op1;
    ic->code->u.binOp.op2=op2;
    insertInterCode(ic);
}

void genMul(Operand result, Operand op1, Operand op2){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=MUL;
    result->isAddr=op1->isAddr & op2->isAddr;
    ic->code->u.binOp.result=result;
    ic->code->u.binOp.op1=op1;
    ic->code->u.binOp.op2=op2;
    insertInterCode(ic);
}

void genDiv(Operand result,Operand op1, Operand op2){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=DIV_IC;
    result->isAddr=op1->isAddr & op2->isAddr;
    ic->code->u.binOp.result=result;
    ic->code->u.binOp.op1=op1;
    ic->code->u.binOp.op2=op2;
    insertInterCode(ic);
}

void genIfGoto(Operand op1,Operand op2, Operand label, char* relop){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=IFGOTO;
    ic->code->u.ifGotoOp.label=label;
    ic->code->u.ifGotoOp.op1=op1;
    ic->code->u.ifGotoOp.op2=op2;
    ic->code->u.ifGotoOp.relop=relop;
    insertInterCode(ic);
}

void genDec(Operand op, int size){
    InterCodes ic=malloc(sizeof(struct InterCodes_));
    ic->code=malloc(sizeof(struct InterCode_));
    ic->code->kind=DEC;
    ic->code->u.decOp.op=op;
    ic->code->u.decOp.size=size;
    insertInterCode(ic);
}

void printOperand(FILE* fp, Operand op){
    if(op==NULL) return;
    switch(op->kind){
        case VAR_OP:
            fprintf(fp," %s ",op->u.val_name);
            break;
        case TEMP_OP:
            fprintf(fp," t%d ",op->u.val_temp);
            break;
        case CONST_OP:
            fprintf(fp," #%d ",op->u.val_const);
            break;
        case LABEL_OP:
            fprintf(fp," label%d ",op->u.val_label);
            break;
        case VREFE_OP:
            fprintf(fp," &%s ",op->u.val_name);
            break;
        case TREFE_OP:
            fprintf(fp," &t%d ",op->u.val_temp);
            break;
        case VADDR_OP:
            fprintf(fp," *%s ",op->u.val_name);
            break;
        case TADDR_OP:
            fprintf(fp," *t%d ",op->u.val_temp);
            break;
        default:
            fprintf(fp,"\n Unknown Error in Operand Print \n");
    }
}

//将中间代码序列输出到一个文件中
void printInterCode(FILE *fp){
    InterCodes ic=codeHead->next;
    while(ic!=codeHead){
        switch(ic->code->kind){
            case ASSIGN:
                printOperand(fp,ic->code->u.assignOp.left);
                fprintf(fp," := ");
                printOperand(fp,ic->code->u.assignOp.right);
                fprintf(fp,"\n");
                break;
            case LABEL:
                fprintf(fp,"LABEL ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,":\n");
                break;
            case FUNCTION:
                fprintf(fp,"FUNCTION ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,":\n");
                break;
            case GOTO:
                fprintf(fp,"GOTO ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;
            case RETURN_IC:
                fprintf(fp,"RETURN ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;
            case ARG:
                fprintf(fp,"ARG ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;    
            case PARAM:
                fprintf(fp,"PARAM ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;
            case READ:
                fprintf(fp,"READ ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;
            case WRITE:
                fprintf(fp,"WRITE ");
                printOperand(fp,ic->code->u.oneOp.op);
                fprintf(fp,"\n");
                break;
            case CALL:
                printOperand(fp,ic->code->u.assignOp.left);
                fprintf(fp," := CALL ");
                printOperand(fp,ic->code->u.assignOp.right);
                fprintf(fp,"\n");
                break;
            case ADD:
                printOperand(fp,ic->code->u.binOp.result);
                fprintf(fp," := ");
                printOperand(fp,ic->code->u.binOp.op1);
                fprintf(fp," + ");
                printOperand(fp,ic->code->u.binOp.op2);
                fprintf(fp,"\n");
                break;
            case SUB:
                printOperand(fp,ic->code->u.binOp.result);
                fprintf(fp," := ");
                printOperand(fp,ic->code->u.binOp.op1);
                fprintf(fp," - ");
                printOperand(fp,ic->code->u.binOp.op2);
                fprintf(fp,"\n");
                break;
            case MUL:
                printOperand(fp,ic->code->u.binOp.result);
                fprintf(fp," := ");
                printOperand(fp,ic->code->u.binOp.op1);
                fprintf(fp," * ");
                printOperand(fp,ic->code->u.binOp.op2);
                fprintf(fp,"\n");
                break;
            case DIV_IC:
                printOperand(fp,ic->code->u.binOp.result);
                fprintf(fp," := ");
                printOperand(fp,ic->code->u.binOp.op1);
                fprintf(fp," / ");
                printOperand(fp,ic->code->u.binOp.op2);
                fprintf(fp,"\n");
                break;
            case IFGOTO:
                fprintf(fp,"IF ");
                printOperand(fp,ic->code->u.ifGotoOp.op1);
                fprintf(fp," %s ",ic->code->u.ifGotoOp.relop);
                printOperand(fp,ic->code->u.ifGotoOp.op2);
                fprintf(fp," GOTO ");
                printOperand(fp,ic->code->u.ifGotoOp.label);
                fprintf(fp,"\n");
                break;
            case DEC: 
                fprintf(fp,"DEC ");
                printOperand(fp,ic->code->u.decOp.op);
                fprintf(fp," %d \n",ic->code->u.decOp.size);
                break;  
            default:
                fprintf(fp,"%s","default\n");
                break;
        }
        ic=ic->next;
    }
}

void replaceOperand(Operand src,Operand dst){
    InterCodes ic=codeHead->next;
    while(ic!=codeHead){
        switch(ic->code->kind){
            case LABEL:
            case FUNCTION:
            case GOTO:
            case RETURN_IC:
            case ARG:
            case PARAM:
            case READ:
            case WRITE:
                if(cmpOperand(ic->code->u.oneOp.op,src)){
                    *src=*dst;
                }
                break;
            case CALL:
            case ASSIGN:
                if(cmpOperand(ic->code->u.assignOp.left,src)){
                    *src=*dst;
                }
                if(cmpOperand(ic->code->u.assignOp.right,src)){
                    *src=*dst;
                }
                break;  
            case ADD:
            case SUB:
            case MUL:
            case DIV_IC:
                if(cmpOperand(ic->code->u.binOp.result,src)){
                    *src=*dst;
                }
                if(cmpOperand(ic->code->u.binOp.op1,src)){
                    *src=*dst;
                }
                if(cmpOperand(ic->code->u.binOp.op2,src)){
                    *src=*dst;
                }
                break;
            case IFGOTO:
                if(cmpOperand(ic->code->u.ifGotoOp.op1,src)){
                    *src=*dst;
                }
                if(cmpOperand(ic->code->u.ifGotoOp.op2,src)){
                    *src=*dst;
                }
                if(cmpOperand(ic->code->u.ifGotoOp.label,src)){
                    *src=*dst;
                }
                break;
            case DEC: 
                if(cmpOperand(ic->code->u.decOp.op,src)){
                    *src=*dst;
                }
                break;
            default:
                break;
        }
        ic=ic->next;
    }
}

bool cmpOperand(Operand op1,Operand op2){
    if(op1->kind!=op2->kind) return false;
    switch(op1->kind){
        case VAR_OP:
        case VADDR_OP:
        case VREFE_OP:
            if(strcmp(op1->u.val_name,op2->u.val_name)==0)
                return true;
            break;
        case CONST_OP:
            if(op1->u.val_const==op2->u.val_const)
                return true;
            break;
        case TADDR_OP:
        case TREFE_OP:
        case TEMP_OP:
            if(op1->u.val_temp==op2->u.val_temp)
                return true;
            break;
        case LABEL_OP:
            if(op1->u.val_label==op2->u.val_label)
                return true;
            break;
    }
    return false;
}