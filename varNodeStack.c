#include "varNodeStack.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stackNode *createVar(char *name, int val) { // create a new user variable
  struct stackNode *node = (struct stackNode *)malloc(sizeof(struct stackNode));
  sscanf(name, "%s", node->var.name);
  node->var.val = val;
  node->next = NULL;
  return node;
}

struct stackNode *findVar(char *name, int val, struct stackNode *node, int rec,
                          char *varList) {
  // if variable is to be newly assigned, add it to the
  // list of user vars.
  if (strcmp(name, node->var.name) == 0) {
    return node;
  } else if (node->next == NULL) {
    node->next = createVar(name, val);
    if (rec == 1) {
      strcat(varList, name);
      strcat(varList, " ");
    }
    return node->next;
  } else {
    return findVar(name, val, node->next, rec, varList);
  }
}

void recordVar(int rec, char *varList, char *name) {
  if (rec == 1) {
    strcat(varList, name);
    strcat(varList, " ");
  }
}

struct stackNode *assignVar(char *name, int val, stackNode *head) {
  char vars[100];
  stackNode *node = findVar(name, val, head, 1, vars);
  node->var.val = val;
  return node;
}

struct stackNode *push(char *name, int val, stack *s) {
  stackNode *node = createVar(name, val);
  strcpy(node->outputs, name);
  node->lineNum = s->counter;

  if (s->counter != 0) {
    s->tail->next = node;
  } else {
    s->head = node;
  }
  s->tail = node;
  s->counter++;
  return s->tail;
}

void pushNode(stackNode *node, stack *s) {
  if (s->counter != 0) {
    s->tail->next = node;
  } else {
    s->head = node;
  }
  s->tail = node;
  s->counter++;
}

struct stackNode *pop(stack *s) {
  stackNode *node = s->tail;
  stackNode *node2 = s->head;
  while (node2->next != node) {
    node2 = node2->next;
  }
  s->tail = node2;
  s->tail->next = NULL;
  s->counter--;
  return node;
}

void printStack(stack *s) {
  stackNode *node = s->head;
  int counter = 1;
  while (node) {
    if (strstr(node->expression, "if(") == NULL) {
      printf("S%d: %s%s", node->lineNum, node->var.name, node->expression);
    } else {
      printf("S%d: %s", node->lineNum, node->expression);
    }
    // printf("\n");
    counter++;
    node = node->next;
  }
}

void task1Main(stack *s) { printStack(s); }

blockNode *createBlockNode(stackNode *node) {
  blockNode *line = (blockNode *)malloc(sizeof(blockNode));
  char expBuf[1000];
  if (strstr(node->expression, "if(") == NULL) {
    sprintf(expBuf, "\t%s%s", node->var.name, node->expression);
  } else {
    sprintf(expBuf, "\t%s", node->expression);
  }
  strcpy(line->expression, expBuf);
  line->next = NULL;
  // printf("Created node\n");
  return line;
}

////////////////Lab 3-1//////////////////////////////////////
block *createBlock(int blockID) {
  block *myBlock = (block *)malloc(sizeof(block));
  if (myBlock) {
    myBlock->blockID = blockID;
    myBlock->head = NULL;
    myBlock->tail = NULL;
    myBlock->numLines = 0;
    myBlock->next = NULL;
    // printf("Created block %d with %d lines\n", myBlock->blockID,
    //  myBlock->numLines);
  }
  return myBlock;
}

void appendBlock(stackNode *node, block *block) {
  blockNode *nodeBuf = createBlockNode(node);
  if (block->numLines != 0) {
    block->tail->next = nodeBuf;
  } else {
    block->head = nodeBuf;
  }
  block->tail = nodeBuf;
  block->numLines++;
  // printf("Appended block\n");
}

void printBlock(block *inblock) {
  blockNode *nodeBuf = inblock->head;
  printf("BB%d:\n", inblock->blockID);
  int statement_counter = 0;
  while (nodeBuf) {
    printf("S%d:%s", statement_counter, nodeBuf->expression);
    nodeBuf = nodeBuf->next;
    statement_counter++;
  }
}

int countBlocks(stack *s) {
  stackNode *node = s->head;
  int blockCounts = 1;
  bool done = 0;
  while (node) {
    if (strstr(node->expression, "if(") == NULL) {

    } else {
      blockCounts += 2;
    }
    node = node->next;
  }
  blockCounts++;
  printf("# of blocks: %d\n", blockCounts);
}

stackNode *newBlock(stack *s, program *p) {
  stackNode *node = s->head;
  block *block1 = (block *)malloc(sizeof(block)); // currently focused block
  bool branch = 0;
  while (node && !branch) {
    if (strstr(node->expression, "if(") == NULL) {
      // printf("%s%s", node->var.name, node->expression);
      // printf("branch:%d\n", branch);
      // no IF found
      appendBlock(node, block1);
      branch = 0;
    } else {
      pushBlock(p, block1);
      createBranches(block1, node, p, block1->blockID + 3, block1->blockID + 3);
      fixNodeExpression(node, block1);
      appendBlock(node, block1);
      branch = 1;
      // printf("branch:%d\n", branch);
    }
    node = node->next;
  }
  // printProgram(p);
  if (branch == 0) {
    pushBlock(p, block1);
    // printf("pushed\n");
  }
  s->head = node;
  return node;
  // block *block2 = (block *)malloc(sizeof(block));
  // appendBlock(node, block2);
  // pushBlock(p, block2);
}

void createBranches(block *b, stackNode *n, program *p, int dst1, int dst2) {
  block *ifBlock = (block *)malloc(sizeof(block));
  b->next = ifBlock;
  stackNode *ifNode = (stackNode *)malloc(sizeof(stackNode));
  block *elseBlock = (block *)malloc(sizeof(block));
  ifBlock->next = elseBlock;
  stackNode *elseNode = (stackNode *)malloc(sizeof(stackNode));

  char *ifBuf = (char *)malloc(100 * sizeof(char));
  char *elseBuf = (char *)malloc(100 * sizeof(char));

  ifBuf = extract_between(n->expression, "{", "}");
  char *goto1 = (char *)malloc(100 * sizeof(char));
  sprintf(goto1, "\tgoto BB%d;\n", dst1);
  strcat(ifBuf, goto1);
  strcpy(ifNode->expression, ifBuf);

  elseBuf = extract_between(n->expression, "else {", "}");
  char *goto2 = (char *)malloc(100 * sizeof(char));
  sprintf(goto2, "\tgoto BB%d;\n", dst2);
  strcat(elseBuf, goto2);
  strcpy(elseNode->expression, elseBuf);

  appendBlock(ifNode, ifBlock);
  appendBlock(elseNode, elseBlock);

  pushBlock(p, ifBlock);
  pushBlock(p, elseBlock);
}

void fixNodeExpression(stackNode *node, block *b) {
  char *goto1 = (char *)malloc(100 * sizeof(char));
  char *goto2 = (char *)malloc(100 * sizeof(char));
  sprintf(goto1, "\n\t\tgoto BB%d;\n", b->blockID + 1);
  sprintf(goto2, "\n\t\tgoto BB%d;\n", b->blockID + 2);
  // printf("%s%s", goto1, goto2);

  char *ifBuf = (char *)malloc(100 * sizeof(char));
  char *elseBuf = (char *)malloc(100 * sizeof(char));
  ifBuf = extract_between(node->expression, "{", "}");
  elseBuf = extract_between(node->expression, "else {", "}");

  str_replace(node->expression, ifBuf, goto1);
  str_replace(node->expression, elseBuf, goto2);
  // printf("%s", node->expression);
}

char *extract_between(const char *str, const char *p1, const char *p2) {
  const char *i1 = strstr(str, p1);
  if (i1 != NULL) {
    const size_t pl1 = strlen(p1);
    const char *i2 = strstr(i1 + pl1, p2);
    if (p2 != NULL) {
      /* Found both markers, extract text. */
      const size_t mlen = i2 - (i1 + pl1);
      char *ret = malloc(mlen + 1);
      if (ret != NULL) {
        memcpy(ret, i1 + pl1, mlen);
        ret[mlen] = '\0';
        return ret;
      }
    }
  }
}

char *strremove(char *str, const char *sub) {
  char *strBuf = str;
  size_t len = strlen(sub);
  if (len > 0) {
    char *p = strBuf;
    while ((p = strstr(p, sub)) != NULL) {
      memmove(p, p + len, strlen(p + len) + 1);
    }
  }
  return strBuf;
}

void str_replace(char *target, const char *needle, const char *replacement) {
  char buffer[1024] = {0};
  char *insert_point = &buffer[0];
  const char *tmp = target;
  size_t needle_len = strlen(needle);
  size_t repl_len = strlen(replacement);

  while (1) {
    const char *p = strstr(tmp, needle);

    // walked past last occurrence of needle; copy remaining part
    if (p == NULL) {
      strcpy(insert_point, tmp);
      break;
    }

    // copy part before needle
    memcpy(insert_point, tmp, p - tmp);
    insert_point += p - tmp;

    // copy replacement string
    memcpy(insert_point, replacement, repl_len);
    insert_point += repl_len;

    // adjust pointers, move on
    tmp = p + needle_len;
  }

  // write altered string back to target
  strcpy(target, buffer);
}

void pushBlock(program *p, block *b) {
  if (p->numBlocks != 0) {
    p->tail->next = b;
  } else {
    p->head = b;
  }
  p->tail = b;
  b->blockID = p->numBlocks;
  p->numBlocks++;
}

void printProgram(program *p) {
  block *myBlock = p->head;
  while (myBlock) {
    printBlock(myBlock);
    myBlock = myBlock->next;
  }
}

program *Lab3Main(stack *s) {
  int blockCount = 1;
  block *block1 = createBlock(blockCount);
  program *p = (program *)malloc(sizeof(program));
  stackNode *ptr = (stackNode *)malloc(sizeof(stackNode));
  while (s->head != NULL) {
    ptr = newBlock(s, p);
  }

  printProgram(p);
  // printStack(s);
  return p;
}

////////////////Final Project//////////////////////////////////////
void finalMain(stack *s) {
  printStack(s);
  stackInputs(s);
  printStackAttributes(s);
  stackDependencies(s);
  stackOperations(s);
  printf("-----------------------\n");
  // printStack(s);
  printf("latency: %d\n", stackLatency(s));
  // recursiveSubExp(s);
  // printf("latency: %d\n", stackLatency(s));
  // elimSubExp(s);
  stackDuplicates(s);
  printStack(s);
  // printf("new latency: %d\n", stackLatency(s));
  printStackDuplicates(s);
  // stackOperations(s);
  // heuristicAnalysis(s);
}

void heuristicAnalysis(stack *s) {
  stackNode *n = s->head;
  int consec = 0;
  char prevOp = ' ';
  while (n) {
    if (n->op != ' ') {
      if (n->op == prevOp) {
        consec += 1;
      }
    }
    prevOp = n->op;
    n = n->next;
  }
  printf("%d consecutive operations\n", consec);
}

char **nodeInputs(stackNode *n) {
  char *expBuf = (char *)malloc(500 * sizeof(char));
  char **inputs = (char **)malloc(4 * sizeof(char *));
  int arrPos = 0;
  int buffPtr = 0;
  strcpy(expBuf, n->expression);
  for (int i = 0; i < strlen(expBuf); i++) {
    if (expBuf[i] == 't') {
      inputs[arrPos] = (char *)malloc(4 * sizeof(char));
      for (int j = 0; j < 4; j++) {
        inputs[arrPos][j] = expBuf[i + j];
      }
      i += 3;
      arrPos += 1;
    } else if (isalpha(expBuf[i])) {
      inputs[arrPos] = (char *)malloc(sizeof(char));
      inputs[arrPos][buffPtr] = expBuf[i];
      arrPos += 1;
      buffPtr += 1;
    }
  }
  if (inputs[0]) {
    n->input1 = (char *)malloc(10 * sizeof(char));
    strcpy(n->input1, inputs[0]);
  }
  if (inputs[1]) {
    n->input2 = (char *)malloc(10 * sizeof(char));
    strcpy(n->input2, inputs[1]);
  }

  return inputs;
}

void stackInputs(stack *s) {
  stackNode *n = s->head;
  while (n) {
    nodeInputs(n);
    n = n->next;
  }
}

void findNodeOperation(stackNode *n) {
  char op = ' ';
  for (int i = 0; i < strlen(n->expression); i++) {
    switch (n->expression[i]) {
    case '+':
      op = '+';
      break;
    case '-':
      op = '-';
      break;
    case '*':
      if (n->expression[i + 1] == '*') {
        op = 'e';
        i += 1;
      } else {
        op = '*';
      }
      break;
    case '/':
      op = '/';
      break;
    case '?':
      op = '?';
      break;
    default:
      break;
    }
  }
  printf("%s:%c\n", n->expression, op);
  n->op = op;
}

void stackOperations(stack *s) {
  stackNode *n = s->head;
  while (n) {
    findNodeOperation(n);
    n = n->next;
  }
}

void checkDependent(stackNode *a, stackNode *b) {
  // printf("checking %d and %d\n", a->lineNum, b->lineNum);
  if (a->input1) {
    if (b->outputs) {
      if (!strcmp(a->input1, b->outputs)) {
        if (a->lineNum > b->lineNum) {
          a->dependency1 = b;
        }
      }
    }
  }
  if (a->input2) {
    if (b->outputs) {
      if (!strcmp(a->input2, b->outputs)) {
        if (a->lineNum > b->lineNum) {
          a->dependency2 = b;
        }
      }
    }
  }
}

void stackDependencies(stack *s) {
  stackNode *n = s->head;

  while (n) {
    stackNode *internal = s->head;
    for (int i = 0; i < s->counter; i++) {
      if (i != n->lineNum) {
        checkDependent(n, internal);
      }
      internal = internal->next;
    }
    if (n->dependency1) {
      printf("%d:%d--", n->lineNum, n->dependency1->lineNum);
      printf("%s:%s\n", n->input1, n->dependency1->outputs);
    }
    if (n->dependency2) {
      printf("%d:%d--", n->lineNum, n->dependency2->lineNum);
      printf("%s:%s\n", n->input2, n->dependency2->outputs);
    }
    n = n->next;
  }
}

void printNodeAttributes(stackNode *n) {
  printf("Output: %s\n ", n->outputs);
  if (n->input1) {
    printf("Input 1: %s\n", n->input1);
  }
  if (n->input2) {
    printf("Input 2: %s\n", n->input2);
  }
  printf("-----------------\n");
}

void printStackAttributes(stack *s) {
  stackNode *n = s->head;
  while (n) {
    printNodeAttributes(n);
    n = n->next;
  }
}

void elimSubExp(stack *s) {
  int tmp_count = 0;
  stackNode *n = s->head;
  char **subexpressions = (char **)malloc(30 * sizeof(char *)); // 30 strings
  int expCount = 0;
  int addSubCnt = 0;
  int multDivCnt = 0;
  int expCnt = 0;
  while (n) {

    subexpressions[expCount] =
        (char *)malloc(500 * sizeof(char)); // 30 chars to a string
    strcpy(subexpressions[expCount], n->expression);
    expCount++;
    n = n->next;
  }
  // printf("%d expressions\n", expCount);
  for (int i = 0; i < expCount; i++) {
    // currently focused line
    for (int j = i; j < expCount; j++) {
      // loop through whole program count
      if (i == j) {

      } else if (!strcmp(subexpressions[i], subexpressions[j])) {
        printf("duplicate found line %d %s %s\n", j, subexpressions[i],
               subexpressions[j]);
        stackNode *src = s->head;
        for (int l = 0; l < i; l++) {
          src = src->next;
        }
        src->dupeCnt += 1;
        char expBuf[100] = "=";
        strcat(expBuf, src->var.name);

        stackNode *dst = s->head;
        for (int k = 0; k < j; k++) {
          dst = dst->next;
        }
        printf("operators: %c %c\n", src->op, dst->op);

        // char expBuf[100] = "=";
        // strcat(expBuf, src->var.name);
        // strcat(expBuf, "\n");
        // strcpy(dst->expression, expBuf);
        s->numSubExpressions += 1;
      }
    }
  }
  n = s->head;
  while (n) {
    if (n->op == '+' || n->op == '-') {
      if (n->dupeCnt >= 2) {
        stackNode *tmp = (stackNode *)malloc(sizeof(stackNode));
        char *nameBuf = (char *)malloc(30 * sizeof(char));
        sprintf(nameBuf, "tmp_cse_%d", tmp_count);
        char *expBuf = (char *)malloc(500 * sizeof(char));
        strcpy(expBuf, n->expression);
      }
    } else if (n->op == '*' || n->op == '/') {

    } else if (n->op == 'e') {
    }
    n = n->next;
  }
  for (int i = 0; i < s->counter; i++) {
    stackNode *tmp = (stackNode *)malloc(sizeof(stackNode));
    char nameBuf[10];
    sprintf(nameBuf, "tmpCSE%d", tmp_count);
    tmp_count++;
    strcpy(tmp->var.name, nameBuf);
  }
}

void stackDuplicates(stack *s) {
  stackNode *tmp1 = s->head;
  stackNode *tmp2 = s->head;
  while (tmp1) {
    tmp1->dupeCnt = 0;
    tmp2 = s->head;
    tmp1->dupeExps = (stackNode **)malloc(30 * sizeof(stackNode *));
    while (tmp2) {
      if (tmp1->lineNum == tmp2->lineNum || tmp1->lineNum > tmp2->lineNum) {

      } else {
        if (!strcmp(tmp1->expression, tmp2->expression)) {
          tmp1->dupeExps[tmp1->dupeCnt] =
              (stackNode *)malloc(sizeof(stackNode));
          tmp1->dupeExps[tmp1->dupeCnt] = tmp2;
          tmp1->dupeCnt += 1;
          printf("duplicate found: %s %s\n", tmp1->expression,
                 tmp2->expression);
        }
      }
      tmp2 = tmp2->next;
    }
    tmp1 = tmp1->next;
  }
}

void printStackDuplicates(stack *s) {
  stackNode *n = s->head;
  while (n) {
    printf("%d:%d duplicates\n", n->lineNum, n->dupeCnt);
    for (int i = 0; i < n->dupeCnt; i++) {
      printf("%d:%s\n", n->dupeExps[i]->lineNum, n->dupeExps[i]->expression);
    }
    switch (n->op) {
    case '+':
      s->addSubDupes += 1;
      break;
    case '-':
      s->addSubDupes += 1;
      break;
    case '*':
      s->multDivDupes += 1;
      break;
    case '/':
      s->multDivDupes += 1;
      break;
    case 'e':
      s->expDupes += 1;
      break;
    default:
      break;
    }
    printf("----------------\n");
    n = n->next;
  }
  printf("Add/Sub dupes: %d\n", s->addSubDupes);
  printf("Mult/Div dupes: %d\n", s->multDivDupes);
  printf("Exp dupes: %d\n", s->expDupes);
}

void recursiveSubExp(stack *s) {
  int prevLatency = 0;
  int currLatency = stackLatency(s);
  while (prevLatency != currLatency) {
    prevLatency = currLatency;
    elimSubExp(s);
    currLatency = stackLatency(s);
    // printStack(s);
  }
}

int stackLatency(stack *s) {
  stackNode *n = s->head;
  int maxlatency = 0;
  int currentLatency = 0;
  while (n) {

    // currentLatency = nodeLatency(n);
    // if (currentLatency > maxlatency) {
    //   maxlatency = currentLatency;
    // }
    maxlatency += nodeLatency(n);
    n = n->next;
  }
  // latency
  return maxlatency;
}

int nodeLatency(stackNode *n) {
  int latency = 0;
  char expBuf[500];
  strcpy(expBuf, n->expression);
  for (int i = 0; i < strlen(expBuf); i++) {
    switch (expBuf[i]) {
    case '+':
      latency += 1;
      break;
    case '-':
      latency += 1;
      break;
    case '=':
      latency += 2;
      break;
    case '?':
      latency += 2;
      break;
    case '*':
      if (expBuf[i + 1] == '*') {
        latency += 8;
        i += 1;
      } else {
        latency += 4;
      }
      break;
    case '/':
      latency += 4;
      break;
    default:
      latency += 0;
    }
  }
  // if (n->dependency1) {
  //   latency += nodeLatency(n->dependency1);
  // }
  // if (n->dependency2) {
  //   latency += nodeLatency(n->dependency2);
  // }
  // printf("node %d latency: %d\n", n->lineNum, latency);
  n->latency = latency;
  return latency;
}
