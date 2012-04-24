all: test
.PHONY: all test

test:  a12
test:  b12
test:  c12
test:  d12


%.tok: %
	../separate <$< >$@

 a1.patch: a.tok  a1.tok
	../idiffh $^ > $@ || true

 a2.patch:  a.tok  a2.tok
	../idiffh $^ > $@ || true

 b1.patch: b.tok  b1.tok
	../idiffh $^ > $@ || true

 b2.patch:  b.tok  b2.tok
	../idiffh $^ > $@ || true

 c1.patch: c.tok  c1.tok
	../idiffh $^ > $@ || true

 c2.patch:  c.tok  c2.tok
	../idiffh $^ > $@ || true

 d1.patch: d.tok  d1.tok
	../idiffh $^ > $@ || true

 d2.patch:  d.tok  d2.tok
	../idiffh $^ > $@ || true


 a12.tok:  a2.tok  a1.patch
	patch -l  a2.tok -i  a1.patch -o $@

 b12.tok:  b2.tok  b1.patch
	patch -l  b2.tok -i  b1.patch -o $@

 c12.tok:  c2.tok  c1.patch
	patch -l  c2.tok -i  c1.patch -o $@

 d12.tok:  d2.tok  d1.patch
	patch -l  d2.tok -i  d1.patch -o $@


 a12:  a12.tok
	../rebuild < $< > $@

 b12:  b12.tok
	../rebuild < $< > $@

 c12:  c12.tok
	../rebuild < $< > $@

 d12:  d12.tok
	../rebuild < $< > $@

