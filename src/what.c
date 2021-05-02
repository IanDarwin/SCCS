static char Sccsid[] = "@(#)what.c	1.2 of 10/5/81";

/* Copyright (c) 1981 by David R. Galloway */

/* what -- a re-write of the SCCS what command.
 */

#include <stdio.h>

extern char *Zstring;		/* see IDstrings.c */

#define USAGE	"usage: what file1 file2 ..."

char *Progname = "what";		/* Used in the error() routine */
FILE *diagfile = stderr;

#define NORMAL	0
#define ONE	1
#define TWO	2
#define THREE	3
#define PRINTIT	4

main(argc, argv)
	int argc;
	char *argv[];
	{
	register int i;

	if(argc < 2)
		fatal(USAGE);

	for(i=1; i<argc; i++)
		{
		printf("%s:\n", argv[i]);
		what(argv[i]);
		}
	exit(0);
	}



what(filename)
	char *filename;
	{
	FILE *input;
	register int c, state;

	if((input = fopen(filename, "r")) == NULL)
		{
		errors("can't open %s", filename);
		return;
		}

	state = NORMAL;
	while((c = getc(input)) != EOF)
		{
		switch(state)
			{
		case NORMAL :
			if(c == Zstring[0])
				state = ONE;
			break;

		case ONE :
			if(c == Zstring[1])
				state = TWO;
			else
				state = (c == Zstring[0]) ? ONE : NORMAL;
			break;

		case TWO :
			if(c == Zstring[2])
				state = THREE;
			else
				state = (c == Zstring[0]) ? ONE : NORMAL;
			break;

		case THREE :
			if(c == Zstring[3])
				{
				state = PRINTIT;
				putchar('\t');
				}
			else
				state = (c == Zstring[0]) ? ONE : NORMAL;
			break;

		case PRINTIT :
			if(c == '\0' || c == '\n' || c == '\r' || c == '"' || c == '>')
				{
				state = NORMAL;
				putchar('\n');
				}
			else
				putchar(c);
			break;
			}
		}
	fclose(input);
	}
