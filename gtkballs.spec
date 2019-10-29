Name: gtkballs
Version: 3.1.5
Release: alt1

Summary: A simple logic game
License: GPL
Group: Games/Puzzles

Url: http://gtkballs.antex.ru
Source0: %name-%version.tar.gz

Packager: Sergey Pinaev <dfo@altlinux.ru>

Summary(fr): Un simple jeu de logique
Summary(ru_RU.KOI8-R): Простая и увлекательная логическая игра
Summary(uk_UA.KOI8-U): Проста та ц╕кава лог╕чна гра

# Automatically added by buildreq on Fri Jan 23 2004
BuildRequires: XFree86-libs fontconfig freetype2 glib2-devel libatk-devel libgtk+2-devel libpango-devel pkgconfig

%description
GtkBalls is a simple logic game. The goal of
the game is to make the maximum number of lines
with balls of the same color. A line is made of
five balls. Each time you don't do a line, extra
balls appear on the grid. You loose when the
grid is full.

%description -l fr
GtkBalls est un simple jeu de logique. Le but du
jeu est de faire le nombre maximum de lignes avec
des balles de la mЙme couleur. Une ligne est
constituИe de cinq balles. A chaque fois que vous
ne faites pas une ligne, des balles suplИmentaires
apparaissent sur la grille. Vous perdez lorsque
la grille est pleine.

%description -l ru_RU.KOI8-R
GtkBalls -- это простая логическая игра.  Цель
игры -- составлять линии максимальной длины из
шариков одинакового цвета.  Минимальная длина линии
за которую дают очки -- 5 шариков.  Если вы не
составили линию за ход, то на следующем ходу
появляются новые шарики.  Игра заканчивается,
когда на поле не остается места для новых шариков.

%description -l uk_UA.KOI8-U
GtkBalls -- проста лог╕чна гра, ц╕ль яко╖ -- 
складати рядки максимально╖ довжини з кульок одного
кольору.  М╕н╕мальна довжина л╕н╕╖, за яку надаються
бали -- 5 кульок.  Якщо ви не склали л╕н╕ю за х╕д,
то перед наступним ходом з'являться нов╕ кульки.
Гра зак╕нчу╓ться, коли не залиша╓ться м╕сця для
нових кульок.

%prep
%setup -q

%build
unset CC CXX
#export CFLAGS="$RPM_OPT_FLAGS"
%configure \
	--datadir=%_datadir \
	--localstatedir=%_localstatedir/games \
	--bindir=%_gamesbindir
%make_build

%install
%makeinstall \
	datadir=%buildroot%_datadir \
	localstatedir=%buildroot%_localstatedir/games \
	bindir=%buildroot%_gamesbindir
%__mkdir_p %buildroot%_menudir
%__cat << EOF >%buildroot%_menudir/%name
?package(%name):command="%_gamesbindir/%name" icon="%name.xpm" \
needs="x11" section="Amusement/Puzzles" title="GTK Balls" \
longtitle="A simple logic game"
EOF

%find_lang %name

%files -f %name.lang
%attr(2711, root, games)%_gamesbindir/*
#%attr(-, games, games) %_localstatedir/*/*
%_datadir/%name
%_menudir/%name
%_man6dir/*
#%_miconsdir/*.xpm
#%_iconsdir/*.xpm

%post
if [ ! -f %_localstatedir/games/gtkballs-scores ]; then
	touch %_localstatedir/games/gtkballs-scores
fi
%__chgrp games %_localstatedir/games/gtkballs-scores
%__chmod 0664 %_localstatedir/games/gtkballs-scores
%update_menus

%postun
%clean_menus

%changelog
* Tue Nov 16 2004 drF_ckoff <dfo@altlinux.ru> 3.1.5-alt1
- New version

* Tue Oct 26 2004 drF_ckoff <dfo@altlinux.ru> 3.1.4-alt1
- New version

* Fri Jul  2 2004 drF_ckoff <dfo@altlinux.ru> 3.1.3-alt2
- Menu entry changed from Amusement/Strategy to Amusement/Puzzles

* Wed May 26 2004 drF_ckoff <dfo@altlinux.ru> 3.1.3-alt1
- New version

* Fri Feb  6 2004 drF_ckoff <dfo@altlinux.ru> 3.1.2-alt1
- New version

* Fri Feb  6 2004 drF_ckoff <dfo@altlinux.ru> 3.1.1-alt1
- New version

* Mon Feb  2 2004 drF_ckoff <dfo@altlinux.ru> 3.1.0-alt5
- build fixes

* Fri Jan 30 2004 drF_ckoff <dfo@altlinux.ru> 3.1.0-alt4
- hasher build fixes

* Thu Jan 29 2004 drF_ckoff <dfo@altlinux.ru> 3.1.0-alt3
- changed @antex.ru to @altlinux.ru to make stupid hasher happy

* Wed Jan 28 2004 drF_ckoff <dfo@antex.ru> 3.1.0-alt2
- small compilation and .spec fixed

* Thu Jan 22 2004 drF_ckoff <dfo@antex.ru> 3.1.0-alt1
- 3.1.0
- fix scorefile installation

* Mon Feb 11 2002 Rider <rider@altlinux.ru> 2.1.1-alt1
- 2.1.1
- specfile cleanup

* Sun Oct 14 2001 Rider <rider@altlinux.ru> 2.0-alt1
- first build for ALT Linux

* Wed Sep  1 1999 Eugene Morozov <jmv@lucifer.dorms.spbu.ru>
- Changed my Email address because I check yahoo mail seldom.
  Added installation of gnome icon.
  Fixed several bugs in the spec file.

