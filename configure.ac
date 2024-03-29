#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtkballs],[3.1.5],[])
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf-m4])
AM_INIT_AUTOMAKE([-Wall foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADER(config.h)
AM_GNU_GETTEXT([external])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

#L#T_INIT

# Checks for programs.
AC_PROG_CC
AC_CHECK_HEADERS([sys/param.h signal.h])

#========================================================================

GTK3_CHECK="gtk+-3.0 >= 3.0.0"
GTK2_CHECK="gtk+-2.0 >= 2.14.0"

AC_ARG_ENABLE(gtk3,
   AS_HELP_STRING([--disable-gtk3],[build with GTK3 (Autodetect)]),[],
   [enable_gtk3=check])

AC_ARG_ENABLE(gtk2,
   AS_HELP_STRING([--enable-gtk2],[build with GTK2 (Autodetect)]),[],
   [enable_gtk2=check])

AS_IF([test "x$enable_gtk3" = xyes],
   [enable_gtk2=no])
AS_IF([test "x$enable_gtk2" = xyes],
   [enable_gtk3=no])

AS_IF([test "x$enable_gtk3" = xcheck],
   [PKG_CHECK_MODULES([GTK],[$GTK3_CHECK],
      [enable_gtk3=yes],[enable_gtk3=no])])

AS_IF([test "x$enable_gtk3" = xyes],
   [
   gtk_modules="$GTK3_CHECK"
   gtk_version="gtk+-3.0"
   errmsg="GTK3 is not installed, use --enable-gtk2 to build with GTK2"
   ],
   [
   gtk_modules="$GTK2_CHECK"
   gtk_version="gtk+-2.0"
   errmsg="GTK2 is not installed, use --enable-gtk3 to build with GTK3"
   CPPFLAGS="$CPPFLAGS -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_DEPRECATED -DGTK_DISABLE_SINGLE_INCLUDES"
   ])

PKG_CHECK_MODULES([GTK], [$gtk_modules], [], [AC_ERROR([$errmsg])])
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

#========================================================================

GETTEXT_PACKAGE=gtkballs
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

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
	po/Makefile
	gtkballs.spec
	gtkballs-data/Makefile
])
AC_OUTPUT

echo
echo "GTK Version: $(pkg-config --modversion $gtk_version)"
echo
