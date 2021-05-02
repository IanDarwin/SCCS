h00000
s 00008/00008/00252
d D 1.2 81/10/05 10:59:39 drg 2 1
c Convert to use version 1.2 of errors.c
e
s 00260/00000/00000
d D 1.1 81/10/05 10:23:26 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

/* Copyright (c) 1981 by David R. Galloway */

/* readsccs -- a collection of routines used to read in the various
 * parts of an sccsfile.
 */

#include <stdio.h>
#include "sccs.h"

int linenumber;
int checksum;
FILE *copyfile = NULL;	/* tells readsccs() to copy the input file to copyfile */



char *readsccs(sfile)
	FILE *sfile;
	{
	static char line[512];

	if(fgets(line, sizeof(line), sfile) == NULL && ferror(sfile))
		{
		error("error in reading sccs file");
		return(NULL);
		}
	if(feof(sfile))
		return(NULL);
	if(copyfile != NULL)
		fputs(line, copyfile);
	return(line);
	}



skiptilE(sfile, deltanumber)
	FILE *sfile;
	int deltanumber;
	{
	register char *line;

	while((line = readsccs(sfile)) != NULL)
		{
		if(isjclline(line) && line[1] == 'E'
				&& atoi(&line[3]) == deltanumber)
			break;
		}
	}



isjclline(line)
	char *line;
	{
	return(*line == CONTROLCHAR);
	}


readchecksum(sfile)
	FILE *sfile;
	{
	char *line;

	if((line = readsccs(sfile)) == NULL || !isjclline(line))
		return(0);
	if(line[1] != 'h')
		return(0);
	checksum = atoi(&line[2]);
	return(1);
	}



/* nextline(sfile, deltanumber) returns the next line of real text
 * from the version of sfile corresponding to deltanumber.
 */

char *nextline(sfile, deltanumber)
	FILE *sfile;
	int deltanumber;
	{
	register char *line;
	register int newlevel;

	line = readsccs(sfile);
	while(line != NULL)
		{
		if(!isjclline(line))
			{
			linenumber++;
			return(line);
			}
		switch(line[1])
			{

		case 'E' :
			break;

		case 'I' :
			newlevel = atoi(&line[3]);
			if(newlevel > deltanumber)
				skiptilE(sfile, newlevel);
			break;

		case 'D' :
			newlevel = atoi(&line[3]);
			if(newlevel <= deltanumber)
				skiptilE(sfile, newlevel);
			break;
			}
		line = readsccs(sfile);
		}
	return(NULL);
	}



readdelta(sfile, dp)
	FILE *sfile;
	struct delta *dp;
	{
	char *line;
	extern char *sname;

	while(1)
		{
		if((line = readsccs(sfile)) == NULL)
			{
D 2
			error("%s ends in middle of delta table", sname);
E 2
I 2
			errors("%s ends in middle of delta table", sname);
E 2
			return(ERROR);
			}
		if(!isjclline(line))
			{
	Bad:
D 2
			error("%s has illegal delta table format", sname);
E 2
I 2
			errors("%s has illegal delta table format", sname);
E 2
			return(ERROR);
			}
		switch(line[1])
			{
		case 'u' :	/* start of user name table */
			return(NULL);

		case 'e' :	/* end of this delta table entry */
			return(OK);

		case 's' :
		case 'i' :
		case 'x' :
		case 'g' :
		case 'm' :
		case 'c' :
			break;

		case 'd' :
			sscanf(&line[3], "%c %s %s %s %s %d %d", &dp->deltatype,
				dp->version, dp->deltadate, dp->deltatime,
				dp->programmer,
				&dp->deltanumber, &dp->olddeltanumber);
			break;

		default :
			goto Bad;
			}
		}
	}



/*	userlist() -- reads in the userlist table from the given SCCS file.
 * Returns 1 if everything OK, 0 otherwise.  Assumes that the @u line
 * has already been read by the readdelta() routine.
 */

userlist(sfile)
	FILE *sfile;
	{
	char *line;
	extern char *sname;

	while(1)
		{
		if((line = readsccs(sfile)) == NULL)
			{
D 2
			error("%s ends in middle of user table", sname);
E 2
I 2
			errors("%s ends in middle of user table", sname);
E 2
			return(0);
			}
		if(isjclline(line))
			if(line[1] == 'U')
				return(1);
			else
				{
D 2
				error("%s has illegal user table format", sname);
E 2
I 2
				errors("%s has illegal user table format", sname);
E 2
				return(0);
				}
		}
	}


/* 	readflags() -- reads in the flag keywords from the SCCS file.
 * Returns 1 if OK, 0 otherwise.  Stops after reading in the start of
 * the comments field (ie the @t line).
 */

readflags(sfile)
	FILE *sfile;
	{
	char *line;
	extern char *sname;

	while(1)
		{
		if((line = readsccs(sfile)) == NULL)
			{
D 2
			error("%s ends in middle of flags", sname);
E 2
I 2
			errors("%s ends in middle of flags", sname);
E 2
			return(0);
			}
		if(isjclline(line))
			{
			if(line[1] == 't')	/* Start of comments */
				return(1);
			else
				if(line[1] == 'f')
					continue;
			}
D 2
		error("%s has illegal flag table format", sname);
E 2
I 2
		errors("%s has illegal flag table format", sname);
E 2
		return(0);
		}
	}



/*	readcomments() -- reads in the comments table from the given SCCS file.
 * Returns 1 if everything OK, 0 otherwise.  Assumes that the @t line
 * has already been read by the readflags() routine.
 */

readcomments(sfile)
	FILE *sfile;
	{
	char *line;
	extern char *sname;

	while(1)
		{
		if((line = readsccs(sfile)) == NULL)
			{
D 2
			error("%s ends in middle of comment table", sname);
E 2
I 2
			errors("%s ends in middle of comment table", sname);
E 2
			return(0);
			}
		if(isjclline(line))
			if(line[1] == 'T')
				return(1);
			else
				{
D 2
				error("%s has illegal comment table format", sname);
E 2
I 2
				errors("%s has illegal comment table format", sname);
E 2
				return(0);
				}
		}
	}
E 1
