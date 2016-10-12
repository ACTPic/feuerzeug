#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "datenstapel.c"
#include "com.c"



enum bool { FALSE, TRUE };

int main(int argc, char **argv)
{
	char option, c, *puffer;
	FILE *fd;
	int i = 0, zeile = 0;

	enum bool _FILE = FALSE;
	enum bool _OPEN = FALSE;


	if (argc < 2) {
		fprintf(stderr, "\n%s -h ist dein freund\n", argv[0]);
		exit(1);
	}

	while ((option = getopt(argc, argv, "-fh")) != EOF) {
		i++;
		switch (option) {

		case 'f':
			_FILE = TRUE;
			break;

		case 'h':
			printf("\nSo gehts:\n"
			       "%s <parameter>\n\n"
			       "Argumente: -h        diese Hilfe aufrufen\n"
			       "           -f <file> inhalt der datei auswerten\n",
			       argv[0]);
			exit(0);
			break;

		case '?':
			fprintf(stderr,
				"\nfalscher schalter, %s -h ist dein freund",
				argv[0]);
			exit(1);
			break;

		case 1:
			if (_FILE == TRUE) {
				printf
				    ("versuche file. argc=%d i=%d argv=\n%s\n",
				     argc, i, argv[i]);
				if ((fd = fopen(argv[i], "r")) != NULL) {
					_OPEN = TRUE;
				} else {
					fprintf(stderr,
						"konnte datei nicht öffnen\n");
					exit(1);
				}
				i++;
			} else
				printf("Normaler String:\n%s\n", argv[i]);
			break;
		}
	}

	printf("\ngebe datei aus:\n");
	puffer = malloc(513 * sizeof(char));
	i = 0;

	while ((c = getc(fd)) != EOF) {
		puffer[i] = c;
		i++;
		if (c == '\n') {
			puffer[i] = '\0';
			printf("%d: <%s>", ++zeile, puffer);
			i = 0;
			continue;
		}
		if (i <= LINELENGTH)
			continue;

		puffer[i] = '\0';
		puffer[i - 1] = '\n';
		puffer[i - 2] = '\r';
		i = 0;
		break;
	}

/*	
	while ( (fscanf(fd,"%s\n", puffer)) != EOF ) {
		i++;
		printf("%d: <%s>",i,puffer);
	}
	
*/
	free(puffer);
	if (_OPEN == TRUE)
		fclose(fd);
	return 0;

}
