all: idiffh
all: separate
all: rebuild

CFLAGS+=-O2
CFLAGS+=-Wall -pedantic
CFLAGS+=-std=c99
CPPFLAGS+=-DUNIX
CPPFLAGS+=-DFGETLN
LOADLIBES+=-lbsd

INSTALL=install
INSTALL_PROGRAM=$(INSTALL) -m0755

prefix=/usr/local
bindir=$(prefix)/bin

install: all
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) idiffh $(DESTDIR)$(bindir)/
	$(INSTALL_PROGRAM) separate $(DESTDIR)$(bindir)/
	$(INSTALL_PROGRAM) rebuild $(DESTDIR)$(bindir)/

clean:
	rm -f idiffh
	rm -f separate
	rm -f rebuild

.PHONY: all install clean
