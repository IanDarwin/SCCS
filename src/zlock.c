static char Sccsid[] = "@(#)zlock.c	1.1 of 10/5/81";

/* zlock and zunlock -- creat and destroy the file z.name with mode 444,
 * to provide exclusive access to the s.name file.  The same protocol is
 * used by all SCCS commands.
 */

/* Copyright (c) 1981 by David R. Galloway */

char *makename();



zlock(sname)
	char *sname;
	{
	int lock, pid;
	char *zname;

	if(!isvalid(sname))
		return(0);
	zname = makename(sname, 'z');
	if((lock = creat(zname, 0444)) == -1)
		{
		error("Can't create lock file");
		makename(sname, 's');
		return(0);
		}
	destroyOnExit('z');
	pid = getpid();
	write(lock, &pid, sizeof(pid));
	close(lock);
	makename(sname, 's');
	return(1);
	}



zunlock(sname)
	char *sname;
	{
	char *zname;

	zname = makename(sname, 'z');
	unlink(zname);
	makename(sname, 's');
	}
