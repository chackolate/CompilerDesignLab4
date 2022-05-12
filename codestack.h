#include <stdbool.h>
#include <stdlib.h>

typedef struct var {
  char name[100];
  int val;
} var;

typedef struct line {
  var *dst;
  var *src1;
  var *src2;
  struct line *next;
  int lineNum;
  char expression[500];
} line;

line *createVar(char *name, int val);
line *findVar(char *name, int val, line *node, int rec, char *varList);
void recordVar(int rec, char *varList, char *name);
line *assignVar(char *name, int val, line *head);
line *push(char *name, int val, stack *s);
line *pop(stack *s);
void printStack(stack *s);

typedef struct stack {
  line *head;
  line *tail;
  int counter;
} stack;

typedef struct blockNode {
  char text[1000];
  struct blockNode *next;
  int latency;
} blockNode;

typedef struct block {
  int blockNum;
  line *head;
  line *tail;
  int numLines;
  struct block *next;
} block;