#ifndef NODE_H
#define NODE_H

#include "def.h"

struct node {
        void *content;
        int type;
        char *name;
        struct node *next;
        struct node *prev;
};

extern struct node *node_create(void *content,int type);
extern struct node *node_copy(struct node *n);
extern int node_destroy(struct node *n);
extern int node_toInt(struct node *n);
extern char *node_toString(struct node *n);
extern struct vector *node_toVector(struct node *n);
extern float node_toFloat(struct node *n);
extern struct node *node_toPointer(struct node *n);
extern struct node *node_toNode(struct node *n);
extern MYSQL_RES *node_toMysqlres(struct node * n);

#endif
