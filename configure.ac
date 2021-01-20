#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtkballs],[3.1.5],[])
AM_INIT_AUTOMAKE([-Wall foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_GNU_GETTEXT([external])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

LT_INIT

# Init intltool
IT_PROG_INTLTOOL

# Checks for programs.
AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/param.h signal.h])

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.8)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GETTEXT_PACKAGE=gtkballs
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

AM_GLIB_GNU_GETTEXT

case "$CC" in
  gcc*) CPPFLAGS="$CPPFLAGS -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers" ;;
  *) ;;
esac


AC_CONFIG_FILES([
	gtkballs.lsm
	Makefile
	src/Makefile
	doc/Makefile
	doc/gtkballs.6x
	po/Makefile.in 
	gtkballs.spec
	gtkballs-data/Makefile
])
AC_OUTPUT
