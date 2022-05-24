
#ifndef RBTREE_RBTREE_H
#define RBTREE_RBTREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//typedef struct Item {
//    int number;
//    const char *key;
//    size_t data;
//    struct Item* next;
//} Item;
//
//typedef struct List {
//    Item *head;
//    int Size;
//} List;

enum color {
    black = 0,
    red = 1
};

typedef struct Node {
    const char *key;
    size_t data;
    //struct List* list;
    enum color color;
    struct Node *right;
    struct Node *left;
    struct Node *parent;
} Node;

typedef struct Tree {
    Node *root;
    size_t size;
} Tree;


bool Insert(Tree*, const char*, size_t);
Node *InsertNode(Node*, Node*);
bool DeleteByKey(Tree*, char*);
void DeleteNode(Tree*, Node*);
Node *Find(Tree*, const char*);
Node *FindNode(Node*, const char*);
void WalkTree(Node*, void (*cb)(Node*, void*), void*, bool, bool);

void ValidateNode(Tree*, Node*);
void ValidateAfterDelete(Tree*, Node*);
void SwapValues(Node*, Node*);
Node *GetBrother(Node*);
Node *GetGrandpa(Node*);
Node *GetUncle(Node*);
void RightRotate(Tree*, Node*);
void LeftRotate(Tree*, Node*);

char* get_str(const Node*);
void FreeTree(Tree*);



Node* FindMinElemGreaterThen(Node*, char*, void*);

#endif
