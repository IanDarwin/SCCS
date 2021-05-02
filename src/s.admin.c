h00000
s 00006/00004/00140
d D 1.2 81/10/05 10:53:18 drg 2 1
c Convert to 1.2 of error.c.  Fix lint.  Forgot to completely
c initialize the d structure.
e
s 00144/00000/00000
d D 1.1 81/10/05 10:23:21 drg 1 0
e
u
U
t
T
I 1
/* admin -- a simplified version of the SCCS admin command.
 */

 /* Copyright (c) 1981 David R. Galloway */

static char Sccsid[] = "%W% of %G%";

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
D 2
	int n;
E 2
I 2
	char *n;
E 2
	register int linecount;

	if((sfile = fopen(sname, "w")) == NULL)
		{
D 2
		error("can't create %s", sname);
E 2
I 2
		errors("can't create %s", sname);
E 2
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
D 2
		error("can't open %s", filename);
E 2
I 2
		errors("can't open %s", filename);
E 2
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
I 2
	d.deleted = 0;
	d.unchanged = 0;
E 2
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
D 2
			error("illegal character on line %d", linecount);
E 2
I 2
			errord("illegal character on line %d", linecount);
E 2
			return;
			}
		fputs(line, sfile);
		}

	fclose(initfile);
	fprintf(sfile, "%cE 1\n", CONTROLCHAR);
	fclose(sfile);
	}
E 1
