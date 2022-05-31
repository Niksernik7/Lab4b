
#ifndef RBTREE_RBTREE_H
#define RBTREE_RBTREE_H


#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Item {
    int number;
    char *key;
    size_t data;
    struct Item* next;
    struct Item* prev;
} Item;

typedef struct List {
    Item *head;
} List;

enum color {
    black = 0,
    red = 1
};

typedef struct Node {
    //const char *key;
    //size_t data;
    struct List* list;
    enum color color;
    struct Node *right;
    struct Node *left;
    struct Node *parent;
} Node;

typedef struct Tree {
    Node *root;
    size_t size;
} Tree;


int Insert(Tree*, const char*, size_t);

bool DeleteByKeyAndNumber(Tree*, char*, int);
void DeleteNode(Tree*, Node*);

Item* Find(Tree*, const char*, int);
Node* FindNode(Node*, const char*);
Item* SpecialFind(Tree*, const char*, int);

void WalkTree(Node*, void (*cb)(Node*, void*), void*, bool, bool);

void RebalanceOnAdd(Tree *tree, Node *currentNode);
void RebalanceOnDelete(Tree *tree, Node *sibiling);
void SwapValues(Node*, Node*);
Node* GetBrother(Node*);
Node* GetGrandpa(Node*);
Node* GetUncle(Node*);
void RightRotate(Tree*, Node*);
void LeftRotate(Tree*, Node*);
Node* FindMax(Tree*);
Node* FindMin(Tree*);
double CallculateHeight(const char*, size_t);


char* get_str(const Item*);
void FreeTree(Tree*);

void GenerateGV(Node*, void*);
void PrintGV(Tree*, const char *);


#endif
