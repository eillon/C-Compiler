CFILE =irisim.c intercode.c main.c symboltable.c treenode.c syntax.tab.c

parser:syntax.y lexical.l *.c *.h 
	bison -d -v syntax.y
	flex lexical.l
	gcc $(CFILE) -lfl -ly -o  parser -g

clean:
	rm -f lex.yy.c parser scanner syntax.tab.* *.output	
