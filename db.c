#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include "vector.h"
#include "botforth.h"

extern void cdb_startup();
extern struct vector *cdb_load_random();
extern void bdb_init();
extern void bdb_store(char *name, struct vector *v);

void puthelp()
{
	printf("Argumente:\n"
	       "    --help                        Diese Hilfe\n"
	       "    --dump    [Eintrag]           Datenbankeintrag mit Spaltennamen anzeigen\n"
	       "    --dumpall                     Alle Datenbankeinträge (mit Spalten) zeigen\n"
	       "    --dumprandom                  Zufälligen Datenbankeintrag liefern\n"
	       "    --content [Eintrag]           Das Feld „Inhalt“ ausgeben\n"
	       "    --allcont                     Das Feld „Inhalt“ aller Einträge ausgeben\n"
	       "    --anlcont                     ^ mit „\\n“ statt „\\0“ als Trenner\n"
	       "    --write   [Eintrag] [Inhalt]  Datenbankeintrag schreiben\n");
}

static void build_node(char *name, struct vector *v, char *content)
{
	char *memp = malloc(strlen(content) + 1);
	assert(memp);
	strcpy(memp, content);
	struct node *node = node_create(memp, BF_TYPE_STRING);
	assert(name && *name);
	vector_put(v, name, node);
}

static int dump(int n, struct vector *v)
{
	printf("#%7d: ", n);
	debug(v);
	putchar('\n');
	return 0;
}

static int dump_content(int n, struct vector *v)
{
	(void) n;

	if (!v)
		return 0;

	char *inhalt = vector_pick_string(v, "inhalt");
	if (!inhalt)
		return 0;

	printf("%s", inhalt);
	putchar(0);
	return 0;
}

static int dump_content_newline(int n, struct vector *v)
{
	(void) n;

	if (!v)
		return 0;

	char *inhalt = vector_pick_string(v, "inhalt");
	if (!inhalt)
		return 0;

	printf("%s", inhalt);
	putchar('\n');
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		puthelp();
		return 1;
	}

	cdb_startup();
	bdb_init();

	char *eintrag = 0;

	for (;;) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", no_argument, 0, 'h'},
			{"dump", required_argument, 0, 'd'},
			{"dumpall", no_argument, 0, 'D'},
			{"dumprandom", no_argument, 0, 'r'},
			{"content", required_argument, 0, 'c'},
			{"allcont", no_argument, 0, 'C'},
			{"anlcont", no_argument, 0, 'N'},
			{"write", required_argument, 0, 'w'},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "d:c:hDrCNw:",
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
		case 'D':
			{
				iterate(dump);
				break;
			}
		case 'r':
			{
				struct vector *v = cdb_load_random();
				if (!v)
					break;
				char *eintrag =
				    vector_pick_string(v, "eintrag");
				char *inhalt =
				    vector_pick_string(v, "inhalt");
				if (!eintrag || !inhalt)
					break;
				printf("%s → „%s“\n", eintrag,
				       inhalt);
				break;
			}
		case 'c':
			{
				struct vector *v = load_file(optarg);
				if (!v)
					break;
				char *inhalt =
				    vector_pick_string(v, "inhalt");
				if (!inhalt)
					break;
				printf("%s", inhalt);
				break;
			}
		case 'C':
			{
				iterate(dump_content);
				break;
			}
		case 'N':
			{
				iterate(dump_content_newline);
				break;
			}
		case 'w':
			eintrag = optarg;
			break;
		case '?':
			break;
		}
	}

	if (eintrag) {
		if (optind >= argc) {
			fprintf(stderr, "Inhalt fehlt.\n");
			return 1;
		}

		char *inhalt = argv[optind];

		struct vector *v = vector_create();

		build_node("eintrag", v, eintrag);
		build_node("auth", v, "0");
		build_node("inhalt", v, inhalt);
		build_node("name", v, "db");
		build_node("bot", v, "db");
		build_node("network", v, "db");
		build_node("channel", v, "");
		build_node("zeit", v, "");
		build_node("type", v, "0");
		build_node("lastcall", v, "0");

		bdb_store(eintrag, v);

		vector_destroy(v);
	}

	return 0;
}
