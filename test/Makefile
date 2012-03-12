all: test
.PHONY: all test

test: a12
test: b12

%.tok: %
	../separate <$< >$@

a1.patch: a.tok a1.tok
	../idiffh $^ > $@ || true

a2.patch: a.tok a2.tok
	../idiffh $^ > $@ || true

b1.patch: b.tok b1.tok
	../idiffh $^ > $@ || true

b2.patch: b.tok b2.tok
	../idiffh $^ > $@ || true

a12.tok: a2.tok a1.patch
	patch -l a2.tok -i a1.patch -o $@

b12.tok: b2.tok b1.patch
	patch -l b2.tok -i b1.patch -o $@

a12: a12.tok
	../rebuild < $< > $@

b12: b12.tok
	../rebuild < $< > $@
