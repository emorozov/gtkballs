EXTRA_DIST=gtkballs-logo.png gtkballs_16x16.png themes

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/gtkballs
	$(INSTALL_DATA) *.png $(DESTDIR)$(datadir)/gtkballs
	$(mkinstalldirs) $(DESTDIR)$(datadir)/gtkballs/themes
	for d in themes/*; do \
	    $(mkinstalldirs) $(DESTDIR)$(datadir)/gtkballs/$$d; \
	    for i in $$d/*; do \
		$(INSTALL_DATA) $$i $(DESTDIR)$(datadir)/gtkballs/$$d; \
	    done; \
	done
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)

uninstall-local:
	rm -rf $(DESTDIR)$(datadir)/gtkballs
