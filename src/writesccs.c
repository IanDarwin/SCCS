/* Copyright (c) 1981 David R. Galloway */

static char Sccsid[] = "@(#)writesccs.c	1.2 of 1/6/82";

#include <stdio.h>

#include <time.h>
long time();
struct tm *localtime();

#include "sccs.h"


/* writedelta() -- write the given delta table into sfile.  Returns 1 if OK,
 * zero otherwise.
 */

writedelta(sfile, d, comment)
	FILE *sfile;
	struct delta *d;
	char *comment;
	{
	register int i, newline;
	long now;
	struct tm *t;
	char *name;
	extern char *getname();

	now = time( (long *) 0);
	t = localtime(&now);
	sprintf(d->deltadate, "%02d/%02d/%02d", t->tm_year, t->tm_mon+1,
					t->tm_mday);
	sprintf(d->deltatime, "%02d:%02d:%02d", t->tm_hour, t->tm_min,
					 t->tm_sec);

	if((name = getname()) == NULL)
		{
		error("Can't find your login name");
		return(0);
		}
	strncpy(d->programmer, name, sizeof(d->programmer));

	fprintf(sfile, "%cs %05d/%05d/%05d\n", CONTROLCHAR, d->added,
			d->deleted, d->unchanged);
	fprintf(sfile, "%cd %c %s %s %s %s %d %d\n", CONTROLCHAR, d->deltatype,
			d->version, d->deltadate, d->deltatime, d->programmer,
			d->deltanumber, d->olddeltanumber);

	newline = 1;
	for(i=0; comment[i] != '\0'; i++)
		{
		if(newline)
			fprintf(sfile, "%cc ", CONTROLCHAR);
		putc(comment[i], sfile);
		newline = (comment[i] == '\n');
		}
	fprintf(sfile, "%ce\n", CONTROLCHAR);
	return(1);
	}
