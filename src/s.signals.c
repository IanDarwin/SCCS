h00000
s 00091/00000/00000
d D 1.1 81/10/05 10:23:27 drg 1 0
e
u
U
t
T
I 1
/* This file contains all of the routines that handle signal catching, and the
 * removal of partially constructed files when a signal is caught.
 */

/* Copyright (c) 1981 by David R. Galloway */

static char Sccsid[] = "%W% of %G%";

#include <signal.h>

int sig[] =
	{
	SIGHUP,
	SIGINT,
	SIGQUIT,
	SIGTERM,
	0
	};

/* catchsignals -- calls the signal system call for the signals in sig[],
 * with the given arg.
 */


catchsignals(arg)
	int arg;
	{
	static int original[16], origset;
	register int i;

	if(!origset)
		{
		for(i=0; sig[i]; i++)
			original[sig[i]] = signal(sig[i], 1);
		origset++;
		}

	for(i=0; sig[i]; i++)
		if(original[sig[i]] != 1)
			signal(sig[i], arg);
	}



char prefixes[10];

/* destroyOnExit -- If the argument c is 'x', then x.filename will be removed
 * if die() is called.
 */

destroyOnExit(c)
	char c;
	{
	register int i;

	if(c == '\0')
		{
		for(i=0; i<10; i++)
			prefixes[i] = '\0';
		return;
		}

	for(i=0; i<10; i++)
		if(prefixes[i] == '\0')
			{
			prefixes[i] = c;
			return;
			}
	fatal("internal error in destroyOnExit");
	}



/* die is only invoked if a signal is detected */

die()
	{
	register int i;
	char *name;
	extern char *sname, *makename();

	for(i=0; i<10; i++)
		{
		if(prefixes[i] != '\0')
			{
			name = makename(sname, prefixes[i]);
			unlink(name);
			}
		}
	exit(1);
	}
E 1
