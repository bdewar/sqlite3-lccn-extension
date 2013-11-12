sqlite3-lcsh-extension
======================

Extends an SQLite3 database with SQL functions related to normalizing and sorting Library of Congress Shelving Numbers

I had dredged this up from an old project and put it here in hopes of refreshing/refinining it a bit.

It is in use at http://library.stfx.ca/circbycallno (Cirulation by Callnumber)  It seems to work well there (on real data and lots of it), but I can make it segfault on test data in the sqlite shell program now and then, so use at your own risk.


Use
==============

Load this extension into your SQLite3 database.  Once that's done, you have access to a new SQL function: NORMALIZE_LCCN and an SQL collation: LCCN.

The NORMALIZE_LCCN function accepts a single string argument.  If it resembles a Library of Congress shelving number, the return value is a normalized shelving number, otherwise NORMALIZE_LCCN evaluates to NULL.

The LCCN collating sequence can be added to a column as the default sort order, or can be used in the ORDER BY clause of any SELECT statement.  It will sort shelving numbers properly.

... more complete documentation coming soon.

