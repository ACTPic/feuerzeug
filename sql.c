#include "node.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"
#include "botforth.h"

// CDB-Krempel
extern int cdb_exists(char *name);

// BDB-Krempel
extern int bdb_exists(char *name);
extern void bdb_store(char *name, struct vector *v);
extern void bdb_delete(char *name);

static char ripple[16384];

static char *build_node(char *name, struct vector *v, char **sqlp)
{
	char *p = *sqlp;
	if (!*p || *p == ')')
		return 0;
	if (*p == ',')
		p++;

	char *end = p;
	if (*p == '\'') {
		end = ++p;
		while (*end && *end != '\'')
			end++;
		if (!*end)
			*sqlp = end;
		else
			*sqlp = end + 1;
		*end = 0;
	} else {
		while (*end && *end != ',' && *end != ')')
			end++;
		if (!*end)
			*sqlp = end;
		else
			*sqlp = end + 1;
		*end = 0;
	}

	char *memp = malloc(strlen(p) + 1);
	assert(memp);
	strcpy(memp, p);
	struct node *node = node_create(memp, BF_TYPE_STRING);
	assert(name && *name);
	vector_put(v, name, node);
	return p;
}

static void insert_sql(char *s)
{
	if (*s++ != '(')
		return;

	struct vector *v = vector_create();

	char *eintrag = build_node("eintrag", v, &s);
	build_node("inhalt", v, &s);
	build_node("name", v, &s);
	build_node("bot", v, &s);
	build_node("network", v, &s);
	build_node("channel", v, &s);
	build_node("zeit", v, &s);
	build_node("type", v, &s);

	if (eintrag && *eintrag)
		bdb_store(eintrag, v);

	vector_destroy(v);
}

static void replace_sql(char *s)
{
	if (*s++ != '(')
		return;

	struct vector *v = vector_create();

	char *eintrag = build_node("eintrag", v, &s);
	build_node("inhalt", v, &s);
	build_node("name", v, &s);
	build_node("bot", v, &s);
	build_node("channel", v, &s);
	build_node("network", v, &s);
	build_node("zeit", v, &s);

	if (eintrag && *eintrag)
		bdb_store(eintrag, v);

	vector_destroy(v);
}

char *rip_query(char *orig_query)
{
	assert(orig_query);
	assert(strlen(orig_query) + 1 < 16383);

	char *buf = ripple;
	strcpy(buf, orig_query);

	char *p;

	const char *sq = "select * from calc where eintrag";
	const char *Rq = "select *,rand() as r from calc where type=0";
	const char *iq =
	    "insert into calc (eintrag,inhalt,name,bot,network,channel,zeit,type) ";
	const char *rc =
	    "replace into calc (eintrag,inhalt,name,bot,channel,network,zeit) ";
	const char *aa =
	    "select count(*) as anzahl from archiv where eintrag='";
	const char *uc = "update calc set count=count+1, lastcall='";
	const char *dc = "delete from calc where ";

	if (!strncmp(buf, sq, strlen(sq))) {
		p = buf + strlen(sq);
		while (*p == ' ')
			p++;
		if (*p == '=')
			p++;
		while (*p == ' ')
			p++;
		if (*p == '\'')
			p++;
		if (p[strlen(p) - 1] == '\'')
			p[strlen(p) - 1] = 0;
		while (*p && p[strlen(p) - 1] == '/')
			p[strlen(p) - 1] = 0;
		return p;
	} else if (!strncmp(buf, Rq, strlen(Rq))) {
		strcpy(ripple, "randcalc()");
		return ripple;
	} else if (!strncmp(buf, dc, strlen(dc))) {
		p = buf + strlen(dc);
		if (strncmp(p, "eintrag='", strlen("eintrag=")))
			return 0;
		p += strlen("eintrag='");
		if (p[strlen(p) - 1] == '\'')
			p[strlen(p) - 1] = 0;
		bdb_delete(p);
		return 0;
	} else if (!strncmp(buf, iq, strlen(iq))) {
		p = buf + strlen(iq);
		if (strncmp(p, "values ", strlen("values ")))
			return 0;
		p += strlen("values ");
		insert_sql(p);
		return 0;
	} else if (!strncmp(buf, rc, strlen(rc))) {
		p = buf + strlen(rc);
		if (strncmp(p, "values ", strlen("values ")))
			return 0;
		p += strlen("values ");
		replace_sql(p);
		return 0;
	} else if (!strncmp(buf, aa, strlen(aa))) {
		strcpy(ripple, "archivanzahl()");
		return ripple;
	} else if (!strncmp(buf, uc, strlen(uc))) {
		p = buf + strlen(uc);

		char *lastcall = malloc(strlen(p) + 1);
		assert(lastcall);
		strcpy(lastcall, p);
		while (isdigit(*p))
			p++;
		const char *we = "' where eintrag='";
		if (strncmp(p, we, strlen(we)))
			return 0;
		p += strlen(we);
		if (p[strlen(p) - 1] == '\'')
			p[strlen(p) - 1] = 0;
		while (*p && p[strlen(p) - 1] == '/')
			p[strlen(p) - 1] = 0;

		char *memp = malloc(strlen(p) + 1);
		assert(memp);
		strcpy(memp, p);
		struct vector *v = load_file(memp);
		if (!v)
			return 0;

		char *count = vector_pick_string(v, "count");
		assert(count);
		int icount = atoi(count);
		icount++;
		free(count);
		count = malloc(100);
		assert(count);
		sprintf(count, "%i", icount);
		vector_replace_string(v, "count", count);
		vector_replace_string(v, "lastcall", lastcall);
		bdb_store(p, v);
		vector_destroy(v);
		return 0;
	} else {
		printf("Unsupported SQL-Query: „%s“\n", buf);
		return 0;
	}
}
