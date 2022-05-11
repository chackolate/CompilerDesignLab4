%{
	#include <stdlib.h>
	#include <string.h>
	#include "infix.tab.h"
	// extern int yylval;

	// #define VERBOSE


	extern int lineNum;
	extern char *yytext;

%}


%%
[ \t]+ {}

[\n] {
	lineNum++;
	return '\n';
}

"=" {
	#ifdef VERBOSE
	printf("'=':%d\n", lineNum);
	#endif
	return '=';
}

"+" {
	#ifdef VERBOSE
	printf("'+':%d\n", lineNum);
	#endif
	return '+';
}

"-" {
	#ifdef VERBOSE
	printf("'-':%d\n", lineNum);
	#endif
	return '-';
}

"*" {
	#ifdef VERBOSE
	printf("'*':%d\n", lineNum);
	#endif
	return '*';
}

"/" {
	#ifdef VERBOSE
	printf("'/':%d\n", lineNum);
	#endif
	return '/';
}

"**" {
	#ifdef VERBOSE
	printf("'EXP':%d\n", lineNum);
	#endif
	return EXP;
}

"(" {
	#ifdef VERBOSE
	printf("'(':%d\n", lineNum);
	#endif
	return '(';
}

")" {
	#ifdef VERBOSE
	printf("')':%d\n", lineNum);
	#endif
	return ')';
}

"!" {
	#ifdef VERBOSE
	printf("'!':%d\n", lineNum);
	#endif
	return '!';
}

"?" {
	#ifdef VERBOSE
	printf("'?':%d\n",lineNum);
	#endif
	return '?';
}

[0-9]+ {
	#ifdef VERBOSE
	printf("%s:%d\n", yytext, lineNum);
	#endif
	yylval.d = atoi(yytext);
	return NUM;
}

[a-zA-Z][a-zA-z0-9]* {
	#ifdef VERBOSE
	printf("'%s':%d\n", yytext, lineNum);
	#endif
	yylval.text = strdup(yytext);
	return TXT;
}

%%

int yywrap(){
	return 1;
}

