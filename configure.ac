AC_PREREQ(2.59)
AC_INIT([mtree], [1.0.0], [https://github.com/archiecobbs/nmtree/issues])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([mtree.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.9 foreign])

# Allow changing program name
AC_ARG_PROGRAM

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB(nbcompat, fparseln)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([ctype.h dirent.h err.h errno.h fcntl.h fnmatch.h fts.h \
	grp.h limits.h netdb.h pwd.h \
	stdarg.h stddef.h stdio.h stdlib.h string.h time.h unistd.h \
	util.h vis.h])
AC_CHECK_HEADERS([sys/cdefs.h sys/param.h sys/queue.h sys/stat.h sys/types.h])
AC_CHECK_HEADERS([nbcompat/md5.h nbcompat/rmd160.h nbcompat/sha1.h nbcompat/sha2.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_CHECK_MEMBERS([struct stat.st_flags])
AC_CHECK_DECLS([UF_SETTABLE, SF_SETTABLE])
AH_BOTTOM([/* Define to 1 if the user- and root-changeable masks were detected */
#if HAVE_STRUCT_STAT_ST_FLAGS && HAVE_DECL_UF_SETTABLE && HAVE_DECL_SF_SETTABLE
#define HAVE_FILE_FLAGS 1
#endif
])
AC_HEADER_TIME

# Checks for library functions.
AC_REPLACE_FNMATCH
AC_CHECK_FUNCS([endgrent endpwent getcwd gethostname mkdir SHA256_File strchr strpbrk strstr strtol strtoul])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
