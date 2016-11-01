extern struct vector *cdb_load(char *name);
extern struct vector *sql_load(char *name);

struct vector *load_file(char *name)
{
	struct vector *v;
	v = sql_load(name);
        v = cdb_load(name);
	return v;
}
