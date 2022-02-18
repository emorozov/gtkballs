include ../config.mk

APP  = gtkballs
# ls *.c | sed 's%\.c%.o%g' | tr '\n' ' ' >> Makefile
OBJS = about.o game.o gfx.o gtkballs.o gtkutils.o halloffame.o inputname.o mainmenu.o mainwin.o path.o preferences.o prefs.o rules.o rulesdialog.o savedialog.o savegame.o scoreboard.o theme.o themerc.o 

#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: $(APP)
	mkdir -p $(DESTDIR)$(bindir)
	install $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)
