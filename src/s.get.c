h00000
s 00004/00006/00344
d D 1.4 82/01/06 09:10:01 drg 4 3
c Use the login name of the user, rather than his real userid.  People
c using su were being identified as root.
e
s 00001/00001/00349
d D 1.3 81/10/29 15:30:55 root 3 2
c Made hack fix to problem that root was getting the "writable" error
c all the time, since access() thinks root can write on anything.  Made root
c a special case.
e
s 00015/00009/00335
d D 1.2 81/10/05 10:56:38 drg 2 1
c Convert to 1.2 of errors.c.  Check filename for legality.
c Expandkw is really called expandkeywords.  Date and time string in
c makepfile were being expanded.
e
s 00344/00000/00000
d D 1.1 81/10/05 10:23:24 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

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
I 2
			{
E 2
			if(!zlock(argv[i]))
				continue;
I 2
			}
		else
			if(!isvalid(argv[i]))
				continue;
E 2
		get(argv[i], vdesired);
		if(eflag)
			zunlock(argv[i]);
		destroyOnExit('\0');
		}
	exit(0);
	}


extern char *makename(), *gname();

char *sname	= NULL;
D 2
struct delta desired;		/* set in get(), used in expandkw() */
E 2
I 2
struct delta desired;		/* set in get(), used in expandkeywords() */
E 2



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

	havedelta = 0;

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
D 2
		error("%s does not contain version %s", sname, vdesired);
E 2
I 2
		errorss("%s does not contain version %s", sname, vdesired);
E 2
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
D 3
		if(access(gname(sname), 2) != -1)
E 3
I 3
		if(access(gname(sname), 2) != -1 && getuid() != 0)
E 3
			{
			fclose(sfile);
D 2
			error("writable %s exists", gname(sname));
E 2
I 2
			errors("writable %s exists", gname(sname));
E 2
			return;
			}
	
		unlink(gname(sname));
	
		if((gfile = fopen(gname(sname), "w")) == NULL)
			{
			fclose(sfile);
D 2
			error("can't create %s", gname(sname));
E 2
I 2
			errors("can't create %s", gname(sname));
E 2
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


D 4
#include <pwd.h>
E 4

D 4
struct passwd *getpwuid();

E 4
/* makepfile -- creates the p.whatever file, when you use get -e.
 */

makepfile(sname, oldsid, newsid)
	char *sname, *oldsid, *newsid;
	{
	FILE *pfile;
	char logname[10];
D 4
	struct passwd *p;
E 4
I 4
	char *name;
	extern char *getname();
E 4
	char *pname;
I 2
	extern char *DTstring;
E 2

D 4
	if((p = getpwuid(getuid())) == NULL)
E 4
I 4
	if((name = getname()) == NULL)
E 4
		{
		error("can't find your login name");
		return(0);
		}
	else
D 4
		strncpy(logname, p->pw_name, sizeof(logname));
E 4
I 4
		strncpy(logname, name, sizeof(logname));
E 4
	pname = makename(sname, 'p');
	if(access(pname, 4) != -1)
		{
D 2
		error("%s already exists", pname);
E 2
I 2
		errors("%s already exists", pname);
E 2
		makename(sname, 's');
		return(0);
		}
	if((pfile = fopen(pname, "w")) == NULL)
		{
D 2
		error("can't create %s", pname);
E 2
I 2
		errors("can't create %s", pname);
E 2
		makename(sname, 's');
		return(0);
		}
	makename(sname, 's');
	fprintf(pfile, "%s %s %s", oldsid, newsid, logname);
D 2
	expandkw(" %D% %T%\n", pfile);		/* date and time */
E 2
I 2
	expandkeywords(DTstring, pfile);		/* date and time */
E 2
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
E 1
