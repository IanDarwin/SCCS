static char Sccsid[] = "@(#)get.c	1.4 of 1/6/82";

/* get -- a simple version of the SCCS get command.  Written by
 *	Dave Galloway, August 1981.
 */

/* Copyright (c) 1981 by David R. Galloway */


#include <stdio.h>
#include "sccs.h"

char usage[] =	"usage: get [-e] [-g] [-rnnn] s.file1 s.file2 ...";

int eflag, gflag;

/* These flags are only used by sccsdiff(1) and delta(1).  Let's leave
 * them out of the usage message.
 */

int kflag, pflag, sflag;

char *Progname = "get";		/* Used in the error() routine */
FILE *diagfile = stdout;

char *nextline();

main(argc, argv)
	int argc;
	char *argv[];
	{
	char *vdesired = NULL;
	int i;
	extern die();

	if(argc<2)
		fatal(usage);

	--argc;
	argv++;


	while(argv[0][0] == '-')
		{
		switch(argv[0][1])
			{
		case 'e' :	/* wants to edit gfile */
			eflag++;
			break;

		case 'g' :	/* don't produce new gfile */
			gflag++;
			break;

		case 'r' :	/* desired version number */
			vdesired = &argv[0][2];
			break;

		case 'k' :	/* don't expand keywords */
			kflag++;
			break;

		case 'p' :	/* put gfile on stdout */
			pflag++;
			break;

		case 's' :	/* suppress superfluous diagnostics */
			sflag++;
			break;

		default :
			fatal(usage);
			}
		--argc;
		argv++;
		}

	if(pflag)
		diagfile = stderr;

	catchsignals(die);

	for(i=0; i<argc; i++)
		{
		if(argc>1 && !sflag)
			fprintf(diagfile, "%s:\n", argv[i]);
		if(eflag)
			{
			if(!zlock(argv[i]))
				continue;
			}
		else
			if(!isvalid(argv[i]))
				continue;
		get(argv[i], vdesired);
		if(eflag)
			zunlock(argv[i]);
		destroyOnExit('\0');
		}
	exit(0);
	}


extern char *makename(), *gname();

char *sname	= NULL;
struct delta desired;		/* set in get(), used in expandkeywords() */



get(s, vdesired)
	char *s, *vdesired;
	{
	FILE *sfile, *gfile;
	struct delta d;
	int retcode, havedelta;
	extern int linenumber;
	char *line, *release, *level, *branch, *sequence;
	char newSid[30], lastSID[30];

	sname = s;

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

	havedelta = 0;

	if((retcode = readdelta(sfile, &d)) == NULL)
		{
		errors("%s does not contain a delta table", sname);
		fclose(sfile);
		return;
		}
	if(retcode == ERROR)
		{
		fclose(sfile);
		return;
		}

	if(vdesired == NULL)
		{

		/* Take the most recent (first) one by default */

		copyb(&d, &desired, sizeof(d));
		havedelta = 1;
		}

	if(newrelease(vdesired, d.version))
		{
		copyb(&d, &desired, sizeof(d));
		havedelta = 1;
		strncpy(lastSID, vdesired, sizeof(lastSID));
		}
	else
		strncpy(lastSID, d.version, sizeof(lastSID));

	do
		{
		if(retcode == ERROR)
			{
			fclose(sfile);
			return;
			}
		if(!havedelta && sidmatch(vdesired, d.version))
			{
			copyb(&d, &desired, sizeof(d));
			havedelta = 1;
			}
		}
		while((retcode=readdelta(sfile, &d)) != NULL);

	if(!userlist(sfile) || !readflags(sfile) || !readcomments(sfile))
		{
		fclose(sfile);
		return;
		}

	if(!havedelta)
		{
		errorss("%s does not contain version %s", sname, vdesired);
		fclose(sfile);
		return;
		}

	if(!sflag)
		fprintf(diagfile, "%s\n", desired.version);

	if(eflag)
		{
		decodeSID(lastSID, &release, &level, &branch, &sequence);
		buildSID(newSid, atoi(release), atoi(level)+1, atoi(branch),
				atoi(sequence));
		if(!makepfile(sname, desired.version, newSid))
			{
			fclose(sfile);
			return;
			}
		}

	if(gflag)
		{
		fclose(sfile);
		return;
		}

	if(!pflag)
		{
		if(access(gname(sname), 2) != -1 && getuid() != 0)
			{
			fclose(sfile);
			errors("writable %s exists", gname(sname));
			return;
			}
	
		unlink(gname(sname));
	
		if((gfile = fopen(gname(sname), "w")) == NULL)
			{
			fclose(sfile);
			errors("can't create %s", gname(sname));
			return;
			}
	
		if(!eflag)
			chmod(gname(sname), 0444);
		}
	else
		gfile = stdout;

	linenumber = 0;
	while((line = nextline(sfile, desired.deltanumber)) != NULL)
		if(!eflag && !kflag)
			expandkeywords(line, gfile);
		else
			fputs(line, gfile);
	if(!sflag)
		fprintf(diagfile, "%d lines\n", linenumber);
	if(!pflag)
		fclose(gfile);
	fclose(sfile);
	}



/* sidmatch() -- returns 1 if the pattern matches the sid, 0 otherwise
 */


sidmatch(pattern, sid)
	char *pattern, *sid;
	{
	char *release, *level, *branch, *sequence;

	if(!strcmp(pattern, sid))
		return(1);
	decodeSID(sid, &release, &level, &branch, &sequence);
	return(!strcmp(pattern, release));
	}



/* newrelease() -- return 1 if the user wants a new release, 0 otherwise.
 */

newrelease(request, lastSID)
	char *request, *lastSID;
	{
	char *release, *level, *branch, *sequence;
	char *index();

	/* to get a new release, the request must consist of a release number
	 * only, and the release number must be higher than any pre-existing
	 * release number.
	 */

	if(index(request, '.') != 0)
		return(0);

	decodeSID(lastSID, &release, &level, &branch, &sequence);
	if(atoi(request) > atoi(release))
		return(1);
	else
		return(0);
	}



/* makepfile -- creates the p.whatever file, when you use get -e.
 */

makepfile(sname, oldsid, newsid)
	char *sname, *oldsid, *newsid;
	{
	FILE *pfile;
	char logname[10];
	char *name;
	extern char *getname();
	char *pname;
	extern char *DTstring;

	if((name = getname()) == NULL)
		{
		error("can't find your login name");
		return(0);
		}
	else
		strncpy(logname, name, sizeof(logname));
	pname = makename(sname, 'p');
	if(access(pname, 4) != -1)
		{
		errors("%s already exists", pname);
		makename(sname, 's');
		return(0);
		}
	if((pfile = fopen(pname, "w")) == NULL)
		{
		errors("can't create %s", pname);
		makename(sname, 's');
		return(0);
		}
	makename(sname, 's');
	fprintf(pfile, "%s %s %s", oldsid, newsid, logname);
	expandkeywords(DTstring, pfile);		/* date and time */
	fclose(pfile);
	return(1);
	}



/* copyb(from, to, nbytes) -- copies bytes from from to to */

copyb(from, to, nbytes)
	register char *from, *to;
	register int nbytes;
	{
	while(nbytes--)
		*to++ = *from++;
	}
