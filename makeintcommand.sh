#!/bin/sh
export COMMANDS="zstack_push zstack_pop halt file_append lstack_push lstack_pop \
setname stop var_put var_pick date is_vector vector_get explain \
frexp ldexp name vector_replace div set_runlimit lexicon_swap lexicon_get interpret pick mod ceil round floor vector_put var_swap vector_shift vector_unshift timestamp sql_escape rand vector_count dstack_unshift dstack_shift fdiv al sql_freeres die clearcache ord chr depth exec exec_ns var_replace var_get print strlen strpos strsplit __sleep concat irc_connect irc_put_raw irc_get sql_query sql_fetch sql_numrows vector vector_pick vector_pop vector_push greaterthan dup drop not nop __b __bz cstack_put_address cstack_get_address add mul equal swap rstack_push rstack_pop cstack_push cstack_push_command"


echo 'void *getintcommand(char *command) {'
echo '  void *p=NULL;'
for a in ${COMMANDS}; do
  echo '  if(strcmp(command,"'$a'")==0) {'
  echo '    p=bf_c_'$a';'
  echo '  }'
done
echo '  return p;'
echo '}'

echo 'char *getintcommandname(void *p) {'
echo '  char *name=NULL;'
for a in ${COMMANDS}; do
  echo '  if(bf_c_'$a'==p) {'
  echo '    name=malloc(strlen("'$a'")+1);'
  echo '    strcpy(name,"'$a'");'
  echo '    name[strlen("'$a'")]=0;'
  echo '  }'
done
echo '  if(name!=NULL) return name;'
echo '  return NULL;';
echo '}'
