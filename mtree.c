/*	$NetBSD: mtree.c,v 1.3 2004/08/21 04:10:45 jlam Exp $	*/

/*-
 * Copyright (c) 1989, 1990, 1993
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
#if defined(__COPYRIGHT) && !defined(lint)
__COPYRIGHT("@(#) Copyright (c) 1989, 1990, 1993\n\
	The Regents of the University of California.  All rights reserved.\n");
#endif /* not lint */

#if defined(__RCSID) && !defined(lint)
#if 0
static char sccsid[] = "@(#)mtree.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD: mtree.c,v 1.3 2004/08/21 04:10:45 jlam Exp $");
#endif
#endif /* not lint */

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_STDIO_H
#include <stdio.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "extern.h"

int	ftsoptions = FTS_PHYSICAL;
int	cflag, Cflag, dflag, Dflag, eflag, iflag, lflag, mflag,
    	rflag, sflag, tflag, uflag, Uflag;
char	fullpath[MAXPATHLEN];

	int	main(int, char **);
static	void	usage(void);

int
main(int argc, char **argv)
{
	int	ch, status;
	char	*dir, *p;

	setprogname(argv[0]);

	dir = NULL;
	init_excludes();

	while ((ch = getopt(argc, argv, "cCdDeE:f:I:ik:K:lLmMN:p:PrR:s:tuUWxX:"))
	    != -1) {
		switch((char)ch) {
		case 'c':
			cflag = 1;
			break;
		case 'C':
			Cflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 'D':
			Dflag = 1;
			break;
		case 'E':
			parsetags(&excludetags, optarg);
			break;
		case 'e':
			eflag = 1;
			break;
		case 'f':
			if (!(freopen(optarg, "r", stdin)))
				mtree_err("%s: %s", optarg, strerror(errno));
			break;
		case 'i':
			iflag = 1;
			break;
		case 'I':
			parsetags(&includetags, optarg);
			break;
		case 'k':
			keys = F_TYPE;
			while ((p = strsep(&optarg, " \t,")) != NULL)
				if (*p != '\0')
					keys |= parsekey(p, NULL);
			break;
		case 'K':
			while ((p = strsep(&optarg, " \t,")) != NULL)
				if (*p != '\0')
					keys |= parsekey(p, NULL);
			break;
		case 'l':
			lflag = 1;
			break;
		case 'L':
			ftsoptions &= ~FTS_PHYSICAL;
			ftsoptions |= FTS_LOGICAL;
			break;
		case 'm':
			mflag = 1;
			break;
		case 'M':
			mtree_Mflag = 1;
			break;
		case 'N':
			if (! setup_getid(optarg))
				mtree_err(
			    "Unable to use user and group databases in `%s'",
				    optarg);
			break;
		case 'p':
			dir = optarg;
			break;
		case 'P':
			ftsoptions &= ~FTS_LOGICAL;
			ftsoptions |= FTS_PHYSICAL;
			break;
		case 'r':
			rflag = 1;
			break;
		case 'R':
			while ((p = strsep(&optarg, " \t,")) != NULL)
				if (*p != '\0')
					keys &= ~parsekey(p, NULL);
			break;
		case 's':
			sflag = 1;
			crc_total = ~strtol(optarg, &p, 0);
			if (*p)
				mtree_err("illegal seed value -- %s", optarg);
			break;
		case 't':
			tflag = 1;
			break;
		case 'u':
			uflag = 1;
			break;
		case 'U':
			Uflag = uflag = 1;
			break;
		case 'W':
			mtree_Wflag = 1;
			break;
		case 'x':
			ftsoptions |= FTS_XDEV;
			break;
		case 'X':
			read_excludes_file(optarg);
			break;
		case '?':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc)
		usage();

	if (dir && chdir(dir))
		mtree_err("%s: %s", dir, strerror(errno));

	if ((cflag || sflag) && !getcwd(fullpath, sizeof(fullpath)))
		mtree_err("%s", strerror(errno));

	if ((cflag && Cflag) || (cflag && Dflag) || (Cflag && Dflag))
		mtree_err("-c, -C and -D flags are mutually exclusive");

	if (iflag && mflag)
		mtree_err("-i and -m flags are mutually exclusive");

	if (lflag && uflag)
		mtree_err("-l and -u flags are mutually exclusive");

	if (cflag) {
		cwalk();
		exit(0);
	}
	if (Cflag || Dflag) {
		dump_nodes("", spec(stdin), Dflag);
		exit(0);
	}
	status = verify();
	if (Uflag & (status == MISMATCHEXIT))
		status = 0;
	exit(status);
}

static void
usage(void)
{

	fprintf(stderr,
	    "usage: %s [-cCdDelLMPruUWx] [-i|-m] [-f spec] [-k key]\n"
	    "\t\t[-K addkey] [-R removekey] [-I inctags] [-E exctags]\n"
	    "\t\t[-N userdbdir] [-X exclude-file] [-p path] [-s seed]\n",
	    getprogname());
	exit(1);
}
