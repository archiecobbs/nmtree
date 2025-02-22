/*	$NetBSD: create.c,v 1.9 2013/09/08 16:20:10 ryoon Exp $	*/

/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <nbcompat.h>
#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#if defined(__RCSID) && !defined(lint)
#if 0
static char sccsid[] = "@(#)create.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD: create.c,v 1.9 2013/09/08 16:20:10 ryoon Exp $");
#endif
#endif /* not lint */

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if ! HAVE_NBTOOL_CONFIG_H
#if HAVE_DIRENT_H
#include <dirent.h>
#endif
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_GRP_H
#include <grp.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#if HAVE_STDIO_H
#include <stdio.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef NO_MD5
#if HAVE_MD5_H
#include <md5.h>
#else
#include <nbcompat/md5.h>
#endif
#endif
#ifndef NO_RMD160
#if HAVE_RMD160_H
#include <rmd160.h>
#else
#include <nbcompat/rmd160.h>
#endif
#endif
#ifndef NO_SHA1
#if HAVE_SHA1_H
#include <sha1.h>
#else
#include <nbcompat/sha1.h>
#endif
#endif
#ifndef NO_SHA2
#if HAVE_SHA2_H && HAVE_SHA512_FILE
#include <sha2.h>
#else
#include <nbcompat/sha2.h>
#endif
#endif

#include "extern.h"

#define	INDENTNAMELEN	15
#define	MAXLINELEN	80

static gid_t gid;
static uid_t uid;
static mode_t mode;
static u_long flags;

static int	dsort(const FTSENT **, const FTSENT **);
static void	output(int *, const char *, ...)
	__attribute__((__format__(__printf__, 2, 3)));
static int	statd(FTS *, FTSENT *, uid_t *, gid_t *, mode_t *, u_long *);
static void	statf(FTSENT *);

void
cwalk(void)
{
	FTS *t;
	FTSENT *p;
	time_t clocktime;
	char host[MAXHOSTNAMELEN + 1];
	char *argv[2];
	char  dot[] = ".";
	argv[0] = dot;
	argv[1] = NULL;

	time(&clocktime);
	gethostname(host, sizeof(host));
	host[sizeof(host) - 1] = '\0';
	printf(
	    "#\t   user: %s\n#\tmachine: %s\n#\t   tree: %s\n#\t   date: %s",
	    getlogin(), host, fullpath, ctime(&clocktime));

	if ((t = fts_open(argv, ftsoptions, dsort)) == NULL)
		mtree_err("fts_open: %s", strerror(errno));
	while ((p = fts_read(t)) != NULL) {
		if (check_excludes(p->fts_name, p->fts_path)) {
			fts_set(t, p, FTS_SKIP);
			continue;
		}
		switch(p->fts_info) {
		case FTS_D:
			printf("\n# %s\n", p->fts_path);
			statd(t, p, &uid, &gid, &mode, &flags);
			statf(p);
			break;
		case FTS_DP:
			if (p->fts_level > 0)
				printf("# %s\n..\n\n", p->fts_path);
			break;
		case FTS_DNR:
		case FTS_ERR:
		case FTS_NS:
			mtree_err("%s: %s",
			    p->fts_path, strerror(p->fts_errno));
			break;
		default:
			if (!dflag)
				statf(p);
			break;

		}
	}
	fts_close(t);
	if (sflag && keys & F_CKSUM)
		mtree_err("%s checksum: %u", fullpath, crc_total);
}

static void
statf(FTSENT *p)
{
	uint32_t len, val;
	int fd, indent;
	const char *name;
#if !defined(NO_MD5) || !defined(NO_RMD160) || !defined(NO_SHA1) || !defined(NO_SHA2)
	char digestbuf[MAXHASHLEN + 1];
#endif

	indent = printf("%s%s",
	    S_ISDIR(p->fts_statp->st_mode) ? "" : "    ", vispath(p->fts_name));

	if (indent > INDENTNAMELEN)
		indent = MAXLINELEN;
	else
		indent += printf("%*s", INDENTNAMELEN - indent, "");

	if (!S_ISREG(p->fts_statp->st_mode))
		output(&indent, "type=%s", inotype(p->fts_statp->st_mode));
	if (keys & (F_UID | F_UNAME) && p->fts_statp->st_uid != uid) {
		if (keys & F_UNAME &&
		    (name = user_from_uid(p->fts_statp->st_uid, 1)) != NULL)
			output(&indent, "uname=%s", name);
		else /* if (keys & F_UID) */
			output(&indent, "uid=%u", p->fts_statp->st_uid);
	}
	if (keys & (F_GID | F_GNAME) && p->fts_statp->st_gid != gid) {
		if (keys & F_GNAME &&
		    (name = group_from_gid(p->fts_statp->st_gid, 1)) != NULL)
			output(&indent, "gname=%s", name);
		else /* if (keys & F_GID) */
			output(&indent, "gid=%u", p->fts_statp->st_gid);
	}
	if (keys & F_MODE && (p->fts_statp->st_mode & MBITS) != mode)
		output(&indent, "mode=%#o", p->fts_statp->st_mode & MBITS);
	if (keys & F_DEV &&
	    (S_ISBLK(p->fts_statp->st_mode) || S_ISCHR(p->fts_statp->st_mode)))
		output(&indent, "device=%#lx", (unsigned long)p->fts_statp->st_rdev);
	if (keys & F_NLINK && p->fts_statp->st_nlink != 1)
		output(&indent, "nlink=%lu", (unsigned long)p->fts_statp->st_nlink);
	if (keys & F_SIZE && S_ISREG(p->fts_statp->st_mode))
		output(&indent, "size=%lld", (long long)p->fts_statp->st_size);
	if (keys & F_TIME)
		output(&indent, "time=%ju.%09ld",
		    (uintmax_t)p->fts_statp->st_mtim.tv_sec,
		    (long)p->fts_statp->st_mtim.tv_nsec);
	if (keys & F_CKSUM && S_ISREG(p->fts_statp->st_mode)) {
		if ((fd = open(p->fts_accpath, O_RDONLY, 0)) < 0 ||
		    crc(fd, &val, &len))
			mtree_err("%s: %s", p->fts_accpath, strerror(errno));
		close(fd);
		output(&indent, "cksum=%lu", (long)val);
	}
#ifndef NO_MD5
	if (keys & F_MD5 && S_ISREG(p->fts_statp->st_mode)) {
		if (MD5File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "MD5File");
		output(&indent, "md5=%s", digestbuf);
	}
#endif	/* ! NO_MD5 */
#ifndef NO_RMD160
	if (keys & F_RMD160 && S_ISREG(p->fts_statp->st_mode)) {
		if (RMD160File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "RMD160File");
		output(&indent, "rmd160=%s", digestbuf);
	}
#endif	/* ! NO_RMD160 */
#ifndef NO_SHA1
	if (keys & F_SHA1 && S_ISREG(p->fts_statp->st_mode)) {
		if (SHA1File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "SHA1File");
		output(&indent, "sha1=%s", digestbuf);
	}
#endif	/* ! NO_SHA1 */
#ifndef NO_SHA2
	if (keys & F_SHA256 && S_ISREG(p->fts_statp->st_mode)) {
		if (SHA256_File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "SHA256_File");
		output(&indent, "sha256=%s", digestbuf);
	}
	if (keys & F_SHA384 && S_ISREG(p->fts_statp->st_mode)) {
		if (SHA384_File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "SHA384_File");
		output(&indent, "sha384=%s", digestbuf);
	}
	if (keys & F_SHA512 && S_ISREG(p->fts_statp->st_mode)) {
		if (SHA512_File(p->fts_accpath, digestbuf) == NULL)
			mtree_err("%s: %s", p->fts_accpath, "SHA512_File");
		output(&indent, "sha512=%s", digestbuf);
	}
#endif	/* ! NO_SHA2 */
	if (keys & F_SLINK &&
	    (p->fts_info == FTS_SL || p->fts_info == FTS_SLNONE))
		output(&indent, "link=%s", vispath(rlink(p->fts_accpath)));
#if HAVE_STRUCT_STAT_ST_FLAGS
	if (keys & F_FLAGS && p->fts_statp->st_flags != flags)
		output(&indent, "flags=%s",
		    flags_to_string(p->fts_statp->st_flags, "none"));
#endif
	putchar('\n');
}

/* XXX
 * FLAGS2INDEX will fail once the user and system settable bits need more
 * than one byte, respectively.
 */
#define FLAGS2INDEX(x)  (((x >> 8) & 0x0000ff00) | (x & 0x000000ff))

#define	MTREE_MAXGID	5000
#define	MTREE_MAXUID	5000
#define	MTREE_MAXMODE	(MBITS + 1)
#if HAVE_STRUCT_STAT_ST_FLAGS
#define	MTREE_MAXFLAGS  (FLAGS2INDEX(CH_MASK) + 1)   /* 1808 */
#else
#define MTREE_MAXFLAGS	1
#endif
#define	MTREE_MAXS 16

static int
statd(FTS *t, FTSENT *parent, uid_t *puid, gid_t *pgid, mode_t *pmode,
      u_long *pflags)
{
	FTSENT *p;
	gid_t sgid;
	uid_t suid;
	mode_t smode;
#if HAVE_STRUCT_STAT_ST_FLAGS
	u_long sflags = 0;
#endif
	const char *name;
	gid_t savegid;
	uid_t saveuid;
	mode_t savemode;
	u_long saveflags;
	u_short maxgid, maxuid, maxmode, maxflags;
	u_short g[MTREE_MAXGID], u[MTREE_MAXUID],
		m[MTREE_MAXMODE], f[MTREE_MAXFLAGS];
	static int first = 1;

	savegid = *pgid;
	saveuid = *puid;
	savemode = *pmode;
	saveflags = *pflags;
	if ((p = fts_children(t, 0)) == NULL) {
		if (errno)
			mtree_err("%s: %s", RP(parent), strerror(errno));
		return (1);
	}

	memset(g, 0, sizeof(g));
	memset(u, 0, sizeof(u));
	memset(m, 0, sizeof(m));
	memset(f, 0, sizeof(f));

	maxuid = maxgid = maxmode = maxflags = 0;
	for (; p; p = p->fts_link) {
		smode = p->fts_statp->st_mode & MBITS;
		if (smode < MTREE_MAXMODE && ++m[smode] > maxmode) {
			savemode = smode;
			maxmode = m[smode];
		}
		sgid = p->fts_statp->st_gid;
		if (sgid < MTREE_MAXGID && ++g[sgid] > maxgid) {
			savegid = sgid;
			maxgid = g[sgid];
		}
		suid = p->fts_statp->st_uid;
		if (suid < MTREE_MAXUID && ++u[suid] > maxuid) {
			saveuid = suid;
			maxuid = u[suid];
		}

#if HAVE_STRUCT_STAT_ST_FLAGS
		sflags = FLAGS2INDEX(p->fts_statp->st_flags);
		if (sflags < MTREE_MAXFLAGS && ++f[sflags] > maxflags) {
			saveflags = p->fts_statp->st_flags;
			maxflags = f[sflags];
		}
#endif
	}
	/*
	 * If the /set record is the same as the last one we do not need to
	 * output a new one.  So first we check to see if anything changed.
	 * Note that we always output a /set record for the first directory.
	 */
	if (((keys & (F_UNAME | F_UID)) && (*puid != saveuid)) ||
	    ((keys & (F_GNAME | F_GID)) && (*pgid != savegid)) ||
	    ((keys & F_MODE) && (*pmode != savemode)) || 
	    ((keys & F_FLAGS) && (*pflags != saveflags)) ||
	    first) {
		first = 0;
		printf("/set type=file");
		if (keys & (F_UID | F_UNAME)) {
			if (keys & F_UNAME &&
			    (name = user_from_uid(saveuid, 1)) != NULL)
				printf(" uname=%s", name);
			else /* if (keys & F_UID) */
				printf(" uid=%lu", (u_long)saveuid);
		}
		if (keys & (F_GID | F_GNAME)) {
			if (keys & F_GNAME &&
			    (name = group_from_gid(savegid, 1)) != NULL)
				printf(" gname=%s", name);
			else /* if (keys & F_UID) */
				printf(" gid=%lu", (u_long)savegid);
		}
		if (keys & F_MODE)
			printf(" mode=%#lo", (u_long)savemode);
		if (keys & F_NLINK)
			printf(" nlink=1");
		if (keys & F_FLAGS)
			printf(" flags=%s",
			    flags_to_string(saveflags, "none"));
		printf("\n");
		*puid = saveuid;
		*pgid = savegid;
		*pmode = savemode;
		*pflags = saveflags;
	}
	return (0);
}

static int
dsort(const FTSENT **a, const FTSENT **b)
{

	if (S_ISDIR((*a)->fts_statp->st_mode)) {
		if (!S_ISDIR((*b)->fts_statp->st_mode))
			return (1);
	} else if (S_ISDIR((*b)->fts_statp->st_mode))
		return (-1);
	return (strcmp((*a)->fts_name, (*b)->fts_name));
}

static void
output(int *offset, const char *fmt, ...)
{
	va_list ap;
	char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (*offset + strlen(buf) > MAXLINELEN - 3) {
		printf(" \\\n%*s", INDENTNAMELEN, "");
		*offset = INDENTNAMELEN;
	}
	*offset += printf(" %s", buf) + 1;
}
