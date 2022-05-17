%{

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "varNodeStack.h"

#define VERBOSE
int lineNum;
int yylex();



extern FILE *yyin;

struct stackNode *head;
struct stack *s;
struct program *p;

char usrVars[1000];

int *temp_counter;



void yyerror(char *ps, ...){
	printf("%s\n", ps);
}
	
%}

%code {

}

%union{
	char *text;
	int d;
	struct stackNode *nPointer;
}

%token <d> NUM
%token <text> TXT
%right '='
%token '(' ')'
%left '+' '-' '*' '/'
%right EXP
%right '!' '?'
%type <nPointer> expression
%right '\n'
%start infix

%%

infix : 
			infix expression '\n'
			|
			;

expression : 
						NUM {
							$$ = createVar("tmp",$1);
							sprintf($$->var.name,"%d",$1);
						}
						| '(' expression ')' {
							$$ = $2;
						}
						| TXT '=' expression {
							struct stackNode *node =  assignVar($1,$3->var.val,head);
							$$ = push($1,$3->var.val,s);
							// strcpy($3->inputs,$1);
							sprintf($$->expression,"=%s;\n",$3->var.name);
						};
						| TXT {
							struct stackNode *node = findVar((char*)$1,0,head,1,usrVars);
							$$ = createVar(node->var.name,node->var.val);
						}
						| expression '+' expression {
							int val = $1->var.val + $3->var.val;
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name, "tmp%d", *temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%s+%s;\n",$1->var.name,$3->var.name);
						}
						| expression '-' expression {
							int val = $1->var.val - $3->var.val;
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name,"tmp%d",*temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%s-%s;\n",$1->var.name,$3->var.name);
						}
						| expression '*' expression {
							int val = $1->var.val * $3->var.val;
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name,"tmp%d", *temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%s*%s;\n",$1->var.name,$3->var.name);
						}
						| expression '/' expression {
							int val = $1->var.val / $3->var.val;
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name,"tmp%d",*temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%s/%s;\n",$1->var.name,$3->var.name);
						}
						| expression EXP expression {
							int val = pow($1->var.val,$3->var.val);
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name,"tmp%d",*temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%s**%s;\n",$1->var.name,$3->var.name);
						}
						| '!' expression {
							int val;
							if($2->var.val==0){
								val = 1;
							}
							else{
								val = 0;
							}
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name,"tmp%d",*temp_counter);
							*temp_counter = *temp_counter + 1;
							sprintf($$->expression,"=%d;\n",val);
						}
						| expression '?' expression {
							int val = ($1->var.val == 0) ? 0 : $3->var.val;
							char exp[1000];
							sprintf(exp,"if(%s){\n",$1->var.name);

							while(s->tail != $1){
								stackNode *node = pop(s);
								char exp2[1100];
								sprintf(exp2,"\t%s%s",node->var.name,node->expression);
								strcat(exp,exp2);
							}
							char name[4];
							sprintf(name,"tmp%d",*temp_counter);
							$$ = push(name,val,s);
							// sprintf($$->var.name, "tmp%d", *temp_counter);
							*temp_counter = *temp_counter + 1;

							char expEnd[1000];
							sprintf(expEnd,"\t%s=%s;\n} else {\n\t%s=0;\n}\n", $$->var.name,$3->var.name, $$->var.name);
							strcat(exp,expEnd);
							sprintf($$->expression, "%s", exp);
						}
						;
%%

int main(){
	temp_counter = (int*)malloc(sizeof(int));
	*temp_counter = 1;
	head = (struct stackNode*)malloc(sizeof(struct stackNode));
	s = (struct stack*)malloc(sizeof(struct stack));
	s->head = NULL;
	s->tail = NULL;
	s->counter = 0;
	printf("initialized stack & head\n");
		yyin = fopen("file.txt","r");
		if(yyin == NULL){
			printf("File open error");
			return 1;
		}
	yyparse();
	fclose(yyin);
	/* task1Main(s); */

	/* *temp_counter = 1;
	s->head = NULL;
	s->tail = NULL;
	s->counter = 0;
	/* printf("initialized stack & head\n"); */
		/* yyin = fopen("file.txt","r");
		if(yyin == NULL){
			printf("File open error");
			return 1;
		}
	yyparse();
	fclose(yyin); */
	/* printf("\ntask2\n");
	task2Main(s); */

	/* printf("\ntask3\n"); */
	/* task3Main(usrVars,s); */

	/* program *p = (program*)malloc(sizeof(program)); */
	/* p = Lab3Main(s); */
	/* Lab32Main(s); */
	/* p = Lab3Main(s); */
	finalMain(s);


	
	return 0;
}