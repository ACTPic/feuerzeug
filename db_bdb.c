#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <db.h>
#include "node.h"
#include "vector.h"

static DB *dbp;

void bdb_init()
{
	int ret = db_create(&dbp, 0, 0);;

	if (ret) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		exit(EXIT_FAILURE);
	}

	ret = dbp->open(dbp, 0, "calc.bdb", 0, DB_BTREE, DB_CREATE, 0664);
	if (ret) {
		dbp->err(dbp, ret, "calc.bdb");
		exit(EXIT_FAILURE);
	}
}

static void bdb_put(char *key, char *val)
{
	assert(key);
	assert(val);

	DBT field = { 0 };
	field.data = key;
	field.size = strlen(key) + 1;

	DBT data = { 0 };
	data.data = val;
	data.size = strlen(val) + 1;

	int ret = dbp->put(dbp, 0, &field, &data, 0);
	if (ret) {
		dbp->err(dbp, ret, "[BDB] DB->put");
		assert(!ret);
	}

	int cret = dbp->close(dbp, 0);
	assert(!cret);
	bdb_init();
}

char *bdballoc(char *key)
{
	DBT field = { 0 };
	field.data = key;
	field.size = strlen(key) + 1;

	DBT data = { 0 };

	int ret = dbp->get(dbp, 0, &field, &data, 0);

	if (ret) {
		if (ret == DB_NOTFOUND)
			return 0;
		dbp->err(dbp, ret, "✝ DB->get");
		assert(0);
	}

	char *s = strdup(data.data);
	return s;
}

static void store_node(char *name, struct vector *v, char *sub)
{
	char *inhalt = vector_pick_string(v, sub);
	if (!inhalt)
		return;
	char key[strlen(name) + sizeof("/") + strlen(sub)];
	char *p = key, *n = name;
	while (*n)
		*p++ = tolower(*n++);
	*p++ = '/';
	n = sub;
	while (*n)
		*p++ = tolower(*n++);
	*p = 0;
	bdb_put(key, inhalt);
	free(inhalt);
}


static void build_node(char *name, struct vector *v, char *sub)
{
	if (!sub) {
		struct node *node = node_create(name, BF_TYPE_STRING);
		vector_put(v, "eintrag", node);
		return;
	}

	char key[strlen(name) + strlen("/") + strlen(sub) + 1];
	char *p = key, *n = name;
	while (*n)
		*p++ = tolower(*n++);
	*p = 0;
	strcat(key, "/");
	strcat(key, sub);

	char *val = bdballoc(key);
	if (val) {
		struct node *node;
		if (!strcmp(sub, "auth") ||
		    (!strcmp(sub, "type") && !*val)) {
			char *nil = malloc(sizeof("0"));
			assert(nil);
			strcpy(nil, "0");
			node = node_create(nil, BF_TYPE_STRING);
		} else
			node = node_create(val, BF_TYPE_STRING);
		vector_put(v, sub, node);
	}
}

int bdb_exists(char *name)
{
	char key[strlen(name) + strlen("/") + strlen("bot") + 1];
	char *p = key, *n = name;
	while (*n)
		*p++ = tolower(*n++);
	*p = 0;
	strcat(key, "/");
	strcat(key, "bot");

	DBT field = { 0 };
	field.data = key;
	field.size = strlen(key) + 1;

	DBT data = { 0 };

	int ret = dbp->get(dbp, 0, &field, &data, 0);
	if (ret) {
		if (ret == DB_NOTFOUND)
			return 0;

		dbp->err(dbp, ret, "✝ DB->get");
		assert(0);
	}
	return 1;
}

struct vector *bdb_load(char *name)
{
	int exists = bdb_exists(name);
	if (!exists)
		return 0;

	struct vector *vb = vector_create();
	build_node(name, vb, "auth");
	build_node(name, vb, "name");
	build_node(name, vb, 0);	// „eintrag“
	build_node(name, vb, "inhalt");
	build_node(name, vb, "zeit");
	build_node(name, vb, "bot");
	build_node(name, vb, "protected");
	build_node(name, vb, "channel");
	build_node(name, vb, "network");
	build_node(name, vb, "count");
	build_node(name, vb, "type");
	build_node(name, vb, "tag");
	build_node(name, vb, "lastcall");

	return vb;
}

void bdb_store(char *name, struct vector *v)
{
	store_node(name, v, "auth");
	store_node(name, v, "name");
	store_node(name, v, "inhalt");
	store_node(name, v, "zeit");
	store_node(name, v, "protected");
	store_node(name, v, "channel");
	store_node(name, v, "network");
	store_node(name, v, "count");
	store_node(name, v, "type");
	store_node(name, v, "tag");
	store_node(name, v, "lastcall");
	store_node(name, v, "eintrag");
	store_node(name, v, "bot");
}
