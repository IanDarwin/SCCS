static char Sccsid[] = "@(#)getname.c	1.1 of 1/6/82";

/* getname -- return the login name of the user.  If you can't find it,
 * return NULL
 */

#include <stdio.h>
#include <pwd.h>

char *
getname()
	{
	char *name;
	struct passwd *p;
	extern struct passwd *getpwuid();
	extern char *getlogin();

	if((name = getlogin()) != NULL)
		return(name);

	if((p == getpwuid(getuid())) == NULL)
		return(NULL);

	return(p->pw_name);
	}
