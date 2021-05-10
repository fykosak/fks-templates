# Makefile for single batch

BATCHNO=B
SOLVEDBATCHNO=AB
RESULTSLIST=resultslist
OUT=out
PROBLEMS=../problems
GRAPHICS=graphics# graphics for the batch (series)
PGRAPHICS=$(PROBLEMS)/graphics# graphics for problems
RESULTS=results

ARG=\def\mdeadline{${DEADLINE[${BATCHNO}]}}\def\mdeadlineS{${DEADLINES[${BATCHNO}]}}
ARG_XML=\def\mdeadline{${DEADLINE_XML[${BATCHNO}]}}\def\mdeadlineS{${DEADLINES_XML[${BATCHNO}]}}

.SUFFIXES:
.PHONY: all clean cleanall brojure web book print solutions problems series

# == Main targets ==
all: brojure web print solutions series
include ../Makefile.inc
brojure: $(OUT)/serie$(BATCHNO).pdf $(OUT)/zadani$(BATCHNO).pdf
web: $(OUT)/zadaniWeb$(BATCHNO)-cs.xml $(OUT)/zadaniWeb$(BATCHNO)-en.xml $(OUT)/zadaniWeb$(BATCHNO)-new.xml
book: $(OUT)/serie$(BATCHNO)-book.pdf
print: $(OUT)/serie$(BATCHNO)-print.pdf
solutions: $(patsubst $(PROBLEMS)/problem%.tex, $(OUT)/reseni%.pdf, $(wildcard $(PROBLEMS)/problem$(BATCHNO)-*.tex))
series: $(OUT)/serial$(BATCHNO).pdf

# == problem and graphics dependence ==
DEPS_PROBS= $(wildcard $(PROBLEMS)/problem$(BATCHNO)*) $(call graphics-deps,$(PGRAPHICS),$(BATCHNO)-)
DEPS_SOLVEDPROBS= $(foreach batchno,$(SOLVEDBATCHNO),$(wildcard $(PROBLEMS)/problem$(batchno)*) \
				  $(call graphics-deps,$(PGRAPHICS),$(batchno)-))
DEPS_GRAPHICS_BATCH = $(call graphics-deps,$(GRAPHICS))
DEPS_GRAPHICS_MANUAL=

# keep exported graphics of current batch
.SECONDARY: $(DEPS_PROBS) $(DEPS_SOLVEDPROBS) $(DEPS_GRAPHICS_BATCH) $(DEPS_GRAPHICS_MANUAL)

SERIEDEP=uvod$(BATCHNO).tex serial$(BATCHNO).tex $(SIGNATURES) \
	$(DEPS_PROBS) $(DEPS_SOLVEDPROBS) $(DEPS_GRAPHICS_BATCH) $(DEPS_GRAPHICS_MANUAL)\
	$(addprefix $(RESULTS)/vysledky, $(addsuffix .tex,$(foreach batchno,$(SOLVEDBATCHNO),$(addprefix $(batchno)-,$(RESULTSLIST)))))\
	$(foreach batchno,$(SOLVEDBATCHNO),$(addprefix $(addprefix $(RESULTS)/statistiky,$(batchno)),.csv))\
	../Deadline.inc


$(OUT)/zadani$(BATCHNO).pdf: problems$(BATCHNO).tex $(DEPS_PROBS) ../Deadline.inc
	$(call XELATEX,'\def\UseOption{web}$(ARG)\input{$<}')
$(OUT)/serie$(BATCHNO).pdf: batch$(BATCHNO).tex $(SERIEDEP)
	$(call XELATEX,'\def\UseOption{web}$(ARG)\input{$<}')
$(OUT)/serie$(BATCHNO)-print.pdf: batch$(BATCHNO).tex $(SERIEDEP)
	$(call XELATEX,'\def\classoptions{twoside}\def\UseOption{print}$(ARG)\input{$<}')
$(OUT)/serie$(BATCHNO)-book.pdf: $(OUT)/serie$(BATCHNO)-print.pdf
	pdfbook --booklet true --outfile '$@' $<
$(OUT)/serial$(BATCHNO).pdf: serial$(BATCHNO).tex $(DEPS_GRAPHICS_BATCH) $(SIGNATURES)
	$(call XELATEX,'\documentclass[SEM]{fksseries}\setcounter{year}{Y}\setcounter{batch}{$(BATCHNO)}\begin{document}\input{$<}\par\makefooter\end{document}')

clean:
	find $(OUT) -type f -not \( -name "*.pdf" -o -name ".*" \) | xargs rm

cleanall:
	rm $(OUT)/*
#	ls $(GRAPHICS) $(PGRAPHICS) | grep -v '\(mp\|plt\|eps\|jpg\|png\)$$' | xargs rm


# == XML for web transport ==
define XML_DEFS_LEGACY
\def\batchno{$(BATCHNO)}\
\def\problemcount{PB}\
$(ARG)
endef

define XML_DEFS
\def\batchno{$(BATCHNO)}\
\def\problemcount{PB}\
$(ARG_XML)
endef

$(OUT)/zadaniWeb$(BATCHNO)-cs.xml: $(PROBLEMS)/web.tex $(SERIEDEP)
	xelatex -output-directory $(@D) -jobname $(basename $(@F)) '$(XML_DEFS_LEGACY)\def\lang{cs}\input{$<}'
$(OUT)/zadaniWeb$(BATCHNO)-en.xml: $(PROBLEMS)/web.tex $(SERIEDEP)
	xelatex -output-directory $(@D) -jobname $(basename $(@F)) '$(XML_DEFS_LEGACY)\def\lang{en}\input{$<}'

$(OUT)/zadaniWeb$(BATCHNO)-new.xml: $(SERIEDEP)
	xelatex -output-directory $(@D) -jobname $(basename $(@F)) '$(XML_DEFS)\documentclass[SEM]{fksbase}\setcounter{year}{Y}\input{fks-web-xml}'
	fks-web-xml.sh $@ > $@.tmp
	cat $@.tmp | tr -d '\n' | sed 's/<\(.*\)>\s*<\/\1>//g' > $@
	rm -f $@.tmp
