# vim: noet:sw=8:ts=8:

bin_PROGRAMS=			mtree

mtree_SOURCES=			compare.c \
				crc.c \
				create.c \
				excludes.c \
				getid.c \
				misc.c \
				mtree.c \
				pack_dev.c \
				spec.c \
				stat_flags.c \
				verify.c

noinst_HEADERS=			extern.h \
				mtree.h \
				pack_dev.h \
				stat_flags.h

man_MANS=			mtree.5 mtree.8

docdir=		    		$(datadir)/doc/packages/$(PACKAGE)

doc_DATA=	    		README

EXTRA_DIST=	    		$(man_MANS)
