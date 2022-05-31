#include <assert.h>
#include "RBTree.h"

Tree *tree;

Node *a;
Node *b;
Node *c;
Node *d;
Node *e;
Node *f;
Node *g;
Node *h;
Node *i;
Node *j;
Node *k;

void fillNodes() {
    a = FindNode(tree->root, "a");
    b = FindNode(tree->root, "b");
    c = FindNode(tree->root, "c");
    d = FindNode(tree->root, "d");
    e = FindNode(tree->root, "e");
    f = FindNode(tree->root, "f");
    g = FindNode(tree->root, "g");
    h = FindNode(tree->root, "h");
    i = FindNode(tree->root, "i");
    j = FindNode(tree->root, "j");
    k = FindNode(tree->root, "k");
}

void setupTree() {
    tree = calloc(1, sizeof(Tree));

    assert(Insert(tree, "d", 666) == 1);
    assert(Insert(tree, "h", 666) == 1);
    assert(Insert(tree, "a", 666) == 1);
    assert(Insert(tree, "f", 666) == 1);
    assert(Insert(tree, "j", 666) == 1);
    assert(Insert(tree, "e", 666) == 1);
    assert(Insert(tree, "b", 666) == 1);
    assert(Insert(tree, "g", 666) == 1);
    assert(Insert(tree, "k", 666) == 1);
    assert(Insert(tree, "i", 666) == 1);
    assert(Insert(tree, "c", 666) == 1);

    fillNodes();
}

void testAdd() {
    setupTree();

    assert(d->color == black);
    assert(d->parent == NULL);
    assert(d->left == b);
    assert(d->right == h);

    assert(b->color == black);
    assert(b->parent == d);
    assert(b->left == a);
    assert(b->right == c);

    assert(h->color == red);
    assert(h->parent == d);
    assert(h->left == f);
    assert(h->right == j);

    assert(a->color == red);
    assert(a->parent == b);
    assert(a->left == NULL);
    assert(a->right == NULL);

    assert(c->color == red);
    assert(c->parent == b);
    assert(c->left == NULL);
    assert(c->right == NULL);

    assert(f->color == black);
    assert(f->parent == h);
    assert(f->left == e);
    assert(f->right == g);

    assert(j->color == black);
    assert(j->parent == h);
    assert(j->left == i);
    assert(j->right == k);

    assert(e->color == red);
    assert(e->parent == f);
    assert(e->left == NULL);
    assert(e->right == NULL);

    assert(g->color == red);
    assert(g->parent == f);
    assert(g->left == NULL);
    assert(g->right == NULL);

    assert(i->color == red);
    assert(i->parent == j);
    assert(i->left == NULL);
    assert(i->right == NULL);

    assert(k->color == red);
    assert(k->parent == j);
    assert(k->left == NULL);
    assert(k->right == NULL);

    FreeTree(tree);
}

void testDelete() {
    setupTree();
    assert(DeleteByKeyAndNumber(tree, "a", 1));
    fillNodes();
    assert(a == NULL);
    assert(b->left == NULL);
    assert(b->parent == d);
    assert(DeleteByKeyAndNumber(tree, "c", 1));
    fillNodes();
    assert(c == NULL);
    assert(b->right == NULL);
    assert(b->parent == d);

    assert(DeleteByKeyAndNumber(tree, "b", 1));
    fillNodes();
    assert(b == NULL);
    assert(h->color == black);
    assert(h->parent == NULL);
    assert(h->left == d);
    assert(h->right == j);
    assert(d->color == red);
    assert(d->parent == h);
    assert(d->left == NULL);
    assert(d->right == f);
    assert(j->color == black);
    assert(j->parent == h);
    assert(j->left == i);
    assert(j->right == k);
    assert(f->color == black);
    assert(f->parent == d);
    assert(f->left == e);
    assert(f->right == g);

    // red, single child
    assert(DeleteByKeyAndNumber(tree, "d", 1));
    fillNodes();
    assert(d == NULL);
    assert(h->left == f);
    assert(f->parent == h);
    assert(f->color == black);

    assert(DeleteByKeyAndNumber(tree, "f", 1));
    fillNodes();
    assert(f == NULL);
    assert(h->left == e || h->left == g);
    assert(h->left->parent == h);
    assert((h->left->left == e && h->left->right == NULL) || (h->left->right == g || h->left->left == NULL));
    assert(h->left->color == black);
    Node *grandChild = h->left->left ? h->left->left : h->left->right;
    assert(grandChild->color == red);
    assert(grandChild->left == NULL);
    assert(grandChild->right == NULL);
    assert(grandChild->parent == h->left);

    assert(DeleteByKeyAndNumber(tree, "g", 1));
    fillNodes();
    assert(g == NULL);
    assert(h->left == e);
    assert(e->color == black);
    assert(e->parent == h);

    assert(DeleteByKeyAndNumber(tree, "h", 1));
    fillNodes();
    assert(tree->root == i);
    assert(i->color == black);
    assert(i->parent == NULL);
    assert(i->left == e);
    assert(i->right == j);
    assert(e->parent == i);
    assert(e->color == black);
    assert(e->right == NULL);
    assert(e->left == NULL);
    assert(j->color == black);
    assert(j->parent == i);
    assert(j->left == NULL);
    assert(j->right == k);
    assert(DeleteByKeyAndNumber(tree, "i", 1));
    assert(DeleteByKeyAndNumber(tree, "k", 1));
    fillNodes();
    assert(k == NULL && i == NULL);
    assert(j->color == black);
    assert(tree->root == j);
    assert(j->left == e);
    assert(j->right == NULL);
    assert(e->color == red);
    assert(e->parent == j);
    assert(e->left == NULL);
    assert(e->right == NULL);
    PrintGV(tree);
    FreeTree(tree);

}

void testFind() {
    Tree* tree = calloc(1, sizeof(Tree));
    int num;
    Node *node;

    assert(Insert(tree, "asd", 1234) == 1);
    assert(Insert(tree, "asd", 1234) == 2);
    assert(Insert(tree, "asd", 1234) == 3);
    assert(Insert(tree, "asd", 1234) == 4);
    assert(Insert(tree, "asd", 1234) == 5);

    num = Insert(tree, "aaaa", 545);
    assert(num == 1);
    node = FindNode(tree->root, "aaaa");
    assert(node != NULL);
    assert(node->parent == tree->root);
    assert(tree->root->left == node);

    num = Insert(tree, "aaaa", 666);
    assert(num == 2);
    node = FindNode(tree->root, "aaaa");
    assert(node != NULL);
    assert(node->parent == tree->root);
    assert(tree->root->left == node);

    assert(Find(tree, "foo", 1) == NULL);
    assert(Find(tree, "asd", 1)->data == 1234);
    assert(Find(tree, "asd", 5)->data == 1234);
    assert(Find(tree, "asd", 0) == NULL);
    assert(Find(tree, "asd", 6) == NULL);
    assert(Find(tree, "aaaa", 1)->data == 545);
    assert(Find(tree, "aaaa", 2)->data == 666);
    assert(Find(tree, "aaaa", 0) == NULL);
    assert(Find(tree, "aaaa", 3) == NULL);

    assert(Insert(tree, "foo", 1234) == 1);
    assert(SpecialFind(tree, "foo", 1)->data == 545);
    assert(SpecialFind(tree, "foo", 3) == NULL);
    assert(SpecialFind(tree, "aaa", 1)->data == 1234);
    assert(SpecialFind(tree, "aaa", 2) == NULL);
    assert(SpecialFind(tree, "asd", 1)->data == 1234);
    assert(SpecialFind(tree, "asd", 5) == NULL);
    assert(SpecialFind(tree, "a", 1)->data == 1234);
    assert(SpecialFind(tree, "a", 3) == NULL);
    assert(SpecialFind(tree, "b", 1)->data == 1234);
    assert(SpecialFind(tree, "b", 3) == NULL);
    assert(SpecialFind(tree, "e", 1)->data == 545);
    assert(SpecialFind(tree, "e", 2)->data == 666);
    assert(SpecialFind(tree, "e", 3) == NULL);
    assert(SpecialFind(tree, "z", 1)->data == 545);
    assert(SpecialFind(tree, "z", 3) == NULL);
}

int main() {
    testFind();
    testAdd();
    testDelete();
    return 0;
}
