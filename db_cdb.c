#include <cdb.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "node.h"
#include "vector.h"

extern struct cdb cdb;

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

void build_node(char *name, struct vector *v, char *sub)
{
	if (!sub) {
		struct node *node = node_create(name, BF_TYPE_STRING);
		vector_put(v, "eintrag", node);
		return;
	}

	char key[strlen(name) + strlen("/") + strlen(sub) + 1];
	strcpy(key, name);
	strcat(key, "/");
	strcat(key, sub);

	char *val = cdballoc(key);
	if (val) {
		struct node *node;
		if (!strcmp(sub, "auth"))
			node = node_create("0", BF_TYPE_STRING);
		else if (!strcmp(sub, "type") && !*val)
			node = node_create("0", BF_TYPE_STRING);
		else
			node = node_create(val, BF_TYPE_STRING);
		vector_put(v, sub, node);
	}
}

struct vector *cdb_load(char *name)
{
	char key[strlen(name) + strlen("/") + strlen("bot") + 1];
	strcpy(key, name);
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
