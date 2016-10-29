#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include <linux/list.h>
#include "botforth.h"

// MySQL-Krempel
extern MYSQL mysql_read;

// CDB-Krempel
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
		struct node *node = node_create(val, BF_TYPE_STRING);
		vector_put(v, sub, node);
	}
}

extern struct vector *sql_load(char *name);

struct vector *load_file(char *name)
{
	struct vector *v;
	v = sql_load(name);

	printf("· load_file (SQL) „%s“:", name);
	if (v)
		debug(v);
	else
		puts("[nil]");
	puts("—————————————");

	char key[strlen(name) + strlen("/") + strlen("bot") + 1];
	strcpy(key, name);
	strcat(key, "/");
	strcat(key, "bot");
	printf("· load_file (CDB) „%s“:", key);

	int cdbret = cdb_find(&cdb, key, strlen(key));
	assert(cdbret != -1);
	if (!cdbret) {
		puts("[nil]");
		puts("—————————————");
		return v;
	}

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
	debug(vc);

	return vc;
}
