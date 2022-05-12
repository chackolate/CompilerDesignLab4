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
  while (node) {
    if (strstr(node->expression, "if(") == NULL) {
      printf("%s%s", node->var.name, node->expression);
    } else {
      printf("%s", node->expression);
    }
    // printf("\n");
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
  while (nodeBuf) {
    printf("%s", nodeBuf->expression);
    nodeBuf = nodeBuf->next;
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

  // printf("*%s%s", ptr->var.name, ptr->expression);
  // s->head = ptr;
  // // printf("*%s%s", s->head->var.name, s->head->expression);
  // printStack(s);
  // ptr = newBlock(s, p);
  printProgram(p);
  // printStack(s);
  return p;
}
/////////////////////////////////////////////////////////////
////////////////Lab 3-2//////////////////////////////////////
void Lab32Main(stack *s) {
  stackNode *node = s->head;
  int latencyCtr = 0;
  while (node) {
    printf("\n[");
    // nodeLatency(node);
    // assignDependencies(node);
    printf("\nnode latency: %d\n", dependenceLatency(s, node));
    latencyCtr += dependenceLatency(s, node);
    node = node->next;
    printf("]\n");
  }
  latencyCtr = printf("Total latency: %d\n", latencyCtr);
  // stackLatency(s);
  // findDependencies(s);
}

// void Lab32Main(stack *s) {
//   // get user variables list
//   int numVars = 0;
//   char **varsBuf = (char **)malloc(20 * sizeof(char *));
//   varsBuf = findUsrVars(s, &numVars, varsBuf);
//   for (int i = 0; i < numVars; i++) {
//     printf("%s", varsBuf[i]);
//   }
//   // printf("vars: %s\n", varsBuf);
//   printf("\nnumber of vars: %d\n", numVars);

//   // split stack into 2d array of strings
//   char **instructions = (char **)malloc(20 * sizeof(char *));
//   int lineCnt;
//   divideStack(s, instructions, &lineCnt);
//   printf("%d lines of instructions\n", lineCnt);
//   for (int i = 0; i < lineCnt; i++) {
//     // printf("%s\n", instructions[i]);
//   }

//   // create variable nodes with info on their names and first/last lines
//   regNode *vars = malloc(numVars * sizeof(*vars));
//   fillNodes(vars, varsBuf, instructions, lineCnt, numVars);
//   int bound1, bound2;

//   // analyze liveness and create edge objects
//   for (int i = 0; i < numVars; i++) {
//   }

//   regNodeEdge **edges = malloc(numVars * sizeof(**edges));
//   int edgeCnts[numVars]; // edge counters for each variable
//   for (int i = 0; i < numVars; i++) {
//     edges[i] = (regNodeEdge *)malloc(10 * sizeof(regNodeEdge));
//     createEdgeArray(vars[i], vars, numVars, edges[i], &edgeCnts[i]);
//   }
//   for (int i = 0; i < numVars;
//        i++) { // top loop: # of edge arrays = # of variables
//     for (int j = 0; j < edgeCnts[i];
//          j++) { // bottom loop: each edge array has its own counter
//       // printf("(%s->%s) ", edges[i][j].src.name, edges[i][j].dst.name);
//     }
//     // printf("\n");
//   }
//   printf("analyzed edges\n");
//   graphNode **node = (graphNode **)malloc(
//       numVars * sizeof(graphNode *)); // array of graph nodes
//   createGraphNode(vars, &vars[0], numVars, edges[0], &edgeCnts[0]);

//   for (int i = 0; i < numVars;
//        i++) { // outer loop: go through every variable and create a graph
//        node
//     node[i] = createGraphNode(vars, &vars[i], numVars, edges[i],
//     &edgeCnts[i]);
//   }

//   for (int i = 0; i < numVars; i++) {
//     printGraphNode(node[i]);
//   }
//   printf("created graph nodes\n");

//   // create graph out of nodes
//   nodeGraph *graph = createGraph(node, numVars);
//   printGraph(graph, numVars);

//   instructionVec *instVec = (instructionVec *)malloc(sizeof(instructionVec));

//   stackNode *n = s->head;
//   while (n) {
//     findNodeOnGraph(graph, n, instructions, lineCnt, instVec);
//     n = n->next;
//   }
// }

int nodeLatency(stackNode *n) {
  int i = 0;
  int latency = 0;
  // printf("%s", n->expression);
  if ((strstr(n->expression, "+")) || (strstr(n->expression, "-"))) {
    latency += 1;
  }
  if ((strstr(n->expression, "=")) || (strstr(n->expression, "?"))) {
    latency += 2;
  }
  if ((strstr(n->expression, "*")) || (strstr(n->expression, "/"))) {
    latency += 4;
  }
  if (strstr(n->expression, "**")) {
    latency += 8;
  }
  printf("\n%s%s Latency = %d\n", n->var.name, n->expression, latency);
  return latency;
}

int dependenceLatency(stack *s, stackNode *n) {
  int latency = 0;
  stackNode *node = s->head;
  while (node) {
    if (strstr(n->expression, node->var.name)) {
      latency += nodeLatency(node);
    }
    node = node->next;
  }
  return latency;
}

char *nodeInputs(stackNode *n) {
  char *inputs = (char *)malloc(30 * sizeof(char));
  int i = 0; // expression iterator
  int j = 0; // output buffer iterator
  char *expBuf = (char *)malloc(sizeof(n->expression));
  strcpy(expBuf, n->expression);

  if (strstr(n->expression, "if")) {
    expBuf = strremove(expBuf, "if");
  }
  if (strstr(n->expression, "else")) {
    expBuf = strremove(expBuf, "else");
  }
  if (strstr(n->expression, "goto")) {
    expBuf = strremove(expBuf, "goto");
  }

  while (i < strlen(expBuf)) {
    if (expBuf[i] == 't') {
      for (int k = 0; k < 4; k++) {
        inputs[j] = expBuf[i];
        j++;
        i++;
      }
      inputs[j] = '/';
      j++;
    }
    if (isalpha(expBuf[i])) {
      inputs[j] = expBuf[i];
      j++;
      inputs[j] = '/';
      j++;
    }
    i++;
  }
  return inputs;
}

char *nodeOutputs(stackNode *n) {
  char *outputs = (char *)malloc(30 * sizeof(char));

  int i = 0;
  int j = 0;

  while (i < strlen(n->var.name)) {
    if (n->var.name[i] == 't') {
      for (int k = 0; k < 4; k++) {
        outputs[k] = n->var.name[i];
        i++;
        j++;
      }
      outputs[j] = '/';
      j++;
    }
    if (isalpha(n->var.name[i])) {
      outputs[j] = n->var.name[i];
      j++;
      outputs[j] = '/';
      j++;
    }
    i++;
  }
  return outputs;
}

void assignDependencies(stackNode *n) {
  char *outputs = (char *)malloc(30 * sizeof(char));
  char *inputs = (char *)malloc(30 * sizeof(char));

  char **inputsList = (char **)malloc(30 * sizeof(char *));

  inputs = nodeInputs(n);
  outputs = nodeOutputs(n);
  strcpy(n->inputs, inputs);
  strcpy(n->outputs, outputs);

  printf("Inputs: %s\n", inputs);
  printf("Outputs: %s\n", outputs);

  char *inputTok = (char *)malloc(30 * sizeof(char));
}

int stackLatency(stack *s) {
  stackNode *node = s->head;
  int latency = 0;
  while (node) {
    latency += nodeLatency(node);
    node = node->next;
  }
  printf("Total latency: %d\n", latency);
  return latency;
}

void findDependencies(stack *s) {
  stackNode *node = s->head;
  char *inputs = (char *)malloc(1000 * sizeof(char));
  char *outputs = (char *)malloc(1000 * sizeof(char));
  while (node) {
    strcat(inputs, nodeInputs(node));
    strcat(outputs, nodeOutputs(node));
    node = node->next;
  }
  printf("Inputs: %s\n", inputs);
  printf("Outputs: %s\n", outputs);
}

// void findDependencies(program *p) {
//   block *blockPtr = p->head;
//   blockNode *nodePtr = blockPtr->head;
//   while (blockPtr) {
//     while (nodePtr) {
//       char *varsList = (char *)malloc(30 * sizeof(char));
//       char *expBuf = (char *)malloc(1000 * sizeof(char));
//       char *outputsList = (char *)malloc(30 * sizeof(char));
//       char *inputsList = (char *)malloc(30 * sizeof(char));
//       // printf("made buffers\n");
//       strcpy(expBuf, nodePtr->expression);
//       printf("%s", expBuf);
//       int i = 0;
//       int j = 0;
//       int k = 0;
//       // printf("%c", expBuf[i]);
//       if (strstr(expBuf, "if")) {
//         expBuf = strremove(expBuf, "if");
//       }
//       if (strstr(expBuf, "else")) {
//         expBuf = strremove(expBuf, "else");
//       }
//       if (strstr(expBuf, "goto")) {
//         expBuf = strremove(expBuf, "goto");
//       }
//       if (strstr(expBuf, "BB")) {
//         expBuf = strremove(expBuf, "BB");
//       }
//       inputsList = strtok(expBuf, ";");
//       outputsList = strtok(expBuf, "=");
//       printf("iterations: %d\n", i);
//       // printf("%s\n", varsList);
//       // printf("%s", expBuf);
//       printf("Input %s\n", inputsList);
//       printf("Output %s\n", outputsList);
//       nodePtr = nodePtr->next;
//     }
//     blockPtr = blockPtr->next;
//   }
// }

void printBlockLatency(block *inblock) {
  blockNode *nodeBuf = inblock->head;
  int subLatency = 0;
  int totalLatency = 0;
  printf("BB%d:\n", inblock->blockID);
  while (nodeBuf) {
    printf("%s", nodeBuf->expression);

    if ((strstr(nodeBuf->expression, "+")) ||
        (strstr(nodeBuf->expression, "-"))) {
      subLatency += 1;
    }
    if ((strstr(nodeBuf->expression, "=")) ||
        (strstr(nodeBuf->expression, "?"))) {
      subLatency += 2;
    }
    if ((strstr(nodeBuf->expression, "*")) ||
        (strstr(nodeBuf->expression, "/"))) {
      subLatency += 4;
    }
    if ((strstr(nodeBuf->expression, "**"))) {
      subLatency += 8;
    }
    printf("Line latency: %d\n", subLatency);
    totalLatency += subLatency;
    subLatency = 0;
    nodeBuf = nodeBuf->next;
  }
  printf("Total block latency:%d\n", totalLatency);
}

/////////////////////////////////////////////////////////////

char **findUsrVars(stack *s, int *size, char **varList) {

  stackNode *node = s->head;
  int i = 0;
  while (node) {
    varList[i] = (char *)malloc(sizeof(char) * 20);
    char nameBuf[100];
    // if (strstr(node->var.name, "tmp") == NULL) {
    sprintf(nameBuf, "%s", node->var.name);
    strcat(varList[i], nameBuf);
    // }
    i++;
    node = node->next;
  }
  *size = i;
  return varList;
}

char *readStack(stack *s) {
  char *stackAttributes = malloc(sizeof(char) * 500);
  stackNode *node = s->head;
  while (node) {
    char lineBuf[100];
    if (strstr(node->expression, "if(") == NULL) {
      sprintf(lineBuf, "%s%s", node->var.name, node->expression);
    } else {
      sprintf(lineBuf, "%s", node->expression);
    }
    strcat(stackAttributes, lineBuf);
    // sprintf(lineBuf, "\n");
    // strcat(stackAttributes, lineBuf);
    node = node->next;
  }
  return stackAttributes;
}

void divideStack(stack *s, char **instructions,
                 int *size) { // return 2D array of instructions
  char *stackRead = readStack(s);
  // printf("%s\n", stackRead);

  char *stackTok = strtok(stackRead, "\n");

  int i = 0;
  while (stackTok != NULL) {
    instructions[i] = malloc(20 * sizeof(char));
    instructions[i] = stackTok;
    // strcat(instructions[i], "\n");
    i++;
    stackTok = strtok(NULL, "\n");
  }
  *size = i;
}

int findFirstLine(char **instructions, int lineCnt, char *usrVar) {
  for (int i = 0; i < lineCnt; i++) {
    if (strstr(instructions[i], usrVar)) {
      // printf("%s first line %d\n", usrVar, i);
      return i;
    }
  }
}

int findLastLine(char **instructions, int lineCnt, char *usrVar) {
  int lastLine = 0;
  for (int i = 0; i < lineCnt; i++) {
    if (strstr(instructions[i], usrVar)) {
      if (i >= lastLine) {
        lastLine = i;
      }
    }
  }
  // printf("%s last line %d\n", usrVar, lastLine);
  return lastLine;
}

void fillNodes(regNode *vars, char **varsBuf, char **instructions, int lineCnt,
               int numVars) {
  for (int i = 0; i < numVars; i++) {
    strcpy(vars[i].name, varsBuf[i]);
    vars[i].firstPos = findFirstLine(instructions, lineCnt, varsBuf[i]);
    vars[i].lastPos = findLastLine(instructions, lineCnt, varsBuf[i]);
    // printf("%s : %d->%d\n", vars[i].name, vars[i].firstPos, vars[i].lastPos);
  }
}

bool checkLive(regNode var1, regNode var2, int *bound1, int *bound2) {
  bool live = false;
  int firstLine, lastLine = 0;

  if ((var1.firstPos >= var2.firstPos) && (var1.firstPos < var2.lastPos)) {
    // var1 starts after/same line as var2 and before var2 ends
    live = true;
    firstLine = var1.firstPos;
    if (var1.lastPos > var2.lastPos) { // var1 ends after var2
      lastLine = var2.lastPos;
    } else if (var2.lastPos >= var1.lastPos) { // var2 ends after var1
      lastLine = var1.lastPos;
    }

  } else if ((var1.firstPos <= var2.firstPos) &&
             (var1.lastPos > var2.firstPos)) {
    // var1 starts before/same line as var2 and ends after var2 starts
    live = true;
    firstLine = var2.firstPos;
    if (var1.lastPos > var2.lastPos) {
      lastLine = var2.lastPos;
    } else if (var2.lastPos >= var1.lastPos) {
      lastLine = var1.lastPos;
    }
  }

  if (live) {
    // printf("%s and %s live from %d to %d\n", var1.name, var2.name, firstLine,
    //  lastLine);
  }
  *bound1 = firstLine;
  *bound2 = lastLine;

  return live;
}

// return edge objects for one variable
void createEdgeArray(regNode var, regNode *vars, int numVars,
                     regNodeEdge *edges, int *numEdges) {
  int counter = 0;
  for (int i = 0; i < numVars; i++) {
    int bound1, bound2;
    if (strcmp(var.name, vars[i].name)) {
      if (checkLive(var, vars[i], &bound1, &bound2)) {
        edges[counter].src = var;
        edges[counter].dst = vars[i];
        counter++;
      }
    }
  }
  *numEdges = counter;
}

graphNode *createGraphNode(regNode *vars, regNode *src, int numVars,
                           regNodeEdge *edges, int *numEdges) {
  createEdgeArray(*src, vars, numVars, edges, numEdges);
  graphNode *node = (graphNode *)malloc(sizeof(graphNode));
  node->src = (char *)malloc(10 * sizeof(char));
  strcpy(node->src, src->name);
  node->dsts = (char **)malloc(*numEdges * sizeof(char *));
  for (int i = 0; i < *numEdges; i++) {
    node->dsts[i] = (char *)malloc(10 * sizeof(char));
    strcpy(node->dsts[i], edges[i].dst.name);
  }
  node->edges = edges;
  node->numEdges = *numEdges;
}

void printGraphNode(graphNode *node) {
  for (int i = 0; i < node->numEdges; i++) {
    printf("(%s->%s)", node->src, node->dsts[i]);
  }
  printf("\n");
}

nodeGraph *createGraph(graphNode **nodes, int numNodes) {
  nodeGraph *graph = (nodeGraph *)malloc(sizeof(nodeGraph));
  graph->nodes = nodes;
  graph->numNodes = numNodes;
}

void printGraph(nodeGraph *graph,
                int numVars) { // print graph by looping through total #
                               // of edges and counts per edge group
  for (int i = 0; i < graph->numNodes; i++) {
    for (int j = 0; j < graph->nodes[i]->numEdges; j++) {
      printf("(%s->%s)", graph->nodes[i]->src, graph->nodes[i]->dsts[j]);
    }
    printf("\n");
  }
}

int findNodeOnGraph(nodeGraph *graph, stackNode *node, char **instructions,
                    int lineCnt, instructionVec *instVec) {
  char *nameBuf = (char *)malloc(30 * sizeof(char));
  strcpy(nameBuf, node->var.name);

  // assign inputs
  for (int i = 0; i < graph->numNodes; i++) {
    for (int j = 0; j < graph->nodes[i]->numEdges; j++) {
      if (!strcmp(graph->nodes[i]->src, nameBuf)) {
        graph->nodes[i]->src_ptr = node;
      }
    }
  }

  // assign outputs
  for (int i = 0; i < graph->numNodes; i++) {
    for (int j = 0; j < graph->nodes[i]->numEdges; j++) {
      if (!strcmp(graph->nodes[i]->dsts[j], node->expression)) {
        graph->nodes[i]->dst_ptr = node;
      }
    }
  }

  for (int i = 0; i < lineCnt; i++) {
    instruction *inst = (instruction *)malloc(sizeof(instruction));
    strcpy(inst->expression, instructions[i]);
    printf("%s\n", inst->expression);
  }
}

void scheduleStack(stack *s, nodeGraph *graph) {
  stackNode *n = s->head;
  int lineCnt = 1;
  while (n) {
    for (int i = 0; i < graph->numNodes; i++) {
      for (int j = 0; j < graph->numNodes; j++) {
        if (strstr(n->expression, graph->nodes[i]->dsts[j])) {
        }
      }
    }
  }
}
