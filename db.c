#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "vector.h"
#include "botforth.h"

extern void cdb_startup();
extern void bdb_init();

void puthelp()
{
	printf("Argumente:\n"
	       "        --help                    Diese Hilfe\n"
	       "        --dump    [Eintrag]       Datenbankeintrag mit Spaltennamen anzeigen\n"
	       "        --content [Eintrag]       Das Feld „Inhalt“ ausgeben\n");
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		puthelp();
		return 1;
	}

	cdb_startup();
	bdb_init();

	for (;;) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", no_argument, 0, 'h'},
			{"dump", required_argument, 0, 'd'},
			{"content", required_argument, 0, 'c'},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "d:c:h",
				    long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			puthelp();
			return 1;
		case 'd':
			{
				struct vector *v = load_file(optarg);
				debug(v);
				break;
			}
		case 'c':
			{
				struct vector *v = load_file(optarg);
                                if(!v)
                                        break;
				char *inhalt =
				    vector_pick_string(v, "inhalt");
				if (!inhalt)
					break;
				printf("%s", inhalt);
				break;
			}
		case '?':
			break;
		}
	}

	return 0;
}
