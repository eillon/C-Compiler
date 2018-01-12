#ifndef _INTERCODE_H_
#define _INTERCODE_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#ifndef __BOOL__
#define __BOOL__
typedef enum {false,true} bool;
#endif

typedef struct Operand_* Operand;
typedef struct Operands_* Operands;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;

//单个操作符定义
struct Operand_{
	enum { VAR_OP,CONST_OP, VADDR_OP, TADDR_OP, TEMP_OP, LABEL_OP, VREFE_OP, TREFE_OP }kind;  
	union{
        int val_const;   //常数值
        int val_temp;
        int val_label;     
		char* val_name;	    //VAR

	}u;
    bool isAddr;
};

//用于实现 参数列表
struct Operands_{
    Operand op;
    Operands next;
};

//单条中间代码数据结构定义
struct InterCode_{
    enum{ 
        LABEL,FUNCTION,GOTO,RETURN_IC,ARG,PARAM,READ,WRITE,
        ASSIGN,CALL,
        ADD,SUB,MUL,DIV_IC,
        IFGOTO,
        DEC } kind;
    union{
        struct { Operand op; }oneOp;
        struct { Operand left,right; } assignOp;
        struct { Operand result,op1,op2; }binOp;
        struct { Operand op1,op2,label; char *relop;} ifGotoOp;
        struct { Operand op; int size;} decOp;
    }u;
};

struct InterCodes_{
    InterCode code;
    InterCodes prev,next;
	int block;
};

InterCodes codeHead;


void initInterCode();
void insertOperand(Operands head, Operand op);
void insertInterCode(InterCodes ic);
void removeInterCode(InterCodes ic);
void printOperand(FILE* fp,Operand op);
void printInterCode(FILE *fp);
void replaceOperand(Operand src,Operand dst);
bool cmpOperand(Operand op1,Operand op2);





Operand newConst(int value);
Operand newVar(char *value);
Operand chAddr(Operand op);
Operand chRefe(Operand op);
Operand newTemp();
Operand newLabel();

void genLabel(Operand op);
void genFunction(Operand op);
void genGoto(Operand op);
void genReturn(Operand op);
void genArg(Operand op);
void genParam(Operand op);
void genRead(Operand op);
void genWrite(Operand op);
void genAssign(Operand left,Operand right);
void genCall(Operand left,Operand right);
void genAdd(Operand result,Operand op1,Operand op2);
void genSub(Operand result,Operand op1,Operand op2);
void genMul(Operand result,Operand op1,Operand op2);
void genDiv(Operand result,Operand op1,Operand op2);
void genIfGoto(Operand op1,Operand op2, Operand label, char* relop);
void genDec(Operand op, int size);


void getBlock();
void Opt();
void Opt2();

#endif
