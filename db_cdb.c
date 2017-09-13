#include <cdb.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "def.h"
#include "node.h"
#include "vector.h"

struct cdb cdb;
unsigned pos;

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

int cdb_exists(char *name)
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

	return cdbret;
}

static char *name;
static char *sub;
static char *val;

static void split_key(char *key)
{
	name = sub = 0;

	if (!key || !*key)
		return;

	char tmp[strlen(key) + 1];

	char *slash = strrchr(key, '/');
	if (!slash || !*slash)
		return;

	char *p = tmp;
	while (key != slash)
		*p++ = *key++;
	*p = 0;

	name = strdup(tmp);
	sub = strdup(slash + 1);
}


static bool build_next()
{
	if (cdb_seqnext(&pos, &cdb) <= 0)
		return false;

	unsigned keylen = cdb_keylen(&cdb);
	char *key = malloc(keylen + 1);
	cdb_read(&cdb, key, keylen, cdb_keypos(&cdb));
	key[keylen] = 0;
	unsigned datalen = cdb_datalen(&cdb);
	val = malloc(datalen + 1);
	cdb_read(&cdb, val, datalen, cdb_datapos(&cdb));
	val[datalen] = 0;

	split_key(key);
	free(key);
	return true;
}

void cdb_loadall_init()
{
	cdb_seqinit(&pos, &cdb);
	if (name) {
		free(name);
		name = 0;
	}
	if (sub) {
		free(sub);
		sub = 0;
	}
	if (val) {
		free(val);
		val = 0;
	}

	build_next();
}



struct vector *cdb_loadall_next()
{
	if (!name)
		cdb_loadall_init();

	struct vector *v = vector_create();
	struct node *node = node_create(name, BF_TYPE_STRING);
	char cur_name[strlen(name) + 1];
	strcpy(cur_name, name);
	vector_put(v, "eintrag", node);

	while (!strcmp(cur_name, name)) {
		if (!sub)
			continue;
		node = node_create(val, BF_TYPE_STRING);
		vector_put(v, sub, node);
		if (!build_next())
			return 0;
	}

	return v;
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
	struct node *node = node_create(val, BF_TYPE_STRING);
	vector_put(v, sub, node);
}

struct vector *cdb_load(char *name)
{
	int cdbret = cdb_exists(name);
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

void uint32_unpack(const char s[4], uint32_t * u)
{
	uint32_t result;

	result = (unsigned char) s[3];
	result <<= 8;
	result += (unsigned char) s[2];
	result <<= 8;
	result += (unsigned char) s[1];
	result <<= 8;
	result += (unsigned char) s[0];

	*u = result;
}

struct vector *cdb_load_random()
{
	int guard;
	for (guard = 0; guard < 23; guard++) {
		int fd = open("calc.cdb.index", O_RDONLY);
		if (fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		off_t size = lseek(fd, 0, SEEK_END);
		assert(size > 0 && size % 4 == 0);

		int rndmax = min(RAND_MAX, size / 4);
		int rnd = rand() % rndmax;
		off_t off = rnd * 4 + 2048;
		lseek(fd, off, SEEK_SET);

		char buf_pos[4];
		read(fd, buf_pos, 4);
		uint32_t pos;
		uint32_unpack(buf_pos, &pos);
		printf("pos: %u\n", pos);
		close(fd);

		fd = open("calc.cdb", O_RDONLY);
		if (fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}
		off_t loc = lseek(fd, pos, SEEK_SET);
		assert(loc == pos);
		char buf_keylen[4];
		read(fd, buf_keylen, 4);
		char buf_datalen[4];
		read(fd, buf_datalen, 4);
		uint32_t keylen;
		uint32_unpack(buf_keylen, &keylen);
		uint32_t datalen;
		uint32_unpack(buf_datalen, &datalen);
		if (!datalen) {
			printf("nope.\n");
			continue;
		}
		char key[keylen + 1];
		read(fd, key, keylen);
		key[keylen] = 0;
		close(fd);
                printf("key: %s\n", key);
                split_key(key);
                printf("name: %s\n", name);
                struct vector *vc = cdb_load(name);
                if(vc)
                        return vc;
	}

        return 0;
}
