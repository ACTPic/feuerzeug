#include <cdb.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "def.h"
#include "node.h"
#include "vector.h"

struct cdb cdb;

void cdb_startup()
{
	int fd = open("calc.cdb", O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	cdb_init(&cdb, fd);
}

char *cdballoc(char *key)
{
	char *val = 0;
	if (cdb_find(&cdb, key, strlen(key)) > 0) {
		unsigned vpos = cdb_datapos(&cdb);
		unsigned vlen = cdb_datalen(&cdb);
		val = malloc(vlen + 1);
		if (!val)
			return 0;
		cdb_read(&cdb, val, vlen, vpos);
		val[vlen] = 0;
	}
	return val;
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

	char *val = cdballoc(key);
	if (val) {
		struct node *node;
		if (!strcmp(sub, "auth") ||
		    (!strcmp(sub, "type") && !*val)) {
			char *nil = malloc(1 + 1);
			assert(nil);
			strcpy(nil, "0");
			node = node_create(nil, BF_TYPE_STRING);
		} else
			node = node_create(val, BF_TYPE_STRING);
		vector_put(v, sub, node);
	}
}

struct vector *cdb_load(char *name)
{
	char key[strlen(name) + strlen("/") + strlen("bot") + 1];
	char *p = key, *n = name;
	while (*n)
		*p++ = tolower(*n++);
	*p = 0;
	strcat(key, "/");
	strcat(key, "bot");

	int cdbret = cdb_find(&cdb, key, strlen(key));
	assert(cdbret != -1);
	if (!cdbret)
		return 0;

	struct vector *vc = vector_create();
	build_node(name, vc, "auth");
	build_node(name, vc, "name");
	build_node(name, vc, 0);	// „eintrag“
	build_node(name, vc, "inhalt");
	build_node(name, vc, "zeit");
	build_node(name, vc, "bot");
	build_node(name, vc, "protected");
	build_node(name, vc, "channel");
	build_node(name, vc, "network");
	build_node(name, vc, "count");
	build_node(name, vc, "type");
	build_node(name, vc, "tag");
	build_node(name, vc, "lastcall");

	return vc;
}
