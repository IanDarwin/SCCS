h00000
s 00004/00002/00105
d D 1.2 81/10/05 10:58:31 drg 2 1
c Fix lint.  Convert to use version 1.2 of error.c.
e
s 00107/00000/00000
d D 1.1 81/10/05 10:23:25 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

/* prt -- a simple version of the SCCS prt command.  Written by
 *	Dave Galloway, August 1981.
 */

/* Copyright (c) 1981 by David R. Galloway */


#include <stdio.h>
#include "sccs.h"

char usage[] =	"usage: prt s.file1 s.file2 ...";

char *Progname = "prt";		/* Used in the error() routine */
FILE *diagfile = stdout;

char *sname;

I 2
extern char *readsccs();

E 2
main(argc, argv)
	int argc;
	char *argv[];
	{
	int i;

	if(argc<2)
		fatal(usage);

	--argc;
	argv++;

	while(argv[0][0] == '-')
		fatal(usage);

	for(i=0; i<argc; i++)
		{
		fprintf(diagfile, "\n%s:\n\n", argv[i]);
		if(isvalid(argv[i]))
			{
			sname = argv[i];
			prt();
			}
		}
	exit(0);
	}


prt()
	{
	FILE *sfile;
	char *line;
	char linecounts[20];
	struct delta d;

	if((sfile = fopen(sname, "r")) == NULL)
		{
D 2
		error("can't open %s", sname);
E 2
I 2
		errors("can't open %s", sname);
E 2
		return;
		}

	if(!readchecksum(sfile))
		{
		fclose(sfile);
		return;
		}

	while((line = readsccs(sfile)) != NULL)
		{
		if(!isjclline(line))
			{
D 2
			error("%s has illegal format", sname);
E 2
I 2
			errors("%s has illegal format", sname);
E 2
			fclose(sfile);
			return;
			}
		switch(line[1])
			{

		case 'u' :	/* end of delta table */
			fclose(sfile);
			return;

		case 'e' :	/* end of delta table entry */
			putchar('\n');
			break;

		case 's' :
			strncpy(linecounts, &line[3], sizeof(linecounts));
			break;

		case 'd' :
			sscanf(&line[3], "%c %s %s %s %s %d %d", &d.deltatype,
				d.version, d.deltadate, d.deltatime,
				d.programmer,
				&d.deltanumber, &d.olddeltanumber);
			printf("%c %s\t%s %s\t%s\t%d\t%d\t%s", d.deltatype,
				d.version, d.deltadate, d.deltatime,
				d.programmer, d.deltanumber, d.olddeltanumber,
				linecounts);
			break;

		default :
			printf("%s", &line[3]);
			break;
			}
		}
	fclose(sfile);
	}
E 1
