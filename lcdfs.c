#include <u.h>
#include <libc.h>
#include <auth.h>
#include <thread.h>
#include <fcall.h>
#include <9p.h>
#include "lcd.h"

static void fsattach(Req* r);
static void fscreate(Req* r);
static void fsread(Req* r);
static void fswrite(Req* r);
static char* fswalk1(Fid* fid, char* name, Qid* qid);
static char* sclone(Fid* fid, Fid* newfid);
static void fsstat(Req* r);
static void fsremove(Req* r);
static void freefid(Fid* fid);

static Srv sfs =
{
	.attach =		fsattach,
	.create =		fscreate,
	.remove =		fsremove,
	.read 	=		fsread,
	.write	=		fswrite,
	.walk1	=		fswalk1,
	.clone	=		fsclone,
	.stat	=		fsstat,
	.destroyfid=	freefid,
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
	mnt = "/mnt/lcd"
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
