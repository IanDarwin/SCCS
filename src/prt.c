static char Sccsid[] = "@(#)prt.c	1.2 of 10/5/81";

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

extern char *readsccs();

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
		errors("can't open %s", sname);
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
			errors("%s has illegal format", sname);
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
