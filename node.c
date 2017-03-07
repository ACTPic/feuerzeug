#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "botforth.h"

#define STRINGLENGTH 255
#define DEBUG printf("\n---DEBUG---\n");


struct node *node_create(void *content, int type)
{
	struct node *n = malloc(sizeof(struct node));
	//char *name;
	assert(type < 20);
	n->content = content;
	n->type = type;
	/*
	   name=malloc(5);
	   sprintf(name,"none");
	 */
	n->name = NULL;
	return n;
}

struct node *node_copy(struct node *n)
{
	struct node *c;
	void *content;
	assert(n != NULL);
	assert(n->type < 20);
	switch (n->type) {
	case BF_TYPE_INT:
		content = malloc(sizeof(int));
		memcpy(content, n->content, sizeof(int));
		break;
	case BF_TYPE_FLOAT:
		content = malloc(sizeof(float));
		memcpy(content, n->content, sizeof(float));
		break;
	case BF_TYPE_STRING:
		assert(n->content != NULL);
		content = malloc(strlen(n->content) + 1);
		memcpy(content, n->content, strlen(n->content) + 1);
		break;
	case BF_TYPE_POINTER:
		content = n->content;
		break;
	case BF_TYPE_VECTOR:
		content =
		    (void *) vector_copy((struct vector *) n->content);
		break;
	default:
		printf
		    ("Der Datentyp %i kann bislang nicht kopiert werden.\n",
		     n->type);
		exit(1);
		break;
	}
	c = node_create(content, n->type);
	if (n->name) {
		c->name = malloc(strlen(n->name) + 1);
		sprintf(c->name, "%s", n->name);
	}
	return c;
}


int node_destroy(struct node *n)
{
	if (n == NULL)
		return 0;
	// todo: alle typen abfragen
	switch (n->type) {
	case BF_TYPE_C_TEXT:
	case BF_TYPE_STRING:
	case BF_TYPE_INT:
	case BF_TYPE_FLOAT:
		free(n->content);
		break;
		//case BF_TYPE_C_EXT:  // leider nein, weil sonst der cache sich rekursiv dematerialisiert
	case BF_TYPE_VECTOR:
                if(n != n->content)
                        vector_destroy((struct vector *) n->content);
		break;
	case BF_TYPE_NODE:
		if(n != n->content)
                        node_destroy((struct node *) n->content);
		break;
	case BF_TYPE_DB:
		{
			struct db *tmp = n->content;
			assert(tmp);
			if (tmp->query)
				free(tmp->query);
			if (tmp->db_field)
				free(tmp->db_field);
			break;
		}
	case BF_TYPE_POINTER:
		// nichts loeschen, der node hier gehoert ja wem anders
		break;
	}
	if (n->name)
		free(n->name);
	free(n);
	return 1;
}

int node_toInt(struct node *n)
{
	int temp;
	if (n == NULL)
		return 0;
	switch (n->type) {
	case BF_TYPE_INT:
		temp = *(int *) n->content;
		node_destroy(n);
		return temp;
		break;
	case BF_TYPE_FLOAT:
		temp = (int) *(float *) n->content;
		node_destroy(n);
		return temp;
		break;
	case BF_TYPE_STRING:
		temp = atoi(n->content);
		node_destroy(n);
		return temp;
		break;
	default:
		node_destroy(n);
	}
	return 0;
}

float node_toFloat(struct node *n)
{
	float temp;
	if (n == NULL)
		return 0.0;
	switch (n->type) {
	case BF_TYPE_FLOAT:
		temp = *(float *) n->content;
		node_destroy(n);
		return temp;
		break;
	case BF_TYPE_INT:
		temp = (float) *(int *) n->content;
		node_destroy(n);
		return temp;
		break;
	case BF_TYPE_STRING:
		temp = atof(n->content);
		node_destroy(n);
		return temp;
		break;
	default:
		node_destroy(n);
	}
	return 0.0;
}

char *node_toString(struct node *n)
{
	char *temp, *r;
	//printf("%i\n",n->type);
	if (n == NULL) {
		temp = malloc(1);
		temp[0] = '\0';
		return temp;
	}
	switch (n->type) {
	case BF_TYPE_STRING:
	case BF_TYPE_C_TEXT:
		r = n->content;
		if (n->name)
			free(n->name);
		free(n);
		break;
	case BF_TYPE_INT:
		r = malloc(sizeof(char) * 100);	// todo: moeglicher bufferoverflow
		sprintf(r, "%i", node_toInt(n));
		break;
	case BF_TYPE_FLOAT:
		r = malloc(sizeof(char) * 100);	// todo: moeglicher bufferoverflow
		sprintf(r, "%g", node_toFloat(n));
		break;
	case BF_TYPE_VECTOR:
		r = malloc(7);
		sprintf(r, "Vector");
		node_destroy(n);
		break;
	case BF_TYPE_NODE:
		r = malloc(5);
		sprintf(r, "Node");
		node_destroy(n);
		break;
	case BF_TYPE_DB:
		r = malloc(strlen("db") + 1);
		strcpy(r, "db");
		node_destroy(n);
		break;
	default:
		r = malloc(1);
		r[0] = '\0';
		node_destroy(n);
		break;
	}
	return r;
}

struct vector *node_toVector(struct node *n)
{
	struct vector *content;
	if (n == NULL)
		return NULL;
	switch (n->type) {
	case BF_TYPE_C_EXT:
	case BF_TYPE_VECTOR:
		content = n->content;
		if (n->name)
			free(n->name);
		free(n);
		return content;
		break;
	default:
		node_destroy(n);
		return NULL;
	}
	return NULL;
}



struct node *node_toPointer(struct node *n)
{
	struct node *content;
	if (n == NULL)
		return NULL;
	if (n->type == BF_TYPE_POINTER) {
		content = n->content;
		if (n->name)
			free(n->name);
		free(n);
		return content;
	}
	node_destroy(n);
	return NULL;
}

struct db *node_todb(struct node *n)
{
	struct db *content = 0;
	if (!n)
		return content;
	if (n->type == BF_TYPE_DB) {
		content = (struct db *) n->content;
		if (n->name)
			free(n->name);
		free(n);
		return content;
	}
	node_destroy(n);
	return content;
}


struct node *node_toNode(struct node *n)
{
	struct node *content;
	if (n == NULL)
		return NULL;
	if (n->type == BF_TYPE_NODE) {
		if (n->name)
			free(n->name);
		content = n->content;
		free(n);
		return content;
	} else {
		node_destroy(n);
	}
	return NULL;
}
