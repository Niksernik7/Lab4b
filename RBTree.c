#include <assert.h>
#include <limits.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "RBTree.h"

static void DeleteNodeFixup(Tree* tree, Node* x);
static void DeleteNodeFixup_1(Tree* tree, Node* x);
static void DeleteNodeFixup_2(Tree* tree, Node* x);
static void DeleteNodeFixup_3l(Tree* tree, Node* x);
static void DeleteNodeFixup_3r(Tree* tree, Node* x);
static void DeleteNodeFixup_4(Tree* tree, Node* x);
static void SwapColors(Node* a, Node* b);

int Insert(Tree* tree, const char* key, size_t data){
    size_t len = strlen(key) + 1;
    if (tree == NULL)
        return -1;
    int i;
    Node* ptr = NULL;
    for (Node* ptrnext = tree->root; ptrnext; ) {
        ptr = ptrnext;
        i = strcmp(ptr->list->head->key, key);
        if (i < 0) {
            ptrnext = ptr->right;
            continue;
        }
        if (i > 0) {
            ptrnext = ptr->left;
            continue;
        }
        if (i == 0) {
            // элемент с таким ключом есть, добавляем в список
            Item* cur = ptr->list->head;
            int j = 1;
            do {
                j++;
                if (cur->next == NULL){
                    Item* newElem = calloc(1, sizeof(Item));
                    newElem->data = data;
                    newElem->key = calloc(len, 1);
                    strcpy(newElem->key, key);
                    newElem->number = j;
                    cur->next = newElem;
                    return newElem->number;
                }
                cur = cur->next;
            } while(1);
        }
    }
    Node* newNode = calloc(1, sizeof(Node));
    newNode->list = calloc(1, sizeof(List));
    newNode->list->head = calloc(1, sizeof(Item));
    newNode->list->head->key = calloc(len,1);
    strcpy(newNode->list->head->key, key);
    newNode->list->head->data = data;
    newNode->list->head->number = 1;
    newNode->parent = ptr;
    newNode->color = red;
    if (ptr == NULL) {
        // tree was empty
        tree->root = newNode;
        tree->root->color = black;
    }
    else
    {
        if (i < 0)
            ptr->right = newNode;
        else
            ptr->left = newNode;
    }
    tree->size++;
    RebalanceOnAdd(tree, newNode);
    return 1;
}




void CheckRotate(Node* node, void* arg){
    if (node->parent == NULL)
        return;
    assert(node->left != node->parent);
    assert(node->right != node->parent);
}

void RebalanceOnAdd(Tree *tree, Node *currentNode) {
    WalkTree(tree->root, CheckRotate, tree->root, 0, 0);
    if (!currentNode || !currentNode->parent) {
        return;
    }
    if (currentNode->parent->color == black) {
        return;
    } else if (currentNode->parent->color == red) {
        // Родитель и дядя одиннаково красные
        enum color uncleOriginalColor = black;
        currentNode->parent->color = black;
        if (GetUncle(currentNode)) {
            uncleOriginalColor = GetUncle(currentNode)->color;
            GetUncle(currentNode)->color = black;
        }
        if (GetGrandpa(currentNode)) {
            GetGrandpa(currentNode)->color = red;
        }
        if (uncleOriginalColor == black) {
            // Родитель красный, а дядя чёрный
            if (currentNode->parent == GetGrandpa(currentNode)->right) {
                // Поворачиваем вправо, если являемся левым потомком
                if (currentNode == currentNode->parent->left) {
                    RightRotate(tree, currentNode->parent);
                }
                // Дядя слева
                LeftRotate(tree, GetGrandpa(currentNode));
            } else {
                // Поворачиваем влево, если являемся правым потомком
                if (currentNode == currentNode->parent->right) {
                    LeftRotate(tree, currentNode->parent);
                }
                // Дядя справа
                RightRotate(tree, GetGrandpa(currentNode));
            }
        }
        tree->root->color = black;
    }
    RebalanceOnAdd(tree, GetGrandpa(currentNode)); // Ту же проверку с дедушки
}

Node *GetBrother(Node *currentNode) {
    if (!currentNode->parent) {
        return NULL;
    }
    if (currentNode->parent->left == currentNode) {
        return currentNode->parent->right;
    } else if(currentNode->parent->right == currentNode) {
        return currentNode->parent->left;
    }
    return NULL;
}

Node *GetGrandpa(Node *currentNode) {
    if (!currentNode->parent || !currentNode->parent->parent) {
        return NULL;
    }
    return currentNode->parent->parent;
}

Node *GetUncle(Node *currentNode) {
    Node *temp = GetGrandpa(currentNode);
    if (!temp) {
        return NULL;
    }
    if (currentNode->parent != temp->left) {
        return temp->left;
    }
    return temp->right;
}


void RightRotate(Tree *tree, Node *temp) {
    if(!temp) {
        return;
    }
    Node *left = temp->left;
    if(!left) {
        return;
    }
    temp->left = left->right;
    if (left->right)
        left->right->parent = temp;
    if (temp->parent) {
        if (temp == temp->parent->left) {
            temp->parent->left = left;
        } else {
            temp->parent->right = left;
        }
    } else {
        tree->root = left;
    }
    left->right = temp;
    left->parent = temp->parent;
    temp->parent = left;
}

void LeftRotate(Tree *tree, Node *temp) {
    if(!temp) {
        return;
    }
    Node *right = temp->right;
    if(!right) {
        return;
    }
    temp->right = right->left;
    if (right->left)
        right->left->parent = temp;
    if (temp->parent) {
        if (temp == temp->parent->left) {
            temp->parent->left = right;
        } else {
            temp->parent->right = right;
        }
    } else {
        tree->root = right;
    }
    right->left = temp;
    right->parent = temp->parent;
    temp->parent = right;
}

Node *FindNode(Node *currentNode, const char *key) {
    if (!key) {
        return NULL;
    }
    if (currentNode == NULL) {
        return NULL;
    }
    if (strcmp(currentNode->list->head->key, key) == 0) {
        return currentNode;
    }
    if (strcmp(key, currentNode->list->head->key) < 0) {
        currentNode = FindNode(currentNode->left, key);
    } else {
        currentNode = FindNode(currentNode->right, key);
    }

    return currentNode;
}

Item *Find(Tree *tree, const char *key, int numofel) {
    if (!key) {
        return NULL;
    }
    Node* node = FindNode(tree->root,key);
    if (node == NULL)
        return NULL;
    Item* res = node->list->head;
    int n = 1;
    while(n != numofel){
        if (res == NULL)
            return NULL;
        res = res->next;
        n++;
    }
    return res;
}

void RebalanceOnDelete2(Tree *tree, Node *x) {

}

void RebalanceOnDelete(Tree *tree, Node *sibiling) {
    if (!sibiling || !tree) {
        return;
    }

    if (sibiling->color == black) {
        if (!sibiling->left && !sibiling->right) {
            return;
        }

        if (sibiling->right && sibiling->right->color == red) {
            enum color tempColor = sibiling->parent->color;
            sibiling->parent->color = black;
            sibiling->color = tempColor;
            sibiling->right->color = black;

            LeftRotate(tree, sibiling->parent);
            return;
        } else if (sibiling->left && sibiling->left->color == red && sibiling->right->color == black) {
            sibiling->left->color = black;
            sibiling->right->color = red;
            RightRotate(tree, sibiling);

            RebalanceOnDelete(tree, sibiling->parent);
            return;
        }
        if (sibiling->right->color == black && sibiling->left->color == black) {

            enum color tempColor = sibiling->parent->color;

            sibiling->color = red;
            sibiling->parent->color = black;

            if (tempColor == red) {
                return;
            }

            RebalanceOnDelete(tree, sibiling->parent);

        }
    } else {
        sibiling->parent->color = red;
        sibiling->color = black;

        LeftRotate(tree, sibiling->parent);
    }
}

void DeleteNode(Tree *tree, Node *nodeToDelete) {
    Node* validationNode = GetBrother(nodeToDelete);

    if (nodeToDelete->left && nodeToDelete->right) {
        // Find max or min and replace
        Node *child = nodeToDelete->right;
        while (child->left) {
            child = child->left;
        }

        SwapValues(child, nodeToDelete);

        if(child->right) {
            DeleteNode(tree, child);
            return;
        }

        if (child != child->parent->right) {
            child->parent->left = NULL;
        } else {
            child->parent->right = NULL;
        }

        if (nodeToDelete->color != child->color) {
            free(child);
            goto balanced;
        }
        else {
            // both are black
            free(child);
        }
    } else if (nodeToDelete->left || nodeToDelete->right) {
        Node* child = nodeToDelete->left ? nodeToDelete->left : nodeToDelete->right;
        if(nodeToDelete->color != child->color) {
            child->color = black;
            if (nodeToDelete->parent) {
                if (nodeToDelete->parent->right == nodeToDelete)
                    nodeToDelete->parent->right = child;
                else
                    nodeToDelete->parent->left = child;
            }
            child->parent = nodeToDelete->parent;
            free(nodeToDelete);

            goto balanced;
        } else  {
            // both are black
            if (nodeToDelete->left) {
                SwapValues(nodeToDelete, nodeToDelete->left);
                nodeToDelete->left = NULL;
                free(nodeToDelete->left);
            } else {
                SwapValues(nodeToDelete, nodeToDelete->right);
                nodeToDelete->right = NULL;
                free(nodeToDelete->right);
            }
        }
    } else {
        if(nodeToDelete->parent) {
            if (nodeToDelete->parent->left == nodeToDelete) {
                nodeToDelete->parent->left = NULL;
            } else {
                nodeToDelete->parent->right = NULL;
            }
        }
        enum color c = nodeToDelete->color;
        if (c == red)
            goto balanced;
    }

    if (validationNode != NULL) {
        DeleteNodeFixup(tree, nodeToDelete);
        free(nodeToDelete);

    }

balanced:
    tree->size--;
    if(tree->size == 0) {
        tree->root = NULL;
    }
}

bool DeleteByKeyAndNumber(Tree *tree, char *key, int numofel) {

    Node *nodeToDelete = FindNode(tree->root, key);

    if (!nodeToDelete) {
        return false;
    }
    if (nodeToDelete->list->head->next != NULL) {
        Item *cur = nodeToDelete->list->head;
        for (int n = 1; n != numofel; n++) {
            cur = cur->next;
            if (cur == NULL)
                return false;
        }
        if (cur->next)
            cur->next->prev = cur->prev;
        if (cur->prev)
            cur->prev->next = cur->next;
        free(cur->key);
        free(cur);
        int a = 1;
        Item *newcur = nodeToDelete->list->head;
        while (newcur != NULL) {
            newcur->number = a;
            newcur = newcur->next;
        }
        return true;

    } else if (numofel != 1) {
        return false;
    } else {
        DeleteNode(tree, nodeToDelete);
    }
    return true;
}

void SwapValues(Node *first, Node *second) {
    Node temp;
    temp.list = first->list;
    first->list = second->list;
    second->list = temp.list;
}

void WalkTree(Node* node, void (*cb)(Node *node, void *arg), void *arg, bool reversed, bool childFirst){
    if (node == NULL)
        return;
    if (reversed) {
        if (childFirst) {
            WalkTree(node->right, cb, arg, reversed, childFirst);
            cb(node, arg);
            WalkTree(node->left, cb, arg, reversed, childFirst);
        } else {
            cb(node, arg);
            WalkTree(node->right, cb, arg, reversed, childFirst);
            WalkTree(node->left, cb, arg, reversed, childFirst);
        }
    } else {
        if (childFirst){
            WalkTree(node->left, cb, arg, reversed, childFirst);
            cb(node, arg);
            WalkTree(node->right, cb, arg, reversed, childFirst);
        } else {
            cb(node, arg);
            WalkTree(node->left, cb, arg, reversed, childFirst);
            WalkTree(node->right, cb, arg, reversed, childFirst);
        }
    }
}

void FreeNode(Node *node) {
    if (!node) {
        return;
    }

    FreeNode(node->left);
    FreeNode(node->right);

    free(node);
}

void FreeTree(Tree *tree) {
    FreeNode(tree->root);
    free(tree);
}


Item* SpecialFind(Tree* tree, const char* key, int numofel) {
    if (tree->root == NULL)
        return NULL;
    Node* MaxNode = FindMax(tree);
    Node* MinNode = FindMin(tree);
    size_t maxlen = strlen(MaxNode->list->head->key);
    size_t minlen = strlen(MinNode->list->head->key);
    size_t curlen = strlen(key);
    size_t len;
    if (maxlen > curlen)
        len = maxlen;
    else
        len = curlen;
    if (minlen > len)
        len = minlen;
    double maxweight = CallculateHeight(MaxNode->list->head->key, len);
    double minweight = CallculateHeight(MinNode->list->head->key, len);
    double xcur = CallculateHeight(key, len);
    double xmax = fabs(maxweight - xcur);
    double xmin = fabs(minweight - xcur);
    if (xmax < 0)
        xmax = -xmax;
    if (xmin < 0)
        xmin = -xmin;
    if (xmax > xmin) {
        int i = 1;
        Item* res = MaxNode->list->head;
        while(numofel != i){
            res = res->next;
            if (res == NULL)
                return NULL;
            i++;
        }
        return res;
    } else if (xmin > xmax) {
        int i = 1;
        Item* res = MinNode->list->head;
        while(numofel != i){
            res = res->next;
            if (res == NULL)
                return NULL;
            i++;
        }
        return res;
    } else {
        int i = 1;
        Item* res = MinNode->list->head;
        while(numofel != i){
            res = res->next;
            if (res == NULL){
                res = MaxNode->list->head;
                break;
            }
            i++;
        }
        while(numofel != i){
            res = res->next;
            if (res == NULL)
                return NULL;
            i++;
        }
        return res;
    }
}

Node* FindMin(Tree* tree) {
    Node* min = tree->root;
    while (min->left != NULL) {
        min = min->left;
    }
    return min;
}

Node* FindMax(Tree* tree) {
    Node* max = tree->root;
    while (max->right != NULL) {
        max = max->right;
    }
    return max;
}



double CallculateHeight(const char* s, size_t len) {
    size_t i = 0;
    double res = 0;
    for (; s[i] != 0; i++) {
        res *= 256;
        res += s[i];
    }
    res *= pow(256, (double)(len - i));
    return res;
}


void PrintGV(Tree* tree){
    fprintf(stderr, "generating GraphViz file...\n");
    FILE* f;
    char nfile[] = "TreeGV.XXXXXX";
    char gfile[L_tmpnam + 4] = "Graph.XXXXXX";
    char cmd[PATH_MAX + 10];

    int fd = mkstemp(nfile);
    mktemp(gfile);
    strcat(gfile, ".png");

    f = fdopen(fd, "w");
    if (f == NULL) {
        fprintf(stderr, "Could not create temporary file: %s\n", strerror(errno));
        return;
    }
    fprintf(f, "digraph Tree {\n");
    WalkTree(tree->root, GenerateGV, f, 0, 1);
    fprintf(f, "}\n");
    fclose(f);

    fprintf(stderr, "running GraphViz...\n");
    snprintf(cmd, sizeof(cmd),"dot -o%s -Tpng %s", gfile, nfile);
    system(cmd);

    fprintf(stderr, "showing picture...\n");
#ifdef WIN32
    snprintf(cmd, sizeof(cmd),",mspaint %s", gfile);
#else
    snprintf(cmd, sizeof(cmd),"xdg-open %s", gfile);
#endif
    system(cmd);

#ifndef DEBUG
#ifdef WIN32
    DeleteFile(nfile);
    DeleteFile(gfile);
#else
    unlink(nfile);
    //unlink(gfile);
#endif
#endif
}

void GenerateGV(Node* node, void* p){      //callback(cb)
    FILE* f = p;

    Item *item = node->list->head;
    fprintf(f, "\"%s: %zu\" [color = %s]\n", item->key, item->data,
            (node->color == red) ? "red" : "black");

    static size_t nullcount = 0;
    if (node->left != NULL){
        fprintf(f, "\"%s: %zu\" -> \"%s: %zu\"\n",
                item->key, item->data,
                node->left->list->head->key, node->left->list->head->data);
    } else {
        fprintf(f, "\"null%d\" [label = EList]\n", nullcount);
        fprintf(f, "\"%s: %zu\" -> \"null%d\"\n",
                item->key, item->data, nullcount);
        nullcount++;
    }
    if (node->right != NULL){
        fprintf(f, "\"%s: %zu\" -> \"%s: %zu\"\n",
                item->key, item->data,
                node->right->list->head->key, node->right->list->head->data);
    } else {
        fprintf(f, "\"null%d\" [label = EList]\n", nullcount);
        fprintf(f, "\"%s: %zu\" -> \"null%d\"\n",
                item->key, item->data, nullcount);
        nullcount++;
    }
}

void SwapColors(Node* a, Node* b) {
    enum color t = a->color;
    a->color = b->color;
    b->color = t;
}

// w - красный
void DeleteNodeFixup_1(Tree* tree, Node* x) {
    Node* w = GetBrother(x);

    Node* p = w->parent;
    SwapColors(w, p);
    LeftRotate(tree, p);
    // TODO
}

// w - чёрный
// потомки w - чёрные
void DeleteNodeFixup_2(Tree* tree, Node* x) {
    Node* w = GetBrother(x);

    w->color = red;
    x = w->parent;
    DeleteNodeFixup(tree, x);
    // TODO
}

// w - чёрный
// w->right - чёрный
// w->left - красный
void DeleteNodeFixup_3l(Tree* tree, Node* x) {
    Node* w = GetBrother(x);

    SwapColors(w, w->left);
    RightRotate(tree, w);
    DeleteNodeFixup_4(tree, x);
}

// w - чёрный
// w->left - чёрный
// w->right - красный
void DeleteNodeFixup_3r(Tree* tree, Node* x) {
    Node* w = GetBrother(x);

    SwapColors(w, w->right);
    LeftRotate(tree, w);
    DeleteNodeFixup_4(tree, x);
    // TODO
}

// w - чёрный
// w->right - красный
void DeleteNodeFixup_4(Tree* tree, Node* x) {
    Node* w = GetBrother(x);
    Node* p = w->parent;
    w->color = p->color;
    p->color = black;
    w->right->color = black;
    DeleteNodeFixup(tree, x);
    // TODO
}

void DeleteNodeFixup(Tree* tree, Node* x) {
    // x - удаляемый узел
    // p - родитель
    // w - брат удаляемого узла
    for (Node *p = x->parent; p != NULL && x->color == black; x->color = black, x = x->parent) {
        Node *w = GetBrother(x);
        bool xIsLeft = (x == p->left);
        if (w && w->color == red) {
            DeleteNodeFixup_1(tree, x);
        } else if ( (!w->left || w->left->color == black) && (!w->right || w->right->color == black)) {
            DeleteNodeFixup_2(tree, x);
        } else if (xIsLeft && (!w->right || w->right->color == black)) {
            DeleteNodeFixup_3l(tree, x);
        } else if (!xIsLeft && (!w->left || w->left->color == black)) {
            DeleteNodeFixup_3r(tree, x);
        } else {
            if (w != NULL)
                DeleteNodeFixup_4(tree, x);
        }
    }
}