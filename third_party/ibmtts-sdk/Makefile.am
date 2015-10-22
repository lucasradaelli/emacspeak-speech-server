SUBDIRS = . docs pyibmtts

AUTOMAKE_OPTIONS = gnu
lib_LTLIBRARIES = libibmeci.la

libibmeci_la_SOURCES = ibmeci.c

include_HEADERS = eci.h

libibmeci_la_CFLAGS = -g -Wall  -fno-builtin

libibmeci_la_DEPENDENCIES = libibmeci.so

libibmeci.so: libibmeci_la-ibmeci.o
	ld -shared -o $@ --version-script Version -h $@ $<

install-data-am: libibmeci.so ibmtts.pc
	$(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) libibmeci.so $(DESTDIR)$(libdir)
	$(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) eci.h $(DESTDIR)$(includedir)
	$(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) -D ibmtts.pc $(DESTDIR)$(datadir)/pkgconfig/ibmtts.pc
	$(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) -D NEWS $(DESTDIR)$(datadir)/doc/NEWS
