
/* Copyright (c) 1981 by David R. Galloway */

/*

 * This file contains some strings used by the expandkeywords() routine
 * to expand the %A% and %W% keywords.  It should NEVER be put
 * through SCCS, as it will cause the following keywords to be expanded,
 * which would make expandkeywords stop working.
 */

/* Note:  Astring should really be %Z%%Y% %M% %I%%Z%", but we don't
 * support %Y%, so we leave it out.
 */

char *Astring = "%Z% %M% %I%%Z%";

char *Wstring = "%Z%%M%\t%I%";

char *Zstring = "@(#)";

char *DTstring = " %D% %T%\n";	/* Used in makepfile() in get.c */
