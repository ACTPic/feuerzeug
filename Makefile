CC = gcc
HELP2MAN ?= help2man
CFLAGS = -g -D_GNU_SOURCE -std=gnu11 -O0 -Og -Wall -Wextra -Werror

LIBS = -lz -lm -lcdb -ldb
OBJECTS = vector.o node.o sql.o intcommands.o file.o db_cdb.o db_bdb.o com.o botforth.o
HEADERS = def.h botforth.h com.h node.h vector.h

all: botforth db db.1

makeintcommand:
	./makeintcommand.sh > getintcommand.c

botforth: $(OBJECTS)
	$(CC) $(CFLAGS) -o botforth $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

db: vector.o node.o file.o db_cdb.o db_bdb.o db.o
	$(CC) $(CFLAGS) -o db vector.o node.o file.o db_cdb.o db_bdb.o db.o $(LIBS)

db.1:
	$(HELP2MAN) -N --version-string=0.1 -o db.1 ./db

static: $(OBJECTS)
	$(CC) $(CFLAGS) -static -o botforth $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
