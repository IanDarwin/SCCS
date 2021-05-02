
/* Copyright (c) 1981 by David R. Galloway */

static char Sccsid[] = "@(#)names.c	1.2 of 10/5/81";

#define NULL	0

/* This file contains all the routines that look at pathnames, and that
 * know about the silly s.filename convention.
 */

/* sdot() returns the position of the 's' in a pathname /blah/blah/s.file
 * NOTE: rindex(string, c) is a library routine that returns the last
 * occurance of c in string, or 0 if none.
 */

static char *sdot(sname)
	char *sname;
	{
	extern char *rindex();
	char *p;

	if((p = rindex(sname, '/')) != 0)
		p++;
	else
		p = sname;
	return(p);
	}



/* gname() given s.filename, returns a pointer to filename.
 */

char *gname(sname)
	char *sname;
	{
	return(sdot(sname) + 2);
	}



/* makename -- given s.filename, returns a pointer to x.filename, where
 * x is the value of c.  Modifies the original string.
 */

char *makename(sname, c)
	char *sname, c;
	{
	char *p;

	p = sdot(sname);
	*p = c;
	return(sname);
	}



/* isvalid() -- returns 1 if sname is well-formed, 0 otherwise
 */

isvalid(sname)
	char *sname;
	{
	char *p;

	p = sdot(sname);
	if(p[0] != 's' || p[1] != '.')
		{
		errors("%s is not a legal name for an SCCS file", sname);
		return(0);
		}
	return(1);
	}
