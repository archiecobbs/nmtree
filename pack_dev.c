/*	$NetBSD: pack_dev.c,v 1.5 2008/11/06 02:14:52 jschauma Exp $	*/

/*-
 * Copyright (c) 1998, 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#if !defined(lint)
__RCSID("$NetBSD: pack_dev.c,v 1.5 2008/11/06 02:14:52 jschauma Exp $");
#endif /* not lint */

#include <sys/sysmacros.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_LIMITS_H
#include <limits.h>
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

#include "pack_dev.h"

static	pack_t	pack_netbsd;
static	pack_t	pack_freebsd;
static	pack_t	pack_8_8;
static	pack_t	pack_12_20;
static	pack_t	pack_14_18;
static	pack_t	pack_8_24;
static	pack_t	pack_bsdos;
static	int	compare_format(const void *, const void *);

static const char iMajorError[] = "invalid major number";
static const char iMinorError[] = "invalid minor number";
static const char tooManyFields[] = "too many fields for format";

	/* exported */
portdev_t
pack_native(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev(numbers[0], numbers[1]);
		if (major(dev) != numbers[0])
			*error = iMajorError;
		else if (minor(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


static portdev_t
pack_netbsd(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_netbsd(numbers[0], numbers[1]);
		if (major_netbsd(dev) != numbers[0])
			*error = iMajorError;
		else if (minor_netbsd(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_freebsd(x)	((int32_t)(((x) & 0x0000ff00) >> 8))
#define	minor_freebsd(x)	((int32_t)(((x) & 0xffff00ff) >> 0))
#define	makedev_freebsd(x,y)	((portdev_t)((((x) << 8) & 0x0000ff00) | \
					 (((y) << 0) & 0xffff00ff)))

static portdev_t
pack_freebsd(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_freebsd(numbers[0], numbers[1]);
		if (major_freebsd(dev) != numbers[0])
			*error = iMajorError;
		if (minor_freebsd(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_8_8(x)		((int32_t)(((x) & 0x0000ff00) >> 8))
#define	minor_8_8(x)		((int32_t)(((x) & 0x000000ff) >> 0))
#define	makedev_8_8(x,y)	((portdev_t)((((x) << 8) & 0x0000ff00) | \
					 (((y) << 0) & 0x000000ff)))

static portdev_t
pack_8_8(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_8_8(numbers[0], numbers[1]);
		if (major_8_8(dev) != numbers[0])
			*error = iMajorError;
		if (minor_8_8(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_12_20(x)		((int32_t)(((x) & 0xfff00000) >> 20))
#define	minor_12_20(x)		((int32_t)(((x) & 0x000fffff) >>  0))
#define	makedev_12_20(x,y)	((portdev_t)((((x) << 20) & 0xfff00000) | \
					 (((y) <<  0) & 0x000fffff)))

static portdev_t
pack_12_20(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_12_20(numbers[0], numbers[1]);
		if (major_12_20(dev) != numbers[0])
			*error = iMajorError;
		if (minor_12_20(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_14_18(x)		((int32_t)(((x) & 0xfffc0000) >> 18))
#define	minor_14_18(x)		((int32_t)(((x) & 0x0003ffff) >>  0))
#define	makedev_14_18(x,y)	((portdev_t)((((x) << 18) & 0xfffc0000) | \
					 (((y) <<  0) & 0x0003ffff)))

static portdev_t
pack_14_18(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_14_18(numbers[0], numbers[1]);
		if (major_14_18(dev) != numbers[0])
			*error = iMajorError;
		if (minor_14_18(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_8_24(x)		((int32_t)(((x) & 0xff000000) >> 24))
#define	minor_8_24(x)		((int32_t)(((x) & 0x00ffffff) >>  0))
#define	makedev_8_24(x,y)	((portdev_t)((((x) << 24) & 0xff000000) | \
					 (((y) <<  0) & 0x00ffffff)))

static portdev_t
pack_8_24(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_8_24(numbers[0], numbers[1]);
		if (major_8_24(dev) != numbers[0])
			*error = iMajorError;
		if (minor_8_24(dev) != numbers[1])
			*error = iMinorError;
	} else
		*error = tooManyFields;
	return (dev);
}


#define	major_12_12_8(x)	((int32_t)(((x) & 0xfff00000) >> 20))
#define	unit_12_12_8(x)		((int32_t)(((x) & 0x000fff00) >>  8))
#define	subunit_12_12_8(x)	((int32_t)(((x) & 0x000000ff) >>  0))
#define	makedev_12_12_8(x,y,z)	((portdev_t)((((x) << 20) & 0xfff00000) | \
					 (((y) <<  8) & 0x000fff00) | \
					 (((z) <<  0) & 0x000000ff)))

static portdev_t
pack_bsdos(int n, u_long numbers[], const char **error)
{
	portdev_t dev = 0;

	if (n == 2) {
		dev = makedev_12_20(numbers[0], numbers[1]);
		if (major_12_20(dev) != numbers[0])
			*error = iMajorError;
		if (minor_12_20(dev) != numbers[1])
			*error = iMinorError;
	} else if (n == 3) {
		dev = makedev_12_12_8(numbers[0], numbers[1], numbers[2]);
		if (major_12_12_8(dev) != numbers[0])
			*error = iMajorError;
		if (unit_12_12_8(dev) != numbers[1])
			*error = "invalid unit number";
		if (subunit_12_12_8(dev) != numbers[2])
			*error = "invalid subunit number";
	} else
		*error = tooManyFields;
	return (dev);
}


		/* list of formats and pack functions */
		/* this list must be sorted lexically */
struct format {
	const char	*name;
	pack_t		*pack;
} formats[] = {
	{"386bsd",  pack_8_8},
	{"4bsd",    pack_8_8},
	{"bsdos",   pack_bsdos},
	{"freebsd", pack_freebsd},
	{"hpux",    pack_8_24},
	{"isc",     pack_8_8},
	{"linux",   pack_8_8},
	{"native",  pack_native},
	{"netbsd",  pack_netbsd},
	{"osf1",    pack_12_20},
	{"sco",     pack_8_8},
	{"solaris", pack_14_18},
	{"sunos",   pack_8_8},
	{"svr3",    pack_8_8},
	{"svr4",    pack_14_18},
	{"ultrix",  pack_8_8},
};

static int
compare_format(const void *key, const void *element)
{
	const char		*name;
	const struct format	*format;

	name = key;
	format = element;

	return (strcmp(name, format->name));
}


pack_t *
pack_find(const char *name)
{
	struct format	*format;

	format = bsearch(name, formats,
	    sizeof(formats)/sizeof(formats[0]),
	    sizeof(formats[0]), compare_format);
	if (format == 0)
		return (NULL);
	return (format->pack);
}
