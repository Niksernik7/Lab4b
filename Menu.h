#ifndef LAB4A_MENU_H
#define LAB4A_MENU_H

#include "RBTree.h"
int GetInt();
void FindInTree(Tree*, size_t);
void Menu();
Tree* Create();
void Delete(Tree*, int);
void AddNode(Tree*);

void GenerateGV(Node*, void*);
void PrintGV(Tree*);


#endif
