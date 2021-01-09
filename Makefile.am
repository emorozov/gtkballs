## Process this file with automake to produce Makefile.in

SUBDIRS = po doc src gtkballs-data

EXTRA_DIST = README.russian \
	     TODO \
	     gnome-gtkballs.png \
             gtkballs.lsm \
	     gtkballs.spec

appddir = $(datadir)/gtkballs
appd_DATA = COPYING

install-data-hook:
	if [ ! -f $(DESTDIR)$(localstatedir)/gtkballs-scores ]; then touch $(DESTDIR)$(localstatedir)/gtkballs-scores; fi
	if [ "${UID}" = 0 ]; then chgrp games $(DESTDIR)$(localstatedir)/gtkballs-scores; fi
	chmod 0664 $(DESTDIR)$(localstatedir)/gtkballs-scores

ACLOCAL_AMFLAGS = -I m4
