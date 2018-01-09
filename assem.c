#include "assem.h"

//将汇编代码序列输出到一个文件中
void printAssem(FILE *fp){
    fprintf(fp,".data\n");
    fprintf(fp,"_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp,"_ret: .asciiz \"\\n\"\n");
    fprintf(fp,".globl main\n");
    fprintf(fp,".text\n");
    //read()
    fprintf(fp,"\nread:\n");
    fprintf(fp," li $v0,4\n");
    fprintf(fp," la $a0,_prompt\n");
    fprintf(fp," syscall\n");
    fprintf(fp," li $v0,5\n");
    fprintf(fp," syscall\n");
    fprintf(fp," jr $ra\n");
    //write()
    fprintf(fp,"\nwrite:\n");
    fprintf(fp," li $v0,1\n");
    fprintf(fp," syscall\n");
    fprintf(fp," li $v0,4\n");
    fprintf(fp," la $a0,_ret\n");
    fprintf(fp," syscall\n");
    fprintf(fp," move $v0,$0\n");
    fprintf(fp," jr $ra\n");

    InterCodes ic=codeHead->next;
    if(ic==codeHead) {printf("IC==CODEHEAD\n");}
    while(ic!=codeHead){
        switch(ic->code->kind){
            case ASSIGN:{
                Operand left=ic->code->u.assignOp.left;
                Operand right=ic->code->u.assignOp.right;
                int x=getReg(fp,left);
                if(left->kind==VADDR_OP||left->kind==TADDR_OP){
                    int y=getReg(fp,right);
                    fprintf(fp," sw $%d,0($%d)\n",y,x);
                }
                else if(right->kind==CONST_OP){
                    fprintf(fp," li $%d, %d\n",x,ic->code->u.assignOp.right->u.val_const);
                }
                else if(right->kind==VADDR_OP|| right->kind==TADDR_OP){
                    int y=getReg(fp,right);
                    fprintf(fp," lw $%d,0($%d)\n",x,y);
                }
                else{
                    int y=getReg(fp,right);
                    fprintf(fp," move $%d,$%d\n",x,y);
                }
                break;
            }
            case LABEL:{
                Operand op=ic->code->u.oneOp.op;
                fprintf(fp,"label%d:\n",op->u.val_label);
                break;
            }
            case FUNCTION:{
                Operand op=ic->code->u.oneOp.op;
                fprintf(fp,"\n%s:\n",op->u.val_name);
                break;
            }
            case GOTO:{
                Operand op=ic->code->u.oneOp.op;
                fprintf(fp," j label%d\n",op->u.val_label);
                break;
            }
            case RETURN_IC:{
                int x=getReg(fp,ic->code->u.oneOp.op);
                fprintf(fp," move $v0,$%d\n",x);
                fprintf(fp," jr $ra");
                break;
            }
            case ARG:{
                
                break;
            }    
            case PARAM:{
                break;
            }
            case READ:{
                fprintf(fp," addi $sp,$sp,-4\n");
                fprintf(fp," sw $ra,0($sp)\n");
                fprintf(fp," jal read\n");
                fprintf(fp," lw $ra,0($sp)\n");
                fprintf(fp," addi $sp,$sp,4\n");
                break;
            }
            case WRITE:{
                break;
            }
            case CALL:{
                Operand op=ic->code->u.assignOp.right;
                int x=getReg(fp,ic->code->u.assignOp.left);
                fprintf(fp," jal %s\n",op->u.val_name);
                fprintf(fp," move $%d, $v0\n",x);
                break;
            }
            case ADD:{
                Operand op1=ic->code->u.binOp.op1;
                Operand op2=ic->code->u.binOp.op2;
                Operand result=ic->code->u.binOp.result;
                int x=getReg(fp,result);
                int y=getReg(fp,op1);
                
                if(op2->kind==CONST_OP){
                    fprintf(fp," addi $%d,$%d,%d\n",x,y,op2->u.val_const);
                }
                else{
                    int z=getReg(fp,op2);
                    fprintf(fp," add $%d,$%d,$%d\n",x,y,z);
                }
                break;
            }
            case SUB:{
                Operand op1=ic->code->u.binOp.op1;
                Operand op2=ic->code->u.binOp.op2;
                Operand result=ic->code->u.binOp.result;
                int x=getReg(fp,result);
                int y=getReg(fp,op1);
                
                if(op2->kind==CONST_OP){
                    fprintf(fp," addi $%d,$%d,-%d\n",x,y,op2->u.val_const);
                }
                else{
                    int z=getReg(fp,op2);
                    fprintf(fp," sub $%d,$%d,$%d\n",x,y,z);
                }
                break;
            }
            case MUL:{
                Operand op1=ic->code->u.binOp.op1;
                Operand op2=ic->code->u.binOp.op2;
                Operand result=ic->code->u.binOp.result;
                int x=getReg(fp,result);
                int y=getReg(fp,op1);
                int z=getReg(fp,op2);
                fprintf(fp," mul $%d,$%d,$%d\n",x,y,z);
                break;
            }
            case DIV_IC:{
                Operand op1=ic->code->u.binOp.op1;
                Operand op2=ic->code->u.binOp.op2;
                Operand result=ic->code->u.binOp.result;
                int x=getReg(fp,result);
                int y=getReg(fp,op1);
                int z=getReg(fp,op2);
                
                fprintf(fp," div $%d,$%d\n",y,z);
                fprintf(fp," mflo $%d\n",x);
                break;
            }
            case IFGOTO:{
                Operand op1=ic->code->u.ifGotoOp.op1;
                char *relop=ic->code->u.ifGotoOp.relop;
                Operand op2=ic->code->u.ifGotoOp.op2;
                Operand label=ic->code->u.ifGotoOp.label;
                int x=getReg(fp,op1);
                int y=getReg(fp,op2);
                if(strcmp(relop,"==")==0) fprintf(fp," beq");
                if(strcmp(relop,"!=")==0) fprintf(fp," bne");
                if(strcmp(relop,">")==0) fprintf(fp," bgt");
                if(strcmp(relop,"<")==0) fprintf(fp," blt");
                if(strcmp(relop,">=")==0) fprintf(fp," bge");
                if(strcmp(relop,"<=")==0) fprintf(fp," ble");
                fprintf(fp," $%d,$%d,label%d\n",x,y,label->u.val_label);
                break;
            }
            case DEC: {
                break; 
            } 
            default:
                fprintf(fp,"%s","default\n");
                break;
        }
        ic=ic->next;
    }
}

int getReg(FILE* fp,Operand op){
    return 24;
}