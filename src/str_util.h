#ifndef _STR_UTIL_H_
#define _STR_UTIL_H_

char* str_toupper(char* str);

//replaces every whitespace character with replace_char, or '' if NULL
char* str_replace_whitespace(char* str, char replace_char);
//replaces every sequence of whitespace with replace_char or '' if NULL
char* str_collapse_whitespace(char* str, char replace_char);

char* str_ltrim(char* str);
char* str_rtrim(char* str);
char* str_trim(char* str);

#endif
