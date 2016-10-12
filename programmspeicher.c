#include <stdlib.h>
#include <stdio.h>
#include "datenstapel.c"

#define DEBUG printf("\n---DEBUG---\n");

struct botforthsymbol {
  char name[256];
  struct node *jumptarget;
  int  type;
};

struct bfprogram {
  char name[256];
  struct stack program;
};

/* ********************************************* */
/* debug gibt listenelemente aus, deren adresse, */
/* inhalt des strings und wohin die * zeigen     */
/* return 0 wenn I.O. und ausgabe                */
/* uebergabe: ein element des stapels            */
/* ********************************************* */
int programm_debug(struct stack *stapel) {
/*
  struct programm *elem = stapel;
  while ( elem->prev != elem ) elem=elem->prev;
  printf("\nKompletter Stack mit Inhalt:\n");
  do {
    printf("%0xd\t%i '%s'\t-> %0xd\t<-%0xd\n",elem,elem->symbol->type,
           elem->symbol->name,
           elem->next,elem->prev);
    elem=elem->next;
  } while ( elem->next!=elem );
  elem=elem->next;
  printf("%0xd\t'%s'\t-> %0xd\t<-%0xd\n",elem,elem->symbol->name,
         elem->next,elem->prev);
  return 0;
*/
}

