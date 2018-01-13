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

void swreg(FILE* fp)
{
	for(int i=0;i<REG;i++){
		if(regtable[i]!=NULL){
			while(regtable[i]!=NULL){
				var_t tmp=findvar(regtable[i]->var);
				tmp->reg=-1;
				fprintf(fp," sw $%d, -%d($fp)\n",i+8,tmp->offset);
				reg_t rtmp=regtable[i];
				regtable[i]=regtable[i]->next;
				free(rtmp);
			}
			regtable[i]=NULL;
		}
	}
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
		reg_t regi = regtable[i];
		for(; regi != NULL;regi = regi ->next)
		{
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

var_t findvar(char *name)
{
	var_t var = varaddr;
	for(;var != NULL;var = var ->next)
	{
		if(strcmp(var->name, name)== 0)
			return var;
	}
	return NULL;
}

int getReg(FILE *fp, Operand op)
{
	char *name = malloc(32);
	memset(name,0,32);
	if(op ->kind == VAR_OP|| op->kind== VADDR_OP || op->kind==VREFE_OP )
	{
		strcpy(name, op->u.val_name);
	}
	else if(op ->kind == TADDR_OP || op->kind==TEMP_OP || op->kind==TREFE_OP )
	{
		name[0] = 't';
		sprintf(name+1,"%d", op->u.val_temp);
	}

	var_t var = findvar(name);
	int flag = 0;
	if(var == NULL)
	{
		var =  malloc(sizeof(struct _var_t));
		var->name = name;
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
			regtable[reg]->var = name;
			regtable[reg]->next = NULL;
		}
		else
		{
			reg = choose();
			var->reg = reg;
			reg_t regi = regtable[reg];
			for(;regi!= NULL;regi = regi->next)
			{
				var_t var1 = findvar(regi->var);
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
					sprintf(temp,"$%d",reg+8);
					fputs(temp,fp);
					fputs(", 0($v1)\n",fp);	//fpƫ��������
				}
			}
			
			free(regtable[reg]);
			regtable[reg] = malloc(sizeof(struct _reg_t));
			regtable[reg]->var = name;
			regtable[reg]->next = NULL;
			
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
			sprintf(temp1,"$%d",reg+8);
			fputs(temp1,fp);
			fputs(", 0($v1)\n",fp);	//fpƫ��������
		}
	 
	}
	
	if(op->kind==TREFE_OP||op->kind==VREFE_OP){
		fprintf(fp," subu $%d, $fp,%d\n",var->reg+8,var->offset);
	}

	return var->reg + 8;
}




