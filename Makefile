infix: infix.tab.o lex.yy.o varNodeStack.o
	gcc lex.yy.o infix.tab.o varNodeStack.o -o infix -lm

lex.yy.o: infix.l
	flex infix.l; gcc -c lex.yy.c

infix.tab.o: infix.y
	bison -d -v infix.y; gcc -c infix.tab.c

varNodeStack.o: varNodeStack.c
	gcc -c varNodeStack.c

clean: 
	rm -f p2 infix.output *.o infix.tab.c lex.yy.c