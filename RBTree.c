#include <assert.h>
#include <limits.h>

#ifdef WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#else
#include <unistd.h>
#endif

#include "RBTree.h"

typedef struct GVParams {
    FILE* f;
    const char* highlightKey;
} GVParams;

static void DeleteNodeFixup(Tree* tree, Node* x);
static void DeleteNodeFixup_1l(Tree* tree, Node* w);
static void DeleteNodeFixup_1r(Tree* tree, Node* w);
static void DeleteNodeFixup_2(Tree* tree, Node* w);
static void DeleteNodeFixup_3l(Tree* tree, Node* w);
static void DeleteNodeFixup_3r(Tree* tree, Node* w);
static void DeleteNodeFixup_4l(Tree* tree, Node* w);
static void DeleteNodeFixup_4r(Tree* tree, Node* w);
static void SwapColors(Node* a, Node* b);
void delete_fixup(Tree* tree, Node* x);

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
    if (nodeToDelete->left && nodeToDelete->right) {
        // Находим ближайший справа (можно и слева) узел.
        // У него будет максимум один потомок, и далее ситуация с двумя потомками
        // нам уже не встретится, иначе нарушалось бы одно из свойств RB-дерева.
        Node *y = nodeToDelete->right;
        while (y->left)
            y = y->left;
        SwapValues(nodeToDelete, y);
        nodeToDelete = y;
    }

    // возможно существующий дочерний элемент
    Node* child = nodeToDelete->left ? nodeToDelete->left : nodeToDelete->right;
    
    // соединяем child и nodeToDelete->parent напрямую
    if (child) {
        child->parent = nodeToDelete->parent;
        if (nodeToDelete->parent) {
            if (nodeToDelete->parent->left == nodeToDelete)
                nodeToDelete->parent->left = child;
            else
                nodeToDelete->parent->right = child;
        }
        nodeToDelete->left = NULL;
        nodeToDelete->right = NULL;

        if (nodeToDelete->color == black) {
            if (child->color == red)
                child->color = black;
            else
                DeleteNodeFixup(tree, nodeToDelete);
        }
    } else {
        if (nodeToDelete->color == black)
            DeleteNodeFixup(tree, nodeToDelete);
        if (nodeToDelete->parent) {
            if (nodeToDelete->parent->left == nodeToDelete)
                nodeToDelete->parent->left = NULL;
            else
                nodeToDelete->parent->right = NULL;
        }
    }

balanced:
    assert(nodeToDelete->left == NULL && nodeToDelete->right == NULL);
    free(nodeToDelete);
    tree->size--;
    if (tree->size == 0)
        tree->root = NULL;
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


void PrintGV(Tree* tree, const char* highlightKey) {
    fprintf(stderr, "generating GraphViz file...\n");
    FILE* f;
    char nfile[L_tmpnam + 4] = "TreeGV.XXXXXX";
    char gfile[L_tmpnam + 4] = "Graph.XXXXXX";
    char cmd[PATH_MAX + 10];

    mktemp(nfile);
    strcat(nfile, ".txt");
    mktemp(gfile);
    strcat(gfile, ".png");

    f = fopen(nfile, "w");
    if (f == NULL) {
        fprintf(stderr, "Could not create temporary file: %s\n", strerror(errno));
        return;
    }
    fprintf(f, "digraph Tree {\n");
    GVParams gvp = { f, highlightKey };
    WalkTree(tree->root, GenerateGV, &gvp, 0, 1);
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
    GVParams *params = p;

    Item *item = node->list->head;
    fprintf(params->f, "\"%s: %zu\" [fontcolor = %s, color = %s]\n", item->key, item->data,
            (node->color == red) ? "red" : "black",
            (params->highlightKey && strcmp(item->key, params->highlightKey) == 0) ? "yellow" : "black");

    static size_t nullcount = 0;
    if (node->left != NULL){
        fprintf(params->f, "\"%s: %zu\" -> \"%s: %zu\"\n",
                item->key, item->data,
                node->left->list->head->key, node->left->list->head->data);
    } else {
        fprintf(params->f, "\"null%zu\" [label = EList]\n", nullcount);
        fprintf(params->f, "\"%s: %zu\" -> \"null%zu\"\n",
                item->key, item->data, nullcount);
        nullcount++;
    }
    if (node->right != NULL){
        fprintf(params->f, "\"%s: %zu\" -> \"%s: %zu\"\n",
                item->key, item->data,
                node->right->list->head->key, node->right->list->head->data);
    } else {
        fprintf(params->f, "\"null%zu\" [label = EList]\n", nullcount);
        fprintf(params->f, "\"%s: %zu\" -> \"null%zu\"\n",
                item->key, item->data, nullcount);
        nullcount++;
    }
}

void SwapColors(Node* a, Node* b) {
    enum color t = a->color;
    a->color = b->color;
    b->color = t;
}

// w - красный правый потомок своего (чёрного) родителя
void DeleteNodeFixup_1l(Tree* tree, Node* w) {
    Node* p = w->parent;
    assert(w->color == red);
    assert(p->color == black);
    assert(p->right == w);
    w->color = black;
    p->color = red;
    LeftRotate(tree, p);
}

// w - красный левый потомок своего (чёрного) родителя
void DeleteNodeFixup_1r(Tree* tree, Node* w) {
    Node* p = w->parent;
    assert(w->color == red);
    assert(p->color == black);
    assert(p->left == w);
    w->color = black;
    p->color = red;
    RightRotate(tree, p);
}

void DeleteNodeFixup_2(Tree* tree, Node* w) {
    assert(w->color == black);
    assert(w->left == NULL || w->left->color == black);
    assert(w->right == NULL || w->right->color == black);
    w->color = red;
}

void DeleteNodeFixup_3l(Tree* tree, Node* w) {
    assert(w->color == black);
    assert(w->left->color == red);
    assert(w->right == NULL || w->right->color == black);
    w->color = red;
    w->left->color = black;
    RightRotate(tree, w);
}

void DeleteNodeFixup_3r(Tree* tree, Node* w) {
    assert(w->color == black);
    assert(w->left == NULL || w->left->color == black);
    assert(w->right->color == red);
    w->color = red;
    w->right->color = black;
    LeftRotate(tree, w);
}

void DeleteNodeFixup_4l(Tree* tree, Node* w) {
    assert(w->color == black);
    assert(w->right->color == red);
    Node* p = w->parent;
    w->color = p->color;
    p->color = black;
    w->right->color = black;
    LeftRotate(tree, p);
}

void DeleteNodeFixup_4r(Tree* tree, Node* w) {
    assert(w->color == black);
    assert(w->left->color == red);
    Node* p = w->parent;
    w->color = p->color;
    p->color = black;
    w->left->color = black;
    RightRotate(tree, p);
}

// x - удаляемый узел
void DeleteNodeFixup(Tree* tree, Node* x) {
    while(x->parent != NULL && x->color == black) {
        Node* p = x->parent;
        Node* w = GetBrother(x);
        if (w == NULL)
            continue;
        Node* savedRoot = tree->root;
        bool xIsLeft = (x == p->left);
        if (w->color == red) {
            if (xIsLeft) {
                DeleteNodeFixup_1l(tree, w);
                w = p->right;
            } else {
                DeleteNodeFixup_1r(tree, w);
                w = p->left;
            }
        }
        if ((!w->left || w->left->color == black) && (!w->right || w->right->color == black)) {
            DeleteNodeFixup_2(tree, w);
            if (p->color == red) {
                p->color = black;
                break;
            }
            x = x->parent;
        } else {
            if (xIsLeft && (w->left && w->left->color == red) && (!w->right || w->right->color == black)) {
                DeleteNodeFixup_3l(tree, w);
                w = p->right;
            } else if (!xIsLeft && (w->right && w->right->color == red) && (!w->left || w->left->color == black)) {
                DeleteNodeFixup_3r(tree, w);
                w = p->left;
            }
            // Из случая 3 получается случай 4
            if (xIsLeft)
                DeleteNodeFixup_4l(tree, w);
            else
                DeleteNodeFixup_4r(tree, w);
            break;
        }
    }
    x->color = black;
}
