#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include "botforth.h"

// MySQL-Krempel
MYSQL_FIELD *fld;
MYSQL mysql_read;
MYSQL mysql_write;
MYSQL_RES *res;
MYSQL_ROW row;

// CDB-Krempel
struct cdb cdb;

int infoblock_get_accesslevel(struct vector *word)
{
	struct vector *infoblock;
	assert(word);
	infoblock = word->head->content;
	return *(int *) infoblock->head->content;
}

void infoblock_set_accesslevel(struct vector *word, int al)
{
	struct vector *infoblock;
	assert(word);
	infoblock = word->head->content;
	*(int *) infoblock->head->content = al;
}

char *infoblock_get_name(struct vector *word)
{
	struct vector *infoblock;
	assert(word);
	infoblock = word->head->content;
	return infoblock->head->next->content;
}

void infoblock_set_name(struct vector * word, char *name)
{
	struct vector *infoblock;
	assert(word);
	infoblock = word->head->content;
	// problem hier
	//(char*)infoblock->head->next->content=name;
	infoblock->head->next->content = name;
}

/*
 * welchen typ hat ein string in wirklichkeit? int? float? string?
 */
int get_stringtype(char *s)
{
	int z = 0;		// zustand
	unsigned i;
	char c;			// aktuelles zeichen
	int ret = -1;
	for (i = 0; i <= strlen(s); i++) {
		c = s[i];
		switch (z) {
		case 0:
			if (c == '\0')	// schon zuende, pech gehat
				ret = BF_TYPE_STRING;
			if (c == '-' || c == '+')
				z = 1;	// ein vorzeichen
			if (c >= '0' && c <= '9')
				z = 2;	// eine zahl beginnt
			if (z == 0)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 1:	// Vorzeichen
			if (c >= '0' && c <= '9')
				z = 2;	// eine zahl beginnt
			if (z == 1 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 2:	// Zahl nach Vorzeichen
			z = 0;
			if (c >= '0' && c <= '9')
				z = 2;	// die zahl geht weiter, gut.
			if (c == '\0')	// zuende? gut, int erkannt.
				ret = BF_TYPE_INT;
			if (c == '.')
				z = 17;	// koennte floating-point sein.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent von floating-point zahl
			if (z == 0 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 17:	// floating-point, direkt nach dem .
			if (c >= '0' && c <= '9')
				z = 18;	// ist wohl doch noch ne ziffer, prima.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent erkannt
			if (c == '\0')	// schon vorbei? das reicht nicht.
				ret = BF_TYPE_STRING;
			if (z == 17 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 18:	// ziffern nach dem .
			z = 0;
			if (c >= '0' && c <= '9')
				z = 18;	// ist wohl doch noch ne ziffer, prima.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent erkannt
			if (c == '\0')
				ret = BF_TYPE_FLOAT;	// prima, float erkannt
			if (z == 0 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 19:	// floating-point: e
			z = 0;	// wenns sonst nichts ist, ists ein string
			if (c == '+' || c == '-')
				z = 20;	// ein vorzeichen
			if (c >= '0' && c <= '9')
				z = 21;	// schoen, eine ziffer
			if (z == 0)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 20:	// floating-point: vorzeichen nach e
			z = 0;	// wie immer, wenns sonst nix ist, ists ein string
			if (c >= '0' && c <= '9')
				z = 21;	// schoen, eine ziffer
			if (z == 0 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		case 21:	// floating-point: ziffern nach e
			z = 0;	// wenns sonst nichts ist, ists ein befehl
			if (c >= '0' && c <= '9')
				z = 21;	// gut, noch immer eine ziffer
			if (c == '\0')
				ret = BF_TYPE_FLOAT;
			if (z == 0 && ret == -1)	// nichts gefunden? schade.
				ret = BF_TYPE_STRING;
			break;
		}
		if (ret > -1) {
			return ret;
		}
	}
	return BF_TYPE_STRING;
}

/*
 * Lexikalische Analyse eines botforth-programms
 *
 * progstring: Das Programm als String
 *
 * return: vector, der die einzelnen Symbole des Programms enthält
 */
struct vector *botforth_parse(char *progstring)
{
	struct vector *program = vector_create();
	struct vector *infoblock = vector_create();
	int lineend(char c) {
		if (c == '\r' || c == '\n' || c == '\0')
			return 1;
		return 0;
	}
	int whitespace(char c) {
		if (c == ' ' || c == '\t' || lineend(c))
			return 1;
		return 0;
	}
	void addsymbol(char *progstring, int *first, int last, int type) {
		struct node *symb = 0;
		char *item;
		int itemlen;
		int *i;
		float *f;
		printf("neues Symbol\n");
		itemlen = last - (*first) + 1;
		//assert(itemlen>0);
		item = malloc(itemlen + 1);
		assert(item != 0);
		strncpy(item, progstring + (*first) - 1, itemlen);
		item[itemlen] = 0;
		//printf("Symbol erkannt: %s (%i)\n",item,type);
		switch (type) {
		case BF_TYPE_STRING:
		case BF_TYPE_C_TEXT:
			symb = node_create(item, type);
			break;
		case BF_TYPE_INT:
			i = malloc(sizeof(int));
			*i = atoi(item);
			symb = node_create(i, type);
			free(item);
			break;
		case BF_TYPE_FLOAT:
			f = malloc(sizeof(float));
			*f = atof(item);
			symb = node_create(f, type);
			free(item);
			break;
		}

		vector_push(program, symb);
		*first = last + 1;
	}
	int z = 0;		// Zustand
	char c;			// Aktuelles Zeichen
	unsigned i;
	int first;
	char item[255];
	char *temps;
	int *tempi;
	struct node *tempn;
	int temp;
	// create infoblock
	tempi = malloc(sizeof(int));
	*tempi = 99;
	tempn = node_create(tempi, BF_TYPE_INT);
	vector_put(infoblock, "accesslevel", tempn);
	temps = malloc(7);
	sprintf(temps, "noname");
	tempn = node_create(temps, BF_TYPE_STRING);
	vector_put(infoblock, "name", tempn);
	tempi = malloc(sizeof(int));
	*tempi = 0;
	tempn = node_create(tempi, BF_TYPE_INT);
	vector_put(infoblock, "clearcache", tempn);
	// add program infoblock to the programm
	vector_put_vector(program, "infoblock", infoblock);
	printf("start parser\n");
	strcpy(item, "");
	first = 1;
	for (i = 0; i <= strlen(progstring); i++) {
		c = progstring[i];
		switch (z) {
		case 0:
			z = 2;	// wenns sonst nix ist, ists ein Befehl
			if ((c >= '0') && (c <= '9'))	// Vermutlich Zahl, oder Befehl
				z = 1;
			if (((c >= 'a') && (c <= 'z')) ||	// Befehl
			    (c == '.'))
				z = 2;
			if ((c == '+') || (c == '-'))	// Vorzeichen einer Zahl oder Befehl
				z = 4;
			if (c == '"')	// Zeichenkette
				z = 5;
			if (c == '{')	// Befehlszeiger
				z = 6;
			if (whitespace(c))	// Leerzeichen ist nix
				z = 0;
			if (c == '/')	// Kommentar-Beginn oder Befehl
				z = 8;
			if (c == '$')	// Variable lesen
				z = 12;
			if (c == '!')	// Variable schreiben
				z = 14;
			break;
		case 1:	// Zahl
			// davon ausgehen, dass es ein befehl wird
			z = 2;
			if ((c >= '0') && (c <= '9'))
				z = 1;	// bleibt eine zahl
			if (c == '.')
				z = 17;	// koennte floating-point sein.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent von floating-point zahl
			if (whitespace(c)) {
				addsymbol(progstring, &first, i,
					  BF_TYPE_INT);
				z = 0;	// zahl fertig erkannt
			}
			break;
		case 2:	// Befehl
			if (whitespace(c)) {
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
				z = 0;
			}
			break;
		case 3:	// Zeichenkette
			if (c == '"') {
				addsymbol(progstring, &first, i,
					  BF_TYPE_STRING);
				z = 0;
			}
			break;
		case 4:	// Vorzeichen
			// wenns sonst nichts ist, ists ein Befehl
			z = 2;
			if ((c >= 'a') && (c <= 'z'))	// Befehl
				z = 2;
			if ((c >= '0') && (c <= '9'))	// Zahl (mit Vorzeichen) oder Befehl
				z = 1;
			if (whitespace(c)) {	// Befehl erkannt
				z = 0;
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
			}
			break;
		case 5:	// führendes " einer Zeichenkette
			// Zeichenkette vielleicht sofort schon wieder zuende?
			if (c == '"') {
				addsymbol(progstring, &first, i,
					  BF_TYPE_STRING);
				z = 0;
			} else
				z = 3;
			break;
		case 6:	// Führendes { eines Befehlszeigers
			z = 7;
			break;
		case 7:	// Befehlszeiger
			if (c == '}') {
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
				z = 0;
			}
			break;
		case 8:	// Kommentar-Beginn
			if (c == '*') {
				z = 9;	// Ja, wirklich ein Kommentar-Beginn, also weitermachen
				first++;
			} else if (c == '/') {	// ein Zeilenkommentar
				z = 11;
				first++;
			} else if (whitespace(c)) {
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
				z = 0;
			} else
				z = 2;
			break;
		case 9:	// Im Kommentar
			first++;
			if (c == '*')
				z = 10;	// Vielleicht hört der Kommentar ja nun auf...
			break;
		case 10:	// Kommentar-Ende
			first++;
			if (c == '/')
				z = 0;
			else
				z = 9;
			break;
		case 11:	// Zeilenkommentar
			first++;
			if (lineend(c)) {
				z = 0;
			}
			break;
		case 12:	// Variable lesen, $-Zeichen
			z = 13;
			first++;
			break;
		case 13:	// Variable lesen, Variablenname
			if (whitespace(c)) {
				temp = 1;
				addsymbol(progstring, &first, i,
					  BF_TYPE_STRING);
				addsymbol("$", &temp, 1, BF_TYPE_C_TEXT);
				z = 0;
			}
			break;
		case 14:	// Variable schreiben, !-Zeichen
			if (c == '=') {
				// koennte sich um != statt um ne variable handeln...
				z = 16;
			} else if (whitespace(c) || c == '=') {
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
				z = 0;
			} else {
				z = 15;
				first++;
			}
			break;
		case 15:	// Variable schreiben, Variablenname
			if (whitespace(c)) {
				temp = 1;
				addsymbol(progstring, &first, i,
					  BF_TYPE_STRING);
				addsymbol("!", &temp, 1, BF_TYPE_C_TEXT);
				z = 0;
			}
			break;
		case 16:	// womoeglich != statt einem Variablen-schreiben
			if (whitespace(c)) {
				addsymbol(progstring, &first, i,
					  BF_TYPE_C_TEXT);
				z = 0;
			} else {
				z = 15;	// wohl doch nur ein Variablen-lesen, wo aber ein = im Variablenname ist...
			}
			break;
		case 17:	// floating-point, direkt nach dem .
			z = 2;	// wenns sonst nichts ist, ists ein befehl
			if (c >= '0' && c <= '9')
				z = 18;	// ist wohl doch noch ne ziffer, prima.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent erkannt
			if (whitespace(c)) {	// leerzeichen direkt nach .? kann nur befehl sein
				addsymbol(progstring, &first, i, BF_TYPE_C_TEXT);	// befehl
				z = 0;
			}

			break;
		case 18:	// ziffern nach dem .
			z = 2;	// wenns sonst nichts ist, ists ein befehl
			if (c >= '0' && c <= '9')
				z = 18;	// ist wohl doch noch ne ziffer, prima.
			if (c == 'e' || c == 'E')
				z = 19;	// exponent erkannt
			if (whitespace(c)) {
				addsymbol(progstring, &first, i, BF_TYPE_FLOAT);	// prima, float erkannt
				z = 0;
			}
			break;
		case 19:	// floating-point: e
			z = 2;	// wenns sonst nichts ist, ists ein befehl
			if (c == '+' || c == '-')
				z = 20;	// ein vorzeichen
			if (c >= '0' && c <= '9')
				z = 21;	// schoen, eine ziffer
			if (whitespace(c)) {	// leerzeichen direkt nach e? kann nur befehl sein
				addsymbol(progstring, &first, i, BF_TYPE_C_TEXT);	// befehl
				z = 0;
			}
			break;
		case 20:	// floating-point: vorzeichen nach e
			z = 2;	// wie immer, wenns sonst nix ist, ists ein befehl
			if (c >= '0' && c <= '9')
				z = 21;	// schoen, eine ziffer        
			if (whitespace(c)) {	// leerzeichen direkt nach vorzeichen? kann nur befehl sein
				addsymbol(progstring, &first, i, BF_TYPE_C_TEXT);	// befehl
				z = 0;
			}
			break;
		case 21:	// floating-point: ziffern nach e
			z = 2;	// wenns sonst nichts ist, ists ein befehl
			if (c >= '0' && c <= '9')
				z = 21;	// gut, noch immer eine ziffer
			if (whitespace(c)) {
				addsymbol(progstring, &first, i, BF_TYPE_FLOAT);	// prima, float erkannt
				z = 0;
			}
			break;
		}
		if ((z == 0) || (z == 5) || (z == 6))
			first++;
	}
	return program;
}


struct vector *word_load(char *name, char *ns)
{
	char *word;
	char *buffer, *buffer2;
	char *tmpns;
	struct vector *program;
	struct node *lex;
	int newaccesslevel;
	int *tempint;
	struct vector *v, *infoblock;
	struct node *n = NULL, *n2 = NULL;
	// todo:  ueberpruefen, ob lexicon leer ist
	assert(lexicon);
	assert(name);
	buffer = NULL;
	v = NULL;
	lex = lexicon->tail;
	do {
		if (buffer)
			free(buffer);
		if (ns == NULL) {
			// search all namespaces
			buffer =
			    malloc(10 + strlen(lex->content) +
				   strlen(name));
			sprintf(buffer, "command/%s/%s", (char *)lex->content,
				name);
			tmpns = lex->content;
		} else {
			// only use given namespace
			buffer = malloc(10 + strlen(ns) + strlen(name));
			sprintf(buffer, "command/%s/%s", ns, name);
			tmpns = ns;
		}
		printf("lade wort: %s\n", buffer);

		// set new accesslevel depending on the name of the namespace
		newaccesslevel = 3;
		if (strcmp(tmpns, "lib") == 0)
			newaccesslevel = 2;
		if (strcmp(tmpns, "service") == 0)
			newaccesslevel = 2;
		if (strcmp(tmpns, "bot") == 0)
			newaccesslevel = 1;
		// is the word in the cache?
		n = vector_pick(words, buffer);
		if (n) {
			if (n->type != BF_TYPE_INT) {
				if (n->type == BF_TYPE_VECTOR) {
					v = n->content;
					printf
					    ("wort %s im cache gefunden\n",
					     buffer);
				} else {
					printf
					    ("falscher typ auf dem word-cache\n");
					exit(1);
				}
			} else {
				printf("Nur Dummy im Cache gefunden\n");
			}
		} else {
			printf("wort muss von platte geholt werden\n");
			v = load_file(buffer);
			if (v == NULL) {
				// Falls das Programm nicht gefunden wurde: Einen Dummy in den Cache legen
				tempint = malloc(sizeof(int));
				*tempint = 0;
				n2 = node_create(tempint, BF_TYPE_INT);
				vector_put(words, buffer, n2);
			}
		}
		lex = lex->prev;
	} while (v == NULL && lex != NULL && ns == NULL);
	if (v) {		// sollte immer erfuellt sein
		// ist schon ein fertig kompiliertes programm enthalten?
		n = vector_pick(v, "program");
		if (n) {
			if (n->type == BF_TYPE_VECTOR) {
				printf
				    ("vorkompiliertes programm gefunden\n");
				program = n->content;
				// soll das programm aus dem cache geloescht werden?
				assert(program->head != NULL);	// todo: kann weg, leere programme sind ok
			} else {
				printf("cache-word ist kein vector\n");
				exit(1);
			}
		} else {
			word = vector_pick_string(v, "inhalt");
			assert(word != NULL);
			program = botforth_parse(word);
			// infoblock ergaenzen
			infoblock =
			    (struct vector *) program->head->content;
			buffer2 = malloc(strlen(buffer) + 1);
			sprintf(buffer2, "%s", buffer);
			n2 = node_create(buffer2, BF_TYPE_STRING);
			vector_put(infoblock, "cachename", n2);
			buffer2 = malloc(strlen(name) + 1);
			sprintf(buffer2, "%s", name);
			free(infoblock->head->next->content);
			infoblock->head->next->content = buffer2;
			// update accesslevel in the infoblock
			infoblock_set_accesslevel(program, newaccesslevel);
			// update program name
			buffer2 = malloc(strlen(name) + 1);
			sprintf(buffer2, "%s", name);
			infoblock_set_name(program, buffer2);
			if (program->head == NULL) {
				printf("leeres programm: %s\n", word);
			}
			// todo: speicherleck?
			if (program) {
				program = botforth_compile(program);
				if (program->head) {
					// todo: und was wenn nicht? speicherleck?
					printf
					    ("Wort im cache speichern\n");
					printf("name: %s\n", buffer);
					buffer2 = malloc(8);
					sprintf(buffer2, "program");
					vector_put_vector(v, buffer2,
							  program);
					vector_replace_vector(words,
							      buffer, v);
				}
			}
		}
		return program;
	} else {
		if (buffer)
			free(buffer);
	}
	return NULL;
}

void botforth_replaceword(struct vector *program)
{
	char *cachename, *newcachename;
	char *shortname;
	struct node *n, *newn;
	struct node *oldcache;
	struct vector *oldcachecontent;
	struct node *newcache;
	struct vector *newcachecontent;
	struct vector *newprogram;
	// Programmname ermitteln
	cachename =
	    (char *) ((struct vector *) program->head->content)->head->
	    next->next->next->content;
	shortname =
	    (char *) ((struct vector *) program->head->content)->head->
	    next->content;
	printf("name des zu ersetzenden programs %s bzw. %s\n", cachename,
	       shortname);
	// den kompletten Cache-Eintrag zu dem Programm laden
	oldcache = vector_get(words, cachename);
	oldcachecontent = (struct vector *) oldcache->content;
	// altes programm entfernen, aber noch im speicher belassen, wird noch
	// durch *program referenziert
	vector_get(oldcachecontent, "program");
	// rest des alten cacheeintrags komplett loeschen
	node_destroy(oldcache);
	newprogram = word_load(shortname, NULL);
	if (newprogram) {
		// wort existiert noch auf platte
		newcachename =
		    (char *) ((struct vector *) program->head->content)->
		    head->next->next->next->content;
		// word_load hat das neue wort auch in den cache geworfen
		newcache = vector_pick(words, newcachename);
		assert(newcache != NULL);
		newcachecontent = (struct vector *) newcache->content;
		// Das neue Programm aus den Cacheeintrag rausnehmen, bleibt durch
		// *newprogram verzeigert
		vector_get(newcachecontent, "program");
		// Das alte Programm erstmal in den neuen Cacheeintrag schieben
		n = node_create(program, BF_TYPE_VECTOR);
		// todo: irgendwas stimmt hier nicht, abstuerze an dieser stelle
		vector_replace(newcachecontent, "program", n);
	} else {
		// programm existiert nicht mehr auf platte. nun wenigstens
		// einen platzhalter anlegen
		// todo: das sollte man auch mal wirklich richtig machen...
		newprogram = vector_create();
		printf("programm existiert nicht mehr!\n");
		exit(0);
	}

	// den _inhalt_ des alten programms manuell loeschen
	n = program->head;
	while (n) {
		newn = n->next;
		node_destroy(n);
		n = newn;
	}
	// Zeiger anpassen
	program->head = newprogram->head;
	program->tail = newprogram->tail;
	program->size = newprogram->size;
}

void botforth_call(struct vector *program)
{
	assert(program != NULL);
	// int die tiefenbegrenzung noch nicht erreicht?
	//if(rstack->size<50) {
	vector_push_node(rstack, pc);
	vector_push_vector(rstack, cword);
	vector_push_int(rstack, accesslevel);
	// soll das programm aus dem cache entfernt werden?
	if (*(int *) ((struct vector *) program->head->content)->head->
	    next->next->content == 1) {
		printf("aufruf eines zu loeschenden programms\n");
		botforth_replaceword(program);
	}
	// set the accesslevel
	//printf("acceslevel: %i\n",infoblock_get_accesslevel(program));
	accesslevel = infoblock_get_accesslevel(program);
	// set the program counter to the new word
	pc = program->head->next;
	cword = program;
	//}
}

void botforth_return()
{
	int oldaccesslevel;
	// todo: speicherleck vielleicht irgendwie?
	oldaccesslevel = vector_pop_int(rstack);
        (void)oldaccesslevel;  // Warnung unterdrücken
	cword = vector_pop_vector(rstack);
	pc = vector_pop_node(rstack);
	if (cword)
		accesslevel = infoblock_get_accesslevel(cword);
	//printf("ruecksprung\n");
}

void botforth_interpreter(struct vector *program, int withinfoblock)
{
	char *command;
	void *temppointer;
	char *temp;
	char *temp2;
	int (*fpointer) ();	// function pointer
	struct vector *newprogram;	// temp var for loading new words;
	struct node *tempnode;
	struct node *c;		// pointer to the actual command
	cword = program;
	pc = program->head;
	if (pc == NULL)
		return;
	if (withinfoblock)
		pc = pc->next;
	if (pc == NULL)
		return;
	do {
		printf(".");
		// increase the program counter and load the new word
		c = pc;
		assert(c != NULL);
		pc = pc->next;
		assert(c->content != NULL);
		// printf("'%s'(%i) ",node_toString(node_copy(c)),c->type);
		switch (c->type) {
		case BF_TYPE_C_TEXT:	// Befehl in Textform
			command = c->content;
			//printf("textbefehl: %s\n",command);
			if (strlen(command) > 1 && command[0] == '_'
			    && command[1] != '_') {
				temp2 = infoblock_get_name(cword);
				printf("hauptfunktionsname: %s\n", temp2);
				temp =
				    malloc(strlen(temp2) +
					   strlen(command) + 1);
				sprintf(temp, "%s%s", temp2, command);
				// free(command);  // temp2 muss nicht geloescht werden
				command = temp;	// problem, wo wird command freigegeben?
				// unterfunktionsaufruf
			}
			temppointer = getintcommand(command);
			if (temppointer) {
				free(c->content);
				c->type = BF_TYPE_C_INT;
				c->content = temppointer;
				pc = c;
			} else {
				printf("neues wort laden: %s\n", command);
				// load and execute a word
				newprogram = word_load(command, NULL);
				printf("programm geladen: %s\n", command);
				if (newprogram) {
					// call the word
					botforth_call(newprogram);
					// change the program
					printf("setze externer befehl\n");
					free(c->content);
					c->content = newprogram;
					c->type = BF_TYPE_C_EXT;
				}
			}
			break;
		case BF_TYPE_C_INT:	// Interner Befehl
			fpointer = c->content;
			fpointer();
			break;
		case BF_TYPE_C_EXT:	// Externer Befehl
			//printf("externer befehl\n");
			if (rstack->size < 8000) {
				botforth_call((struct vector *) c->
					      content);
			} else {
				printf("RSTACK UEBERGELAUFEN!\n");
			}
			break;
		case BF_TYPE_STRING:	// String
			temp = malloc(strlen(c->content) + 1);
			sprintf(temp, "%s", (char *)c->content);
			tempnode = node_create(temp, c->type);
			vector_push(dstack, tempnode);
			break;
		case BF_TYPE_INT:	// Integer
		case BF_TYPE_FLOAT:	// Float
		case BF_TYPE_POINTER:	// Pointer
			tempnode = node_copy(c);
			vector_push(dstack, tempnode);
			break;
		default:	// Unbekannt (todo!)
			printf("unbekanntes symbol im programm: %i\n",
			       c->type);
			exit(0);
			break;
		}
		// end of the word? have a look at the return stack...
		while ((pc == NULL && rstack->tail != NULL) ||
		       (accesslevel > 1 && runlimit > 0
			&& (int) time(NULL) > runlimit)) {
			botforth_return();
		}
	} while (pc != NULL);
	printf("Interpreter ende\n");
}

struct vector *botforth_compile(struct vector *word)
{
	struct node *n, *newn, *cn, *newcn;
	char *buffer;
//	char *text;
	struct vector *compword;
//	struct vector *ncstack;
	struct vector *nword = vector_create();
	printf("starte compiler\n");
	assert(cstack != NULL);
	if (word == NULL)
		return NULL;
	n = word->head;
	while (n != NULL) {
		newn = n->next;
		if (n->type == BF_TYPE_C_TEXT) {
			if (getintcommand(n->content)) {
				// internal command
				// todo: hier direkt die adresse reinsetzen
				vector_push(nword, n);
			} else {
				// external command
				// save the environment to estack
				//vector_push_vector(estack,lexicon);
				vector_push_vector(estack, rstack);
				vector_push_node(estack, pc);
				vector_push_vector(estack, cword);
				// create a new environment
				//lexicon=vector_create();
				buffer = malloc(9);
				sprintf(buffer, "compiler");
				vector_push_string(lexicon, buffer);
				rstack = vector_create();
				// try to load the compiler-word
				compword =
				    word_load(n->content, "compiler");
				// restore the old environment
				cword = vector_pop_vector(estack);
				pc = vector_pop_node(estack);
				vector_destroy(rstack);
				rstack = vector_pop_vector(estack);
				assert(rstack != NULL);
				/*
				   vector_destroy(lexicon);
				   lexicon=vector_pop_vector(estack);
				 */
				node_destroy(vector_pop(lexicon));
				assert(lexicon != NULL);
				if (compword) {
					assert(compword->head != NULL);	// todo: das sollte weg, leere programme sind ok
					printf("kompiliere %s\n",
					       (char *)n->content);
					// save the environment to estack
					assert(cstack != NULL);
					vector_push_vector(estack, cstack);
					vector_push_vector(estack, rstack);
					vector_push_node(estack, pc);
					vector_push_vector(estack, cword);
					// create a new environment
					cstack = vector_create();
					rstack = vector_create();
					// start the interpreter
					printf("start interpreter\n");
					botforth_interpreter(compword, 1);
					printf
					    ("den neuen teil nochmal kompilieren\n");
					cstack = botforth_compile(cstack);
					// copy the cstack to the new word
					printf("umkopieren\n");
					cn = cstack->head;
					while (cn) {
						assert(cn != NULL);
						newcn = cn->next;
						vector_push(nword, cn);
						//newn=cn;
						//vector_insert(word,n,cn);
						cn = newcn;
					}
					// restore the old environment
					cword = vector_pop_vector(estack);
					pc = vector_pop_node(estack);
					vector_destroy(rstack);
					rstack = vector_pop_vector(estack);
					assert(rstack != NULL);
					cstack->head = cstack->tail = NULL;
					vector_destroy(cstack);
					cstack = vector_pop_vector(estack);
					assert(cstack != NULL);
					node_destroy(n);
				} else {
					vector_push(nword, n);
				}
			}
		} else {
			vector_push(nword, n);
		}
		n = newn;
	}
	free(word);
	return nword;
}


int main(int argc, char **argv)
{
	//struct vector *rstack=vector_create();
	struct vector *program;
	char *temp;
	int i;
	words = vector_create();
	cstack = vector_create();
	rstack = vector_create();
	dstack = vector_create();
	estack = vector_create();
	lstack = vector_create();
	// help stacks
	zstack = vector_create();
	var = vector_create();
	lexicon = vector_create();
	interpretcounter = 0;
	// zufall initialisieren
	srand(time(NULL));

	// mysql-verbindung aufbauen
	mysql_init(&mysql_read);
	if (!mysql_real_connect
	    (&mysql_read, "127.0.0.1", "feuerzeugread", "amoklauf", NULL,
	     0, NULL, 0)) {
		printf("mysql read verbindungsaufbau gescheitert.\n");
		//ptr = mysql_error(&mysql);
		//problem(ptr);
		exit(1);
	}
	// Datenbank auswaehlen
	if (mysql_select_db(&mysql_read, "calc")) {
		printf("mysql read datenbankwahl gescheitert.\n");
		exit(1);
	}
	mysql_init(&mysql_write);
	if (!mysql_real_connect
	    (&mysql_write, "127.0.0.1", "feuerzeug", "slightomat", NULL, 0,
	     NULL, 0)) {
		printf("mysql write verbindungsaufbau gescheitert.\n");
		//ptr = mysql_error(&mysql);
		//problem(ptr);
		exit(1);
	}
	// Datenbank auswaehlen
	if (mysql_select_db(&mysql_write, "calc")) {
		printf("mysql write datenbankwahl gescheitert.\n");
		exit(1);
	}

        int fd = open("calc.cdb", O_RDONLY);
        if(fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
        }
        cdb_init(&cdb, fd);

	// push parameters to dstack
	for (i = 1; i < argc; i++) {
		temp = malloc(strlen(argv[i]) + 1);
		sprintf(temp, "%s", argv[i]);
		vector_push_string(dstack, temp);
	}

	// set lexicon
	//temp=malloc(5); sprintf(temp,"dope");
	//vector_push_string(lexicon,temp);
	temp = malloc(strlen("lib")+1);
	sprintf(temp, "lib");
	vector_push_string(lexicon, temp);
	temp = malloc(strlen("service")+1);
	sprintf(temp, "service");
	vector_push_string(lexicon, temp);
	temp = malloc(strlen("bot")+1);
	sprintf(temp, "bot");
	vector_push_string(lexicon, temp);

	program = word_load("main", "bot");
	debug(program);
	botforth_interpreter(program, 1);
	printf("Datenstapel: ");
	debug(dstack);

        return 0;
}
