#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include <linux/list.h>
#include "botforth.h"

// MySQL-Krempel
extern MYSQL_FIELD *fld;
extern MYSQL mysql_read;
extern MYSQL mysql_write;
extern MYSQL_RES *res;
extern MYSQL_ROW row;

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
	return;			// TODO

	if (!sub) {
		printf("Eintrag „%s“\n", name);
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

#if 0
	char key[strlen(name) + strlen("/") + strlen("bot") + 1];
	strcpy(key, name);
	strcat(key, "/");
	strcat(key, "bot");
	printf("load_file: „%s“\n", key);
	int cdbret = cdb_find(&cdb, key, strlen(key));
	assert(cdbret != -1);
	if (cdbret == 0) {
		printf("Kein Eintrag „%s“.\n", name);
		return 0;
	}

	struct vector *vc = 0;	// vector_create();
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
#endif

	return v;
}


/*
  fp=fopen(buffer,"r");
  if(fp) {
    i=0;
    word=(char *)malloc(sizeof(char));
    while(feof(fp)==0) {
      word=realloc(word,sizeof(char)*(i+2));
      charbuffer=fgetc(fp);
      word[i]=charbuffer;
      i++;
    }
    word[i]='\0';
    fclose(fp);
    printf("programmtext: %s\n",word);
    program=botforth_parse(word);
  } else {
    printf("wort nicht gefunden\n");
    program=NULL;
  }
  */
