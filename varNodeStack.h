#include <stdbool.h>
#include <stdlib.h>

typedef struct var {
  char name[100];
  int val;
} var;

typedef struct stackNode {
  var var;
  struct stackNode *next;
  int lineNum;
  char *input1;
  char *input2;
  char outputs[30];
  struct stackNode *dependency1;
  struct stackNode *dependency2;
  char op;
  int dupeCnt;
  bool modified;
  struct stackNode **dupeExps;
  char expression[500];
  int latency;
} stackNode;

typedef struct stack {
  stackNode *head;
  stackNode *tail;
  int counter;
  int numSubExpressions;
  int addSubDupes;
  int multDivDupes;
  int expDupes;
} stack;

typedef struct blockNode {
  char expression[1000];
  struct blockNode *next;
  int latency;
  struct blockNode *dependency1;
  struct blockNode *dependency2;
} blockNode;

typedef struct block {
  int blockID;
  struct blockNode *head;
  struct blockNode *tail;
  int numLines;
  struct block *next;
} block;

typedef struct program {
  block *head;
  block *tail;
  int numBlocks;
} program;

// task 1
struct stackNode *createVar(char *name, int val);
struct stackNode *findVar(char *name, int val, stackNode *node, int rec,
                          char *varList);
void recordVar(int rec, char *varList, char *name);
struct stackNode *assignVar(char *name, int val, stackNode *head);
struct stackNode *push(char *name, int val, stack *s);
void pushNode(stackNode *node, stack *s);
struct stackNode *pop(stack *s);
void printStack(stack *s);
void task1Main(stack *s);

// Lab 3-1
blockNode *createBlockNode(stackNode *node);
block *createBlock();
void appendBlock(stackNode *node, block *block);
void printBlock(block *block);
int countBlocks(stack *s);
stackNode *newBlock(stack *s, program *p);
void createBranches(block *b, stackNode *n, program *p, int dst1, int dst2);
void fixNodeExpression(stackNode *node, block *b);
char *extract_between(const char *str, const char *p1, const char *p2);
char *strremove(char *str, const char *sub);
void str_replace(char *target, const char *needle, const char *replacement);
void pushBlock(program *p, block *b);
void printProgram(program *p);
program *Lab3Main(stack *s);

// Final Project
void finalMain(stack *s);
int nodeLatency(stackNode *n);
int stackLatency(stack *s);
void printNodeAttributes(stackNode *n);
void printStackAttributes(stack *s);
char **nodeInputs(stackNode *n);
void stackInputs(stack *s);
void findNodeOperation(stackNode *n);
void stackOperations(stack *s);
void checkDependent(stackNode *a, stackNode *b);
void stackDependencies(stack *s);
void heuristicAnalysis(stack *s);
stack *elimSubExp(stack *s, stack *newStack, stackNode *n, int *tmpCnt);
void recursiveSubExp(stack *s);
void printStackDuplicates(stack *s);
void stackDuplicates(stack *s);