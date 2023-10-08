#include <u.h>
#include <libc.h>
#include <auth.h>
#include <thread.h>
#include <fcall.h>
#include <9p.h>
#include "lcd.h"

#define MAX 16

char* vfiles[] = {
	"line0",
	"line1",
};

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
		d->mode = DMDIR|0555;
		d->name = estrdup9p("/");
		d->length = 0;
	} else if (n >= 0 && n <= 1) {
		d->qid = (Qid){n+1, 0, QTFILE};
		d->mode = 0664;
		d->name = estrdup9p(vfiles[n]);
		d->length = 42;
	} else
		return -1;
	return 0;
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
	Fid*	fid;
	Qid		q;
	
	fid	= r->fid;
	q = fid->qid;
	if (q.type&QTDIR) {
		dirread9p(r, getdirent, nil);
		respond(r, nil);
		return;
	}

	readstr(r, "\n");
	respond(r, nil);
}

static void
fswrite(Req* r)
{
	Fid*	fid;
	Qid		q;
	char	str[MAX+1];

	fid = r->fid;
	q = fid->qid;
	if (q.type&QTDIR) {
		respond(r, "permission denied");
		return;
	}

	if (r->ifcall.count > MAX) {
		respond(r, "string is too large");
		return;
	}

	memmove(str, r->ifcall.data, r->ifcall.count);
	str[r->ifcall.count] = 0;

	print(
		"so you want to write[%s](%d) to %s(%ulld) huh?\n", 
		str,
		r->ifcall.count,
		vfiles[q.path-1],
		q.path
	);
	respond(r, nil);
}

static char* 
fswalk1(Fid* fid, char* name, Qid* qid)
{
	int 	i;
	Qid 	q;
	
	q = fid->qid;
	if (!(q.type&QTDIR)) {
		if (!strcmp(name, "..")) {
			fid->qid = (Qid){0,0,QTDIR};
			*qid = fid->qid;
			return nil;
		}
	} else {
		for (i = 0; i < 2; i++)
			if (vfiles[i] && !strcmp(name, vfiles[i])) {
				fid->qid = (Qid){i+1, 0, QTFILE};
				*qid = fid->qid;
				return nil;
			}
	}
	return "???";
};

static void
fsstat(Req* r)
{
	Fid* 	fid;
	Qid 	q;
	
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
	.read	=		fsread,
	.write 	=		fswrite,
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
