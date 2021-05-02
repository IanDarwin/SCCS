h00000
s 00003/00000/00480
d D 1.3 82/04/26 16:34:55 drg 3 2
c Fixed up bug that occurred when reading comments from a file or pipe.
e
s 00014/00010/00466
d D 1.2 81/10/05 10:45:16 drg 2 1
c Fixes to error routine calls to go with 1.2 of error.c
e
s 00476/00000/00000
d D 1.1 81/10/05 10:23:22 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

/* delta -- a simple version of the SCCS delta command.  Written by
 *	Dave Galloway, September 1981.
 */

/* Copyright (c) 1981 by David R. Galloway */


#include <stdio.h>
#include "sccs.h"

char usage[] =	"usage: delta [-p] s.file1 s.file2 ...";

#define DIFF	"/bin/diff"

char *sname	= NULL;
int pflag;
char *Progname = "delta";		/* Used in the error() routine */
FILE *diagfile = stdout;

char *nextline();

main(argc, argv)
	int argc;
	char *argv[];
	{
	int i;
	char comments[512];
	extern die();

	if(argc<2)
		fatal(usage);

	--argc;
	argv++;

	while(argv[0][0] == '-')
		{
		switch(argv[0][1])
			{
		case 'p' :	/* print differences */
			pflag++;
			break;

		default :
			fatal(usage);
			}
		--argc;
		argv++;
		}

	printf("comments ?");
	for(i=0; i<sizeof(comments); i++)
		{
		comments[i] = getchar();
		if(feof(stdin) || ferror(stdin))
I 3
			{
			comments[i] = '\n';
E 3
			break;
I 3
			}
E 3
		if(comments[i] == '\n')
			{
			if(i==0 || comments[i-1] != '\\')
				break;
			comments[i-1] = '\n';
			--i;
			}
		}
	if(i >= sizeof(comments) - 1)
D 2
		fatal("comments must be less than %d characters", sizeof(comments)-1);
E 2
I 2
		{
		fprintf(diagfile, "%s: comments must have less than %d characters\n",
			Progname, sizeof(comments)-1);
		exit(1);
		}
E 2
	comments[i+1] = '\0';

	for(i=0; i<argc; i++)
		{
		if(argc>1)
			fprintf(diagfile, "%s:\n", argv[i]);
		if(zlock(argv[i]))
			{
			catchsignals(die);
			sname = argv[i];
			delta(comments);
			zunlock(argv[i]);
			destroyOnExit('\0');
			}
		}
	exit(0);
	}


extern char *makename(), *gname();

char diffname[] = "/tmp/deltaXXXXXX";

struct diff
	{
	int d_ostart;
	int d_deleted;
	char d_function;
	int d_nstart;
	int d_added;
	} diffentry;

#define ADD	'a'
#define DELETE	'd'
#define CHANGE	'c'


extern FILE *copyfile;



delta(comments)
	char *comments;
	{
	FILE *sfile, *tempfile, *xfile, *diffile;
	char xname[512], command[512];
	char *pname;
	char *line;
	extern int linenumber;
	char newtext[512];
	int retcode, lastdelta, noldlines, i;
	char newSid[30];
	struct delta d;

	if((sfile = fopen(sname, "r")) == NULL)
		{
D 2
		error("Can't open %s", sname);
E 2
I 2
		errors("Can't open %s", sname);
E 2
		return;
		}

	if(access(gname(sname), 4) == -1)
		{
D 2
		error("can't open %s", gname(sname));
E 2
I 2
		errors("can't open %s", gname(sname));
E 2
		fclose(sfile);
		return;
		}

	if(!readpfile(sname, newSid))
		{
		fclose(sfile);
		return;
		}

	printf("%s\n", newSid);

	copyfile = NULL;	/* see readsccs(), or comment lower down */

	if(!readchecksum(sfile))
		{
		fclose(sfile);
		return;
		}

	if((retcode = readdelta(sfile, &d)) == NULL)
		{
D 2
		error("%s does not contain a delta table", sname);
E 2
I 2
		errors("%s does not contain a delta table", sname);
E 2
		fclose(sfile);
		return;
		}

	if(retcode == ERROR)
		{
		fclose(sfile);
		return;
		}

	lastdelta = d.deltanumber;

	while((retcode = readdelta(sfile, &d)) != NULL)
		if(retcode == ERROR)
			{
			fclose(sfile);
			return;
			}

	if(!userlist(sfile) || !readflags(sfile) || !readcomments(sfile))
		{
		fclose(sfile);
		return;
		}

	strncpy(xname, makename(sname, 'x'), sizeof(xname));
	makename(sname, 's');

	if((tempfile = fopen(xname, "w")) == NULL)
		{
D 2
		error("can't create %s", xname);
E 2
I 2
		errors("can't create %s", xname);
E 2
		fclose(sfile);
		return;
		}

	destroyOnExit('x');

	linenumber = 0;
	while((line = nextline(sfile, lastdelta)) != NULL)
		fputs(line, tempfile);
	noldlines = linenumber;
	fclose(tempfile);

	mktemp(diffname);

	sprintf(command, "%s %s %s > %s", DIFF, xname, gname(sname),
				diffname);
	retcode = system(command);
	if((retcode & 0377) != 0)	/* diff died on signal */
		{
		unlink(diffname);
		die();
		}
	if(retcode != 0 && (retcode != (1<<8)))
		{
D 2
		error("error in %s command", DIFF);
E 2
I 2
		errors("error in %s command", DIFF);
E 2
		unlink(xname);
		unlink(diffname);
		fclose(sfile);
		return;
		}

	if((diffile = fopen(diffname, "r")) == NULL)
		{
D 2
		error("can't open %s", diffname);
E 2
I 2
		errors("can't open %s", diffname);
E 2
		unlink(xname);
		unlink(diffname);
		fclose(sfile);
		return;
		}

	unlink(diffname);

	/* Use the differences to build a new s.file in x.file */

	rewind(sfile);

	if((xfile = fopen(xname, "w")) == NULL)
		{
D 2
		error("can't open %s", xname);
E 2
I 2
		errors("can't open %s", xname);
E 2
		unlink(xname);
		fclose(sfile);
		fclose(diffile);
		return;
		}

	/* We don't support checksums, but write out a zero one
	 * to be compatible
	 */

	readchecksum(sfile);
	fprintf(xfile, "%ch00000\n", CONTROLCHAR);

	/* copyfile will cause readsccs() to copy everything it
	 * reads from sfile to xfile.
	 */

	copyfile = xfile;

	d.deltanumber = lastdelta+1;
	d.olddeltanumber = lastdelta;
	strncpy(d.version, newSid, sizeof(d.version));
	countchanges(diffile, &d.added, &d.deleted);
	d.unchanged = noldlines - d.deleted;
	if(!writedelta(xfile, &d, comments))
		{
		unlink(xname);
		fclose(sfile);
		fclose(diffile);
		fclose(xfile);
		return;
		}

	while((retcode = readdelta(sfile, &d)) != NULL)
		if(retcode == ERROR)
			{
			unlink(xname);
			fclose(sfile);
			fclose(diffile);
			fclose(xfile);
			return;
			}

	if(!userlist(sfile) || !readflags(sfile) || !readcomments(sfile))
		{
		unlink(xname);
		fclose(sfile);
		fclose(diffile);
		fclose(xfile);
		return;
		}

	rewind(diffile);

	linenumber = 0;
	while(readdiff(diffile, &diffentry) != NULL)
		{
		while(linenumber != diffentry.d_ostart)
			line = nextline(sfile, lastdelta);

		switch(diffentry.d_function)
			{

		case ADD :
			fprintf(xfile, "%cI %d\n", CONTROLCHAR, lastdelta+1);
			for(i=0; i<diffentry.d_added; i++)
				{
				fgets(newtext, sizeof(newtext), diffile);
				fputs(&newtext[2], xfile);
				}
			fprintf(xfile, "%cE %d\n", CONTROLCHAR, lastdelta+1);
			break;

		case DELETE :
			fprintf(xfile, "%cD %d\n", CONTROLCHAR, lastdelta+1);
			for(i=0; i<diffentry.d_deleted; i++)
				{
				fgets(newtext, sizeof(newtext), diffile);
				line = nextline(sfile, lastdelta);
				}
			fprintf(xfile, "%cE %d\n", CONTROLCHAR, lastdelta+1);
			break;

		case CHANGE :
			fprintf(xfile, "%cD %d\n", CONTROLCHAR, lastdelta+1);
			for(i=0; i<diffentry.d_deleted; i++)
				{
				fgets(newtext, sizeof(newtext), diffile);
				line = nextline(sfile, lastdelta);
				}
			fprintf(xfile, "%cE %d\n", CONTROLCHAR, lastdelta+1);
			fgets(newtext, sizeof(newtext), diffile);
			fprintf(xfile, "%cI %d\n", CONTROLCHAR, lastdelta+1);
			for(i=0; i<diffentry.d_added; i++)
				{
				fgets(newtext, sizeof(newtext), diffile);
				fputs(&newtext[2], xfile);
				}
			fprintf(xfile, "%cE %d\n", CONTROLCHAR, lastdelta+1);
			break;

			}
		}

	while((line = nextline(sfile, lastdelta)) != NULL)
		;

	fclose(sfile);
	fclose(xfile);

	if(pflag)
		{
		rewind(diffile);
		while(fgets(newtext, sizeof(newtext), diffile) != NULL)
			fputs(newtext, stdout);
		}

	fclose(diffile);

	catchsignals(1);

	unlink(sname);
	if(link(xname, sname) == -1)
		{
D 2
		error("error in linking %s to %s", xname, sname);
E 2
I 2
		errorss("error in linking %s to %s", xname, sname);
E 2
		return;
		}
	unlink(xname);
	chmod(sname, 0444);
	unlink(gname(sname));
	pname = makename(sname, 'p');
	unlink(pname);
	makename(sname, 's');
	printf("%d added\n%d deleted\n%d unchanged\n", d.added, d.deleted,
			d.unchanged);
	}



/* readpfile -- reads newSid from the p.whatever file that get -e produced.
 * Returns 1 on success, 0 on failure.
 */

readpfile(sname, newSid)
	char *sname, newSid[];
	{
	char *pname;
	FILE *pfile;

	pname = makename(sname, 'p');
	if((pfile = fopen(pname, "r")) == NULL)
		{
D 2
		error("can't open %s", pname);
E 2
I 2
		errors("can't open %s", pname);
E 2
		makename(sname, 's');
		return(0);
		}
	makename(sname, 's');

	fscanf(pfile, "%s", newSid);
	fscanf(pfile, "%s", newSid);
	fclose(pfile);
	return(1);
	}



/* countchanges -- reads through the output of diff(1), counting up the
 * number of lines added and deleted.
 */

countchanges(diffile, addp, delp)
	FILE *diffile;
	int *addp, *delp;
	{
	struct diff d;
	register int i;
	char line[512];

	*addp = 0;
	*delp = 0;

	while(readdiff(diffile, &d) != NULL)
		{
		*addp += d.d_added;
		*delp += d.d_deleted;
		for(i=0; i<d.d_deleted; i++)
			fgets(line, sizeof(line), diffile);
		if(d.d_function == 'c')
			fgets(line, sizeof(line), diffile);
		for(i=0; i<d.d_added; i++)
			fgets(line, sizeof(line), diffile);
		}
	}



/* readdiff -- reads a change line from the output of diff(1), of the form
 * mmm[,nnn]xiii[,jjj] -- where x is a, d or c.
 */

readdiff(diffile, dp)
	FILE *diffile;
	struct diff *dp;
	{
	int c;

	fscanf(diffile, "%d", &dp->d_ostart);
	c = getc(diffile);
	if(c == ',')
		{
		fscanf(diffile, "%d", &dp->d_deleted);
		dp->d_deleted = dp->d_deleted - dp->d_ostart + 1;
		c = getc(diffile);
		}
	else
		dp->d_deleted = 1;

	if(c != ADD && c != DELETE && c != CHANGE)
		return(NULL);
	dp->d_function = c;

	fscanf(diffile, "%d", &dp->d_nstart);
	c = getc(diffile);
	if(c == ',')
		{
		fscanf(diffile, "%d", &dp->d_added);
		dp->d_added = dp->d_added - dp->d_nstart + 1;
		c = getc(diffile);
		}
	else
		dp->d_added = 1;
	if(dp->d_function == ADD)
		dp->d_deleted = 0;
	else
		dp->d_ostart--;
	if(dp->d_function == DELETE)
		dp->d_added = 0;
	if(c == '\n')
		return(OK);
	else
		return(NULL);
	}
E 1
