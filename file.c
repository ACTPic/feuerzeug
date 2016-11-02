#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "node.h"
#include "vector.h"

extern struct vector *cdb_load(char *name);
extern struct vector *sql_load(char *name);

bool vector_str_equal(struct vector *v0, struct vector *v1)
{
	if (!v0 && !v1)
		return true;

	if ((!v0 && v1) || (v0 && !v1))
		return false;

	struct node *elem0 = v0->head, *elem1 = v1->head;
	if ((elem0 && !elem1) || (!elem0 && elem1))
		return false;

	while (elem0 && elem1) {
		char *c0 = elem0->content;
		char *c1 = elem1->content;
		if (strcmp(c0, c1))
			return false;

		elem0 = elem0->next;
		elem1 = elem1->next;
	}
	if ((elem0 && !elem1) || (!elem0 && elem1))
		return false;
	return true;
}

struct vector *load_file(char *name)
{
	struct vector *v_sql, *v_cdb;

	v_sql = sql_load(name);
	v_cdb = cdb_load(name);

	if (vector_str_equal(v_sql, v_cdb))
		puts("CDB==SQL");
	else {
		puts("ø CDB");
		debug(v_cdb);
		puts("ø SQL");
		debug(v_sql);
	}

	return v_cdb;
}
