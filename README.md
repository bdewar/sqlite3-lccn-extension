sqlite3-lccn-extension
======================

Extends an SQLite3 database with SQL functions related to normalizing and sorting Library of Congress Call Numbers.

I had dredged this up from an old project and put it here in hopes of refreshing/refinining it a bit.

It is in use at http://library.stfx.ca/circbycallno (Cirulation by Callnumber)  It seems to work well there (on real data and lots of it).


Use
==============

Load this extension into your SQLite3 database.  Once that's done, you have access to a new SQL function: NORMALIZE_LCCN and an SQL collation: LCCN.

The NORMALIZE_LCCN function accepts a single string argument.  If it resembles a Library of Congress shelving number, the return value is a normalized shelving number, otherwise NORMALIZE_LCCN evaluates to NULL.

The LCCN collating sequence can be added to a column as the default sort order, or can be used in the ORDER BY clause of any SELECT statement.  It will sort shelving numbers properly.  For example:

sqlite>
sqlite> -- create a table and INSERT a bunch of loosely formatted callnumbers
sqlite>
sqlite>CREATE TABLE books (
  title string,
  callnumber string
);
sqlite>INSERT INTO books VALUES ('title1', 'bf 1 a1');
sqlite>INSERT INTO books VALUES ('title2', 'BF2 A1');
sqlite>INSERT INTO books VALUES ('title3', 'BF 2 .A12');
sqlite>INSERT INTO books VALUES ('title4', 'BF 2 A2');
sqlite>INSERT INTO books VALUES ('title5', 'BF 12 A1');
sqlite>INSERT INTO books VALUES ('title6', 'BF12 .A12');
sqlite>INSERT INTO books VALUES ('title7', 'BF 12 A2');
sqlite>
sqlite> SELECT * FROM books ORDER BY callnumber;
title5 | BF 12 A1
title7 | BF 12 A2
title3 | BF 2 .A12
title4 | BF 2 A2
title6 | BF12 .A12
title2 | BF2 A1
title1 | bf 1 a1
sqlite>
sqlite> -- terrible.  But now for the magic!
sqlite>
sqlite>.load /path/to/sqlite3-lccn-extension
sqlite>
sqlite>SELECT * FROM books ORDER BY callnumber COLLATE LCCN;
title1 | bf 1 a1
title2 | BF2 A1
title3 | BF 2 .A12
title4 | BF 2 A2
title5 | BF 12 A1
title6 | BF12 .A12
title7 | BF 12 A2
sqlite>
sqlite> -- OK, but we can do even better.
sqlite>
sqlite>SELECT title, NORMALIZE_LCCN(callnumber) FROM books ORDER BY callnumber COLLATE LCCN;
title1 | BF 1 .A1
title2 | BF 2 .A1
title3 | BF 2 .A12
title4 | BF 2 .A2
title5 | BF 12 .A1
title6 | BF 12 .A12
title7 | BF 12 .A2
sqlite>
sqlite> -- beautiful.

