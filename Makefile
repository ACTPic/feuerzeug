#CC = gcc-3.4
CC = gcc
CFLAGS = -g -D_GNU_SOURCE  -O0 -march=i586
#LIBS = `sdl-config --libs` -lSGE -lSDL_mixer -lSDL_image -lz
LIBS = -lmysqlclient -lz
OBJECTS = vector.o node.o intcommands.o file.o com.o botforth.o
HEADERS = botforth.h

all: botforth

makeintcommand:
	./makeintcommand.sh > getintcommand.c

botforth: makeintcommand $(OBJECTS)
	$(CC) $(CFLAGS) -o botforth $(OBJECTS) $(LIBS)

$(OBJECTS): $(HEADERS)

static: $(OBJECTS)
	$(CC) $(CFLAGS) -static -o botforth $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
