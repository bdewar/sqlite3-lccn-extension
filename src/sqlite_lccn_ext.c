#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sqlite3ext.h"
#include "callnumber.h"
#include "lccn_patterns.h"

SQLITE_EXTENSION_INIT1

static void normalize_callnumber_sqlite(
  sqlite3_context* ctx,
  int n_vals,
  sqlite3_value** vals
){
  const char* val = sqlite3_value_text(vals[0]); // this is just a pointer, not allocated.  Does not need to be free()d.
  char* cn_string = normalize_callnumber(val);   // this does need to be free()d.

  if (cn_string == NULL){
    sqlite3_result_null(ctx);
  }else{
    sqlite3_result_text(ctx, cn_string, strlen(cn_string), free);
  }
}

static int compare_callnumber_strings_sqlite(
  void*       not_used,
  int         s1len,
  const void* s1,
  int         s2len,
  const void* s2
){
  char* str1 = (char*) s1;
  char* str2 = (char*) s2;
  return compare_callnumber_strings(s1len, str1, s2len, str2);
}

int sqlite3_extension_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  const char *e_msg;
  int e_offset;
  //these global variables are required within callnumber.c
  re_is_cn = pcre_compile(P_IS_CN, 0, &e_msg, &e_offset, NULL);
  if (re_is_cn == NULL){
    printf("Error compiling P_IS_CN at character: %d\n. Message: %s\n", e_offset, e_msg);
    exit(1);
  }
  pe_is_cn = pcre_study(re_is_cn, 0, &e_msg);

  SQLITE_EXTENSION_INIT2(pApi)

  sqlite3_create_collation(db, "LCCN", SQLITE_UTF8, NULL, compare_callnumber_strings_sqlite);
  sqlite3_create_function(db, "NORMALIZE_LCCN", 1, SQLITE_UTF8, NULL, normalize_callnumber_sqlite, NULL, NULL);

  return 0;
}
