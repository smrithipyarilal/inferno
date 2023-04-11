#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include 	"libsec.h"

enum {
	Qdir,
	Qdata,
	Qctl,
};

Dirtab skeltab[] =
{
	".",		{Qdir, 0, QTDIR},	0,	0555,
	"skeldata",	{Qdata},	0,	0666,
	"skelctl",	{Qctl},	0,	0666,
};

enum {
	CMbind,
	CMkey,
};

Cmdtab skelcmd[] = 
{
	CMbind, "bind", 0,
	CMkey, "key", 0,
};

Chan	*gc;
char	*gkey;

static Chan *
skelattach(char *spec)
{
	Chan *c;

	c = devattach('S', spec);
	return c;

}

static Walkqid *
skelwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, skeltab, nelem(skeltab), devgen);
}

static int
skelstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, skeltab, nelem(skeltab), devgen);
}

static Chan *
skelopen(Chan *c, int omode)
{
	return devopen(c, omode, skeltab, nelem(skeltab), devgen);
}

static void
skelclose(Chan *c)
{
	USED(c);
}

static void
unionrewind(Chan *c)
{
	qlock(&c->umqlock);
	c->uri = 0;
	if(c->umc){
		cclose(c->umc);
		c->umc = nil;
	}
	qunlock(&c->umqlock);
}

static void
fileread(void *va, ulong *count, vlong offset)
{
	int dir;
	ulong size;
	Lock *cl;
	int off;

	size = *count;
	dir = gc->qid.type & QTDIR;
	if(dir && gc->umh)
		size = unionread(gc, va, size);
	else{
		cl = &gc->l;
		if(&offset == nil){
			lock(cl);	/* lock for vlong assignment */
			off = gc->offset;
			unlock(cl);
		}else
			off = offset;
		if(off < 0)
			error(Enegoff);
		if(off == 0){
			if(&offset == nil){
				lock(cl);
				gc->offset = 0;
				gc->dri = 0;
				unlock(cl);
			}
			unionrewind(gc);
		}
		size = devtab[gc->type]->read(gc, va, size, off);
		lock(cl);
		gc->offset += size;
		unlock(cl);
	}
	*count = size;
}

static long
skelread(Chan *c, void *va, long count, vlong offset)
{
	USED(offset);
	ulong size;
	int key;
	int i;
	char *vabuff;

	if (c->qid.type && QTDIR)
		return devdirread(c, va, count, skeltab, nelem(skeltab), devgen);

	key = 1;
	switch((ulong)(c->qid.path)){	
		case Qdata:
			size = skeltab[1].length;
			if (size != 0){
				fileread(va, &size, offset);
				vabuff = malloc(size + 1);
				memmove(vabuff, va, size);
				for (i = 0; vabuff[i] != '\0'; i++) {
					vabuff[i] -= key;
				}
		}
		break;
	case Qctl:
		if (gc || gkey){
			if (gkey)
				print("Key: %s\n", gkey);
			if (gc)
				print("File: %s\n", gc->name->s);
		} else{
			print("No ctl file made\n");
		}
	default:
		break;
	}
	
	return 0;
}

static long
filewrite(void *va, long n, vlong *offp)
{
	Lock *cl;
	volatile struct { Chan *c; } c;
	vlong off;
	long m;

	if(waserror())
		return -1;
	c.c = gc;

	if(waserror()){
		cclose(c.c);
		nexterror();
	}
	if(c.c->qid.type & QTDIR)
		error(Eisdir);

	if(n < 0)
		error(Etoosmall);

	cl = &c.c->l;
	if(offp == nil){
		lock(cl);
		off = c.c->offset;
		c.c->offset += n;
		unlock(cl);
	}else
		off = *offp;

	if(waserror()){
		if(offp == nil){
			lock(cl);
			c.c->offset -= n;
			unlock(cl);
		}
		nexterror();
	}
	if(off < 0)
		error(Enegoff);
	m = devtab[c.c->type]->write(c.c, va, n, off);
	poperror();

	if(offp == nil && m < n){
		lock(cl);
		c.c->offset -= n - m;
		unlock(cl);
	}

	poperror();

	poperror();
	return m;
}

static long
skelwrite(Chan *c, void *va, long count, vlong offset)
{
	USED(c);
	USED(va);
	USED(offset);

	ulong len;
	int i;
	int key;
	Dir	*d;
	char *vabuff;
	Cmdbuf *cb;
	Cmdtab *ct;

	key = 1;
	switch((ulong)c->qid.path) {
		case Qdata:
			if (!gc){
				print("no path given.\n");
				vabuff = malloc(count + 2);
				memmove(vabuff, va, count);
				for (i = 0; vabuff[i] != '\0'; i++) {
					vabuff[i] += key;
				}
				print("%s\n", vabuff);
			}
			else{
				offset = skeltab[1].length;
				vabuff = malloc(strlen(va) + 1);
				memmove(vabuff, va, strlen(va));
				for (i = 0; vabuff[i] != '\0'; i++) {
					vabuff[i] += key;
				}
				memmove(va, vabuff, strlen(vabuff));
				len = strlen(va);
				filewrite(va, len, &offset);
				skeltab[1].length += len;
			}
			break;
		case Qctl:
			cb = parsecmd(va, count);
			if(waserror()){
				free(cb);
				nexterror();
			}
			ct = lookupcmd(cb, skelcmd, nelem(skelcmd));
			switch(ct->index){
				case CMbind:
					strdup(cb->f[1]);
					gc = malloc(sizeof(Chan*));
					gc = namec(cb->f[1], Aopen, 2, 0);
					d = chandirstat(gc);
					skeltab[1].length = d->length;
					print("Binded!\n");
					break;
				case CMkey:
					strdup(cb->f[1]);
					gkey = malloc(strlen(cb->f[1]));
					memmove(gkey, cb->f[1], strlen(cb->f[1]));
					print("Got key!\n");
					break;
			}
			poperror();
			free(cb);
			break;

		default:
			panic("skelwrite");
	}

	return count;
}

Dev skeldevtab = {
	'S',
	"skel",

	devinit,	
	skelattach,
	skelwalk,
	skelstat,
	skelopen,
	devcreate,
	skelclose,
	skelread,
	devbread,
	skelwrite,
	devbwrite,
	devremove,
	devwstat,
};
