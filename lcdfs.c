#include <u.h>
#include <libc.h>
#include <auth.h>
#include <thread.h>
#include <fcall.h>
#include <9p.h>
#include "lcd.h"

static int
getdirent(int n, Dir* d, void*)
{
	d->atime = time(nil);
	d->mtime = d->atime;
	d->uid = estrdup9p(getuser());
	d->gid = estrdup9p(d->uid);
	d->muid = estrdup9p(d->uid);
	if (n == -1) {
		d->qid = (Qid){0, 0, QTDIR};
		d->mode = 0775;
		d->name = estrdup9p("/");
		d->length = 0;
	} else
		return -1;
	return 0;
}

static void
fsstart(Srv* s)
{
	print("hello everybody\n");
}

static void 
fsattach(Req* r)
{
	r->fid->qid = (Qid){0,0,QTDIR};
	r->ofcall.qid = r->fid->qid;
	respond(r, nil);
}

static void
fsread(Req* r)
{
	print("readin!\n");

	Fid* fid;
	Qid q;
	fid = r->fid;
	q = fid->qid;
	if (q.type&QTDIR) {
		dirread9p(r, getdirent, nil);
		respond(r, nil);
		return;
	}

	print("extrange file :0\n");
}

static char* 
fswalk1(Fid* fid, char* name, Qid* qid)
{
	return "???";

	Qid q;

	q = fid->qid;
	if (!(q.type&QTDIR)) {
		print(">>>>>%s<<<<<<\n", name);
		fid->qid = (Qid){0,0,QTDIR};
		*qid = fid->qid;
		return nil;
	}

	return "no such file!";
};

static void
fsstat(Req* r)
{
	Fid* fid;
	Qid q;

	fid = r->fid;
	q = fid->qid;
	if (q.type&QTDIR)
		getdirent(-1, &r->d, nil);
	else
		getdirent(q.path, &r->d, nil);
	respond(r, nil);
}

static Srv sfs =
{
	.start	= 		fsstart,
	.read	=		fsread,
	.attach =		fsattach,
	.walk1 	=		fswalk1,
	.stat	=		fsstat,
};

void
usage(void)
{
	fprint(2, "usage: %s [-D] [-s srv] [-m mnt]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char*	mnt;
	char*	srv;

	srv = nil;
	mnt = "/mnt/lcd";
	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 's':
		srv = EARGF(usage());
		break;
	case 'm':
		mnt = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND;

	if(argc != 0)
		usage();
	threadpostmountsrv(&sfs, srv, mnt, MREPL|MCREATE);
	threadexits(nil);
}
