h00000
s 00004/00006/00055
d D 1.2 82/01/06 09:11:08 drg 2 1
c Use the login name of the user, rather that his real usered.  People
e
s 00061/00000/00000
d D 1.1 81/10/05 10:23:28 drg 1 0
e
u
U
t
T
I 1
/* Copyright (c) 1981 David R. Galloway */

static char Sccsid[] = "%W% of %G%";

#include <stdio.h>

D 2
#include <pwd.h>
struct passwd *getpwuid();

E 2
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
D 2
	struct passwd *p;
E 2
	long now;
	struct tm *t;
I 2
	char *name;
	extern char *getname();
E 2

	now = time( (long *) 0);
	t = localtime(&now);
	sprintf(d->deltadate, "%02d/%02d/%02d", t->tm_year, t->tm_mon+1,
					t->tm_mday);
	sprintf(d->deltatime, "%02d:%02d:%02d", t->tm_hour, t->tm_min,
					 t->tm_sec);

D 2
	if((p = getpwuid(getuid())) == NULL)
E 2
I 2
	if((name = getname()) == NULL)
E 2
		{
		error("Can't find your login name");
		return(0);
		}
D 2
	strncpy(d->programmer, p->pw_name, sizeof(d->programmer));
E 2
I 2
	strncpy(d->programmer, name, sizeof(d->programmer));
E 2

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
E 1
