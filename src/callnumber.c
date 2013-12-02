#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "pcre.h"
#include "lccn_patterns.h"
#include "callnumber.h"
#include "str_util.h"


//sets the callnumber memeber
//assumes all other members are set already.
static int set_callnumber_member(callnumber cn){
  int normalized_len = 0, sofar;

  normalized_len += strlen(cn->broad_subject) + 1;
  normalized_len += strlen(cn->subject_class) + 1;
  if (cn->date_cutter)
    normalized_len += strlen(cn->date_cutter) + 1;
  if (cn->first_cutter)
    normalized_len += strlen(cn->first_cutter) + 2;
  if (cn->second_cutter)
    normalized_len += strlen(cn->second_cutter) +1;
  if (cn->extraneous)
    normalized_len += strlen(cn->extraneous) + 1;

  cn->callnumber = (char*)malloc(normalized_len);
  sofar = sprintf(cn->callnumber, "%s %s", cn->broad_subject, cn->subject_class);
  if(cn->date_cutter)
    sofar+= sprintf(cn->callnumber+sofar, " %s", cn->date_cutter);
  if (cn->first_cutter)
    sofar+= sprintf(cn->callnumber+sofar, " .%s", cn->first_cutter);
  if (cn->second_cutter)
    sofar+= sprintf(cn->callnumber+sofar, " %s", cn->second_cutter);
  if (cn->extraneous)
    sprintf(cn->callnumber+sofar, " %s", cn->extraneous);

  return normalized_len - 1;
}

int init_callnumber(callnumber cn, int in_len, const char* in){
  int i, r_code = 0, match_vect[MATCH_VECT_SIZE], matched_len;
  char* buffer;
  memset(cn, 0, sizeof(struct callnumber_st));

  if (in != NULL && in_len > 0){
    buffer = (char*)alloca(in_len+1);
    cn->original_string = (char*)malloc(in_len+1);
    strncpy(cn->original_string, in, in_len);
    cn->original_string[in_len] = '\0';
    r_code = pcre_exec(re_is_cn, pe_is_cn, in, in_len, 0, 0, match_vect, MATCH_VECT_SIZE);
    if (r_code < 1){
      return 0;
    }
    for (i=0;i<r_code;i++){
      matched_len = pcre_copy_substring(in, match_vect, r_code, i, buffer, in_len+1);
      if (matched_len > 0){
        switch(i){
          //these numbers are known because of the structure of the is_cn regular expression
          //i.e. the 1st capturing group is the broad subject, the 2nd is the classifier,
          //     the date Cutter might show up in the 3rd or the 6th, etc.
          case 0:
            // buffer contains the full callnumber.
            // We don't want to do anything with it here;
            // we want to normalize the callnumber before storing it.
            break;
          case 1:
            cn->broad_subject = (char*)malloc(matched_len+1);
            strcpy(cn->broad_subject, str_toupper(buffer));
            break;
          case 2:
            cn->subject_class = (char*)malloc(matched_len+1);
            strcpy(cn->subject_class, str_collapse_whitespace(buffer, 0));
            break;
          case 3:
          case 6:
            cn->date_cutter = (char*)malloc(matched_len+1);
            strcpy(cn->date_cutter, str_toupper(buffer));
            break;
          case 4:
          case 5:
            cn->first_cutter = (char*)malloc(matched_len+1);
            strcpy(cn->first_cutter, str_toupper(str_collapse_whitespace(buffer, 0)));
            break;
          case 7:
            cn->second_cutter = (char*)malloc(matched_len+1);
            strcpy(cn->second_cutter, str_toupper(str_collapse_whitespace(buffer, 0)));
            break;
          case 8:
            cn->extraneous = (char*)malloc(matched_len+1);
            strcpy(cn->extraneous, str_trim(buffer));
            break;
        }
      }
    }
    set_callnumber_member(cn);
  }

  return r_code; //return true if there was a callnumber.
}

void clear_callnumber(callnumber cn){
  if (cn !=  NULL){
    if (cn->original_string != NULL){
      free(cn->original_string);
    }
    if (cn->callnumber != NULL){
      free(cn->callnumber);
    }
    if (cn->broad_subject != NULL){
      free(cn->broad_subject);
    }
    if (cn->subject_class != NULL){
      free(cn->subject_class);
    }
    if (cn->date_cutter != NULL){
      free(cn->date_cutter);
    }
    if (cn->first_cutter != NULL){
      free(cn->first_cutter);
    }
    if (cn->second_cutter != NULL){
      free(cn->second_cutter);
    }
    if (cn->extraneous != NULL){
      free(cn->extraneous);
    }
    memset(cn, 0, sizeof(struct callnumber_st));
  }
}
callnumber new_callnumber(int in_len, const char* in){
  callnumber cn;
  cn = (callnumber)malloc(sizeof(struct callnumber_st));
  init_callnumber(cn, in_len, in);
  return cn;
}
void free_callnumber(callnumber cn){
  if (cn != NULL){
    clear_callnumber(cn);
    free(cn);
    cn = NULL;
  }
}





static int compare_subject_classifiers(const char* a, const char* b){
  double class_a, class_b, diff;

  //assert(a != NULL && b != NULL);

  class_a = strtod(a, NULL);
  class_b = strtod(b, NULL);
  diff = class_b - class_a;

  if (diff > 0.00000001){
    return -1;
  }
  if (diff < -0.00000001){
    return 1;
  }

  //the numbers are equal.  If one has a longer strlen, though, we want that to sort later (probably trailing zeroes).
  return strlen(a) - strlen(b);
}

static int compare_date_cutters(const char* a, const char* b){
  char date_a[8], date_b[8];
  int i, c, date_int_a, date_int_b;

    //there should be no real-life use case in which we are comparing
  // date cutters and only one callnumber has one, and the other doesn't.
  // that's not really "coded in" though, so we'll account for it anyway.
  if (a != NULL && b != NULL){
        //substitute '-' or '?' with '0' and try to compare numbers
    // only if equal, then try to compare as strings
    for (i=0;i<8;i++){
      date_a[i] = a[i] == '-' || a[i] == '?' ? '0' : a[i];
      date_b[i] = b[i] == '-' || b[i] == '?' ? '0' : b[i];
    }
    date_int_a = strtol(date_a, NULL, 10);
    date_int_b = strtol(date_b, NULL, 10);
    c = date_int_b - date_int_a;
    if (c){
      return c;
    }
    c = strcmp(date_a, date_b);
    if (c){
      return c;
    }
    c = strcmp(a, b);
    if (c){
      return c;
    }
  }else if (b != NULL){
    return -1;
  }else if (a != NULL){
    return 1;
  }
  return 0;
}

static int compare_cutter_numbers(const char* a, const char* b){
  return strcmp(a, b);
  /*
  double a_num, b_num, diff;
  char a_chars[3], b_chars[3], a_num_buffer[11], b_num_buffer[11];
  int i, c, a_num_idx, b_num_idx, a_len, b_len;

  a_len = strlen(a);
  b_len = strlen(b);

  for (i=0;i<3;i++){
      a_chars[i] = isalpha(a[i]) ? a[i] : '\0';
    b_chars[i] = isalpha(b[i]) ? b[i] : '\0';
  }

  c = strcmp(a_chars, b_chars);
  if (c){
    return c;
  }

  a_num_buffer[0] = b_num_buffer[0] = '0';
  a_num_buffer[1] = b_num_buffer[1] = '.';
  a_num_idx       = b_num_idx       = 2;

  for (i=strlen(a_chars);i<a_len;i++){
    if (isdigit(a[i])){
      a_num_buffer[a_num_idx] = a[i];
    a_num_idx++;
  }else{
    break;
  }
  }
  a_num_buffer[a_num_idx] = '\0';
  a_num = strtod(a_num_buffer, NULL);

  for(i=strlen(b_chars);i<b_len;i++){
    if (isdigit(b[i])){
    b_num_buffer[b_num_idx] = b[i];
    b_num_idx++;
  }else{
    break;
  }
  }
  b_num_buffer[b_num_idx] = '\0';
  b_num = strtod(b_num_buffer, NULL);

  diff = b_num - a_num;
  if (diff > 0.00000001){
  return -1;
  }
  if (diff < -0.00000001){
  return 1;
  }

  //the numbers are the same.  If a longer strlen is there (trailing zeros), then it sorts later.
  c = strlen(a_num_buffer) - strlen(b_num_buffer);
  if (c){
    return c;
  }

  //if there is anything after the AA99999 part, strcmp it; otherwise, return 0.
  return strcmp(a + strlen(a_chars) + strlen(a_num_buffer), b + strlen(b_chars) + strlen(b_num_buffer));
  */
}



//return -1 if a sorts first, +1 if b sorts first, 0 if they are equal
// "NULL" is less than any callnumber (but equal to another NULL)
// Any string (including "") that is not a callnumber is greater than any callnumber.
// Two strings that are not callnumbers are compared as with strcmp()
int compare_callnumber_strings(int a_len, const char* a, int b_len, const char* b){
  struct callnumber_st cn_a, cn_b;
  int a_is_cn, b_is_cn, c;

  if (a == NULL || a_len < 1){
    return b == NULL || b_len < 1 ? 0 : -1;
  }
  if (b == NULL || b_len < 1){
    return 1;
  }

  a_is_cn = init_callnumber(&cn_a, a_len, a);
  b_is_cn = init_callnumber(&cn_b, b_len, b);

  if (a_is_cn < 1){
    c = (b_is_cn < 1) ? strcmp(a, b) : 1;
  }else if (b_is_cn < 1){
    c = -1;
  }else{
    c = compare_callnumbers(&cn_a, &cn_b);
  }

  clear_callnumber(&cn_a);
  clear_callnumber(&cn_b);

  return c;
}

//return -1 if a sorts first, +1 if b sorts first, 0 if they are equal
int compare_callnumbers(const callnumber a, const callnumber b){
  int c;

  if (a == NULL){
    return b == NULL ? 0 : -1;
  }
  if (b == NULL){
    return 1;
  }

  //a->broad_subject == NULL implies a is not a callnumber
  //if both are not a callnumber, return strcmp(a,b)
  if (a->broad_subject == NULL){
    if (b->broad_subject == NULL){
      if (a->original_string == NULL){
        return b->original_string == NULL ? 0 : -1;
      }
      if (b->original_string == NULL){
        return 1;
      }
      return strcmp(a->original_string, b->original_string);
    }
    return 1;
  }
  if (b->broad_subject == NULL){
    return -1;
  }

  //finally.  We know we are comparing two actual callnumbers now.
  //compare each component according to its own rules.
  //return as soon as we find a difference.

  c = strcmp(a->broad_subject, b->broad_subject);
  if (c){
    return c;
  }

    c = compare_subject_classifiers(a->subject_class, b->subject_class);
  if (c){
    return c;
  }

  c = compare_date_cutters(a->date_cutter, b->date_cutter);
  if (c){
    return c;
  }

  //first cutter
  if (a->first_cutter != NULL && b->first_cutter != NULL){
    c = compare_cutter_numbers(a->first_cutter, b->first_cutter);
    if (c){
      return c;
    }
  }else if (a->first_cutter != NULL){
    return  b->extraneous != NULL ? 1 : -1;
  }else if (b->first_cutter != NULL){
    return  a->extraneous != NULL ? -1 : 1;
  }

  //second cutter
  if (a->second_cutter != NULL && b->second_cutter != NULL){
    c = compare_cutter_numbers(a->second_cutter, b->second_cutter);
    if (c){
      return c;
    }
  }else if (a->second_cutter != NULL){
    return 1;
  }else if (b->second_cutter != NULL){
    return -1;
  }

  //extraneous
  if (a->extraneous == NULL){
    return (b->extraneous == NULL) ? 0 : -1;
  }
  if (b->extraneous == NULL){
    return 1;
  }
  return strcmp(a->extraneous, b->extraneous);

}

int is_callnumber(const char* in){
  int len = strlen(in);
  return 0 < pcre_exec(re_is_cn, pe_is_cn, in, len, 0, 0, 0, 0);
}
//int has_callnumber(const char* in){
//  int len = strlen(in);
//  return 0 < pcre_exec(global_has_cn, has_cn_extra, in, len, 0, 0, 0, 0);
//}

char* normalize_callnumber(const char* str){
  int r, len = strlen(str);
  struct callnumber_st cn;
  char* normalized = NULL;

  r = init_callnumber(&cn, len, str);

  if (r > 0 && cn.callnumber != NULL){
    normalized = (char*)malloc(strlen(cn.callnumber) + 1);
    strcpy(normalized, cn.callnumber);
  }

  clear_callnumber(&cn);

  return normalized;
}
