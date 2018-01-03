#ifndef _ASSEM_H_
#define _ASSEM_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "intercode.h"

//extern InterCodes codeHead;

void printAssem(FILE *fp);
int getReg(Operand op);


#endif