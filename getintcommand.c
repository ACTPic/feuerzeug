void *getintcommand(char *command) {
  void *p=NULL;
  if(strcmp(command,"zstack_push")==0) {
    p=bf_c_zstack_push;
  }
  if(strcmp(command,"zstack_pop")==0) {
    p=bf_c_zstack_pop;
  }
  if(strcmp(command,"halt")==0) {
    p=bf_c_halt;
  }
  if(strcmp(command,"file_append")==0) {
    p=bf_c_file_append;
  }
  if(strcmp(command,"lstack_push")==0) {
    p=bf_c_lstack_push;
  }
  if(strcmp(command,"lstack_pop")==0) {
    p=bf_c_lstack_pop;
  }
  if(strcmp(command,"setname")==0) {
    p=bf_c_setname;
  }
  if(strcmp(command,"stop")==0) {
    p=bf_c_stop;
  }
  if(strcmp(command,"var_put")==0) {
    p=bf_c_var_put;
  }
  if(strcmp(command,"var_pick")==0) {
    p=bf_c_var_pick;
  }
  if(strcmp(command,"date")==0) {
    p=bf_c_date;
  }
  if(strcmp(command,"is_vector")==0) {
    p=bf_c_is_vector;
  }
  if(strcmp(command,"vector_get")==0) {
    p=bf_c_vector_get;
  }
  if(strcmp(command,"explain")==0) {
    p=bf_c_explain;
  }
  if(strcmp(command,"frexp")==0) {
    p=bf_c_frexp;
  }
  if(strcmp(command,"ldexp")==0) {
    p=bf_c_ldexp;
  }
  if(strcmp(command,"name")==0) {
    p=bf_c_name;
  }
  if(strcmp(command,"vector_replace")==0) {
    p=bf_c_vector_replace;
  }
  if(strcmp(command,"div")==0) {
    p=bf_c_div;
  }
  if(strcmp(command,"set_runlimit")==0) {
    p=bf_c_set_runlimit;
  }
  if(strcmp(command,"lexicon_swap")==0) {
    p=bf_c_lexicon_swap;
  }
  if(strcmp(command,"lexicon_get")==0) {
    p=bf_c_lexicon_get;
  }
  if(strcmp(command,"interpret")==0) {
    p=bf_c_interpret;
  }
  if(strcmp(command,"pick")==0) {
    p=bf_c_pick;
  }
  if(strcmp(command,"mod")==0) {
    p=bf_c_mod;
  }
  if(strcmp(command,"ceil")==0) {
    p=bf_c_ceil;
  }
  if(strcmp(command,"round")==0) {
    p=bf_c_round;
  }
  if(strcmp(command,"floor")==0) {
    p=bf_c_floor;
  }
  if(strcmp(command,"vector_put")==0) {
    p=bf_c_vector_put;
  }
  if(strcmp(command,"var_swap")==0) {
    p=bf_c_var_swap;
  }
  if(strcmp(command,"vector_shift")==0) {
    p=bf_c_vector_shift;
  }
  if(strcmp(command,"vector_unshift")==0) {
    p=bf_c_vector_unshift;
  }
  if(strcmp(command,"timestamp")==0) {
    p=bf_c_timestamp;
  }
  if(strcmp(command,"sql_escape")==0) {
    p=bf_c_sql_escape;
  }
  if(strcmp(command,"rand")==0) {
    p=bf_c_rand;
  }
  if(strcmp(command,"vector_count")==0) {
    p=bf_c_vector_count;
  }
  if(strcmp(command,"dstack_unshift")==0) {
    p=bf_c_dstack_unshift;
  }
  if(strcmp(command,"dstack_shift")==0) {
    p=bf_c_dstack_shift;
  }
  if(strcmp(command,"fdiv")==0) {
    p=bf_c_fdiv;
  }
  if(strcmp(command,"al")==0) {
    p=bf_c_al;
  }
  if(strcmp(command,"sql_freeres")==0) {
    p=bf_c_sql_freeres;
  }
  if(strcmp(command,"die")==0) {
    p=bf_c_die;
  }
  if(strcmp(command,"clearcache")==0) {
    p=bf_c_clearcache;
  }
  if(strcmp(command,"ord")==0) {
    p=bf_c_ord;
  }
  if(strcmp(command,"chr")==0) {
    p=bf_c_chr;
  }
  if(strcmp(command,"depth")==0) {
    p=bf_c_depth;
  }
  if(strcmp(command,"exec")==0) {
    p=bf_c_exec;
  }
  if(strcmp(command,"exec_ns")==0) {
    p=bf_c_exec_ns;
  }
  if(strcmp(command,"var_replace")==0) {
    p=bf_c_var_replace;
  }
  if(strcmp(command,"var_get")==0) {
    p=bf_c_var_get;
  }
  if(strcmp(command,"print")==0) {
    p=bf_c_print;
  }
  if(strcmp(command,"strlen")==0) {
    p=bf_c_strlen;
  }
  if(strcmp(command,"strpos")==0) {
    p=bf_c_strpos;
  }
  if(strcmp(command,"strsplit")==0) {
    p=bf_c_strsplit;
  }
  if(strcmp(command,"__sleep")==0) {
    p=bf_c___sleep;
  }
  if(strcmp(command,"concat")==0) {
    p=bf_c_concat;
  }
  if(strcmp(command,"irc_connect")==0) {
    p=bf_c_irc_connect;
  }
  if(strcmp(command,"irc_put_raw")==0) {
    p=bf_c_irc_put_raw;
  }
  if(strcmp(command,"irc_get")==0) {
    p=bf_c_irc_get;
  }
  if(strcmp(command,"sql_query")==0) {
    p=bf_c_sql_query;
  }
  if(strcmp(command,"sql_fetch")==0) {
    p=bf_c_sql_fetch;
  }
  if(strcmp(command,"sql_numrows")==0) {
    p=bf_c_sql_numrows;
  }
  if(strcmp(command,"vector")==0) {
    p=bf_c_vector;
  }
  if(strcmp(command,"vector_pick")==0) {
    p=bf_c_vector_pick;
  }
  if(strcmp(command,"vector_pop")==0) {
    p=bf_c_vector_pop;
  }
  if(strcmp(command,"vector_push")==0) {
    p=bf_c_vector_push;
  }
  if(strcmp(command,"greaterthan")==0) {
    p=bf_c_greaterthan;
  }
  if(strcmp(command,"dup")==0) {
    p=bf_c_dup;
  }
  if(strcmp(command,"drop")==0) {
    p=bf_c_drop;
  }
  if(strcmp(command,"not")==0) {
    p=bf_c_not;
  }
  if(strcmp(command,"nop")==0) {
    p=bf_c_nop;
  }
  if(strcmp(command,"__b")==0) {
    p=bf_c___b;
  }
  if(strcmp(command,"__bz")==0) {
    p=bf_c___bz;
  }
  if(strcmp(command,"cstack_put_address")==0) {
    p=bf_c_cstack_put_address;
  }
  if(strcmp(command,"cstack_get_address")==0) {
    p=bf_c_cstack_get_address;
  }
  if(strcmp(command,"add")==0) {
    p=bf_c_add;
  }
  if(strcmp(command,"mul")==0) {
    p=bf_c_mul;
  }
  if(strcmp(command,"equal")==0) {
    p=bf_c_equal;
  }
  if(strcmp(command,"swap")==0) {
    p=bf_c_swap;
  }
  if(strcmp(command,"rstack_push")==0) {
    p=bf_c_rstack_push;
  }
  if(strcmp(command,"rstack_pop")==0) {
    p=bf_c_rstack_pop;
  }
  if(strcmp(command,"cstack_push")==0) {
    p=bf_c_cstack_push;
  }
  if(strcmp(command,"cstack_push_command")==0) {
    p=bf_c_cstack_push_command;
  }
  return p;
}
char *getintcommandname(void *p) {
  char *name=NULL;
  if(bf_c_zstack_push==p) {
    name=malloc(strlen("zstack_push")+1);
    strcpy(name,"zstack_push");
    name[strlen("zstack_push")]=0;
  }
  if(bf_c_zstack_pop==p) {
    name=malloc(strlen("zstack_pop")+1);
    strcpy(name,"zstack_pop");
    name[strlen("zstack_pop")]=0;
  }
  if(bf_c_halt==p) {
    name=malloc(strlen("halt")+1);
    strcpy(name,"halt");
    name[strlen("halt")]=0;
  }
  if(bf_c_file_append==p) {
    name=malloc(strlen("file_append")+1);
    strcpy(name,"file_append");
    name[strlen("file_append")]=0;
  }
  if(bf_c_lstack_push==p) {
    name=malloc(strlen("lstack_push")+1);
    strcpy(name,"lstack_push");
    name[strlen("lstack_push")]=0;
  }
  if(bf_c_lstack_pop==p) {
    name=malloc(strlen("lstack_pop")+1);
    strcpy(name,"lstack_pop");
    name[strlen("lstack_pop")]=0;
  }
  if(bf_c_setname==p) {
    name=malloc(strlen("setname")+1);
    strcpy(name,"setname");
    name[strlen("setname")]=0;
  }
  if(bf_c_stop==p) {
    name=malloc(strlen("stop")+1);
    strcpy(name,"stop");
    name[strlen("stop")]=0;
  }
  if(bf_c_var_put==p) {
    name=malloc(strlen("var_put")+1);
    strcpy(name,"var_put");
    name[strlen("var_put")]=0;
  }
  if(bf_c_var_pick==p) {
    name=malloc(strlen("var_pick")+1);
    strcpy(name,"var_pick");
    name[strlen("var_pick")]=0;
  }
  if(bf_c_date==p) {
    name=malloc(strlen("date")+1);
    strcpy(name,"date");
    name[strlen("date")]=0;
  }
  if(bf_c_is_vector==p) {
    name=malloc(strlen("is_vector")+1);
    strcpy(name,"is_vector");
    name[strlen("is_vector")]=0;
  }
  if(bf_c_vector_get==p) {
    name=malloc(strlen("vector_get")+1);
    strcpy(name,"vector_get");
    name[strlen("vector_get")]=0;
  }
  if(bf_c_explain==p) {
    name=malloc(strlen("explain")+1);
    strcpy(name,"explain");
    name[strlen("explain")]=0;
  }
  if(bf_c_frexp==p) {
    name=malloc(strlen("frexp")+1);
    strcpy(name,"frexp");
    name[strlen("frexp")]=0;
  }
  if(bf_c_ldexp==p) {
    name=malloc(strlen("ldexp")+1);
    strcpy(name,"ldexp");
    name[strlen("ldexp")]=0;
  }
  if(bf_c_name==p) {
    name=malloc(strlen("name")+1);
    strcpy(name,"name");
    name[strlen("name")]=0;
  }
  if(bf_c_vector_replace==p) {
    name=malloc(strlen("vector_replace")+1);
    strcpy(name,"vector_replace");
    name[strlen("vector_replace")]=0;
  }
  if(bf_c_div==p) {
    name=malloc(strlen("div")+1);
    strcpy(name,"div");
    name[strlen("div")]=0;
  }
  if(bf_c_set_runlimit==p) {
    name=malloc(strlen("set_runlimit")+1);
    strcpy(name,"set_runlimit");
    name[strlen("set_runlimit")]=0;
  }
  if(bf_c_lexicon_swap==p) {
    name=malloc(strlen("lexicon_swap")+1);
    strcpy(name,"lexicon_swap");
    name[strlen("lexicon_swap")]=0;
  }
  if(bf_c_lexicon_get==p) {
    name=malloc(strlen("lexicon_get")+1);
    strcpy(name,"lexicon_get");
    name[strlen("lexicon_get")]=0;
  }
  if(bf_c_interpret==p) {
    name=malloc(strlen("interpret")+1);
    strcpy(name,"interpret");
    name[strlen("interpret")]=0;
  }
  if(bf_c_pick==p) {
    name=malloc(strlen("pick")+1);
    strcpy(name,"pick");
    name[strlen("pick")]=0;
  }
  if(bf_c_mod==p) {
    name=malloc(strlen("mod")+1);
    strcpy(name,"mod");
    name[strlen("mod")]=0;
  }
  if(bf_c_ceil==p) {
    name=malloc(strlen("ceil")+1);
    strcpy(name,"ceil");
    name[strlen("ceil")]=0;
  }
  if(bf_c_round==p) {
    name=malloc(strlen("round")+1);
    strcpy(name,"round");
    name[strlen("round")]=0;
  }
  if(bf_c_floor==p) {
    name=malloc(strlen("floor")+1);
    strcpy(name,"floor");
    name[strlen("floor")]=0;
  }
  if(bf_c_vector_put==p) {
    name=malloc(strlen("vector_put")+1);
    strcpy(name,"vector_put");
    name[strlen("vector_put")]=0;
  }
  if(bf_c_var_swap==p) {
    name=malloc(strlen("var_swap")+1);
    strcpy(name,"var_swap");
    name[strlen("var_swap")]=0;
  }
  if(bf_c_vector_shift==p) {
    name=malloc(strlen("vector_shift")+1);
    strcpy(name,"vector_shift");
    name[strlen("vector_shift")]=0;
  }
  if(bf_c_vector_unshift==p) {
    name=malloc(strlen("vector_unshift")+1);
    strcpy(name,"vector_unshift");
    name[strlen("vector_unshift")]=0;
  }
  if(bf_c_timestamp==p) {
    name=malloc(strlen("timestamp")+1);
    strcpy(name,"timestamp");
    name[strlen("timestamp")]=0;
  }
  if(bf_c_sql_escape==p) {
    name=malloc(strlen("sql_escape")+1);
    strcpy(name,"sql_escape");
    name[strlen("sql_escape")]=0;
  }
  if(bf_c_rand==p) {
    name=malloc(strlen("rand")+1);
    strcpy(name,"rand");
    name[strlen("rand")]=0;
  }
  if(bf_c_vector_count==p) {
    name=malloc(strlen("vector_count")+1);
    strcpy(name,"vector_count");
    name[strlen("vector_count")]=0;
  }
  if(bf_c_dstack_unshift==p) {
    name=malloc(strlen("dstack_unshift")+1);
    strcpy(name,"dstack_unshift");
    name[strlen("dstack_unshift")]=0;
  }
  if(bf_c_dstack_shift==p) {
    name=malloc(strlen("dstack_shift")+1);
    strcpy(name,"dstack_shift");
    name[strlen("dstack_shift")]=0;
  }
  if(bf_c_fdiv==p) {
    name=malloc(strlen("fdiv")+1);
    strcpy(name,"fdiv");
    name[strlen("fdiv")]=0;
  }
  if(bf_c_al==p) {
    name=malloc(strlen("al")+1);
    strcpy(name,"al");
    name[strlen("al")]=0;
  }
  if(bf_c_sql_freeres==p) {
    name=malloc(strlen("sql_freeres")+1);
    strcpy(name,"sql_freeres");
    name[strlen("sql_freeres")]=0;
  }
  if(bf_c_die==p) {
    name=malloc(strlen("die")+1);
    strcpy(name,"die");
    name[strlen("die")]=0;
  }
  if(bf_c_clearcache==p) {
    name=malloc(strlen("clearcache")+1);
    strcpy(name,"clearcache");
    name[strlen("clearcache")]=0;
  }
  if(bf_c_ord==p) {
    name=malloc(strlen("ord")+1);
    strcpy(name,"ord");
    name[strlen("ord")]=0;
  }
  if(bf_c_chr==p) {
    name=malloc(strlen("chr")+1);
    strcpy(name,"chr");
    name[strlen("chr")]=0;
  }
  if(bf_c_depth==p) {
    name=malloc(strlen("depth")+1);
    strcpy(name,"depth");
    name[strlen("depth")]=0;
  }
  if(bf_c_exec==p) {
    name=malloc(strlen("exec")+1);
    strcpy(name,"exec");
    name[strlen("exec")]=0;
  }
  if(bf_c_exec_ns==p) {
    name=malloc(strlen("exec_ns")+1);
    strcpy(name,"exec_ns");
    name[strlen("exec_ns")]=0;
  }
  if(bf_c_var_replace==p) {
    name=malloc(strlen("var_replace")+1);
    strcpy(name,"var_replace");
    name[strlen("var_replace")]=0;
  }
  if(bf_c_var_get==p) {
    name=malloc(strlen("var_get")+1);
    strcpy(name,"var_get");
    name[strlen("var_get")]=0;
  }
  if(bf_c_print==p) {
    name=malloc(strlen("print")+1);
    strcpy(name,"print");
    name[strlen("print")]=0;
  }
  if(bf_c_strlen==p) {
    name=malloc(strlen("strlen")+1);
    strcpy(name,"strlen");
    name[strlen("strlen")]=0;
  }
  if(bf_c_strpos==p) {
    name=malloc(strlen("strpos")+1);
    strcpy(name,"strpos");
    name[strlen("strpos")]=0;
  }
  if(bf_c_strsplit==p) {
    name=malloc(strlen("strsplit")+1);
    strcpy(name,"strsplit");
    name[strlen("strsplit")]=0;
  }
  if(bf_c___sleep==p) {
    name=malloc(strlen("__sleep")+1);
    strcpy(name,"__sleep");
    name[strlen("__sleep")]=0;
  }
  if(bf_c_concat==p) {
    name=malloc(strlen("concat")+1);
    strcpy(name,"concat");
    name[strlen("concat")]=0;
  }
  if(bf_c_irc_connect==p) {
    name=malloc(strlen("irc_connect")+1);
    strcpy(name,"irc_connect");
    name[strlen("irc_connect")]=0;
  }
  if(bf_c_irc_put_raw==p) {
    name=malloc(strlen("irc_put_raw")+1);
    strcpy(name,"irc_put_raw");
    name[strlen("irc_put_raw")]=0;
  }
  if(bf_c_irc_get==p) {
    name=malloc(strlen("irc_get")+1);
    strcpy(name,"irc_get");
    name[strlen("irc_get")]=0;
  }
  if(bf_c_sql_query==p) {
    name=malloc(strlen("sql_query")+1);
    strcpy(name,"sql_query");
    name[strlen("sql_query")]=0;
  }
  if(bf_c_sql_fetch==p) {
    name=malloc(strlen("sql_fetch")+1);
    strcpy(name,"sql_fetch");
    name[strlen("sql_fetch")]=0;
  }
  if(bf_c_sql_numrows==p) {
    name=malloc(strlen("sql_numrows")+1);
    strcpy(name,"sql_numrows");
    name[strlen("sql_numrows")]=0;
  }
  if(bf_c_vector==p) {
    name=malloc(strlen("vector")+1);
    strcpy(name,"vector");
    name[strlen("vector")]=0;
  }
  if(bf_c_vector_pick==p) {
    name=malloc(strlen("vector_pick")+1);
    strcpy(name,"vector_pick");
    name[strlen("vector_pick")]=0;
  }
  if(bf_c_vector_pop==p) {
    name=malloc(strlen("vector_pop")+1);
    strcpy(name,"vector_pop");
    name[strlen("vector_pop")]=0;
  }
  if(bf_c_vector_push==p) {
    name=malloc(strlen("vector_push")+1);
    strcpy(name,"vector_push");
    name[strlen("vector_push")]=0;
  }
  if(bf_c_greaterthan==p) {
    name=malloc(strlen("greaterthan")+1);
    strcpy(name,"greaterthan");
    name[strlen("greaterthan")]=0;
  }
  if(bf_c_dup==p) {
    name=malloc(strlen("dup")+1);
    strcpy(name,"dup");
    name[strlen("dup")]=0;
  }
  if(bf_c_drop==p) {
    name=malloc(strlen("drop")+1);
    strcpy(name,"drop");
    name[strlen("drop")]=0;
  }
  if(bf_c_not==p) {
    name=malloc(strlen("not")+1);
    strcpy(name,"not");
    name[strlen("not")]=0;
  }
  if(bf_c_nop==p) {
    name=malloc(strlen("nop")+1);
    strcpy(name,"nop");
    name[strlen("nop")]=0;
  }
  if(bf_c___b==p) {
    name=malloc(strlen("__b")+1);
    strcpy(name,"__b");
    name[strlen("__b")]=0;
  }
  if(bf_c___bz==p) {
    name=malloc(strlen("__bz")+1);
    strcpy(name,"__bz");
    name[strlen("__bz")]=0;
  }
  if(bf_c_cstack_put_address==p) {
    name=malloc(strlen("cstack_put_address")+1);
    strcpy(name,"cstack_put_address");
    name[strlen("cstack_put_address")]=0;
  }
  if(bf_c_cstack_get_address==p) {
    name=malloc(strlen("cstack_get_address")+1);
    strcpy(name,"cstack_get_address");
    name[strlen("cstack_get_address")]=0;
  }
  if(bf_c_add==p) {
    name=malloc(strlen("add")+1);
    strcpy(name,"add");
    name[strlen("add")]=0;
  }
  if(bf_c_mul==p) {
    name=malloc(strlen("mul")+1);
    strcpy(name,"mul");
    name[strlen("mul")]=0;
  }
  if(bf_c_equal==p) {
    name=malloc(strlen("equal")+1);
    strcpy(name,"equal");
    name[strlen("equal")]=0;
  }
  if(bf_c_swap==p) {
    name=malloc(strlen("swap")+1);
    strcpy(name,"swap");
    name[strlen("swap")]=0;
  }
  if(bf_c_rstack_push==p) {
    name=malloc(strlen("rstack_push")+1);
    strcpy(name,"rstack_push");
    name[strlen("rstack_push")]=0;
  }
  if(bf_c_rstack_pop==p) {
    name=malloc(strlen("rstack_pop")+1);
    strcpy(name,"rstack_pop");
    name[strlen("rstack_pop")]=0;
  }
  if(bf_c_cstack_push==p) {
    name=malloc(strlen("cstack_push")+1);
    strcpy(name,"cstack_push");
    name[strlen("cstack_push")]=0;
  }
  if(bf_c_cstack_push_command==p) {
    name=malloc(strlen("cstack_push_command")+1);
    strcpy(name,"cstack_push_command");
    name[strlen("cstack_push_command")]=0;
  }
  if(name!=NULL) return name;
  return NULL;
}
