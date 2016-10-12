#include "/usr/include/mysql/mysql.h"
#include <assert.h>

#define BF_TYPE_NODE 0
#define BF_TYPE_C_TEXT 1
#define BF_TYPE_C_INT 2
#define BF_TYPE_C_EXT 3
#define BF_TYPE_INT 4
#define BF_TYPE_FLOAT 5
#define BF_TYPE_POINTER 6
#define BF_TYPE_STRING 7
#define BF_TYPE_VECTOR 8
#define BF_TYPE_MYSQLRES 9

// einige globale Variablen
struct node *pc;		// Program Counter - Zeiger auf den nächsten Befehl
struct vector *cword;		// pointer to the actual running word
int accesslevel;
int interpretcounter;
int runlimit;

// stacks
struct vector *rstack;		// Return Stack, hält Rücksprungadressen
struct vector *dstack;		// Data Stack
struct vector *cstack;		// Compiler Stack
struct vector *estack;		// Environment Stack for the Compiler
struct vector *lexicon;		// In which directories will words be searched
struct vector *var;		// Variable stack, for storing variables
struct vector *lstack;		// loop-stack
// help stacks
struct vector *zstack;

// speicher fuer fertig kompilierte Worte
struct vector *words;

// mysql-krempel
MYSQL_FIELD *fld;
MYSQL mysql_read;
MYSQL mysql_write;
MYSQL_RES *res;
MYSQL_ROW row;

// irc-kram
int ircsocket;

// botforth functions
struct vector *botforth_compile(struct vector *word);
struct vector *botforth_parse(char *progstring);
int get_stringtype(char *s);
void botforth_interpreter(struct vector *program, int withinfoblock);
void botforth_call(struct vector *program);
void botforth_return();


// node functions
struct node {
	void *content;
	int type;
	char *name;
	struct node *next;
	struct node *prev;
};
struct node *node_create(void *content, int type);
struct node *node_copy(struct node *n);
int node_destroy(struct node *n);
int node_toInt(struct node *n);
char *node_toString(struct node *n);
struct vector *node_toVector(struct node *n);
float node_toFloat(struct node *n);
struct node *node_toNode(struct node *n);

// vector functions
struct vector {
	struct node *head;
	struct node *tail;
	int size;
};
int vector_delete(struct vector *v, struct node *n);
int vector_clear(struct node *stapel);
struct node *vector_top(struct vector *v);
int vector_push(struct vector *v, struct node *n);
int vector_push_string(struct vector *v, char *content);
int vector_push_int(struct vector *v, int content);
int vector_push_vector(struct vector *v, struct vector *content);
int vector_push_node(struct vector *v, struct node *content);
int vector_push_pointer(struct vector *v, struct node *content);
struct node *vector_pop(struct vector *v);
char *vector_pop_string(struct vector *v);
int vector_pop_int(struct vector *v);
struct vector *vector_pop_vector(struct vector *v);
struct node *vector_pop_node(struct vector *v);
struct node *vector_pop_pointer(struct vector *v);
struct node *vector_pick(struct vector *v, char *name);
char *vector_pick_string(struct vector *v, char *name);
int vector_pick_int(struct vector *v, char *name);
struct vector *vector_pick_vector(struct vector *v, char *name);
struct node *vector_pick_node(struct vector *v, char *name);
struct node *vector_get(struct vector *v, char *name);
char *vector_get_string(struct vector *v, char *name);
int vector_get_int(struct vector *v, char *name);
int vector_put(struct vector *v, char *name, struct node *n);
int vector_replace(struct vector *v, char *name, struct node *n);
int vector_replace_string(struct vector *v, char *name, char *content);
int vector_replace_int(struct vector *v, char *name, int content);
int vector_replace_vector(struct vector *v, char *name,
			  struct vector *content);
int vector_replace_node(struct vector *v, char *name,
			struct node *content);
int vector_empty(struct vector *v);
struct vector *vector_create();
struct node *vector_shift(struct vector *v);
int vector_unshift(struct vector *v, struct node *n);
int vector_push_float(struct vector *v, float content);
float vector_pop_float(struct vector *v);


// getintcommand
void *getintcommand(char *command);
char *getintcommandname(void *p);


// file
struct vector *load_file(char *name);

// makros
#define min(a,b) ((a)<(b) ? a : b)
#define max(a,b) ((a)>(b) ? a : b)
