This is the README for gtkballs.


## Requirements

 - GTK 2 >= 2.14 / GTK 3

## Compiling

Run

- ./configure --prefix=/usr
- make

By default the GTK3 port is compiled, use `--enable-gtk2`
or `--disable-gtk3` to compile the GTK2 port.

- ./configure --prefix=/usr --enable-gtk3

The GTK3 version provides HiDPI support with:

- `GDK_SCALE=2 ./gtkballs` - make UI 2x bigger
- `GDK_SCALE=3 ./gtkballs` - make UI 3x bigger

## Installing

```
$ make install
```

## Bugs

If you find a bug, tell me what you did (or fix it and send a
patch).

## Development

You can participate in development of GtkBalls by sending patches or pull requests.

## Contacts
drF_ckoff <dfo@antex.ru>

Eugene Morozov <gtkballs@emorozov.net>

GtkBalls homepage: http://gtkballs.antex.ru
