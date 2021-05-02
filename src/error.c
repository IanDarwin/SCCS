
/* Copyright (c) 1981 by David R. Galloway */

static char Sccsid[] = "@(#)error.c	1.2 of 10/5/81";

#include <stdio.h>

error(string)
	char *string;
	{
	extern char *Progname;
	extern FILE *diagfile;

	fprintf(diagfile, "%s: %s\n", Progname, string);
	}


errors(str1, str2)
	char *str1, *str2;
	{
	extern char *Progname;
	extern FILE *diagfile;

	fprintf(diagfile, "%s: ", Progname);
	fprintf(diagfile, str1, str2);
	fprintf(diagfile, "\n");
	}


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


fatal(args)
	char *args;
	{
	error(args);
	exit(1);
	}
