static char Sccsid[] = "@(#)expandkw.c	1.1 of 10/5/81";

/* Copyright (c) 1981 by David R. Galloway */


/* expandkeywords(line, outfile) -- expands any SCCS id keywords in line
 * and puts the result out on to outfile
 */

#include <stdio.h>
#include <time.h>
#include "sccs.h"

expandkeywords(line, outfile)
	register char *line;
	FILE *outfile;
	{
	extern char *Astring, *Wstring, *Zstring;
	extern int linenumber;
	extern char *sname;
	extern char *gname();
	extern struct delta desired;
	extern struct tm *localtime();
	struct tm *t;
	extern long time();
	long now;
	char *toMMDDYY();
	char *release, *level, *branch, *sequence;

	while(*line)
		{
		if(*line == '%' && line[1] != '\0' && line[2] == '%')
			{
			switch(line[1])
				{

			case 'A' :
				expandkeywords(Astring, outfile);
				break;

			case 'W' :
				expandkeywords(Wstring, outfile);
				break;

			case 'I' :
				fputs(desired.version, outfile);
				break;

			case 'M' :
				fputs(gname(sname), outfile);
				break;

			case 'R' :
				decodeSID(desired.version, &release, &level, &branch, &sequence);
				fputs(release, outfile);
				break;

			case 'L' :
				decodeSID(desired.version, &release, &level, &branch, &sequence);
				fputs(level, outfile);
				break;

			case 'B' :
				decodeSID(desired.version, &release, &level, &branch, &sequence);
				if(branch[0] != '\0')
					fputs(branch, outfile);
				else
					putc('0', outfile);
				break;

			case 'S' :
				decodeSID(desired.version, &release, &level, &branch, &sequence);
				if(sequence[0] != '\0')
					fputs(sequence, outfile);
				else
					putc('0', outfile);
				break;

			case 'E' :
				fputs(desired.deltadate, outfile);
				break;

			case 'G' :
				fputs(toMMDDYY(desired.deltadate), outfile);
				break;

			case 'U' :
				fputs(desired.deltatime, outfile);
				break;

			case 'F' :
				fputs(sname, outfile);
				break;

			case 'C' :
				fprintf(outfile, "%d", linenumber);
				break;

			case 'Z' :
				fputs(Zstring, outfile);
				break;

			case 'D' :
				now = time( (long *) 0);
				t = localtime(&now);
				fprintf(outfile, "%02d/%02d/%02d", t->tm_year,
					t->tm_mon+1, t->tm_mday);
				break;

			case 'H' :
				now = time( (long *) 0);
				t = localtime(&now);
				fprintf(outfile, "%d/%02d/%02d", t->tm_mon+1,
					t->tm_mday, t->tm_year);
				break;

			case 'T' :
				now = time( (long *) 0);
				t = localtime(&now);
				fprintf(outfile, "%02d:%02d:%02d", t->tm_hour,
					t->tm_min, t->tm_sec);
				break;

			default :
				fprintf(outfile, "%%%c%%", line[1]);
				break;
				}
			line += 2;
			}
		else
			putc(*line, outfile);
		line++;
		}
	}



/* toMMDDYY(date) -- given a date in YY/MM/DD form, returns a date in
 * MM/DD/YY form.
 */

char *toMMDDYY(date)
	char *date;
	{
	static char newdate[9];
	int i;

	i = 0;
	if(date[3] != '0')
		newdate[i++] = date[3];
	newdate[i++] = date[4];
	newdate[i++] = '/';
	if(date[6] != '0')
		newdate[i++] = date[6];
	newdate[i++] = date[7];
	newdate[i++] = '/';
	newdate[i++] = date[0];
	newdate[i++] = date[1];
	newdate[i++] = '\0';
	return(newdate);
	}



decodeSID(version, rp, lp, bp, sp)
	char *version, **rp, **lp, **bp, **sp;
	{
	static char sid[4][6];
	register int i,j;

	*rp = sid[0];
	*lp = sid[1];
	*bp = sid[2];
	*sp = sid[3];
	for(i=0; i<4; i++)
		for(j=0; j<6; j++)
			{
			if(*version == '.')
				{
				sid[i][j] = '\0';
				version++;
				break;
				}
			sid[i][j] = *version;
			if(*version)
				version++;
			else
				break;
			}
	}



buildSID(result, r, l, b, s)
	char *result;
	int r, l, b, s;
	{
	if(b)
		sprintf(result, "%d.%d.%d.%d", r, l, b, s);
	else
		sprintf(result, "%d.%d", r, l);
	}
