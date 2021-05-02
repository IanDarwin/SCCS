h00000
s 00010/00000/00430
d D 1.9 81/10/14 10:34:29 drg 9 8
c no commentstill no comment
e
s 00098/00064/00332
d D 1.8 81/10/14 10:33:20 drg 8 7
c no commentstill no comment
e
s 00041/00044/00355
d D 1.7 81/10/14 10:32:23 drg 7 6
c no commentstill no comment
e
s 00004/00004/00395
d D 1.6 81/10/14 10:31:03 drg 6 5
c no commentstill no comment
e
s 00001/00001/00398
d D 1.5 81/10/14 10:29:54 drg 5 4
c no commentstill no comment
e
s 00002/00002/00397
d D 1.4 81/10/14 10:28:51 drg 4 3
c no commentstill no comment
e
s 00001/00001/00398
d D 1.3 81/10/14 10:27:47 drg 3 2
c no commentstill no comment
e
s 00029/00027/00370
d D 1.2 81/10/14 10:25:47 drg 2 1
c no commentstill no comment
e
s 00397/00000/00000
d D 1.1 81/10/14 10:23:59 drg 1 0
e
u
U
t
T
D 8
D 2
I 1
#
E 2
I 2
static char Sccsid[] ="%W% of %G%";

E 2
extern char buffers[][514];
E 8
I 8
static char Sccsid[] = "%W% of %G%";
E 8
/*
 * DV Disk Driver
 */

D 2
#include "../param.h"
#include "../buf.h"
#include "../conf.h"
#include "../user.h"
E 2
I 2
#include "../h/param.h"
I 9
#include "../h/systm.h"
E 9
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"
E 2

struct {
	int	dvcsr;
	int	dvdbr;
	int	dvmar;
	int	dvwcr;
	int	dvcbr;
	int	dvssr;
	int	dvair;
};
struct { char lbyte, hbyte; };

#define	DVADDR	0164100
#define	NDV	1
#define	INTLV	2		/* 2 sectors skipped between optimally
				 * consecutive sectors.
				 */

struct {
D 7
	char	*nblocks;
E 7
I 7
	daddr_t	nblocks;
D 8
E 7
	int	cyloff;
E 8
I 8
	unsigned int	cyloff;
E 8
D 7
} dv_sizes[] {
D 4
	(40*240)-12,	39,	/* dv0: cyl 39 - 0 for /tmp */
	(161*240)-12,	40,	/* dv1: cyl 40 - 200 for /u2 */
E 4
I 4
	(0*240)-12,	0,	/* dv0: unused */
D 5
	(201*240)-12,	0,	/* dv1: cyl 0 - 200 for /u2 */
E 5
I 5
	(203*240)-12,	0,	/* dv1: cyl 0 - 202 for /u2 */
E 7
I 7
} dv_sizes[] = {
D 8
	(0*240L)-12,	0,	/* dv0: unused */
	(203*240L)-12,	0,	/* dv1: cyl 0 - 202 for /u2 */
E 7
E 5
E 4
		0,	0,	/* unused */
		0,	0,	/* unused */
E 8
I 8
	(203*(daddr_t)240)-12,	0,	/* cyl 0 - 202 for /u2 */
E 8
		0,	0,	/* unused			*/
		0,	0,	/* unused			*/
		0,	0,	/* unused			*/
		0,	0,	/* unused			*/
I 8
		0,	0,	/* unused			*/
		0,	0,	/* unused			*/
		0,	0,	/* unused			*/
E 8
};

struct {
	int	hd1;
	int	hd2;
	int	cksum;
	char	*chadr;
} dvhdr;

D 8
D 7
int	dv_unit -1;
int	intcyl;
E 7
I 7
int	dv_unit =	-1;
E 8
I 8
int	dv_unit = -1;
int	intcyl;
E 8
E 7
int	dvhist;

D 2
struct	devtab	dvtab;
E 2
I 2
struct	buf	dvtab;
E 2
struct	buf	rdvbuf;

struct dvq {
	struct	buf *dv_bufp;
	int	dv_lcyl;
	char	dv_dirf;
	char	dv_nreq;
} dvq[NDV];
struct	dvq	*dv_ap;

#define	SWTUNIT	10
int	dv_nxfr;

#define	HTBCOM	000000
#define	CTBCOM	020000
#define	CNBCOM	030000
#define		INCHD	01
#define		RECAL	02
#define		RESET	020
#define		SEEK	040
#define		CLRDA	0100
#define	CHRCOM	070000
#define	CHWCOM	0130000
#define	LDSCOM	0140000
#define	CTLRST	0170000

#define	DRVRDY	04000
#define	ATTN	0400
#define	DONE	0200
#define	IENABLE	0100
#define	GO	01

/*
 * Use av_back to save cylinder,
 * b_resid for sector+track.
I 8
 * NOTE THIS IS INCOMPATIBLE WITH USE OF DISKSORT!!!
E 8
 */

#define	dvcyl	av_back
#define	dvsec	b_resid.lbyte
#define	dvhd	b_resid.hbyte

I 9
/*
 * Monitoring device number
 */
#define DK_N	0

E 9
D 8
D 7
dvstrategy(abp)
struct buf *abp;
E 7
I 7
dvstrategy(bp)
register struct buf *bp;
E 8
I 8
int dvinit_done;

dvopen(){
	register i,j;
	if(dvinit_done==0){
		dvinit_done++;
		DVADDR->dvcbr = CNBCOM|RECAL;
		dvwait();
		DVADDR->dvcbr = CNBCOM|RESET;
		dvwait();
	}
}
dvstrategy(abp)
struct buf *abp;
E 8
E 7
{
D 8
D 7
	register struct buf *bp;
	register char *p1, *p2;
E 7
I 7
	daddr_t	p1,p2;
	register char *q1, *q2;
E 8
I 8
	register struct buf *bp;
	register char *p1, *p2;
E 8
E 7
	int f;

I 8
	bp = abp;
/*
if(bp->b_bcount > 512)
	printf("dv%d %d b%D at 0%o/%o\n", bp->b_flags&B_READ, bp->b_bcount,
	bp->b_blkno, bp->b_xmem, bp->b_un.b_addr);
*/
	p1 = &dv_sizes[minor(bp->b_dev)&07];
E 8
D 7
	bp = abp;
D 2
	p1 = &dv_sizes[bp->b_dev.d_minor&07];
	if(bp->b_dev.d_minor >= (NDV<<3) ||
E 2
I 2
	p1 = &dv_sizes[minor(bp->b_dev)&07];
E 7
	if(minor(bp->b_dev) >= (NDV<<3) ||
D 8
D 7
E 2
	   bp->b_blkno >= p1->nblocks ||
D 2
	   bp->b_blkno + ((255 - bp->b_wcount)/256) > p1->nblocks) {
E 2
I 2
	   bp->b_blkno + ((bp->b_bcount + 511)/512) > p1->nblocks) {
E 7
I 7
	   bp->b_blkno + ((bp->b_bcount + 511)/512)
			> dv_sizes[minor(bp->b_dev)].nblocks) {
E 8
I 8
	   bp->b_blkno >= p1->nblocks ||
	   bp->b_blkno + ((511 + bp->b_bcount)/512) > p1->nblocks) {
E 8
E 7
E 2
		bp->b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
I 8
	if (bp->b_flags&B_PHYS)
		mapalloc(bp);
E 8
	bp->av_forw = 0;
D 6
	p2 = ldiv(bp->b_blkno, 240);
E 6
I 6
	p2 = bp->b_blkno / 240;
D 8
D 7
E 6
	p1 = p1->cyloff;
E 7
I 7
	p1 = dv_sizes[minor(bp->b_dev)].cyloff;
E 7
D 2
	if((bp->b_dev.d_minor&01)==0)
E 2
I 2
	if((minor(bp->b_dev)&01)==0)
E 2
		p1 =- p2;
	else
		p1 =+ p2;
E 8
I 8
	p1 = p1->cyloff;
	p1 =+ p2;
E 8
	bp->dvcyl = p1;
D 6
	p2 = lrem(bp->b_blkno, 240);
E 6
I 6
	p2 = bp->b_blkno % 240;
E 6
	/*
	 * the following code depends on INTLV being 2
	 */

	p1 = (p2&03) * 3;
	p2 =>> 2;

	/*
	 *
	 */

	bp->dvsec = (p1 =+ p2/20);
	bp->dvhd = 19 - p2%20;
	spl5();
D 8
D 7
D 2
	p2 = &dvq[(bp->b_dev.d_minor)>>3];
E 2
I 2
	p2 = &dvq[(minor(bp->b_dev))>>3];
E 2
	(*p2).dv_nreq++;
	if((p1 = (*p2).dv_bufp)==NULL) {
		(*p2).dv_bufp = bp;
E 7
I 7
	q2 = &dvq[(minor(bp->b_dev))>>3];
	(*q2).dv_nreq++;
	if((q1 = (*q2).dv_bufp)==NULL) {
		(*q2).dv_bufp = bp;
E 8
I 8
	p2 = &dvq[(minor(bp->b_dev))>>3];
	(*p2).dv_nreq++;
	if((p1 = (*p2).dv_bufp)==NULL) {
		(*p2).dv_bufp = bp;
E 8
E 7
		if(dv_ap==NULL) {
D 8
D 7
			dv_ap = p2;
E 7
I 7
			dv_ap = q2;
E 8
I 8
			dv_ap = p2;
E 8
E 7
			dvstart();
		}
	} else {
D 8
D 7
		f = (*p2).dv_dirf;
		for (; p2 = p1->av_forw; p1 = p2) {
			if (p1->dvcyl <= bp->dvcyl
			 && bp->dvcyl <= p2->dvcyl
			 || p1->dvcyl >= bp->dvcyl
			 && bp->dvcyl >= p2->dvcyl) {
				while(p1->dvcyl==p2->dvcyl) {
					if(p2->dvsec > p1->dvsec) {
						if(bp->dvsec>p1->dvsec+INTLV
						 &&bp->dvsec<p2->dvsec-INTLV)
E 7
I 7
		f = (*q2).dv_dirf;
		for (; q2 = q1->av_forw; q1 = q2) {
			if (q1->dvcyl <= bp->dvcyl
			 && bp->dvcyl <= q2->dvcyl
			 || q1->dvcyl >= bp->dvcyl
			 && bp->dvcyl >= q2->dvcyl) {
				while(q1->dvcyl==q2->dvcyl) {
					if(q2->dvsec > q1->dvsec) {
						if(bp->dvsec>q1->dvsec+INTLV
						 &&bp->dvsec<q2->dvsec-INTLV)
E 8
I 8
		f = (*p2).dv_dirf;
		for (; p2 = p1->av_forw; p1 = p2) {
			if (p1->dvcyl <= bp->dvcyl
			 && bp->dvcyl <= p2->dvcyl
			 || p1->dvcyl >= bp->dvcyl
			 && bp->dvcyl >= p2->dvcyl) {
				while(p1->dvcyl==p2->dvcyl) {
					if(p2->dvsec > p1->dvsec) {
						if(bp->dvsec>p1->dvsec+INTLV
						 &&bp->dvsec<p2->dvsec-INTLV)
E 8
E 7
							goto out;
					} else
D 8
D 7
						if(bp->dvsec>p1->dvsec+INTLV
						||bp->dvsec<p2->dvsec-INTLV)
E 7
I 7
						if(bp->dvsec>q1->dvsec+INTLV
						||bp->dvsec<q2->dvsec-INTLV)
E 8
I 8
						if(bp->dvsec>p1->dvsec+INTLV
						||bp->dvsec<p2->dvsec-INTLV)
E 8
E 7
							goto out;
D 8
D 7
					p1 = p2;
					if(!(p2 = p1->av_forw)) goto out;
E 7
I 7
					q1 = q2;
					if(!(q2 = q1->av_forw)) goto out;
E 8
I 8
					p1 = p2;
					if(!(p2 = p1->av_forw)) goto out;
E 8
E 7
				}
				goto out;
			}
			if(f) {
D 8
D 7
				if(p2->dvcyl < p1->dvcyl)
					if(bp->dvcyl > p1->dvcyl)
E 7
I 7
				if(q2->dvcyl < q1->dvcyl)
					if(bp->dvcyl > q1->dvcyl)
E 8
I 8
				if(p2->dvcyl < p1->dvcyl)
					if(bp->dvcyl > p1->dvcyl)
E 8
E 7
						goto out;
					else
						f = 0;
			} else {
D 8
D 7
				if(p2->dvcyl > p1->dvcyl)
					if(bp->dvcyl < p1->dvcyl)
E 7
I 7
				if(q2->dvcyl > q1->dvcyl)
					if(bp->dvcyl < q1->dvcyl)
E 8
I 8
				if(p2->dvcyl > p1->dvcyl)
					if(bp->dvcyl < p1->dvcyl)
E 8
E 7
						goto out;
					else
						f++;
			}
		}
out:
D 8
D 7
		bp->av_forw = p2;
		p1->av_forw = bp;
E 7
I 7
		bp->av_forw = q2;
		q1->av_forw = bp;
E 8
I 8
		bp->av_forw = p2;
		p1->av_forw = bp;
E 8
E 7
	}
	spl0();
}

dvstart()
{
	register *rp, *hp;
	register struct buf *bp;
	int i;

	if ((bp = (*dv_ap).dv_bufp) == NULL)
		return;
D 2
	dvtab.d_active++;
E 2
I 2
	dvtab.b_active++;
I 9
	dk_busy |= 1<<DK_N;
E 9
E 2
	rp = &DVADDR->dvcbr;
D 2
	if (dv_unit!=(bp->b_dev.d_minor>>3)) {	/* select unit */
		*rp = LDSCOM | (dv_unit = bp->b_dev.d_minor>>3);
E 2
I 2
	if (dv_unit!=(minor(bp->b_dev)>>3)) {	/* select unit */
		*rp = LDSCOM | (dv_unit = minor(bp->b_dev)>>3);
D 8
E 2
		/*   waste time if we un-strap delay  */
E 8
I 8
		dvwait();
E 8
	}
	i = bp->dvcyl;
	*rp++ = CNBCOM | RESET | CLRDA;	/* reset and clear */
I 8
	dvwait();
E 8
	if (i != (~((*rp)|0177000))) {	/* seek */
		DVADDR->dvcbr = CTBCOM | i;
		if (dvrdy())
			return;
		DVADDR->dvcbr = CNBCOM | SEEK | RESET;
I 8
		dvwait();
E 8
		hp = dv_ap;
		if((*hp).dv_lcyl > i)
			(*hp).dv_dirf = 0;
		else
			(*hp).dv_dirf = 1;
		(*hp).dv_lcyl = i;
		DVADDR->dvair = 1<<dv_unit;
		DVADDR->dvcsr = DONE | IENABLE;
		return;
	}
	*--rp = HTBCOM | bp->dvhd;	/* select head */
D 8
D 2
	if (bp->b_wcount <= -256)
E 2
I 2
	if(bp->b_bcount > 512)
E 8
I 8
	dvwait();
	if (bp->b_bcount >= 512)
E 8
E 2
		*--rp = -512;
	else
D 2
		*--rp = bp->b_wcount<<1;
E 2
I 2
		*--rp = -bp->b_bcount;
E 2
	hp = &dvhdr.hd2;
	*hp = 0170000 | bp->dvsec;;	/* set up header */
	*--hp = (bp->dvhd<<8) + i;
	*--rp = hp;			/* set up MAR */
	*hp++ = -(*hp++ + *hp++);	/* set up checksum */
D 7
	*hp = bp->b_addr;
E 7
I 7
	*hp = bp->b_un.b_addr;
E 7
	DVADDR->dvcbr = ((bp->b_flags&B_READ)? CHRCOM : CHWCOM ) |
		(bp->dvsec<<1);
	DVADDR->dvcsr = IENABLE | GO | ((bp->b_xmem&03)<<4);
I 9
	dk_numb[DK_N] += 1;
	dk_wds[DK_N] += bp->b_bcount>>6;
E 9
}

dvintr()
{
	register struct buf *bp;
	register int csr;
	register *p1;

D 2
	if (dvtab.d_active == 0)
E 2
I 2
	if (dvtab.b_active == 0)
E 2
		return;
I 9
	dk_busy &= ~(1<<DK_N);
E 9
	bp = (*dv_ap).dv_bufp;
D 2
	dvtab.d_active = 0;
E 2
I 2
	dvtab.b_active = 0;
E 2
	csr = DVADDR->dvcsr;
	DVADDR->dvcsr = DONE;
	if (csr&ATTN) {		/* seeek complete */
		DVADDR->dvair.lbyte = 0;
		if (DVADDR->dvssr > 0) {	/* error */
D 8
			printf("dvssr %o\n",DVADDR->dvssr);
E 8
I 8
			printf("dvssr 0%o\n",DVADDR->dvssr);
E 8
			DVADDR->dvcbr = CNBCOM | RECAL | RESET;
			dv_unit = -1;
			if (dvrdy())
				return;
			DVADDR->dvcbr = CTLRST;
I 8
			dvwait();
E 8
D 2
			if (++dvtab.d_errcnt <= 10) {
E 2
I 2
			if (++dvtab.b_errcnt <= 10) {
E 2
				dvstart();
				return;
			}
			dvherr(0);
			return;
		} else {
			dvstart();
			return;
		}
	} else {	/* r/w complete */

		if (csr < 0) {	/* error */
			dvhist++;
			dv_unit = -1;
D 2
			if ((++dvtab.d_errcnt & 03) == 0) {
E 2
I 2
			if ((++dvtab.b_errcnt & 03) == 0) {
E 2
				DVADDR->dvcbr = CNBCOM | RECAL | RESET;
D 8
				printf("dvcsr %o on %o\n",csr, bp->b_dev);
/*		Add if you have hardware head problems  */
				printf("dvblk= %d cyl= %d sec= %d hd=%d mar=%o\n",
E 8
I 8
				printf("dvcsr 0%o on 0%o\n",csr, bp->b_dev);
				printf("dvblk= %D cyl= %d sec= %d hd=%d mar=0%o\n",
E 8
					bp->b_blkno, bp->dvcyl, bp->dvsec,
					bp->dvhd, DVADDR->dvmar);
D 8
/**/
E 8
				if (dvrdy())
					return;
			}
			DVADDR->dvcbr = CTLRST;
I 8
			dvwait();
E 8
D 2
			if (dvtab.d_errcnt <= 36) {
E 2
I 2
			if (dvtab.b_errcnt <= 36) {
E 2
				dvstart();
				return;
			}
			dvherr(0);
			return;
		} else {
D 8
D 2
			if ((bp->b_wcount =+ 256) < 0) {     /* more to do */
E 2
I 2
			if((bp->b_bcount =- 512) > 0) {	   /* more to do */
E 8
I 8
			bp->b_bcount = ( bp->b_bcount > 512 ? bp->b_bcount-512 : 0 );
			if (bp->b_bcount > 0) {	/* more to do */
E 8
D 7
E 2
				csr = bp->b_addr;
E 7
I 7
				csr = bp->b_un.b_addr;
E 7
				csr =>> 1;
				csr =& ~0100000;
D 8
D 7
D 3
				if((csr =+ 256) <= 0)
E 3
I 3
				if((unsigned)(csr =+ 256) > 0100000)
E 7
I 7
				if(((unsigned)(csr =+ 256)) > (unsigned) 0100000)
E 8
I 8
				if((unsigned int)(csr =+ 256) >= 0100000)
E 8
E 7
E 3
					bp->b_xmem++;
				csr =<< 1;
D 7
				bp->b_addr = csr;
E 7
I 7
				bp->b_un.b_addr = csr;
I 8
/*
printf("dvx 0%o/%o\n", bp->b_xmem, bp->b_un.b_addr);
*/
E 8
E 7
				if ((bp->dvsec =+ INTLV+1) >= 12) {
					bp->dvsec =- 12;
					if (--bp->dvhd < 0) {
						bp->dvhd = 19;
						if(++bp->dvsec==INTLV+1){
							bp->dvsec = 0;
							csr = bp->dvcyl;
D 8
D 2
							if(bp->b_dev.d_minor&01)
E 2
I 2
							if(minor(bp->b_dev)&01)
E 2
								csr++;
							else
								csr--;
E 8
I 8
							csr--;
E 8
							bp->dvcyl = csr;
						}
					}
				}
				dvstart();
				return;
			}
		}
	}
D 8
	bp->b_resid = DVADDR->dvwcr + 0160000;
E 8
I 8
	bp->b_resid = -(DVADDR->dvwcr + 0160000);
E 8
	p1 = dv_ap;
	(*p1).dv_nreq--;
	if(((*p1).dv_bufp = bp->av_forw)==NULL || ++dv_nxfr > SWTUNIT) {
		dv_nxfr = 0;
		csr = NDV;
		do {
			if(++dv_ap >= &dvq[NDV])
				dv_ap = dvq;
			if((*dv_ap).dv_bufp != NULL)
				goto out0;
		} while (--csr);
		dv_ap = 0;
		goto out1;
	}
out0:
	dvstart();
out1:
D 2
	dvtab.d_errcnt = 0;
E 2
I 2
	dvtab.b_errcnt = 0;
E 2
	iodone(bp);
}

dvrdy()
{
	register int cnt;

	cnt = 0;
D 8
	while(--cnt && (DVADDR->dvssr&DRVRDY));
E 8
I 8
	while(--cnt && (DVADDR->dvcsr&DRVRDY));
E 8
	if (cnt==0) {
		printf("diva not ready\n");
		dvherr(1);
		return(1);
	} else
		return(0);
}

I 8
dvwait()
{
	register int cnt;

	cnt = 0;
	while(--cnt && (DVADDR->dvcsr&DRVRDY));
	if (cnt==0) {
		printf("diva not ready\n");
		return(1);
	} else
		return(0);
}

E 8
dvherr(n)
{
	register struct buf *bp;

	bp = (*dv_ap).dv_bufp;
D 8
	printf("dvflg %o\n",bp->b_flags);
E 8
I 8
	printf("dvflg 0%o\n",bp->b_flags);
E 8
	bp->b_flags =| B_ERROR;
D 2
	dvtab.d_errcnt = 0;
	dvtab.d_active = 0;
E 2
I 2
	dvtab.b_errcnt = 0;
	dvtab.b_active = 0;
E 2
	(*dv_ap).dv_bufp = bp->av_forw;
	for(dv_ap = dvq; dv_ap < &dvq[NDV]; dv_ap++)
		if((*dv_ap).dv_bufp != NULL) {
			if(n==0)
				dvstart();
			goto out;
		}
	dv_ap = 0;
out:
	iodone(bp);
}

dvread(dev)
{

	if (dvphys(dev))
		physio(dvstrategy, &rdvbuf, dev, B_READ);
}

dvwrite(dev)
{

	if (dvphys(dev))
		physio(dvstrategy, &rdvbuf, dev, B_WRITE);
}

dvphys(dev)
{
D 8
	register c;
E 8
I 8
	daddr_t c;
E 8

D 8
D 6
	c = lshift(u.u_offset, -9);
	c =+ ldiv(u.u_count+511, 512);
E 6
I 6
	c = u.u_offset >> 9;
	c += (u.u_count + 511) / 512;
E 8
I 8
	c = (u.u_offset+u.u_count+511)>>BSHIFT;
E 8
E 6
D 2
	if (c > dv_sizes[dev.d_minor & 07].nblocks) {
E 2
I 2
	if (c > dv_sizes[minor(dev) & 07].nblocks) {
E 2
		u.u_error = ENXIO;
		return(0);
	}
	return(1);
}
E 1
