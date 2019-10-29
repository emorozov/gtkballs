bin_PROGRAMS = gtkballs

LIBS = @GTK_LIBS@ @LIBS@
AM_CFLAGS = @CFLAGS@ @GTK_CFLAGS@ -DDATADIR=\"$(datadir)\" -DLOCALSTATEDIR=\"$(localstatedir)\" -DLOCALEDIR=\"$(nlsdir)\"
INCLUDES = -I../intl

gtkballs_SOURCES = gtkballs.c gtkballs.h gfx.c gfx.h \
		license.c license.h \
                path.c path.h preferences.c preferences.h \
                scoreboard.c scoreboard.h themerc.c themerc.h \
                gtkutils.c gtkutils.h mainmenu.c mainmenu.h \
                rules.c rules.h about.c about.h child.c child.h \
                theme.c theme.h savegame.c savegame.h \
                savedialog.c savedialog.h halloffame.c halloffame.h \
                inputname.c inputname.h rulesdialog.c rulesdialog.h \
                game.c game.h mainwin.c mainwin.h prefs.c prefs.h

install-exec-hook:
	if [ "${UID}" = 0 ]; then \
		chgrp games $(DESTDIR)$(bindir)/gtkballs; \
		chmod g+s $(DESTDIR)$(bindir)/gtkballs; \
	fi
