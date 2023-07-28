DESTDIR=
DST=$(DESTDIR)/usr/share/fks/templates

build:
	true

clean:
	true

install:
	rm -rf $(DST)/*
	mkdir -p $(DST)
	cp -r templates/* -t $(DST)/
