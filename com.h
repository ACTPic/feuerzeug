
// *************************************************************
// stellt verbindung zum (IPv6) ircd her
// int port = ircd-port
// char *ipnum = ipnummer des ircd in xxx.xxx.xxx.xxx -notation
// return -1 = fehler beim socket-erstellen
// return -2 = fehler beim connecten
// return num>0 = socketfd
// 
int connect_irc(int port, char *ipnum);
int connect_irc6(int port, char *ipnum);



// *************************************************************
// resolved die IPv4 / IPv6 des ircd
//
int resolve_ircd(char *host);



// *************************************************************
// schliesst die verbindung zum ircd
// int socketnum = socketfd
// 
int disconnect_irc(int socketnum);



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
int netsend(int socketnum, char *msg);



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
char *netrecv(int socketnum);
