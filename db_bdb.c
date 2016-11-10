#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <db.h>
#include "vector.h"

static DB *dbp;

void bdb_init()
{
	int ret;

	if ((ret = db_create(&dbp, 0, 0)) != 0) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		exit(EXIT_FAILURE);
	}
	if ((ret = dbp->open(dbp,
			     0, "calc.bdb", 0, DB_BTREE, DB_CREATE,
			     0664)) != 0) {
		dbp->err(dbp, ret, "calc.bdb");
		exit(EXIT_FAILURE);
	}
}

static void bdb_put(char *key, char *val)
{
	assert(key);
	assert(val);

	DBT field, data;
	memset(&field, 0, sizeof(field));
	memset(&data, 0, sizeof(data));

	field.data = key;
	field.size = strlen(key) + 1;
	data.data = val;
	data.size = strlen(val) + 1;

	int ret = dbp->put(dbp, 0, &field, &data, 0);
	if (ret) {
		dbp->err(dbp, ret, "[BDB] DB->put");
		assert(0);
	}

	int cret = dbp->close(dbp, 0);
	assert(!cret);
	bdb_init();
	//fprintf(stderr, "[BDB] „%s“ → „%s“\n", key, val);

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
