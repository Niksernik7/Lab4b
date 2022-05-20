#include "RBTree.h"

bool Insert(Tree *tree, const char *key, size_t data) {

    if (tree == NULL) {
        return false;
    }

    Node* newNode = malloc(sizeof(Node));
    newNode->data = data;
    newNode->key = key;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->parent = NULL;
    newNode->color = red;

    if (tree->root == NULL) {
        tree->root = InsertNode(tree->root, newNode);
        tree->root->color = black;
        tree->size = 1;
    } else {
        InsertNode(tree->root, newNode);
        tree->size++;
    }


    ValidateNode(tree, newNode);

    return true;
}

void ValidateNode(Tree *tree, Node *currentNode) {
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
                // Поворачиваем влево, если являемся правым потомком
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


    ValidateNode(tree, GetGrandpa(currentNode)); // Ту же проверку с дедушки
}
Node *GetSibiling(Node *currentNode) {
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

Node* InsertNode(Node *currentNode, Node *nodeToInsert) {
    if (currentNode == NULL) {
        return nodeToInsert;
    }

    if (strlen(nodeToInsert->key) < strlen(currentNode->key)) {
        currentNode->left = InsertNode(currentNode->left, nodeToInsert);
        currentNode->left->parent = currentNode;
    } else {
        currentNode->right = InsertNode(currentNode->right, nodeToInsert);
        currentNode->right->parent = currentNode;
    }

    return currentNode;
}



void RightRotate(Tree *tree, Node *temp) {
    Node *left = temp->left;
    if(!left) {
        return;
    }
    temp->right = left->right;
    if (temp->left) {
        temp->left->parent = temp;
    }
    left->parent = temp->parent;
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
    temp->parent = left;
}

void LeftRotate(Tree *tree, Node *temp) {
    Node *right = temp->right;
    if(!right) {
        return;
    }
    temp->right = right->left;
    if (temp->right) {
        temp->right->parent = temp;
    }
    right->parent = temp->parent;
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
    temp->parent = right;
}

Node *FindNode(Node *currentNode, const char *key) {
    if (currentNode == NULL) {
        return NULL;
    }
    if (strcmp(currentNode->key, key) == 0) {
        return currentNode;
    }

    if (strlen(key) < strlen(currentNode->key)) {
        currentNode = FindNode(currentNode->left, key);
    } else {
        currentNode = FindNode(currentNode->right, key);
    }

    return currentNode;
}

Node *Find(Tree *tree, const char *key) {
    if (!key) {
        return NULL;
    }

    return FindNode(tree->root, key);
}

void ValidateAfterDelete(Tree *tree, Node *sibiling) {
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

            ValidateAfterDelete(tree, sibiling->parent);
            return;
        }
        if (sibiling->right->color == black && sibiling->left->color == black) {

            enum color tempColor = sibiling->parent->color;

            sibiling->color = red;
            sibiling->parent->color = black;

            if (tempColor == red) {
                return;
            }

            ValidateAfterDelete(tree, sibiling->parent);

        }
    } else {
        sibiling->parent->color = red;
        sibiling->color = black;

        LeftRotate(tree, sibiling->parent);
    }
}

bool DeleteByKey(Tree *tree, char *key) {

    Node *nodeToDelete = Find(tree, key);

    if (!nodeToDelete) {
        return false;
    }

    Node *validationNode = GetSibiling(nodeToDelete);

    if (nodeToDelete->left && nodeToDelete->right) {
        // Find max or min and replace
        Node *currentNode = nodeToDelete->right;
        while (currentNode->left) {
            currentNode = currentNode->left;
        }

        SwapValues(currentNode, nodeToDelete);

        if(nodeToDelete->parent) {
            if (currentNode != nodeToDelete->right) {
                currentNode->parent->left = NULL;
            } else {
                currentNode->parent->right = NULL;
            }
        }

        free(currentNode);

    } else if (nodeToDelete->left || nodeToDelete->right) {
        if (nodeToDelete->color == black) {
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

        free(nodeToDelete);
    }

    if (validationNode != NULL) {
        ValidateAfterDelete(tree, validationNode);
    }

    tree->size--;
    if(tree->size == 0) {
        tree->root = NULL;
    }
    return true;

}

void SwapValues(Node *first, Node *second) {
    Node temp;
    temp.key = first->key;
    temp.data = first->data;

    first->key = second->key;
    first->data = second->data;

    second->key = temp.key;
    second->data = temp.data;
}

void WalkTree(Node* node, void (*cb)(Node *node, void *arg), void *arg){
    if (node == NULL)
        return;
    WalkTree(node->left, cb, arg);
    cb(node, arg);
    WalkTree(node->right, cb, arg);
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