#include "object.h"


void initreg()
{
	int i = 0;
	for(;i< REG;i++)
	{
		regtable[i] = NULL;
	}
	varaddr = NULL;
	offset = 0;
}

void swreg()
{
	
}

void add_var( var_t  v)
{
	if(v == NULL)
	{
		printf("error: add NULL var to varaddr;\n");
	}
	else if(varaddr == NULL)
	{
		varaddr = v;
	}	
	else
	{
		var_t vc = varaddr;
		for(;vc ->next != NULL;vc = vc->next);
		vc->next = v;
	}
}


int findreg(Operand op) //find op in regtable 
{
	int i ;
	for(i =0;i<REG;i++)
	{
		Operands opc = regtable[i]->op;
		for(;opc!= NULL;opc = opc->next)
		{
			if(op == opc->op)
				return i;
		}
	
	}
	return -1;
}

// find free place in regtable
int findfreg()
{
	int i ;
	for(i = 0 ; i<REG;i++)
	{
		if(regtable[i] == NULL)
			return i;
	}
	return -1;
}

//choose one reg to replace
int choose() 
{
	int grade[2];
	grade[0]= -1;grade[1] = 100;
	int i = 0;
	for(;i< REG;i++)
	{
		if(regtable[i] == NULL)
		{
			printf("error: at choose\n");
			exit(1);
		}
		int gradei = 0;
		Operands opc = regtable[i]->op;
		for(;opc != NULL;opc = opc->next)
		{
			var_t var = findvar(opc->op);
			gradei++;
		}
		if(gradei < grade[1])
		{
			grade[0] = i;
			grade[1] = gradei;
		}
	}
	
	return grade[0];
}

var_t findvar(Operand op)
{
	var_t var = varaddr;
	for(;var != NULL;var = var ->next)
	{
		if(var->op == op)
			return var;
	}
	return NULL;
}

int getReg(FILE *fp, Operand op)
{
	var_t var = findvar(op);
	int flag = 0;
	if(var == NULL)
	{
		var =  malloc(sizeof(struct _var_t));
		var->op = op;
		var->reg = -1;
		
		offset = offset + 4;  
		var->offset = offset;

		var->next = NULL;
		add_var(var);
		flag = 1;
	}
	
	if(var ->reg == -1)
	{
		int reg = findfreg();
		if(reg != -1)
		{
			var->reg = reg;
			regtable[reg] = malloc(sizeof(struct _reg_t));
			regtable[reg]->op = malloc(sizeof(struct Operands_));
			regtable[reg]->op ->op = op;
			regtable[reg]->op ->next = NULL;
		}
		else
		{
			reg = choose();
			var->reg = reg;
			Operands ops = regtable[reg]->op;
			for(;ops!= NULL;ops = ops->next)
			{
				var_t var1 = findvar(ops->op);
				if(var1 == NULL)
				{
					printf("error about vartable!\n");
					exit(1);
				}
				var1->reg = -1;
				if(var1->offset == -1)
				{
					printf("error at var1->offset\n");
					exit(1);
				}
				else
				{
					fputs("subu $v1 ,$fp , ",fp);
					char temp[32];
					memset(temp,0,32);
					sprintf(temp,"%d",var1->offset);
					fputs(temp,fp);

					fputs("\nsw ",fp);
					memset(temp,0,32);
					sprintf(temp,"$%d",reg);
					fputs(temp,fp);
					fputs(", 0($v1)\n",fp);	
				}
			}
			
		}
		if(var->offset >= 0 && !flag)
		{
			fputs("subu $v1 ,$fp , ",fp);
			char temp1[32];
			memset(temp1,0,32);
			sprintf(temp1,"%d",var->offset);
			fputs(temp1,fp);

			fputs("\nlw ",fp);
			memset(temp1,0,32);
			sprintf(temp1,"$%d",reg);
			fputs(temp1,fp);
			fputs(", 0($v1)\n",fp);	
		}
	 
	}
	
	return var->reg + 8;
}




