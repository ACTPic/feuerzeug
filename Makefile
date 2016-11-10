CC = gcc
CFLAGS = -g -D_GNU_SOURCE -std=gnu11 -O0 -Og -Wall -Wextra -Werror

LIBS = -lz -lm -lcdb -ldb
OBJECTS = vector.o node.o intcommands.o file.o db_cdb.o db_bdb.o com.o botforth.o
HEADERS = def.h botforth.h com.h node.h vector.h

all: botforth

makeintcommand:
	./makeintcommand.sh > getintcommand.c

botforth: $(OBJECTS)
	$(CC) $(CFLAGS) -o botforth $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

static: $(OBJECTS)
	$(CC) $(CFLAGS) -static -o botforth $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
