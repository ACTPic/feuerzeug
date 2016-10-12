//ver 0.2 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "com.c"

// #include "resolv.c"

#define PORT 6667

void help()
{
	printf("Usage: ./irc <irc server>\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int _socket = 0;
	char eingabe[1000];
	int l = 0;
	pid_t _pid;

	if (argc != 2)
		help();

	printf("\ncreate socket...\n");
	_socket = resolve_ircd(argv[1]);
	printf("socketfd: %d\n", _socket);

	netsend(_socket, "USER arzt arzt.arzt.be arzt arzt\r\n");
	netsend(_socket, "NICK ArztTest\r\n");

	printf("aktiviere netrecv()\n");
	switch (_pid = fork()) {
	case -1:
		fprintf(stderr, "fehler in fork()\n");
		exit(-1);
		break;
	case 0:
		printf("kindprozess aktiv, aktiviere netrecv()\n");
		while (1) {
			printf("%s", netrecv(_socket));
		}
		exit(0);
		break;
	default:
		printf("elternprozess aktiv. kind hat pid %d\n", _pid);
		break;
	}

	printf("EINGABE (max. 1000b) - LEERE ZEILE FÜR QUIT.\n\n ");
	while (1) {
//                      fflush(stdin);
		gets(eingabe);
//                      printf("%s",eingabe);
		if ((l = strlen(eingabe)) == 0)
			break;
//                      printf("L=%d\n",l);
		eingabe[l] = '\r';
		eingabe[l + 1] = '\n';
		eingabe[l + 2] = '\0';
		netsend(_socket, eingabe);
	}
	printf("exit\n");


	kill(_pid, SIGINT);
	disconnect_irc(_socket);
	return 0;
}
