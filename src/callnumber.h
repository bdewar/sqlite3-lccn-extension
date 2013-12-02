#ifndef CALLNUMBER_H
#define CALLNUMBER_H

typedef struct callnumber_st{
  char* original_string;
  char* callnumber;
  char* broad_subject;
  char* subject_class;
  char* date_cutter;
  char* first_cutter;
  char* second_cutter;
  char* extraneous;
}*callnumber;
/*
there's a whole lot of mallocing going on when we sort a database table
by callnumber.
stack-based storage here would likely offer HUGE performance benefits.
e.g.
typedef struct callnumber_st{
  char* original_string;
  char* callnumber;
  char broad_subject[4];
  double subject_class;
  char date_cutter[8];
  char first_cutter[20];
  char second_cutter[20];
  char* extraneous;
} *callnumber;
*/


// @param cn must point to existing callnumber space.
// if @param in is not NULL, attempt to parse
// the in string and populate the callnumber fields
// returns >0 if in was a callnumber, 0 if not, <0 on error.
int init_callnumber(callnumber cn, int in_len, const char* in);

// free any dynamically allocated memory in the callnumber struct
// and memset(0) the rest.
void clear_callnumber(callnumber cn);

// dynamically allocate a new call number.
// if @param in is not NULL, attempt to parse
// the in string and populate the callnumber fields
callnumber new_callnumber(int in_len, const char* in);

// de-allocate a dynamically allocated callnumber
void free_callnumber(callnumber cn);

//returns non-zero if @param in is parsable as a callnumber
int is_callnumber(const char* in);

// returns non-zero if @param contains at least one callnumber
//int has_callnumber(const char* in);

int compare_callnumber_strings(int a_len, const char* a, int b_len, const char* b);
int compare_callnumbers(const callnumber a, const callnumber b);

//if the input string is a callnumber, this function allocates a new string and returns the normalized callnumber
//if the input string is not a callnumber, nothing is allocated and NULL is returned.
char* normalize_callnumber(const char* str);

#endif
