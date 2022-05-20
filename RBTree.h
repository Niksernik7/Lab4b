
#ifndef RBTREE_RBTREE_H
#define RBTREE_RBTREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

enum color {
    black = 0,
    red = 1
};

typedef struct Node {
    const char *key;
    size_t data;
    enum color color;
    struct Node *right;
    struct Node *left;
    struct Node *parent;
} Node;

typedef struct Tree {
    Node *root;
    size_t size;
} Tree;

bool Insert(Tree *tree, const char *key, size_t data);
Node *InsertNode(Node *currentNode, Node *nodeToInsert);
bool DeleteByKey(Tree *tree, char *key);
Node *Find(Tree *, const char *);
Node *FindNode(Node *currentNode, const char *key);
void WalkTree(Node* node, void (*cb)(Node *node, void *arg), void *arg);

void ValidateNode(Tree *tree, Node *currentNode);
void ValidateAfterDelete(Tree *tree, Node *currentNode);
void SwapValues(Node *first, Node *second);
Node *GetGrandpa(Node *currentNode);
Node *GetUncle(Node *currentNode);
void RightRotate(Tree *tree, Node *temp);
void LeftRotate(Tree *tree, Node *temp);

char* get_str(const Node* item);
void FreeTree(Tree*);



Node* FindMinElemGreaterThen(Node* node, char* key, void* res);

#endif //RBTREE_RBTREE_H
