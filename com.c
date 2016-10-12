// com.c - v0.3.1 

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define LINELENGTH 900
#define PORT 6667

int set_socket_timeout(int connectSocket, int milliseconds)
{
	struct timeval tv;
	tv.tv_sec = milliseconds / 1000;
	tv.tv_usec = (milliseconds % 1000) * 1000;
	return setsockopt(connectSocket, SOL_SOCKET, SO_RCVTIMEO,
			  (char *) &tv, sizeof tv);
}


// *************************************************************
// stellt verbindung zum ircd her
// int port = ircd-port
// char *ipnum = ipnummer des ircd in xxx.xxx.xxx.xxx -notation
// return -1 = fehler beim socket-erstellen
// return -2 = fehler beim connecten
// return num>0 = socketfd
// 
int connect_irc(int port, char *ipnum)
{
	int create_socket = 0;
	struct sockaddr_in address;

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0)
		printf("The Socket was created\n");
	else {
		fprintf(stderr, "Unable to create socket\n");
		return -1;
	}

	address.sin_family = AF_INET;

	assert(port >= 1 && port <= 65535);
	address.sin_port = htons(port);
	inet_pton(AF_INET, ipnum, &address.sin_addr);

	if (connect
	    (create_socket, (struct sockaddr *) &address,
	     sizeof(address)) == 0)
		printf("The connection was accepted by %s...\n",
		       inet_ntoa(address.sin_addr));
	else {
		fprintf(stderr,
			"The connection wasn't accepted by %s...\n",
			inet_ntoa(address.sin_addr));
		return -2;
	}

	set_socket_timeout(create_socket, 10);
	return create_socket;
}



// *************************************************************
// stellt verbindung zum IPv6 ircd her
// int port = ircd-port
// char *ipnum = ipnummer des ircd in 
// xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx -notation
// return -1 = fehler beim socket-erstellen
// return -2 = fehler beim connecten
// return num>0 = socketfd
// 
int connect_irc6(int port, char *ipnum)
{
	int create_socket = 0;
	struct sockaddr_in6 address;

	if ((create_socket = socket(AF_INET6, SOCK_STREAM, 0)) > 0)
		printf("The Socket was created %s\n", ipnum);
	else {
		fprintf(stderr, "Unable to create socket\n");
		return -1;
	}

	address.sin6_family = AF_INET6;

	assert(port >= 1 && port <= 65535);
	address.sin6_port = htons(port);
	inet_pton(AF_INET6, ipnum, &address.sin6_addr);

	if (connect
	    (create_socket, (struct sockaddr *) &address,
	     sizeof(address)) == 0)
		printf("The connection was accepted by %s...\n", ipnum);
	else {
		fprintf(stderr,
			"the connection wasn't accepted by %s...\n",
			ipnum);
		return -2;
	}

	set_socket_timeout(create_socket, 10);
	return create_socket;
}

// **************************************************************
// schliesst den socket / beendet die Verbindung
//
int disconnect_irc(int socketnum)
{
	return (close(socketnum));
}

// ***************************************************************
// sendet einen string an den ircd, 
//      zu lange strings werden nach stelle LINELENGTH-2 abgeschnitten
//      und nen CR-LF (also \r\n) angefügt
// int socketnum = socket-handler
// char *msg = zu sendender string (1-LINELENGTH byte)
// return -2 = string leer
// return -1 = fehler in send()
// return num>0 = übertragene bytes     
//
int netsend(int socketnum, char *msg)
{
	int len = 0, sended = 0;

	len = strlen(msg);
	assert(len > 0 && len <= LINELENGTH);
	assert(socketnum > 0);

	if (len <= 1) {
		fprintf(stderr,
			"string zu kurz. mindestlänge 2byte (CR-LF)!\n");
		return -2;
	}

	else if (len > LINELENGTH) {
		fprintf(stderr,
			"string zu lang. maximallänge %dbyte (inkl. CR-LF)! \n",
			LINELENGTH);
		msg[LINELENGTH + 1] = '\0';
		msg[LINELENGTH] = '\n';
		msg[LINELENGTH - 1] = '\r';
		len = strlen(msg);
	}

	else
// BAUSTELLE  if ( BOTFORTH_CRLF == TRUE ) 


		sended = send(socketnum, msg, len, 0);
	if (sended == -1) {
		fprintf(stderr, "Failed to send to server\n>%s<\n", msg);
		return -1;
	}
	fprintf(stderr, "sended to server: >%s<\n", msg);
	return (sended);
}


// ************************************************
// empfängt eine zeile vom ircd und gibt einen zeiger auf den string zurück.
// jede zeile des ircd SOLLTE mit einem \r\n abgeschlossen sein.
// die zeilenlänge ist in LINELENGTH definiert. im RFC ist diese mit maximal
// 512byte (inklusive \r\n) angegeben. standard it aber 255byte
// sollte die zeilenlänge LINELENGTH überschreiten wird socket geschlossen.                                                             
// int socketnum = socketfd zum ircd
// return '\0' = zeile zu lang oder CR-LF-fehler leerer string wird zurückgegeben und disconnect
// return *char = zeiger auf den empfangenen string                              
//
char *netrecv(int socketnum)
{
	char static buffer[LINELENGTH];
	char *a;
	int i = 0;

	assert(socketnum > 0);
	a = malloc(sizeof(char));

	buffer[0] = '\0';
	while ((recv(socketnum, a, 1, 0) > 0) && (*a != '\n')) {
		if (i >= LINELENGTH - 1) {
			fprintf(stderr, "\nERROR: zu lange zeile vom server empfangen\ndisconnect!\n\
          i=%d,a=%s,b=%s", i, a, buffer);
			exit(1);
			disconnect_irc(socketnum);
			buffer[0] = '\0';

		}
		if (*a != '\r') {
			buffer[i] = *a;
			i++;
		}
	}


	if (i >= 1 && (*a == '\n')) {
		//recv(socketnum,a,1,0);
		//i++;

		if (i > LINELENGTH) {
			fprintf(stderr, "\nERROR: zu lange zeile vom server empfangen\ndisconnect!\n\
          eingestelle maximallänge einer zeile ist zZ %d\n",
				LINELENGTH);
			exit(1);
			disconnect_irc(socketnum);
			buffer[0] = '\0';
		}

		if (*a == '\n') {
			fprintf(stderr, "\nZeilenende gefunden\n");
			buffer[i + 2] = '\0';
			buffer[i + 1] = '\n';
			buffer[i] = '\r';
		} else {
			fprintf(stderr,
				"\nERROR: mit den zeilenenden stimmt was nicht\n");
			disconnect_irc(socketnum);
			buffer[0] = '\0';
			return (buffer);
		}
	}
	// todo: was ist mit a? speicherleck?
	free(a);
	return (buffer);
}

int resolve_ircd(char *host, int port)
{
	struct hostent *he;
	struct in6_addr addr6;
	struct in_addr addr;
	char ipv6[INET6_ADDRSTRLEN];

	printf("Resolving host: %s\n", host);

	// Resolve IPv6
	he = gethostbyname2(host, AF_INET6);
	//he=NULL;
	if (he == NULL)
		printf("Host %s got no IPv6 address\n", host);

	else {
		inet_ntop(AF_INET6, (void *) he->h_addr_list[0], ipv6,
			  INET6_ADDRSTRLEN);
		printf
		    ("Host %s got IPv6 address %s\nConnecting (IPv6)...\n",
		     host, ipv6);
		return (connect_irc6(port, ipv6));
	}
	// Resolve IPv4
	he = gethostbyname(host);
	if (he == NULL)
		printf("Host %s got no IPv4 address.\n", host);
	else {
		memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
		printf("Host %s got IPv4 address %s\nConnecting (IPv4)...",
		       host, inet_ntoa(addr));
		return (connect_irc(port, inet_ntoa(addr)));
	}

	return 0;
}
