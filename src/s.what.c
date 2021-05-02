h00000
s 00001/00001/00099
d D 1.2 81/10/05 11:06:18 drg 2 1
c Convert to use version 1.2 of error.c
e
s 00100/00000/00000
d D 1.1 81/10/05 10:23:27 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

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
D 2
		error("can't open %s", filename);
E 2
I 2
		errors("can't open %s", filename);
E 2
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
E 1
