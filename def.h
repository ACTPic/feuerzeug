#ifndef DEF_H
#define DEF_H

#include <cdb.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

#define BF_TYPE_NODE 0
#define BF_TYPE_C_TEXT 1
#define BF_TYPE_C_INT 2
#define BF_TYPE_C_EXT 3
#define BF_TYPE_INT 4
#define BF_TYPE_FLOAT 5
#define BF_TYPE_POINTER 6
#define BF_TYPE_STRING 7
#define BF_TYPE_VECTOR 8
#define BF_TYPE_DB 9

// Generische Struktur für alle unterstützten Datenbanken
struct db {
	char *query;
	char *field;
};

// Makros
#define min(a,b) \
        ({ __typeof__ (a) _a = (a); \
           __typeof__ (b) _b = (b); \
                _a < _b ? _a : _b; })

#define max(a,b)                    \
        ({ __typeof__ (a) _a = (a); \
           __typeof__ (b) _b = (b); \
                _a > _b ? _a : _b; })

#endif
