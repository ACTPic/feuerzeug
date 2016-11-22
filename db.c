#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "botforth.h"

extern void cdb_startup();
extern void bdb_init();

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "%s [Eintrag]\n", argv[0]);
		exit(1);
	}

	cdb_startup();
	bdb_init();

	struct vector *v = load_file(argv[1]);
	debug(v);

	return 0;
}
