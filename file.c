#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include <linux/list.h>
#include "botforth.h"

struct vector *sql_load(char *name)
{
	char *query;
	MYSQL_RES *res;
	char *buffer, *buffer2, *nameescape;
	struct node *n;
	struct vector *v;
	int i;
	nameescape = malloc(strlen(name) * 3 * sizeof(char));
	mysql_escape_string(nameescape, name, strlen(name));
	query = malloc((39 + strlen(nameescape) + 1) * sizeof(char));
	sprintf(query, "select * from calc where eintrag='%s'",
		nameescape);
	if (mysql_real_query(&mysql_read, query, strlen(query))) {
		printf
		    ("sql query fehlgeschlagen bei laden von %s mittels %s\n",
		     name, query);
		free(query);
		free(nameescape);
		v = vector_create();
		n = node_create(buffer, BF_TYPE_STRING);
		vector_put(v, fld->name, n);
		return (n);
	} else {
		free(query);
		free(nameescape);
		if (!(res = mysql_store_result(&mysql_read))) {
			printf("sql store results fehlgeschlagen\n");
			exit(0);
		}
		if (res) {
			if (row = mysql_fetch_row(res)) {
				v = vector_create();
				for (i = 0; i < mysql_num_fields(res); i++) {
					if (row[i]) {
						fld =
						    mysql_fetch_field_direct
						    (res, i);
						buffer =
						    malloc(strlen(row[i]) +
							   1);
						sprintf(buffer, "%s",
							row[i]);
						n = node_create(buffer,
								BF_TYPE_STRING);
						vector_put(v, fld->name,
							   n);
					}
				}
				// todo: was ist mit dem freeen von row und fld usw.?
				mysql_free_result(res);
				//free(row);
				//free(fld);
				printf("vector in file:");
				return v;
			} else {
				// todo: ist das hier ok so?
				mysql_free_result(res);
			}
		} else {
			printf("kein mysql-res auf stack gefunden.\n");
		}
	}
	return NULL;
}


struct vector *load_file(char *name)
{
	struct vector *v;
	char *buffer;
	v = sql_load(name);
	return v;
}


/*
  fp=fopen(buffer,"r");
  if(fp) {
    i=0;
    word=(char *)malloc(sizeof(char));
    while(feof(fp)==0) {
      word=realloc(word,sizeof(char)*(i+2));
      charbuffer=fgetc(fp);
      word[i]=charbuffer;
      i++;
    }
    word[i]='\0';
    fclose(fp);
    printf("programmtext: %s\n",word);
    program=botforth_parse(word);
  } else {
    printf("wort nicht gefunden\n");
    program=NULL;
  }
  */
