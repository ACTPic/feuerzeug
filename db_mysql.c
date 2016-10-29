#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "botforth.h"

// MySQL-Krempel
extern MYSQL mysql_read;
extern MYSQL mysql_write;

struct vector *sql_load(char *name)
{
	 MYSQL_RES *res;
	 struct vector *v;

	 char *nameescape = malloc(strlen(name) * 3 * sizeof(char));
	 mysql_escape_string(nameescape, name, strlen(name));
	 char *query =
	     malloc((39 + strlen(nameescape) + 1) * sizeof(char));
	 sprintf(query, "select * from calc where eintrag='%s'",
		 nameescape);

	 assert(!mysql_real_query(&mysql_read, query, strlen(query)));
	 free(query);
	 free(nameescape);
	 assert((res = mysql_store_result(&mysql_read)));

	 MYSQL_ROW row = mysql_fetch_row(res);
	 if (!row) {
		  mysql_free_result(res);
		  return 0;
	 }

	 v = vector_create();
	 for (unsigned i = 0; i < mysql_num_fields(res); i++) {
		  if (row[i]) {
			   MYSQL_FIELD *fld;
			   fld = mysql_fetch_field_direct(res, i);
			   char *buf = malloc(strlen(row[i]) + 1);
			   strcpy(buf, row[i]);
			   struct node *n;
			   n = node_create(buf, BF_TYPE_STRING);
			   vector_put(v, fld->name, n);
		  }
	 }
	 mysql_free_result(res);
	 return v;
}
