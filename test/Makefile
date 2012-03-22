all: test
.PHONY: all test

SEPARATE=../separate
DIFF=../idiffh
PATCH=../patch-2.6/src/patch
REBUILD=../rebuild

test: a12
test: b12

%.tok: %
	$(SEPARATE) <$< >$@

a1.patch: a.tok a1.tok
	$(DIFF) $^ > $@ || true

a2.patch: a.tok a2.tok
	$(DIFF) $^ > $@ || true

b1.patch: b.tok b1.tok
	$(DIFF) $^ > $@ || true

b2.patch: b.tok b2.tok
	$(DIFF) $^ > $@ || true

a12.tok: a2.tok a1.patch
	$(PATCH) -l a2.tok -i a1.patch -o $@

b12.tok: b2.tok b1.patch
	$(PATCH) -l b2.tok -i b1.patch -o $@

a12: a12.tok
	$(REBUILD) < $< > $@

b12: b12.tok
	$(REBUILD) < $< > $@
