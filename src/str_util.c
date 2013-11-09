#include <string.h>
#include "str_util.h"
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>


int count_leading_whitespace(const char* str){
  int len, idx = 0, num = 0;

  if (str != NULL){
    len = strlen(str);
    while ((idx < len) && isspace(str[idx++])){
      num++;
    }
  }
  return num;
}
char* str_toupper(char* str){
  char *s = str;
	if (str == NULL){
		return NULL;
	}
	for(s; *s!='\0'; s++){
		*s = toupper(*s);
	}
	return str;
}

//replaces every whitespace character with replace_char
char* str_replace_whitespace(char* str, char replace_char){
  char *s = str;

  if (str == NULL)
	  return NULL;

  if (replace_char == '\0'){
	  return str_collapse_whitespace(str, replace_char);
  }
  for (s; *s!='\0'; s++){
	  if (isspace(*s)){
		  *s = replace_char;
	  }
  }
  return str;
}
//replaces every sequence of whitespace with replace_char or '' if NULL ('\0')
char* str_collapse_whitespace(char* str, char replace_char){
  char *s = str, *tmp = (char*)alloca(strlen(str)+1);
  int i = 0, num_replaced = 0;

  for(s; *s!= '\0'; s++){
	  if (!isspace(*s)){
		  tmp[i] = *s;
		  i++;
		  num_replaced = 0;
	  }else{
		  if (replace_char != '\0' && num_replaced == 0){
			  tmp[i] = replace_char;
			  i++;
			  num_replaced++;
		  }
	  }
  }
  strncpy(str, tmp, i);
  str[i] = '\0';

  return str;
}

char* str_ltrim(char* str){
  int len;
  int num_chars = count_leading_whitespace(str);

  if (num_chars > 0){
	  len = strlen(str) - num_chars;
	  memmove(str, str+num_chars, len);
	  str[len] = '\0';
  }
  return str;
}

char* str_rtrim(char* str){
  int len_less_1;
  if (str != NULL){
    len_less_1 = strlen(str)-1;
	  while((len_less_1 >= 0) && isspace(str[len_less_1])){
      str[len_less_1--] = '\0';
	  }
  }
  return str;
}

char* str_trim(char* str){
  if (str == NULL){
    return NULL;
  }
  str_ltrim(str);
  str_rtrim(str);
  return str;
}
