h00000
s 00005/00005/00033
d D 1.2 82/01/07 10:49:32 drg 2 1
c Added the getname routine.
e
s 00038/00000/00000
d D 1.1 82/01/07 10:47:17 drg 1 0
e
u
U
t
T
I 1
CFLAGS= -O
BINDIR=/bin

SCCS:	admin get what delta prt

install:	SCCS
	cp admin get what delta prt sccsdiff ${BINDIR}

clean:
	rm -f *.o admin get what delta prt

test:	../test SCCS
	cp admin get what delta prt sccsdiff ../test
	cd ../test;testme

D 2
admin:	admin.o writesccs.o error.o zlock.o signals.o names.o
	cc -s admin.o writesccs.o error.o zlock.o signals.o names.o
E 2
I 2
admin:	admin.o writesccs.o getname.o error.o zlock.o signals.o names.o
	cc -s admin.o writesccs.o getname.o error.o zlock.o signals.o names.o
E 2
	mv a.out admin

D 2
delta:	delta.o writesccs.o error.o zlock.o signals.o names.o readsccs.o
	cc -s delta.o writesccs.o error.o zlock.o signals.o names.o readsccs.o
E 2
I 2
delta:	delta.o writesccs.o getname.o error.o zlock.o signals.o names.o readsccs.o
	cc -s delta.o writesccs.o getname.o error.o zlock.o signals.o names.o readsccs.o
E 2
	mv a.out delta

D 2
GETOBJ=get.o IDstrings.o expandkw.o readsccs.o zlock.o signals.o error.o names.o
E 2
I 2
GETOBJ=get.o getname.o IDstrings.o expandkw.o readsccs.o zlock.o signals.o error.o names.o
E 2

get:	$(GETOBJ)
	cc -s $(GETOBJ)
	mv a.out get

prt:	prt.o readsccs.o error.o names.o
	cc -s prt.o readsccs.o error.o names.o
	mv a.out prt

what:	what.o error.o IDstrings.o
	cc -s what.o error.o IDstrings.o
	mv a.out what

admin.o writesccs.o get.o expandkw.o readsccs.o delta.o prt.o :	sccs.h
E 1
