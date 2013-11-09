#ifndef LCCN_PATTERNS_H
#define LCCN_PATTERNS_H

#include "pcre.h"

//some global variables we're going to use
pcre       *re_is_cn;
pcre_extra *pe_is_cn;


//the various parts of the massive call number regex

//whitespace/separators
// the various parts of the call number may or may not be separated.
// usually, it's with whitespace, sometimes with the "/" character.
//  this doesn't handle the $$i $$j, $$etc notation that Aleph uses
//  in it's database sometimes (I'm not sure where that comes from).
#define P_OPTIONAL_WSPACE "(?:/|\\s{0,3})"
#define P_REQUIRED_WSPACE "(?:/|\\s{1,3})"

//each of the following will generate one capturing group in whatever regex it ends up in
#define P_BROAD_SUBJECT   P_OPTIONAL_WSPACE "([A-Za-z]{1,3})"
#define P_SUBJECT_CLASS   P_OPTIONAL_WSPACE "([1-9]\\d{0,4}(?:" P_OPTIONAL_WSPACE "\\.\\d{1,8})?)"
#define P_DATE_CUTTER     P_REQUIRED_WSPACE "([1-9][-?0-9]{0,3}(?:st|ST|nd|ND|rd|RD|th|TH)?)"
#define P_CUTTER          P_OPTIONAL_WSPACE "\\.?([A-Za-z]{1,2}\\s{0,3}(?:[0-9][0-9\\.]{0,7}[A-Z]?[a-z]{0,2})?)"
#define P_EXTRANEOUS      "(?:" P_REQUIRED_WSPACE "(.*))?"


//now we put the parts together into useful regular expressions:

//a regular expression that matches strings that ARE library of congress call numbers
#define P_IS_CN   "^" P_BROAD_SUBJECT P_SUBJECT_CLASS \
			      "(?:" P_DATE_CUTTER P_CUTTER "|" P_CUTTER "|" P_DATE_CUTTER ")" \
				  "(?:" P_CUTTER ")?" \
				  P_EXTRANEOUS "$"

//a regular expression that matches strings that CONTAIN library of congress call numbers
#define P_HAS_CN  P_BROAD_SUBJECT P_SUBJECT_CLASS \
			      "(?:" P_DATE_CUTTER P_CUTTER "|" P_CUTTER "|" P_DATE_CUTTER ")" \
				  "(?:" P_CUTTER ")?" \
				  P_EXTRANEOUS

//a regular expression that matches the leading part of a library of congress call number (i.e. no Cutters)
#define P_PART_CN "^" P_BROAD_SUBJECT "(?:" P_SUBJECT_CLASS ")?$"


//3 * max number of captured groups
// pcre_exec() needs this number to allocate enough space to tell us about matches
//
// it's defined here because this is where we define all our patterns
//  (we can see by inspecting this file that the pattern
//   with the most capturing groups has 8 of them)
#define MATCH_VECT_SIZE 30

#endif

