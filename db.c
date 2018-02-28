// Kredite: Michael Tokarev (tinycdb),  Daniel Julius Bernstein (CDB)

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <sys/types.h>
#include "cdb.h"
#include "vector.h"
#include "botforth.h"


#define blen 2048
static unsigned char buf[blen];

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
	       "    --genindex_cdb                Indexdatei für CDB-Datenbank anlegen\n"
	       "    --genindex_cdb_cmd            Indexdatei für CDB-Datenbank anlegen, nur Kommandos\n"
	       "    --genindex_cdb_nocmd          Indexdatei für CDB-Datenbank anlegen, Kommandos rausgefiltert\n");
}

static void
fget(FILE * f, unsigned char *b, unsigned len, unsigned *posp,
     unsigned limit)
{
	if (posp && limit - *posp < len)
		exit(1);
	if (fread(b, 1, len, f) != len) {
		if (ferror(f))
			exit(1);
		fprintf(stderr, "unable to read: short file\n");
		exit(2);
	}
	if (posp)
		*posp += len;
}

static bool fskip_iscmd(FILE * fi, unsigned len, unsigned *posp,
			unsigned limit)
{
	while (len > blen) {
		fget(fi, buf, blen, posp, limit);
		len -= blen;
	}
	if (len) {
		fget(fi, buf, len, posp, limit);
		if (len != blen) {
			buf[len] = 0;
			char *cmdname =
			    strstr((char *) buf, "command/dope/");
			if (!cmdname)
				return false;
			return true;
			/*
			   unsigned char cmdfirstchr =
			   cmdname[strlen("command/dope/")];
			   if (!isalnum(cmdfirstchr))
			   return false;
			   if (strchr(cmdname, '_'))
			   return true;
			 */
		}
	}
	return false;
}

static void genindex_cdb()
{
	unsigned eod, klen, vlen;
	unsigned pos = 0;
	FILE *f, *fo;
	if (!(f = fopen("calc.cdb", "r")))
		exit(1);
	else if (!(fo = fopen("calc.cdb.index", "w")))
		exit(1);
	fget(f, buf, blen, &pos, blen);
	eod = cdb_unpack(buf);
	while (pos < eod) {
		cdb_pack(pos, buf);
		fwrite(buf, 4, 1, fo);
		fget(f, buf, 8, &pos, eod);
		klen = cdb_unpack(buf);
		vlen = cdb_unpack(buf + 4);
		fskip_iscmd(f, klen, &pos, eod);
		fskip_iscmd(f, vlen, &pos, eod);
	}
	if (fflush(fo) < 0 || pos != eod)
		exit(1);
}

static void genindex_cdb_nocmd()
{
	unsigned eod, klen, vlen;
	unsigned pos = 0;
	FILE *f, *fo;
	if (!(f = fopen("calc.cdb", "r")))
		exit(1);
	else if (!(fo = fopen("calc.cdb.index.nocmd", "w")))
		exit(1);
	fget(f, buf, blen, &pos, blen);
	eod = cdb_unpack(buf);
	while (pos < eod) {
		int savepos = pos;
		fget(f, buf, 8, &pos, eod);
		klen = cdb_unpack(buf);
		vlen = cdb_unpack(buf + 4);

		bool iscmd = fskip_iscmd(f, klen, &pos, eod);
		fskip_iscmd(f, vlen, &pos, eod);

		if (iscmd)
			continue;

		cdb_pack(savepos, buf);
		fwrite(buf, 4, 1, fo);
	}
	if (fflush(fo) < 0 || pos != eod)
		exit(1);
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
			{"genindex_cdb", no_argument, 0, 'g'},
			{"genindex_cdb_nocmd", no_argument, 0, 'G'},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "d:c:hdrCNw:g",
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
		case 'g':
			genindex_cdb();
			break;
		case 'G':
			genindex_cdb_nocmd();
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
