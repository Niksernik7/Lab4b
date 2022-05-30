#include "Menu.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

char* enterString(){
    const int bufsize = 1000;
    char* buf = malloc(bufsize);
    buf = fgets(buf, bufsize, stdin);
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    return buf;
}
void ShowFindMenu() {
    printf("Find\n");
    printf("Choice\n");
    printf("1) Special find by key and number(max difference)\n");
    printf("2) Find by key and number\n");
    printf("0) Exit\n");
    printf(" : ");
}

void ShowDeleteMenu() {
    printf("1) Delete by key\n");
    printf("0) Exit\n");
    printf(" : ");
}

void ShowError() {
    printf("Miss choice\n");
}

void ShowMainMenu() {
    printf("Menu\n");
    printf("Choice\n");
    printf("1. Print tree\n");
    printf("2. Insert node\n");
    printf("3. Delete node\n");
    printf("4. Find node in tree\n");
    printf("0. Quit\n");
    printf(" : ");
}

void printNode(const Item* i) {
    char* s;
    if (i != NULL) {
        s = get_str(i);
        printf("%s\n", s);
        free(s);
    }
    else {
        printf("Node with this key doesn't exist\n");
    }
}




int GetInt() {
    char* str = "Doesn't number!\n : ";
    char* ptr;
    do {
        char* input = (char*) enterString();
        int n = (int)strtol(input, &ptr, 10);
        if (input == ptr - strlen(input) && strlen(input) != 0) {
            free(input);
            return n;
        }
        printf("%s", str);
        free(input);
    } while (1);
}



void Menu() {
    Tree *tree = Create();
    do {
        ShowMainMenu();
        int n = GetInt();
        switch (n) {
            case 0: {
                FreeTree(tree);
                return;
            }
            case 1: {
                PrintGV(tree);
                break;
            }
            case 2: {
                AddNode(tree);
                break;
            }
            case 3: {
                ShowDeleteMenu();
                n = GetInt();
                Delete(tree, n);
                break;
            }
            case 4: {
                ShowFindMenu();
                n = GetInt();
                FindInTree(tree, n);
                break;
            }
            default: {
                ShowError();
                break;
            }
        }
    } while (1);
}

Tree* Create() {
    Tree* tree = calloc(1, sizeof(Tree));
    return tree;
}

void Delete(Tree *tree, int mod) {
    char* key, * msg = "";
    switch (mod) {
        case 0:
            break;

        case 1: {
            printf("Enter:\n");
            printf(" Key : ");
            do {
                printf("%s", msg);
                key = enterString();
                msg = "Key cannot be empty\n Key : ";
                if(strlen(key) != 0) break;
                free(key);
            } while (1);
            int n = 0;
            if(!DeleteByKeyAndNumber(tree, key, n))
                printf("Doesn't exist\n");
            free(key);
            break;
        }
    }
}

void AddNode(Tree* tree) {
    char* str = "Data cannot be empty\n Data : ";
    printf(" Enter\n");
    printf(" Key : ");
    char* key;
    do {
        key = (char*) enterString();
        if (strlen(key) != 0)
            break;
        free(key);
        printf("%s", str);
    } while (1);
    printf(" Data : ");
    size_t data;
    data = GetInt();
    if (!Insert(tree, key, data)) {
        printf("The elem with this key exists\n");
    }
}

void FindInTree(Tree* tree, size_t mod) {
    switch (mod) {
        case 0:
            break;
        case 1: {
            printf("Enter:\n");
            printf(" Key : ");
            char *key = enterString();
            printf(" Number: ");
            int n = GetInt();
            Item* node = SpecialFind(tree, key, n);
            if (!node){
                printf("Doesn't exists!\n");
                break;
            }
            free(key);
            printNode(node);
            break;
        }
        case 2: {
            printf("Enter:\n");
            printf(" Key : ");
            char *key = enterString();
            printf(" Number: ");
            int n = GetInt();
            Item* node = Find(tree, key, n);
            if (!node){
                printf("Doesn't exists!\n");
                break;
            }

            free(key);
            printNode(node);
            break;
        }
        default:
            ShowError();
            break;
    }
}
char* get_str(const Item* item) {
    if (item == NULL)
        return NULL;
    size_t lenkey = strlen(item->key);
    size_t buflen = lenkey + 3 + 30 + 1;
    char* s = malloc(buflen);
    snprintf(s, buflen, "%s | %zu | %d", item->key, item->data, item->number);
    return s;
}
