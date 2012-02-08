/*
This code is copyright (C) 1998-1999 Robert O'Callahan.
See LICENSE.TXT for the license.
*/

#ifndef __MATCHER_H
#define __MATCHER_H

#define is_pattern_char(ch) ((ch) != 0 && (ch) != ' ' && (ch) != ',' && (ch) != '\n' && (ch) != '\r' && (ch) != '\t')

/*
Matches a string against a glob pattern. The pattern is a string of
pattern characters (everything except 0, space, comma, \n, \r, and \t)
followed by one or more non-pattern characters. ? and * in the pattern
are metacharacters interpreted in the normal way.

Before entry, ensure that str does not contain any non-pattern characters
and that it does not contain any pattern metacharacters.

This function is optimized for matching many patterns against one string,
such as occurs when searching an ssh_known_hosts file for a host key.
*/
BOOL match_pattern(char const FAR * pattern, char const FAR * str);

#endif
