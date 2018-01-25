CC = gcc
CFLAGS = -g -D_GNU_SOURCE -std=gnu11 -O0 -Og -Wall -Wextra -Werror

LIBS = -lz -lm -lcdb -ldb
OBJECTS = vector.o node.o sql.o intcommands.o file.o db_cdb.o db_bdb.o com.o botforth.o
HEADERS = def.h botforth.h com.h node.h vector.h

all: botforth db db_cdb_index

makeintcommand:
	./makeintcommand.sh > getintcommand.c

botforth: $(OBJECTS)
	$(CC) $(CFLAGS) -o botforth $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

db: vector.o node.o file.o db_cdb.o db_bdb.o db.o
	$(CC) $(CFLAGS) -o db vector.o node.o file.o db_cdb.o db_bdb.o db.o $(LIBS)

db_cdb_index: db_cdb_index.o
	$(CC) $(CFLAGS) -o db_cdb_index db_cdb_index.o $(LIBS)

static: $(OBJECTS)
	$(CC) $(CFLAGS) -static -o botforth $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
