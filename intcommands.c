#include <stdlib.h>
#include <stdio.h>
#include "botforth.h"
#include <time.h>
#include <math.h>



void bf_c_swap() {
  struct node *n1;
  struct node *n2;
  n1=vector_pop(dstack);
  n2=vector_pop(dstack);
  if(n1)
    vector_push(dstack,n1);
  if(n2)
    vector_push(dstack,n2);
}

void bf_c_drop() {
  struct node *n1;
  n1=vector_pop(dstack);
  node_destroy(n1);
}

void bf_c_dup() {
  struct node *n1;
  struct node *n2;
  int *i;
  n1=vector_pop(dstack);
  if(n1 && (n1->type!=BF_TYPE_MYSQLRES)) {
    n2=node_copy(n1);
    vector_push(dstack,n1);
  } else {
    i=malloc(sizeof(int));
    *i=0;
    n2=node_create(i,BF_TYPE_INT);
  }
  vector_push(dstack,n2);
}

void bf_c_pick() {
  int num=vector_pop_int(dstack);
  int i;
  struct node *pos;
  pos=dstack->tail;
  for(i=1; i<num; i++) {
    if(pos->prev)
      pos=pos->prev;
  }
  if(pos)
    vector_push(dstack,node_copy(pos));
  else
    vector_push_int(dstack,0);
}

void bf_c_depth() {
  int i;
  i=vector_size(dstack);
  vector_push_int(dstack,i);
}

void bf_c_name() {
  struct node *n=vector_pop(dstack);
  char *name;
  if(n) {
    if(n->name) {
      name=malloc(strlen(n->name)+1);
      sprintf(name,"%s",n->name);
      vector_push(dstack,n);
      vector_push_string(dstack,name);
    } else {
      vector_push(dstack,n);
      vector_push_int(dstack,0);
    }
  }
}

void bf_c_setname() {
  char *name = vector_pop_string(dstack);
  struct node *n = vector_pop(dstack);
  if(n==NULL) {
    free(name);
  } else {
    free(n->name);
    n->name=name;
    vector_push(dstack,n);
  }
}  


void bf_c_add() {
  struct node *n1,*n2;
  n1=vector_pop(dstack);
  n2=vector_pop(dstack);
  if(n1==NULL && n2==NULL) {
    vector_push_int(dstack,0);
    return;
  }
  // parameter fehlt?
  if(n1==NULL) {
    if(n2->type==BF_TYPE_INT)
      vector_push_int(dstack,node_toInt(n2));
    else
      vector_push_float(dstack,node_toFloat(n2));
    return;
  }
  // parameter fehlt?
  if(n2==NULL) {
    if(n1->type==BF_TYPE_INT)
      vector_push_int(dstack,node_toInt(n1));
    else
      vector_push_float(dstack,node_toFloat(n1));
    return;
  }
  if(n1->type==BF_TYPE_INT && n2->type==BF_TYPE_INT) {
    // both int
    vector_push_int(dstack,node_toInt(n1)+node_toInt(n2));
    return;
  }
  if((n2->type==BF_TYPE_INT || 
     (n2->type==BF_TYPE_STRING && 
      get_stringtype(n2->content)==BF_TYPE_INT)) &&
    (n1->type==BF_TYPE_INT ||
     (n1->type==BF_TYPE_STRING &&
      get_stringtype(n1->content)==BF_TYPE_INT))) {
    // both int-like
    vector_push_int(dstack,node_toInt(n1)+node_toInt(n2));
    return;
  }
  // wenn sonst nichts erkannt wurde: als float behandeln
  vector_push_float(dstack,node_toFloat(n1)+node_toFloat(n2));
}

void bf_c_mul() {
  struct node *n1,*n2;
  n1=vector_pop(dstack);
  n2=vector_pop(dstack);
  if(n1==NULL && n2==NULL) {
    vector_push_int(dstack,0);
    return;
  }
  if(n1==NULL) {
    vector_push_int(dstack,0);
    node_destroy(n2);
    return;
  }
  if(n2==NULL) {
    vector_push_int(dstack,0);
    node_destroy(n1);
    return;
  }
  if(n1->type==BF_TYPE_INT && n2->type==BF_TYPE_INT) {
    // both int
    vector_push_int(dstack,node_toInt(n1)*node_toInt(n2));
    return;
  }
  if((n2->type==BF_TYPE_INT || 
     (n2->type==BF_TYPE_STRING && 
      get_stringtype(n2->content)==BF_TYPE_INT)) &&
    (n1->type==BF_TYPE_INT ||
     (n1->type==BF_TYPE_STRING &&
      get_stringtype(n1->content)==BF_TYPE_INT))) {
    // both int-like
    vector_push_int(dstack,node_toInt(n1)*node_toInt(n2));
    return;
  }
  vector_push_float(dstack,node_toFloat(n1)*node_toFloat(n2));
}

void bf_c_mod() {
  int i2=vector_pop_int(dstack);
  int i1=vector_pop_int(dstack);
  if(i2!=0) {
    vector_push_int(dstack,i1%i2);
  } else {
    // todo: besser NaN zurück
    vector_push_int(dstack,0);
  }
}

void bf_c_div() {
  div_t d;
  int i2=vector_pop_int(dstack);
  int i1=vector_pop_int(dstack);
  if(i2!=0) {
    d=div(i1,i2);
    vector_push_int(dstack,d.quot);
  } else {
    // todo: besser inf zurück
    vector_push_float(dstack,HUGE_VAL);
  }
}

/*********************** Floating Point *********************/

void bf_c_fdiv() {
  float f1=vector_pop_float(dstack);
  float f2=vector_pop_float(dstack);
  vector_push_float(dstack,(float)f2/f1);
}

void bf_c_round() {
  float f1=vector_pop_float(dstack);
  vector_push_int(dstack,(int)roundf(f1));
}

void bf_c_ceil() {
  float f1=vector_pop_float(dstack);
  vector_push_int(dstack,(int)ceilf(f1));
}

void bf_c_floor() {
  float f1=vector_pop_float(dstack);
  vector_push_int(dstack,(int)floorf(f1));
}

void bf_c_frexp() {
  int exp;
  float mant=(float)frexp((double)vector_pop_float(dstack),&exp);
  vector_push_float(dstack,mant);
  vector_push_int(dstack,exp);
}

void bf_c_ldexp() {
  int exp = vector_pop_int(dstack);
  float mant=vector_pop_float(dstack);
  float f=ldexp(mant,exp);
  vector_push_float(dstack,f);
}



/************************** Logic ***************************/

void bf_c_not() {
  int i1=vector_pop_int(dstack);
  if(i1)
    vector_push_int(dstack,0);
  else
    vector_push_int(dstack,1);
}


void bf_c_equal() {
  struct node *n1=vector_pop(dstack);
  struct node *n2=vector_pop(dstack);
  char *s1,*s2;
  int ret;
  ret=0;
  if(n1==NULL || n2==NULL) {
    vector_push_int(dstack,0);
    node_destroy(n1);
    node_destroy(n2);
    return;
  }
  if((n1->type==BF_TYPE_INT || 
     (n1->type==BF_TYPE_STRING && 
      get_stringtype(n1->content)==BF_TYPE_INT)) &&
    (n2->type==BF_TYPE_INT ||
     (n2->type==BF_TYPE_STRING &&
      get_stringtype(n2->content)==BF_TYPE_INT))) {
    // both int or int-like
    ret=node_toInt(n1)==node_toInt(n2);
  } else if((n1->type==BF_TYPE_FLOAT || 
             (n1->type==BF_TYPE_STRING && 
              (get_stringtype(n1->content)==BF_TYPE_FLOAT ||
               get_stringtype(n1->content)==BF_TYPE_INT))) &&
            (n2->type==BF_TYPE_FLOAT ||
             (n2->type==BF_TYPE_STRING &&
              (get_stringtype(n2->content)==BF_TYPE_FLOAT ||
               get_stringtype(n2->content)==BF_TYPE_INT)))) {
    // both float, or float-like
    ret=node_toFloat(n1)==node_toFloat(n2);
  } else if(n1->type==BF_TYPE_STRING && n2->type==BF_TYPE_STRING) {
    // both string
    s1=node_toString(n1);
    s2=node_toString(n2);
    if(strcmp(s1,s2)==0)
      ret=1;
    else
      ret=0;
    free(s1);
    free(s2);
  } else {
    // don't know, try to convert to string and compare then
    s1=node_toString(n1);
    s2=node_toString(n2);
    if(strcmp(s1,s2)==0)
      ret=1;
    else
      ret=0;
    free(s1);
    free(s2);
  }
  vector_push_int(dstack,ret);
}

void bf_c_greaterthan() {
  struct node *n1=vector_pop(dstack);
  struct node *n2=vector_pop(dstack);
  int ret;
  ret=0;
  if(n1==NULL || n2==NULL) {
    node_destroy(n1);
    node_destroy(n2);
    return;
  }
  if((n1->type==BF_TYPE_INT || 
     (n1->type==BF_TYPE_STRING && 
      get_stringtype(n1->content)==BF_TYPE_INT)) &&
    (n2->type==BF_TYPE_INT ||
     (n2->type==BF_TYPE_STRING &&
      get_stringtype(n2->content)==BF_TYPE_INT))) {
    // both int or int-like?
    ret=node_toInt(n1)<node_toInt(n2);
  } else {
    ret=node_toFloat(n1)<node_toFloat(n2);
  }
  vector_push_int(dstack,ret);
}

/****************************** Data Stack ********************/

void bf_c_dstack_shift() {
  struct node *n = vector_shift(dstack);
  if(n)
    vector_push(dstack,n);
}

void bf_c_dstack_unshift() {
  struct node *n;
  n=vector_pop(dstack);
  if(n)
    vector_unshift(dstack,n);
}

/*************** Help Stack Z *****************/
void bf_c_zstack_push() {
  struct node *n = vector_pop(dstack);
  if(n)
    vector_push(zstack,n);
  else
    vector_push_int(zstack,0);
}

void bf_c_zstack_pop() {
  struct node *n;
  n=vector_pop(zstack);
  if(n)
    vector_push(dstack,n);
  else
    vector_push_int(dstack,0);
}

/*************** Loop Stack *****************/
void bf_c_lstack_push() {
  struct node *n = vector_pop(dstack);
  if(n)
    vector_push(lstack,n);
  else
    vector_push_int(lstack,0);
}

void bf_c_lstack_pop() {
  struct node *n;
  n=vector_pop(lstack);
  if(n)
    vector_push(dstack,n);
  else
    vector_push_int(dstack,0);
}


/*************** Return Stack *****************/

void bf_c_rstack_push() {
  struct node *n;
  if(accesslevel<3) {
    n = vector_pop(dstack);
    if(n)
      vector_push(rstack,n);
    else
      vector_push_int(rstack,0);
  }
}

void bf_c_rstack_pop() {
  struct node *n;
  if(accesslevel<3) {
    n=vector_pop(rstack);
    if(n)
      vector_push(dstack,n);
    else
      vector_push_int(dstack,0);
  }
}

/*************** Compiler Stack ****************/

void bf_c_cstack_push() {
  struct node *n = vector_pop(dstack);
  if(n) {
    if(cstack)
      vector_push(cstack,n);
    else
      node_destroy(n);
  }
}

void bf_c_cstack_push_command() {
  struct node *n = vector_pop(dstack);
  if(n && cstack && n->type==BF_TYPE_STRING) {
    n->type=BF_TYPE_C_TEXT;
    vector_push(cstack,n);
  }
  // todo free, falls cstack nicht existent
}

void bf_c_cstack_get_address() {
  struct node *n = vector_top(cstack);
  if(n) {
    vector_push_pointer(dstack,n);
  }
}

void bf_c_cstack_put_address() {
  struct node *n1 = vector_pop_pointer(dstack);
  struct node *n2 = vector_pop_pointer(dstack);
  if(n1 && n2) {
    n2->content=n1;
    n2->type=BF_TYPE_POINTER;
  }
  // todo: speicherleck, n1 loeschen?
}

/******************** Vectors *********************/

void bf_c_vector() {
  struct vector *v=vector_create();
  vector_push_vector(dstack,v);
}

void bf_c_vector_push() {
  char *name;
  struct vector *v=vector_pop_vector(dstack);
  struct node *n=vector_pop(dstack);
  if(v) {
    if(n) {
      if(n->name==NULL) {
        name=malloc(20);
        sprintf(name,"%i",v->size+1);
        n->name=name;
      }
      vector_push(v,n);
    }
    vector_push_vector(dstack,v);
  }
}

void bf_c_vector_pop() {
  struct vector *v=vector_pop_vector(dstack);
  struct node *n;
  if(v) {
    n=vector_pop(v);
    vector_push_vector(dstack,v);
    if(n)
      vector_push(dstack,n);
    else
      vector_push_int(dstack,0);
  }
}

void bf_c_vector_unshift() {
  char *name;
  struct vector *v=vector_pop_vector(dstack);
  struct node *n=vector_pop(dstack);
  if(v) {
    if(n) {
      if(n->name==NULL) {
        name=malloc(20);
        sprintf(name,"%i",v->size+1);
        n->name=name;
      }
      vector_unshift(v,n);
    }
    vector_push_vector(dstack,v);
  }
}

void bf_c_vector_shift() {
  struct vector *v=vector_pop_vector(dstack);
  struct node *n;
  if(v) {
    n=vector_shift(v);
    vector_push_vector(dstack,v);
    if(n)
      vector_push(dstack,n);
    else
      vector_push_int(dstack,0);
  }
}

void bf_c_vector_put() {
  struct vector *v=vector_pop_vector(dstack);
  char *name;
  struct node *n;
  if(v) {
    name=vector_pop_string(dstack);
    n=vector_pop(dstack);  
    if(n) {
      if(n->name)
        free(n->name);
      n->name=name;
      vector_push(v,n);
    } else {
      free(name);
    }
    vector_push_vector(dstack,v);
  }
}

void bf_c_vector_replace() {
  struct vector *v=vector_pop_vector(dstack);
  char *name;
  struct node *n;
  name=vector_pop_string(dstack);
  if(v) {
    n=vector_pop(dstack);
    if(n) {
      if(n->name) {
        free(n->name);
        n->name=NULL;
      }
      vector_replace(v,name,n);
    }
    vector_push_vector(dstack,v);
  }
  free(name);
}

void bf_c_vector_pick() {
  char *name,*temp;
  struct vector *v;
  struct node *n;
  name=vector_pop_string(dstack);
  v=vector_pop_vector(dstack);
  if(v) {
    n=vector_pick(v,name);
    vector_push_vector(dstack,v);
    if(n) {
      vector_push(dstack,node_copy(n));
    } else {
      temp=malloc(1);
      sprintf(temp,"");
      vector_push_string(dstack,temp);
    }    
  }
  free(name);
}

void bf_c_vector_get() {
  char *name,*temp;
  struct vector *v;
  struct node *n;
  name=vector_pop_string(dstack);
  v=vector_pop_vector(dstack);
  if(v) {
    n=vector_get(v,name);
    vector_push_vector(dstack,v);
    if(n) {
      vector_push(dstack,n);
    } else {
      temp=malloc(1);
      sprintf(temp,"");
      vector_push_string(dstack,temp);
    }    
  }
  free(name);
}


void bf_c_vector_count() {
  struct vector *v;
  int i;
  v=vector_pop_vector(dstack);
  if(v) {
    i=v->size;
    vector_push_vector(dstack,v);
    vector_push_int(dstack,i);
  }
}

void bf_c_is_vector() {
  struct node *n=vector_pop(dstack);
  if(n) {
    vector_push(dstack,n);
    if(n->type==BF_TYPE_VECTOR)
      vector_push_int(dstack,1);
    else
      vector_push_int(dstack,0);
  } else {
    vector_push_int(dstack,0);
  }
}

/********************** Variables *******************/

void bf_c_var_replace() {
  char *name=vector_pop_string(dstack);
  char *temp;
  struct node *n;
  if(accesslevel<3 || strlen(name)==0 || name[0]!='_') {  
    n=vector_pop(dstack);
    if(!n) {
      temp=malloc(1);
      temp[0]='\0';
      n=node_create(temp,BF_TYPE_STRING);
    }
    vector_replace(var,name,n);
  }
  free(name);
}

void bf_c_var_put() {
  char *name=vector_pop_string(dstack);
  char *temp;
  if(accesslevel<3 || strlen(name)==0 || name[0]!='_') {
    struct node *n=vector_pop(dstack);
    if(!n) {
      temp=malloc(1);
      temp[0]='\0';
      n=node_create(temp,BF_TYPE_STRING);
    }
    vector_put(var,name,n);
  }
  free(name);
}

void bf_c_var_get() {
  char *name,*temp;
  struct node *n;
  name=vector_pop_string(dstack);
  n=vector_get(var,name);
  if(n) {
    vector_push(dstack,n);
  } else {
    temp=malloc(1);
    temp[0]='\0';
    vector_push_string(dstack,temp);
  }
  free(name);
}

void bf_c_var_pick() {
  char *name,*temp;
  struct node *n;
  name=vector_pop_string(dstack);
  n=vector_pick(var,name);
  if(n) {
    vector_push(dstack,node_copy(n));
  } else {
    temp=malloc(1);
    temp[0]='\0';
    vector_push_string(dstack,temp);
  }
  free(name);
}



void bf_c_var_swap() {
  struct vector *v,*temp;
  v=vector_pop_vector(dstack);
  if(v) {
    temp=var;
    var=v;
    v=temp;
    vector_push_vector(dstack,v);
  }
}

/********************* Lexicon *******************/

void bf_c_lexicon_swap() {
  struct vector *v,*temp;
  v=vector_pop_vector(dstack);
  if(v) {
    temp=lexicon;
    lexicon=v;
    v=temp;
    vector_push_vector(dstack,v);
  }
}

void bf_c_lexicon_get() {
  struct node *n;
  char *name;
  name=vector_pop_string(dstack);
  n=vector_pick(words,name);
  if(n) {
    vector_push(dstack,n);
  }
  // todo...
}

/***************** Minibotforth *****************/

void bf_c___b() {
  struct node *n = vector_pop_pointer(dstack);
  if(n)
    pc=n;
}

void bf_c___bz() {
  struct node *n1 = vector_pop_pointer(dstack);
  int n2 = vector_pop_int(dstack);
  if(n1 && n2==0)
    pc=n1;
}

void bf_c_stop() {
  botforth_return();
}

void bf_c_nop() {
}

void bf_c___sleep() {
  int i;
  i=vector_pop_int(dstack);
  if(accesslevel>2)
    i=min(i,1);
  sleep(i);
}


void bf_c_print() {
  char *s;
  s=vector_pop_string(dstack);
  printf("Ausgabe: %s\n",s);
  free(s);
}

/********************** string manipulation ***************/

void bf_c_chr() {
  int i = vector_pop_int(dstack);
  char *buffer;
  buffer=malloc(2);
  buffer[0]=(signed char)i;
  buffer[1]='\0';
  vector_push_string(dstack,buffer);
}

void bf_c_ord() {
  char* s = vector_pop_string(dstack);
  short unsigned int u;
  if(strlen(s)>0) {
    u=(short unsigned int)(unsigned char)s[0];
    vector_push_int(dstack,u);
  } else {
    vector_push_int(dstack,0);
  }
  free(s);
}

void bf_c_concat() {
  char *s1,*s2,*s3;
  s1=vector_pop_string(dstack);
  s2=vector_pop_string(dstack);
  s3=malloc(strlen(s1)+strlen(s2)+1);
  sprintf(s3,"%s%s",s2,s1);
  free(s1);
  free(s2);
  vector_push_string(dstack,s3);
}

void bf_c_strsplit() {
  char *s1,*s2,*s3;
  int n;
  n=vector_pop_int(dstack);
  s3=vector_pop_string(dstack);
  n=min(n,strlen(s3));
  s1=malloc(n+1);
  strncpy(s1,s3,n);
  s1[n]='\0';
  vector_push_string(dstack,s1);
  s2=malloc(strlen(s3)-n+1);
  strcpy(s2,&s3[n]);
  s2[strlen(s3)-n]='\0';
  vector_push_string(dstack,s2);
  free(s3);
}

void bf_c_strpos() {
  char *s1,*s2;
  int n;
  s1=vector_pop_string(dstack);
  s2=vector_pop_string(dstack);
  n=strstr(s2,s1);
  free(s1);
  free(s2);
  if(n)
   n=n-(int)s2+1;
  vector_push_int(dstack,n);
}

void bf_c_strlen() {
  char *s1;
  int n;
  s1=vector_pop_string(dstack);
  n=strlen(s1);
  free(s1);
  vector_push_int(dstack,n);
}

/******************** mysql ***********************/

void bf_c_sql_query() {
  char *query;
  int ok;
  MYSQL_RES *res=NULL;
  MYSQL *mysql;
  if(accesslevel<3)
    mysql=&mysql_write;
  else
    mysql=&mysql_read;
  ok=1;
  query=vector_pop_string(dstack);
  if(mysql_real_query(mysql,query,strlen(query))) {
    printf("sql query (intcommands) fehlgeschlagen\n");
    ok=0;
  } else if((res = mysql_store_result(mysql)) ) {
    // Lesezugriff (SELECT)
    vector_push_mysqlres(dstack,res);
  } else {
    // Schreibzugriff (INSERT, REPLACE, usw.)
  }
/*
  if(ok) {
    if(res) {
      vector_push_mysqlres(dstack,res);
    }
    //vector_push_int(dstack,ok);
  }
*/
  free(query);
}  

void bf_c_sql_fetch() {
  MYSQL_RES *res=(MYSQL_RES*)vector_pop_mysqlres(dstack);
  char *buffer,*buffer2;
  struct node *n;
  struct vector *v;
  int i;
  if(res) {
    if(row = mysql_fetch_row(res)) {
      v=vector_create();
      for( i=0; i < mysql_num_fields(res); i++ ){
        if(row[i]) {
          fld = mysql_fetch_field_direct(res,i);
          buffer=malloc(strlen(row[i])+1);
          sprintf(buffer,"%s",row[i]);
          n=node_create(buffer,BF_TYPE_STRING);
          buffer2=malloc(strlen(fld->name)*sizeof(char));
          strcpy(buffer2,fld->name);
          n->name=buffer2;
          vector_push(v,n);
        }
      }
      vector_push_mysqlres(dstack,res);
      vector_push_vector(dstack,v);
    } else {
      vector_push_mysqlres(dstack,res);    
    }
  } else {
    printf("kein mysql-res auf stack gefunden.\n");
  }
}

void bf_c_sql_numrows() {
  MYSQL_RES *res=(MYSQL_RES*)vector_pop_mysqlres(dstack);
  int numrows;
  if(res) {
    numrows = mysql_num_rows(res);
    vector_push_mysqlres(dstack,res);
    vector_push_int(dstack,numrows);
  } else {
    printf("kein mysql-res auf stack gefunden.\n");
  }
}

void bf_c_sql_freeres() {
  MYSQL_RES *res=(MYSQL_RES*)vector_pop_mysqlres(dstack);
  if(res) {
    mysql_free_result(res);  
  }
}

void bf_c_sql_escape() {
  char* escape;
  char* s;
  s=vector_pop_string(dstack);
  escape=malloc(strlen(s)*3);
  mysql_escape_string(escape,s,strlen(s));
  free(s);
  vector_push_string(dstack,escape);
}
    

/************************* IRC *********************/

void bf_c_irc_connect() {
  char *buffer;
  int port;
  port=vector_pop_int(dstack);
  buffer=vector_pop_string(dstack);
  ircsocket=resolve_ircd(buffer,port);
  free(buffer);
}

void bf_c_irc_disconnect() {
  disconnect_irc(ircsocket);
}

void bf_c_irc_put_raw() {
  char *line,*buffer;
  if(accesslevel<3) {
    line=vector_pop_string(dstack);
    buffer=malloc(strlen(line)+3);
    sprintf(buffer,"%s\r\n",line);
    netsend(ircsocket,buffer);
    free(buffer);
    free(line);
  }
}

void bf_c_irc_get() {
  char *buffer,*s;
  if(accesslevel<3) {
    buffer=(char*)netrecv(ircsocket);
    s=malloc(strlen(buffer)+1);
    sprintf(s,"%s",buffer);
    vector_push_string(dstack,s);
  }
}


void bf_c_exec() {
  char *name=vector_pop_string(dstack);
  struct vector *program;
  assert(name);
  program=(struct vector*)word_load(name,NULL);
  printf("Exec: programm geladen: %s\n",name);
  if(program) {
    botforth_call(program);
  }
  free(name);
}

void bf_c_exec_ns() {
  char *ns=vector_pop_string(dstack);
  char *name=vector_pop_string(dstack);
  struct vector *program;
  if(accesslevel<3) {
    assert(name);
    program=(struct vector*)word_load(name,ns);
    printf("Exec: programm geladen: %s/%s\n",ns,name);
    if(program) {
      botforth_call(program);
    }
  }
  free(name);
  free(ns);
}


void bf_c_clearcache() {
  struct node *word,*newword;
  struct vector *infoblock,*cachev,*program;
  word=words->head;
  while(word!=NULL) {
    newword=word->next;
    assert(word->type<10);
    // set clearcache=1 to mark program to be updated as soon as possible
    if(word->type==BF_TYPE_VECTOR) {
      cachev=(struct vector*)word->content;
      program=vector_pick(cachev,"program")->content;
      infoblock=program->head->content;
      *(int*)infoblock->head->next->next->content=1;
    } else {
      vector_delete(words,word);
      node_destroy(word);
    }
    word=newword;
  }
}

void bf_c_al() {
  vector_push_int(dstack,accesslevel);
}

void bf_c_rand() {
  vector_push_int(dstack,(int)(vector_pop_int(dstack)*1.0*rand()/(RAND_MAX+1.0)));
}

/******************* Time *******************/

void bf_c_timestamp() {
  time_t t;
  t=time(NULL);
  vector_push_int(dstack,(int)t);
}

void bf_c_date() {
  time_t t;
  struct tm *l_time;
  char string[200];
  char *s1,*s2;
  int timestamp;
  s1=vector_pop_string(dstack);
  if(s1) {
    timestamp=vector_pop_int(dstack);
    //t=time(NULL);
    t=(time_t)timestamp;
    l_time = localtime(&t);
    // todo: bufferoverflow, falls s1 > 200 zeichen
    strftime(string, sizeof string, s1, l_time);
    printf("%s", string);
    s2=malloc(strlen(string)+1);
    sprintf(s2,"%s",string);  
    vector_push_string(dstack,s2);
    free(s1);
  }
}

/********************** Filesystem ********************/

void bf_c_file_append() {
  char* filename=vector_pop_string(dstack);
  char* text=vector_pop_string(dstack);
  FILE *file;
  if(accesslevel<3) {
    if(filename && text) {
      file=fopen(filename,"a");
      fprintf(file,"%s",text);  
      fclose(file);
    }
  }
  if(filename)
    free(filename);
  if(text)
    free(text);
}  

void bf_c_interpret() {
  char* word=vector_pop_string(dstack);
  struct vector *infoblock;
  struct vector *program,*v;
  char *buffer2,*name;
  assert(word!=NULL);
  program=botforth_parse(word);
  // set name
  name=malloc(20);
  interpretcounter++;
  sprintf(name,"interpret%i",interpretcounter);
  // infoblock ergaenzen
  /*
  infoblock=(struct vector*)program->head->content;
  buffer2=malloc(strlen(buffer)+1);
  sprintf(buffer2,"%s",buffer);
  n2=node_create(buffer2,BF_TYPE_STRING);
  vector_put(infoblock,"cachename",n2);
  buffer2=malloc(strlen(name)+1);
  sprintf(buffer2,"%s",name);
  free(infoblock->head->next->content);
      infoblock->head->next->content=buffer2;
      // update accesslevel in the infoblock
  */
  infoblock_set_accesslevel(program,3);
  // update program name
  buffer2=malloc(9+1);
  sprintf(buffer2,"interpret");
  infoblock_set_name(program,buffer2);
  if(program->head==NULL) {
    printf("leeres programm: %s\n",word);
  }
  if(program) {
    program=botforth_compile(program);
    if(program->head) {
      printf("Wort im cache speichern\n");
      printf("name: %s\n",name);
      buffer2=malloc(8);
      sprintf(buffer2,"program");
      v=vector_create();
      vector_put_vector(v,buffer2,program);
      vector_put_vector(words,name,v);
      // call program
      botforth_call(program);
    } else {
      vector_destroy(program);
    }
  } else {
    vector_destroy(program);
  }
  free(name);
  // todo: interpret-programme landen zwar im cache, aber dort werden
  // versauern. die muessen da wenigstens bei clearcache rausfliegen
}

void bf_c_die() {
  if(accesslevel<3)
    exit(1);
}

void bf_c_set_runlimit() {
  if(accesslevel<3) {
    runlimit=vector_pop_int(dstack);
  }
}

void bf_c_halt() {
  runlimit=1;
}

void bf_c_explain() {
  char* name=vector_pop_string(dstack);
  char* tmp;
  char* tmp2;
  struct vector *program;
  struct node *word;
  struct vector *tmpinfoblock; 
  int count=0;
  program=(struct vector*)word_load(name,0);  
  if(program==NULL) {
    vector_push_int(dstack,0);
    return;
  }
        
//      program=vector_pick(cachev,"program")->content;
  word=program->head;
  while(word!=NULL) {
    switch(word->type) {
      case BF_TYPE_STRING:
        tmp=malloc(strlen(word->content)+3);
        tmp[0]='"';
        strcpy(&tmp[1],word->content);
        tmp[strlen(word->content)+1]='"';
        tmp[strlen(word->content)+2]=0;
        vector_push_string(dstack,tmp);
        count++;
        break;
      case BF_TYPE_INT:
        vector_push_int(dstack,word->content);
        count++;
        break;
      case BF_TYPE_FLOAT:
        vector_push_float(dstack,0.0);
        count++;
        break;
      case BF_TYPE_C_TEXT:
        tmp=malloc(strlen(word->content)+1);
        strcpy(&tmp[0],word->content);
        tmp[strlen(word->content)+0]=0;
        vector_push_string(dstack,tmp);
        count++;
        break;
      case BF_TYPE_C_INT:
        tmp=getintcommandname(word->content);
        if(tmp!=NULL) {
          vector_push_string(dstack,tmp);
          count++;
        } else {
          vector_push_int(dstack,42);
          count++;
        }
        break;
      case BF_TYPE_C_EXT:
        tmpinfoblock=word->content;
        tmp2=infoblock_get_name(tmpinfoblock);
        tmp=malloc(strlen(tmp2)+1);
        strcpy(&tmp[0],tmp2);
        tmp[strlen(tmp2)+0]=0;
        vector_push_string(dstack,tmp);
        count++;
        break;
      case BF_TYPE_POINTER:
        vector_push_int(dstack,word->content);
        count++;
        break;
    }
    //vector_push_int(dstack,word->type);
    word=word->next;
  }
  vector_push_int(dstack,count);
}


#include "getintcommand.c"
