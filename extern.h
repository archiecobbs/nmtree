/*	$NetBSD: extern.h,v 1.5 2011/07/27 15:31:00 seb Exp $	*/

/*-
 * Copyright (c) 1991, 1993
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
 *
 *	@(#)extern.h	8.1 (Berkeley) 6/6/93
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "mtree.h"

#if 0
#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#else 
#define HAVE_STRUCT_STAT_ST_FLAGS 1
#endif
#endif
 
#include <nbcompat.h>
#if HAVE_ERR_H
#include <err.h> 
#endif
#if HAVE_FTS_H
#include <fts.h>
#endif

#if HAVE_NETDB_H
/* For MAXHOSTNAMELEN on some platforms. */
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

void	 addtag(slist_t *, char *);
int	 check_excludes(const char *, const char *);
int	 compare(NODE *, FTSENT *);
int	 crc(int, uint32_t *, uint32_t *);
void	 cwalk(void);
void	 dump_nodes(const char *, NODE *, int);
void	 init_excludes(void);
int	 matchtags(NODE *);
void	 mtree_err(const char *, ...)
	    __attribute__((__format__(__printf__, 1, 2)));
const char *nodetype(u_int);
u_int	 parsekey(const char *, int *);
void	 parsetags(slist_t *, char *);
u_int	 parsetype(const char *);
void	 read_excludes_file(const char *);
const char *rlink(const char *);
int	 verify(void);

extern int	dflag, eflag, gflag, iflag, lflag, mflag, rflag, sflag, tflag, uflag;
extern int	mtree_Mflag, mtree_Wflag;
extern size_t	mtree_lineno;
extern uint32_t crc_total;
extern int	ftsoptions, keys;
extern char	fullpath[];
extern slist_t	includetags, excludetags;


#include "stat_flags.h"
