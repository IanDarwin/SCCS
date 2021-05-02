h00000
s 00043/00006/00018
d D 1.2 81/10/05 10:42:08 drg 2 1
c Found out that %r format won't work on the VAX.  Added a bunch of
c different error routines.
e
s 00024/00000/00000
d D 1.1 81/10/05 10:23:23 drg 1 0
e
u
U
t
T
I 1

/* Copyright (c) 1981 by David R. Galloway */

I 2
static char Sccsid[] = "%W% of %G%";

E 2
#include <stdio.h>

D 2
/*VARARGS*/
error(args)
	int args;
E 2
I 2
error(string)
	char *string;
E 2
	{
	extern char *Progname;
	extern FILE *diagfile;

D 2
	fprintf(diagfile, "%s: %r", Progname, &args);
E 2
I 2
	fprintf(diagfile, "%s: %s\n", Progname, string);
	}


errors(str1, str2)
	char *str1, *str2;
	{
	extern char *Progname;
	extern FILE *diagfile;

	fprintf(diagfile, "%s: ", Progname);
	fprintf(diagfile, str1, str2);
E 2
	fprintf(diagfile, "\n");
	}


D 2
/*VARARGS*/
E 2
I 2
errorss(str1, str2, str3)
	char *str1, *str2;
	char *str3;
	{
	extern char *Progname;
	extern FILE *diagfile;

	fprintf(diagfile, "%s: ", Progname);
	fprintf(diagfile, str1, str2, str3);
	fprintf(diagfile, "\n");
	}


errord(str1, num)
	char *str1;
	int num;
	{
	extern char *Progname;
	extern FILE *diagfile;

	fprintf(diagfile, "%s: ", Progname);
	fprintf(diagfile, str1, num);
	fprintf(diagfile, "\n");
	}


E 2
fatal(args)
D 2
	int args;
E 2
I 2
	char *args;
E 2
	{
	error(args);
	exit(1);
	}
E 1
