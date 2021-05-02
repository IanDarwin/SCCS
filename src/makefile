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

admin:	admin.o writesccs.o getname.o error.o zlock.o signals.o names.o
	cc -s admin.o writesccs.o getname.o error.o zlock.o signals.o names.o
	mv a.out admin

delta:	delta.o writesccs.o getname.o error.o zlock.o signals.o names.o readsccs.o
	cc -s delta.o writesccs.o getname.o error.o zlock.o signals.o names.o readsccs.o
	mv a.out delta

GETOBJ=get.o getname.o IDstrings.o expandkw.o readsccs.o zlock.o signals.o error.o names.o

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
