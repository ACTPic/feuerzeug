#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "node.h"
#include "vector.h"

extern struct vector *cdb_load(char *name);
extern void cdb_loadall_init();
extern struct vector *cdb_loadall_next();
extern struct vector *cdb_load_random();

// Zum Debuggen
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
		if (strcmp(c0, c1)) {
			printf("\n⋄%s\n„%s“\n⚡%s\n„%s“\n",
			       elem0->name, c0, elem1->name, c1);
			return false;
		}
		elem0 = elem0->next;
		elem1 = elem1->next;
	}
	if ((elem0 && !elem1) || (!elem0 && elem1))
		return false;
	return true;
}

extern struct vector *bdb_load(char *name);
extern void bdb_store(char *name, struct vector *v);

struct vector *load_file(char *name)
{
	struct vector *v_bdb = bdb_load(name);
	struct vector *v_cdb = cdb_load(name);

	if (!v_bdb && !v_cdb)
		return 0;

	if (v_bdb)
		return v_bdb;

	if (!v_cdb)
		return 0;

	bdb_store(name, v_cdb);
	return v_cdb;
}

void iterate(int (*action) (int64_t n, struct vector * content))
{
	int64_t count = 0;
	struct vector *v_cdb = 0;

	for (cdb_loadall_init(); (v_cdb = cdb_loadall_next()); count++) {
		action(count, v_cdb);
	}

	// TODO: Berkeley-DB auch.
}

struct vector *load_rand_file()
{
	return cdb_load_random();

	// TODO: Berkeley-DB auch.
}
