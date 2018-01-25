// Kredite: Michael Tokarev (tinycdb),  Daniel Julius Bernstein (CDB)

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "cdb.h"

static unsigned char *buf;
static unsigned blen;

static void allocbuf(unsigned len)
{
	if (blen < len) {
		buf =
		    (unsigned char *) (buf ? realloc(buf, len) :
				       malloc(len));
		if (!buf)
			exit(1);
		blen = len;
	}
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

static int fskip(FILE * fi, unsigned len, unsigned *posp, unsigned limit)
{
	while (len > blen) {
		fget(fi, buf, blen, posp, limit);
		len -= blen;
	}
	if (len) {
		fget(fi, buf, len, posp, limit);
	}
	return 0;
}

static void dmode()
{
	unsigned eod, klen, vlen;
	unsigned pos = 0;
	FILE *f, *fo;
	if (!(f = fopen("calc.cdb", "r")))
		exit(1);
	else if (!(fo = fopen("calc.cdb.index", "w")))
		exit(1);
	allocbuf(2048);
	fget(f, buf, 2048, &pos, 2048);
	eod = cdb_unpack(buf);
	while (pos < eod) {
		cdb_pack(pos, buf);
		fwrite(buf, 4, 1, fo);
		fget(f, buf, 8, &pos, eod);
		klen = cdb_unpack(buf);
		vlen = cdb_unpack(buf + 4);
		if (fskip(f, klen, &pos, eod) != 0)
			exit(1);
		if (fskip(f, vlen, &pos, eod) != 0)
			exit(1);
	}
	if (fflush(fo) < 0 || pos != eod)
		exit(1);
}


int main()
{
	dmode();
	return 0;
}
