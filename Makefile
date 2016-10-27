CC = gcc
CFLAGS = -g -D_GNU_SOURCE -O0 -Og -Wall -Wextra -Werror -L/usr/include/mysql

LIBS = -lmysqlclient -lz -lm -lcdb
OBJECTS = vector.o node.o intcommands.o file.o db_mysql.o com.o botforth.o
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
