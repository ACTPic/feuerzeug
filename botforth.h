#ifndef BOTFORTH_H
#define BOTFORTH_H

#include "def.h"
#include "vector.h"
#include "node.h"

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

// irc-kram
int ircsocket;

// botforth functions
struct vector *botforth_compile(struct vector *word);
struct vector *botforth_parse(char *progstring);
int get_stringtype(char *s);
void botforth_interpreter(struct vector *program, int withinfoblock);
void botforth_call(struct vector *program);
void botforth_return();

// getintcommand
void *getintcommand(char *command);
char *getintcommandname(void *p);


// file
struct vector *load_file(char *name);

#endif
