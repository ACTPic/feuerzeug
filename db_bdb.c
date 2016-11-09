#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <db.h>

DB *dbp;

void bdb_init()
{
	int ret;

	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		exit(EXIT_FAILURE);
	}
	if ((ret = dbp->open(dbp,
			     NULL, "calc.bdb", NULL, DB_BTREE, DB_CREATE,
			     0664)) != 0) {
		dbp->err(dbp, ret, "calc.bdb");
		exit(EXIT_FAILURE);
	}
}
