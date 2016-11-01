extern struct vector *cdb_load(char *name);
extern struct vector *sql_load(char *name);

struct vector *load_file(char *name)
{
	struct vector *v;
        v = cdb_load(name);
        printf("ø CDB\n");
        debug(v);
        printf("ø SQL\n");
	v = sql_load(name);
        debug(v);
	return v;
}
