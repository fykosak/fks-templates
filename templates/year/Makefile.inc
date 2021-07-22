# Common makefile parts
# vim: syn=make

include ../Makefile.conf
include ../Deadline.inc
include ../Deadline_xml.inc

# == Graphics part ==

# -- Folders --
DEPS_FOLDERS = $(GRAPHICS)/color $(PGRAPHICS)/color

$(DEPS_FOLDERS):
	mkdir -p $@

# -- Metapost figures --
METAPOST_EPS = mpost -jobname=$(subst .src.mp,,$(<F)) -s 'outputtemplate="%j.%o"' $(<F)
METAPOST_SVG = mpost -jobname=$(subst .src.mp,,$(<F)) -s 'outputtemplate="%j.%o"' -s 'outputformat="svg"' $(<F)

%.eps: %.src.mp
	cd $(@D); $(METAPOST_EPS)
%.svg: %.src.mp
	cd $(@D); $(METAPOST_SVG)

# -- Gnuplot figures --
GNUPLOT = gnuplot -e "set format '$$\"%g\"$$'; set terminal epslatex monochrome\
	size 12.7cm,7.7cm; set output '$(patsubst %.eps,%.tex,$(@F))'" $(<F)
GNUPLOT_COLOR = $(subst output ', output 'color/, $(subst monochrome,color,$(GNUPLOT)))

# gnuplot with datafiles
%.eps %.tex: %.src.plt %.src.dat
	cd $(<D); $(GNUPLOT)
$(PGRAPHICS)/color/%.eps $(PGRAPHICS)/color/%.tex: $(PGRAPHICS)/%.src.plt $(PGRAPHICS)/%.src.dat | $(PGRAPHICS)/color
	cd $(<D); $(GNUPLOT_COLOR)
$(GRAPHICS)/color/%.eps $(GRAPHICS)/color/%.tex: $(GRAPHICS)/%.src.plt $(GRAPHICS)/%.src.dat | $(GRAPHICS)/color
	cd $(<D); $(GNUPLOT_COLOR)

# gnuplot without datafiles or with differently named datafiles
%.eps %.tex: %.src.plt
	cd $(<D); $(GNUPLOT)
$(PGRAPHICS)/color/%.eps $(PGRAPHICS)/color/%.tex: $(PGRAPHICS)/%.src.plt | $(PGRAPHICS)/color
	cd $(<D); $(GNUPLOT_COLOR)
$(GRAPHICS)/color/%.eps $(GRAPHICS)/color/%.tex: $(GRAPHICS)/%.src.plt | $(GRAPHICS)/color
	cd $(<D); $(GNUPLOT_COLOR)

# -- IPE figures --
%.eps: %.src.ipe
	iperender -eps $< $@
%.svg: %.src.ipe
	iperender -svg $< $@
%.pdf: %.src.ipe
	iperender -pdf $< $@

# -- SVG figures --
#INKSCAPE_PNG=inkscape -w 1024 $< -o $@ # Inkscape 1.0 version
INKSCAPE_PNG=inkscape -z -w 1024 $< --export-png=$@ # Inkscape 0.9* version

%.eps: %.src.svg
	inkscape $< -E $@ --export-ignore-filters # Inkscape 0.9* version
#	inkscape $< -o $@ # Inkscape 1.0 version
%.pdf: %.src.svg
	inkscape $< --export-pdf=$@ # Inkscape 0.9* version
#	inkscape $< -o $@ # Inkscape 1.0 version
%.png: %.src.svg
	$(INKSCAPE_PNG)
%.png: %.svg # generic rule to export png from exported svg, for metapost and ipe
	$(INKSCAPE_PNG)

# -- EPS figures --
INKSCAPE_SVG = inkscape --export-plain-svg=$@ $< 
CONVERT = convert -density 1200 -geometry 1024 $< $@

%.pdf: %.src.eps
	epstopdf $< --outfile=$@
%.pdf: %.eps
	epstopdf $< --outfile=$@
%.svg: %.src.eps
	$(INKSCAPE_SVG)
%.png: %.src.eps
	$(CONVERT)

# -- PDF figures --
%.svg: %.src.pdf
	$(INKSCAPE_SVG)
%.png: %.src.pdf
	$(CONVERT)

# -- direct output --
COPY = cp $< $@

%.eps: %.src.eps
	$(COPY)
%.svg: %.src.svg
	$(COPY)
%.pdf: %.src.pdf
	$(COPY)
%.png: %.src.png
	$(COPY)
%.jpg: %.src.jpg
	$(COPY)
%.jpeg: %.src.jpeg
	$(COPY)


# == Graphics dependence ==
# - args:
# 		$(1) - graphics base folder
# 		$(2) - filename search pattern
# 		$(3) - source file extension
# 		$(4) - output file extension list (space devided)
# 		$(5) - output subfolder (with leading /, optional)
# - function: find source files and print expected output file names
define find-graphics-deps
	$(foreach ext,$(4),$(patsubst $(1)/%.src.$(3), $(1)/$(5)%.$(ext),\
		$(wildcard $(1)/*$(2)*.src.$(3))))
endef

# - args:
# 		$(1) - graphics base folder
# 		$(2) - filename search pattern
# - function: find all dependent graphic files for problem
# - rules: preferably export everything to pdf (for tex), svg and png (both for web)
# 		exceptions: export gnuplot to tex+eps; keep raster img format
graphics-deps = \
	$(call find-graphics-deps,$(1),$(2),mp, pdf svg png)\
	$(call find-graphics-deps,$(1),$(2),plt,tex eps)\
	$(call find-graphics-deps,$(1),$(2),plt,tex eps,color/)\
	$(call find-graphics-deps,$(1),$(2),ipe,pdf svg png)\
	$(call find-graphics-deps,$(1),$(2),svg,pdf svg png)\
	$(call find-graphics-deps,$(1),$(2),eps,pdf svg png)\
	$(call find-graphics-deps,$(1),$(2),pdf,pdf svg png)\
	$(call find-graphics-deps,$(1),$(2),png,png)\
	$(call find-graphics-deps,$(1),$(2),jpg,jpg)\
	$(call find-graphics-deps,$(1),$(2),jpeg,jpeg)

# generate dependencies only needed by TeX
graphics-deps-tex = \
	$(call find-graphics-deps,$(1),$(2),mp, pdf)\
	$(call find-graphics-deps,$(1),$(2),plt,tex eps)\
	$(call find-graphics-deps,$(1),$(2),plt,tex eps,color/)\
	$(call find-graphics-deps,$(1),$(2),ipe,pdf)\
	$(call find-graphics-deps,$(1),$(2),svg,pdf)\
	$(call find-graphics-deps,$(1),$(2),eps,pdf)\
	$(call find-graphics-deps,$(1),$(2),pdf,pdf)\
	$(call find-graphics-deps,$(1),$(2),png,png)\
	$(call find-graphics-deps,$(1),$(2),jpg,jpg)\
	$(call find-graphics-deps,$(1),$(2),jpeg,jpeg)


# == Data retrieval ==
GLOB_DATA := ../data
# credentials are at least obfuscated so they aren't easily grep'able from logs
SOAP_REQUEST = $(SOAPCLIENT) $(WS_URL) $(1) $(2) $(shell echo -e $(WS_USERNAME) | base64) $(shell echo -e $(WS_PASSWORD) | base64)

# vysledky?-% is implicit rule, results can't be also imlicit, so mention it explicitly
# see: https://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
$(GLOB_DATA)/results.xml: $(GLOB_DATA)/resultsRequest.soap
$(GLOB_DATA)/stats.xml: $(GLOB_DATA)/statsRequest.soap

$(GLOB_DATA)/%.xml: $(GLOB_DATA)/%Request.soap
	$(call SOAP_REQUEST,$<,$@)

.PRECIOUS: $(GLOB_DATA)/*.xml $(GLOB_DATA)/results.xml


# == Data processing ==
SIGNATURES := ../signatures.tex
$(SIGNATURES): $(GLOB_DATA)/signatures.xml $(SIGNATURES_STYLE)
	xsltproc $(XSL_SIGNATURES) $< >$@

$(RESULTS)/vysledky%.tex: $(GLOB_DATA)/results.xml $(XSL_RESULTS)
	xsltproc --stringparam type brojure \
		--stringparam category $(word 2,$(subst -, ,$(subst .tex,,$(subst $(RESULTS)/vysledky,,$@)))) \
		--stringparam series   $(word 1,$(subst -, ,$(subst .tex,,$(subst $(RESULTS)/vysledky,,$@)))) \
		$(XSL_RESULTS) $< >$@

$(RESULTS)/statistiky%.csv: $(GLOB_DATA)/stats.xml $(XSL_STATS)
	xsltproc --stringparam series $* \
		$(XSL_STATS) $< >$@
	# apply created results to statistics
	$(STATS_SH) $(PROBLEMS) $@

$(RESULTS)/%.tex: $(RESULTS)/%.csv
	iconv -f latin2 -t utf8 $< | \
	cut -d\; -f1,2,3,4,5,6,7,8,9,10,12,13,14 | \
	sed '/^$$/d;/^%/d;s/;;/;--;/g;s/;;/;--;/g;s/;/ \& /g;s/$$/\\\\/' | sed -e '2i\\\\midrule'  > $@


# == Deadlines ==
../Deadline.inc: ../Deadline_xml.inc
	echo "#" > $@
	echo "# Automaticky generovaný" >> $@
	sed -e 's/DEADLINE_XML\[\([0-9]*\)\]=\([0-9]*\)-[0]*\([1-9][0-9]*\)-[0]*\([1-9][0-9]*\)T[0]*\([1-9][0-9]*\):\([0-9]*\):\([0-9]*\)/DEADLINE\[\1\]=\4.~\3.~\2 \5.\6/;s/DEADLINES_XML\[\([0-9]*\)\]=\([0-9]*\)-[0]*\([1-9][0-9]*\)-[0]*\([1-9][0-9]*\)/DEADLINES\[\1\]=\4.~\3.~\2/' $< >> $@

# == Makefile configuration ==
../Makefile.conf:
	cp ../Makefile.conf.sample ../Makefile.conf

# == TEX to PDF compiler ==
define XELATEX
	xelatex -output-directory $(@D) -jobname '$(basename $(@F))' -no-pdf $(1)\
	&& (\
		grep -i "Package rerunfilecheck Warning: File .* has changed.\|There were undefined references.\|Rerun to get cross-references right." $(basename $@).log && (\
			xelatex -output-directory $(@D) -jobname '$(basename $(@F))' $(1)\
		) || (\
			xdvipdfmx -E $(basename $@).xdv -o $(basename $@).pdf\
		)\
	) || false
endef

# == Author solutions ==
.SECONDEXPANSION:
$(OUT)/reseni%.pdf: $(PROBLEMS)/problem%.tex $$(call graphics-deps,$(PGRAPHICS),$$*) $(SIGNATURES)
	$(call XELATEX,'\documentclass[SEM]{fkssolution}\setcounter{year}{YEAR}\input{$<}\begin{document}\problemsolutionsingle\makefooter\end{document}')
