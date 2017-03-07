#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "vector.h"
#include "botforth.h"

#define STRINGLENGTH 255
#define DEBUG printf("\n---DEBUG---\n");

/* ********************************************* */
/* delete löscht ein element vom stapel  */
/* return 1 wenn stack leer, 0 wenn IO           */
/* übergabewert: ein element des stapels         */
/* ********************************************* */
int vector_delete(struct vector *v, struct node *n)
{
	// todo: rueckgabewerte einbauen
	if (n == NULL || v->tail == NULL)
		return 0;
	v->size--;
	assert(v->size >= 0);
	if (n->prev != NULL)
		n->prev->next = n->next;
	else
		v->head = n->next;
	if (n->next != NULL)
		n->next->prev = n->prev;
	else
		v->tail = n->prev;
	return 2342;
}

/* ********************************************* */
/* clear gibt stapelspeicher komplett frei       */
/* return 0 wenn I.O.                            */
/* übergabe: ein element des stapels             */
/* umbauen, dass man den zu löschenden Vektor angibt, und nicht ein element */
/* ********************************************* */
int vector_destroy(struct vector *stapel)
{
	struct node *elem, *newelem;
	elem = stapel->head;
	while (elem) {
		//printf("\ngebe %0xd frei",elem);
		newelem = elem->next;
		node_destroy(elem);
		elem = newelem;
	}
	//printf("\ngebe %0xd frei ... stack freigegeben\n",elem);
	free(stapel);
	return 0;
}


/* ********************************************* */
/* debug gibt listenelemente aus, deren adresse, */
/* inhalt des strings und wohin die * zeigen     */
/* return 0 wenn I.O. und ausgabe                */
/* uebergabe: ein element des stapels            */
/* ********************************************* */
int debug(struct vector *v)
{
	struct node *elem;
	if (!v) {
		printf("\n· Vector [leer]\n");
		return 0;
	}
	printf("\n· Vector:\n");
	if (!v->head)
		printf("       [leer]\n");
	elem = v->head;
	while (elem) {
		assert(elem->type < 20);
		printf("       ");
		switch (elem->type) {
		case BF_TYPE_INT:
			printf("int:   %11s → %i\n", elem->name,
			       node_toInt(node_copy(elem)));
			break;
		case BF_TYPE_FLOAT:
			printf("float: %11s → %g\n", elem->name,
			       node_toFloat(node_copy(elem)));
			break;
		case BF_TYPE_STRING:
		case BF_TYPE_C_TEXT:
			printf("str:   %11s → „%s“\n", elem->name,
			       (char *) elem->content);
			break;
		default:
			printf("%6d: „%s“ → ??? [%09lx]\n",
			       elem->type, elem->name,
			       (unsigned long) elem->content);
			break;
		}
		assert(elem != elem->next);
		assert(elem != elem->prev);
		elem = elem->next;
	}
	return 0;
}

/***************************************
 * top HOLT oberstes element vom stapel
 *
 * v: Vektor, von dem gelesen werden soll
 *
 * return: pointer auf das oberste Element
 ****************************************/
struct node *vector_top(struct vector *v)
{
	if (v->tail != NULL) {
		return v->tail;
	}
	return NULL;
}

/***************************************
 * bottom HOLT unterstes element vom stapel
 *
 * v: Vektor, von dem gelesen werden soll
 *
 * return: pointer auf das unterste Element
 ****************************************/
struct node *vector_bottom(struct vector *v)
{
	if (v->head != NULL) {
		return v->head;
	}
	return NULL;
}


/* ************************************************ */
/* push legt ein neues element mit inhalt buffer    */
/* auf den stapel                                   */
/* return 0 wenn I.O.                               */
/* übergabe: element der liste und *buffer (string) */
/* ************************************************ */
int vector_push(struct vector *v, struct node *n)
{
	assert(v != NULL);
	assert(n != NULL);
	assert(n->type < 20);
	n->prev = v->tail;
	n->next = NULL;
	if (v->tail != NULL)
		v->tail->next = n;
	if (v->head == NULL)
		v->head = n;
	v->tail = n;
	v->size++;
	return 1;
}


// todo: vector_put_string verwenden
int vector_push_string(struct vector *v, char *content)
{
	struct node *n = node_create(content, BF_TYPE_STRING);
	char *name = malloc(1);
	name[0] = '\0';
	n->name = name;
	return vector_push(v, n);
}

int vector_push_int(struct vector *v, int content)
{
	int *temp;
	struct node *n;
	char *name = malloc(1);
	name[0] = '\0';
	temp = malloc(sizeof(int));
	*temp = content;
	n = node_create(temp, BF_TYPE_INT);
	n->name = name;
	return vector_push(v, n);
}

int vector_push_float(struct vector *v, float content)
{
	float *temp;
	struct node *n;
	char *name = malloc(1);
	name[0] = '\0';
	temp = malloc(sizeof(float));
	*temp = content;
	n = node_create(temp, BF_TYPE_FLOAT);
	n->name = name;
	return vector_push(v, n);
}


// todo: vector_put_vector verwenden
int vector_push_vector(struct vector *v, struct vector *content)
{
	struct node *n = node_create(content, BF_TYPE_VECTOR);
	char *name = malloc(1);
	name[0] = '\0';
	n->name = name;
	return vector_push(v, n);
}

int vector_push_node(struct vector *v, struct node *content)
{
	struct node *n = node_create(content, BF_TYPE_NODE);
	char *name = malloc(1);
	name[0] = '\0';
	n->name = name;
	return vector_push(v, n);
}

int vector_push_pointer(struct vector *v, struct node *content)
{
	struct node *n = node_create(content, BF_TYPE_POINTER);
	char *name = malloc(1);
	name[0] = '\0';
	n->name = name;
	return vector_push(v, n);
}

int vector_push_db(struct vector *v, struct db *content)
{
	struct node *n = node_create(content, BF_TYPE_DB);
	char *name = malloc(1);
	name[0] = '\0';
	n->name = name;
	return vector_push(v, n);
}

/* ************************************************
 * unshift legt ein neues element mit inhalt buffer
 * unter den stapel
 * return 1 wenn I.O.
 * übergabe: element der liste und *buffer (string)
 * ************************************************ */
int vector_unshift(struct vector *v, struct node *n)
{
	assert(v != NULL);
	assert(n != NULL);
	assert(n->type < 20);
	n->next = v->head;
	n->prev = NULL;
	if (v->head != NULL)
		v->head->prev = n;
	if (v->tail == NULL)
		v->tail = n;
	v->head = n;
	v->size++;
	return 1;
}


/* ************************************************* */
/* pop HOLT operstes element vom stapel und schreibt */
/* dess inhalt in *buffer, dann wird das oberste     */
/* element entfernt                                  */
/* return: zeiger auf buffer (string/charray)        */
/* ************************************************* */
struct node *vector_pop(struct vector *v)
{
	struct node *n;
	n = vector_top(v);
	if (n) {
		vector_delete(v, n);
		return n;
	}
	return NULL;
}

char *vector_pop_string(struct vector *v)
{
	char *temp;
	struct node *n = vector_pop(v);
	temp = node_toString(n);
	return temp;
}

int vector_pop_int(struct vector *v)
{
	int temp;
	struct node *n = vector_pop(v);
	temp = node_toInt(n);
	return temp;
}

float vector_pop_float(struct vector *v)
{
	float temp;
	struct node *n = vector_pop(v);
	temp = node_toFloat(n);
	return temp;
}


struct vector *vector_pop_vector(struct vector *v)
{
	struct vector *temp;
	struct node *n = vector_pop(v);
	temp = node_toVector(n);
	return temp;
}

struct node *vector_pop_node(struct vector *v)
{
	struct node *temp;
	struct node *n = vector_pop(v);
	temp = node_toNode(n);
	return temp;
}

struct node *vector_pop_pointer(struct vector *v)
{
	struct node *temp;
	struct node *n = vector_pop(v);
	temp = (struct node *) node_toPointer(n);
	return temp;
}

struct db *vector_pop_db(struct vector *v)
{
	struct node *n = vector_pop(v);
	struct db *tmp = node_todb(n);
	return tmp;
}

/* *************************************************
 * shift HOLT das unterste element vom stapel und 
 * *************************************************/
struct node *vector_shift(struct vector *v)
{
	struct node *n;
	n = vector_bottom(v);
	if (n) {
		vector_delete(v, n);
		return n;
	}
	return NULL;
}


/*********************************
 * Lädt einen Node aus einem Vektor anhand seines Namens, ohne ihn
 * zu löschen
 *********************************/
struct node *vector_pick(struct vector *v, char *name)
{
	struct node *pos;
	assert(v != NULL);
	pos = v->tail;
	while (pos != NULL && strcmp(pos->name, name) != 0) {
		pos = pos->prev;
		if (pos != NULL)
			assert(pos->name != NULL);
	}
	if (pos != NULL) {
		assert(pos->type < 20);
		return pos;
	}
	return NULL;
}

char *vector_pick_string(struct vector *v, char *name)
{
	char *temp;
	struct node *n = vector_pick(v, name);
	if (n)
		return node_toString(node_copy(n));
	temp = malloc(1);
	*temp = 0;
	return temp;
}

int vector_pick_int(struct vector *v, char *name)
{
	int temp;
	struct node *n = vector_pick(v, name);
	temp = node_toInt(n);
	return temp;
}

struct vector *vector_pick_vector(struct vector *v, char *name)
{
	struct vector *temp;
	struct node *n = vector_pick(v, name);
	temp = node_toVector(n);
	return temp;
}

struct node *vector_pick_node(struct vector *v, char *name)
{
	struct node *temp;
	struct node *n = vector_pick(v, name);
	temp = node_toNode(n);
	return temp;
}




/*************************************
 * Lädt einen Node aus einem Vektor anhand seines Namens und löscht iht
 *************************************/
struct node *vector_get(struct vector *v, char *name)
{
	struct node *pos;
	pos = v->tail;
	while (pos != NULL && strcmp(pos->name, name) != 0) {
		pos = pos->prev;
		if (pos != NULL)
			assert(pos->name != NULL);
	}
	if (pos != NULL) {
		v->size--;
		assert(v->size >= 0);
		// node zwar nicht loeschen, aber aus dem vector "entlinken"
		if (v->tail == pos)
			v->tail = pos->prev;
		if (v->head == pos)
			v->head = pos->next;
		if (pos->prev)
			pos->prev->next = pos->next;
		if (pos->next)
			pos->next->prev = pos->prev;
		return pos;
	}
	return NULL;
}

char *vector_get_string(struct vector *v, char *name)
{
	struct node *n = vector_get(v, name);
	return node_toString(n);
}

int vector_get_int(struct vector *v, char *name)
{
	struct node *n = vector_get(v, name);
	return node_toInt(n);
}


/*************************************
 * Hängt genauso wie push einen Node hinten an den Vektor an, wobei hier
 * der Node noch einen Namen bekommt
 */
int vector_put(struct vector *v, char *name, struct node *n)
{
	char *newname;
	assert(name != NULL);
	assert(n != NULL);
	assert(v != NULL);
	assert(n->type < 10);
	if (v == NULL || n == NULL)
		return 0;
	newname = malloc(strlen(name) + 1);
	sprintf(newname, "%s", name);
	if (n->name)
		free(n->name);
	n->name = newname;
	return vector_push(v, n);
}

int vector_put_vector(struct vector *v, char *name, struct vector *content)
{
	struct node *n = node_create(content, BF_TYPE_VECTOR);
	assert(name != NULL);
	return vector_put(v, name, n);
}

int vector_put_string(struct vector *v, char *name, char *content)
{
	struct node *n = node_create(content, BF_TYPE_STRING);
	return vector_put(v, name, n);
}

/*************************************
 * Ersetzt einen Knoten eines Vektors durch einen neuen Knoten, wobei
 * der Knoten durch seinen Namen gegeben wird
 *************************************/
int vector_replace(struct vector *v, char *name, struct node *n)
{
	struct node *temp;
	char *newname;
	if (v == NULL || n == NULL)
		return 0;
	assert(n != NULL);
	temp = vector_pick(v, name);
	if (temp) {
		// Ein zu ersetzender Knoten gefunden, nun Referenzen umhängen
		if (v->head == temp)
			v->head = n;
		if (v->tail == temp)
			v->tail = n;
		if (temp->next)
			temp->next->prev = n;
		if (temp->prev)
			temp->prev->next = n;
		n->next = temp->next;
		n->prev = temp->prev;
		newname = malloc(strlen(name) + 1);
		sprintf(newname, "%s", name);
		n->name = newname;
		//free(temp->name);
		//free(temp);
		node_destroy(temp);
		// todo: warum absturz, wenn node_destroy statt free bei !clearcache?
		// momentan ist das hier jedenfalls ein speicherleck
	} else {
		// Kein Knoten gefunden, den neuen Knoten also einfach putten
		printf("put\n");
		vector_put(v, name, n);
	}
	return 1;
}

int vector_replace_string(struct vector *v, char *name, char *content)
{
	struct node *n = node_create(content, BF_TYPE_STRING);
	return vector_replace(v, name, n);
}

int vector_replace_int(struct vector *v, char *name, int content)
{
	int *temp;
	struct node *n;
	temp = malloc(sizeof(int));
	*temp = content;
	n = node_create(temp, BF_TYPE_INT);
	return vector_replace(v, name, n);
}

int vector_replace_vector(struct vector *v, char *name,
			  struct vector *content)
{
	struct node *n = node_create(content, BF_TYPE_VECTOR);
	assert(n != NULL);
	return vector_replace(v, name, n);
}

int vector_replace_node(struct vector *v, char *name, struct node *content)
{
	struct node *n = node_create(content, BF_TYPE_NODE);
	return vector_replace(v, name, n);
}

/*******************
 * fügt einen Node nach einem gegebenen Node ein
 ******************/
int vector_insert(struct vector *v, struct node *n, struct node *new)
{
	new->next = n->next;
	if (v->tail == n)
		v->tail = new;
	new->prev = n;
	if (new->next)
		new->next->prev = new;
	n->next = new;
	v->size++;
	assert(v->size >= 0);
	return 0;
}


/* ***************************************** */
/* empty prüft ob elemente im stapel liegen  */
/* return 1: stapel ist LEER                 */
/* return 0; stapel ist NICHT leer           */
/* uebergabe: ein element vom stapel         */
/* ***************************************** */
int vector_empty(struct vector *v)
{
	if (v->head == NULL)
		return 1;
	return 0;
}

int vector_size(struct vector *v)
{
	return v->size;
}

struct vector *vector_create()
{
	struct vector *v = malloc(sizeof(struct vector));
	v->head = NULL;
	v->tail = NULL;
	v->size = 0;
	return v;
}

struct vector *vector_copy(struct vector *v)
{
	struct vector *newv;
	struct node *newn, *pos;
	assert(v);
	newv = vector_create();
	pos = v->head;
	if (pos == NULL) {
		newv->head = NULL;
		newv->tail = NULL;
	}
	// todo: was ist mit den namen der elemente?
	while (pos != NULL) {
		assert(pos->type < 20);
		newn = node_copy(pos);
		vector_push(newv, newn);
		pos = pos->next;
	}
	return newv;
}

/*
int main() {
  struct vector *v=vector_create();
  char *buffer;
  struct node *n;
  int i;
  vector_push_string(v,"null");
  vector_push_string(v,"eins");
  vector_push_int(v,2);
  vector_push_string(v,"3");
  vector_push_string(v,"vier");
  vector_replace_string(v,"erstes","ERSTES");
  vector_replace_string(v,"zweites","ZWEITES");
  vector_replace_string(v,"erstes","NEUES ERSTES");
  debug(v);
  for(i=0;i<8;i++) {
    buffer=vector_pop_string(v);
    printf("%s\n",buffer);
  }
}
*/
