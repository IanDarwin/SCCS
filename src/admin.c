/* admin -- a simplified version of the SCCS admin command.
 */

 /* Copyright (c) 1981 David R. Galloway */

static char Sccsid[] = "@(#)admin.c	1.2 of 10/5/81";

#include <stdio.h>
#include "sccs.h"

char usage[] = "usage: admin -ifile s.file";

char *filename = NULL;
char *sname;

char *Progname = "admin";	/* used in error() and fatal() */
FILE *diagfile = stdout;

main(argc, argv)
	int argc;
	char *argv[];
	{
	extern die();

	if(argc < 2)
		fatal(usage);
	--argc;
	argv++;
	while(argv[0][0] == '-')
		{
		switch(argv[0][1])
			{
		case 'i' :
			if(argv[0][2] == '\0' || filename != NULL)
				fatal(usage);
			filename = &argv[0][2];
			break;

		default :
			fatal(usage);
			}
		--argc;
		argv++;
		}

	if(filename == NULL)
		fatal(usage);

	if(argc != 1)
		fatal(usage);

	catchsignals(die);

	sname = argv[0];
	if(!zlock(sname))
		exit(1);

	admin();
	zunlock(sname);
	exit(0);
	}



admin()
	{
	FILE *sfile, *initfile;
	struct delta d;
	char line[512];
	char *n;
	register int linecount;

	if((sfile = fopen(sname, "w")) == NULL)
		{
		errors("can't create %s", sname);
		return;
		}

	destroyOnExit('s');

	chmod(sname, 0444);

	/* We don't bother keeping checksums, but write one out to be
	 * compatible.
	 */

	fprintf(sfile, "%ch00000\n", CONTROLCHAR);

	if((initfile = fopen(filename, "r")) == NULL)
		{
		errors("can't open %s", filename);
		fclose(sfile);
		unlink(sname);
		return;
		}

	/* Count the number of lines in the initial file.
	 */

	linecount = 0;
	while(fgets(line, sizeof(line), initfile) != NULL)
		linecount++;
	d.added = linecount;
	d.deleted = 0;
	d.unchanged = 0;
	d.deltatype = 'D';
	strcpy(d.version, "1.1");
	d.deltanumber = 1;
	d.olddeltanumber = 0;

	if(!writedelta(sfile, &d, ""))
		{
		fclose(sfile);
		unlink(sname);
		return;
		}

	/* Write out null user table and comment table */

	fprintf(sfile, "%cu\n%cU\n", CONTROLCHAR, CONTROLCHAR);
	fprintf(sfile, "%ct\n%cT\n", CONTROLCHAR, CONTROLCHAR);

	fprintf(sfile, "%cI 1\n", CONTROLCHAR);

	rewind(initfile);

	for(linecount=0; ; linecount++)
		{
		n = fgets(line, sizeof(line), initfile);
		if(n == NULL)
			break;
		if(line[0] == CONTROLCHAR)
			{
			unlink(sname);
			fclose(sfile);
			fclose(initfile);
			errord("illegal character on line %d", linecount);
			return;
			}
		fputs(line, sfile);
		}

	fclose(initfile);
	fprintf(sfile, "%cE 1\n", CONTROLCHAR);
	fclose(sfile);
	}
