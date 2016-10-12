#ifndef VECTOR_H
#define VECTOR_H

struct vector {
        struct node *head;
        struct node *tail;
        int size;
};

extern int vector_destroy(struct vector *stapel);
extern int vector_delete(struct vector *v,struct node *n);
extern int vector_clear(struct node *stapel);
extern struct vector *vector_copy(struct vector *v);
extern struct node *vector_top(struct vector *v);
extern int vector_push(struct vector *v,struct node* n);
extern int vector_push_string(struct vector *v,char* content);
extern int vector_push_int(struct vector *v, int content);
extern int vector_push_vector(struct vector *v,struct vector *content);
extern int vector_push_node(struct vector *v,struct node *content);
extern int vector_push_pointer(struct vector *v,struct node *content);
extern struct node *vector_pop(struct vector *v);
extern char *vector_pop_string(struct vector *v);
extern int vector_pop_int(struct vector *v);
extern struct vector *vector_pop_vector(struct vector *v);
extern struct node *vector_pop_node(struct vector *v);
extern struct node *vector_pop_pointer(struct vector *v);
extern struct node *vector_pick(struct vector *v,char *name);
extern char *vector_pick_string(struct vector *v, char *name);
extern int vector_pick_int(struct vector *v, char *name);
extern struct vector *vector_pick_vector(struct vector *v, char *name);
extern struct node *vector_pick_node(struct vector *v, char *name);
extern struct node *vector_get(struct vector *v,char *name);
extern char *vector_get_string(struct vector *v, char *name);
extern int vector_get_int(struct vector *v, char *name);
extern int vector_put(struct vector *v,char *name,struct node* n);
extern int vector_put_vector(struct vector *v,char *name, struct vector *content);
extern int vector_replace(struct vector *v,char *name,struct node* n);
extern int vector_replace_string(struct vector *v,char* name,char* content);
extern int vector_replace_int(struct vector *v,char* name,int content);
extern int vector_replace_vector(struct vector *v,char* name,struct vector *content);
extern int vector_replace_node(struct vector *v,char *name,struct node *content);
extern int vector_empty(struct vector *v);
extern struct vector *vector_create();
extern int vector_size(struct vector *v);
extern struct node *vector_shift(struct vector *v);
extern int vector_unshift(struct vector *v,struct node* n);
extern int vector_push_float(struct vector *v,float content);
extern float vector_pop_float(struct vector *v);

extern int debug(struct vector *v);

#endif
