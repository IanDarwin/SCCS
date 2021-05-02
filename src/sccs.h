
/* Copyright (c) 1981 by David R. Galloway */

#define ERROR	-1
#define OK	1
#define CONTROLCHAR	'\01'

struct delta
	{
	int added;
	int deleted;
	int unchanged;
	char deltatype;
	char version[4*5+4];
	char deltadate[9];
	char deltatime[9];
	char programmer[9];
	int deltanumber;
	int olddeltanumber;
	};
