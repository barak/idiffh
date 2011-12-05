all: idiffh
all: separate

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

clean:
	rm -f idiffh

.PHONY: all install clean
