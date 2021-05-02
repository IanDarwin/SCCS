h00000
s 00025/00000/00000
d D 1.1 82/01/06 09:06:58 drg 1 0
e
u
U
t
T
I 1
static char Sccsid[] = "%W% of %G%";

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
E 1
