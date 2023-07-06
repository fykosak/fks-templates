# Makefile for single batch

BATCHNO=B
SOLVEDBATCHNO=AB
RESULTSLIST=resultslist
OUT=out
PROBLEMS=../problems
GRAPHICS=graphics# graphics for the batch (series)
PGRAPHICS=$(PROBLEMS)/graphics# graphics for problems
RESULTS=results

ARG_CS=\def\mdeadline{${DEADLINE_CS[${BATCHNO}]}}\def\mdeadlineS{${DEADLINE_CS_POST[${BATCHNO}]}}
ARG_EN=\def\mdeadline{${DEADLINE_EN[${BATCHNO}]}}\def\mdeadlineS{${DEADLINE_EN_POST[${BATCHNO}]}}
ARG_XML=\def\mdeadline{${DEADLINE_XML[${BATCHNO}]}}\def\mdeadlineS{${DEADLINES_XML[${BATCHNO}]}}

.SUFFIXES:
.PHONY: all clean cleanall brojure web book print solutions problems series

# == Main targets ==
all: brojure web print solutions series
include ../Makefile.inc
brojure: $(OUT)/serie$(BATCHNO).pdf $(OUT)/zadani$(BATCHNO).pdf
web: $(OUT)/zadaniWeb$(BATCHNO)-new.xml
book: $(OUT)/serie$(BATCHNO)-book.pdf
print: $(OUT)/serie$(BATCHNO)-print.pdf
solutions: $(patsubst $(PROBLEMS)/problem%.tex, $(OUT)/solution%.cs.pdf, $(wildcard $(PROBLEMS)/problem$(BATCHNO)-*.tex))\
	$(patsubst $(PROBLEMS)/problem%.tex, $(OUT)/solution%.en.pdf, $(wildcard $(PROBLEMS)/problem$(BATCHNO)-*.tex))
series: $(OUT)/serial$(BATCHNO).pdf $(OUT)/serial$(BATCHNO)-en.pdf

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


$(OUT)/zadani$(BATCHNO).pdf: problems$(BATCHNO).tex $(DEPS_PROBS) ../Deadline.inc | $(OUT)
	$(call XELATEX,'\def\UseOption{web}$(ARG_CS)\input{$<}')
$(OUT)/serie$(BATCHNO).pdf: batch$(BATCHNO).tex $(SERIEDEP) | $(OUT)
	$(call XELATEX,'\def\UseOption{web}$(ARG_CS)\input{$<}')
$(OUT)/serie$(BATCHNO)-print.pdf: batch$(BATCHNO).tex $(SERIEDEP) | $(OUT)
	$(call XELATEX,'\def\classoptions{twoside}\def\UseOption{print}$(ARG_CS)\input{$<}')
$(OUT)/serie$(BATCHNO)-book.pdf: $(OUT)/serie$(BATCHNO)-print.pdf | $(OUT)
	pdfbook --booklet true --outfile '$@' $<
$(OUT)/serial$(BATCHNO).pdf: serial$(BATCHNO).tex $(DEPS_GRAPHICS_BATCH) $(SIGNATURES) | $(OUT)
	$(call XELATEX,'\documentclass[fykos]{fksseries}\setcounter{year}{37}\setcounter{batch}{$(BATCHNO)}\begin{document}\input{$<}\par\makefooter\end{document}')
$(OUT)/serial$(BATCHNO)-en.pdf: serial$(BATCHNO)-en.tex $(DEPS_GRAPHICS_BATCH) $(SIGNATURES) | $(OUT)
	$(call XELATEX,'\documentclass[fykos$(comma)english]{fksseries}\setcounter{year}{37}\setcounter{batch}{$(BATCHNO)}\begin{document}\input{$<}\par\makefooter\end{document}')

clean:
	find $(OUT) -type f -not \( -name "*.pdf" -o -name ".*" \) | xargs rm

cleanall:
	rm $(OUT)/*
#	ls $(GRAPHICS) $(PGRAPHICS) | grep -v '\(mp\|plt\|eps\|jpg\|png\)$$' | xargs rm


# == XML for web transport ==
define XML_DEFS_LEGACY
\def\batchno{$(BATCHNO)}\
\def\problemcount{8}\
$(ARG_CS)
endef

define XML_DEFS
\def\batchno{$(BATCHNO)}\
\def\problemcount{8}\
$(ARG_XML)
endef

$(OUT)/zadaniWeb$(BATCHNO)-new.xml: $(wildcard $(PROBLEMS)/problem$(BATCHNO)*) ../Deadline_xml.inc | $(OUT)
	xelatex -output-directory $(@D) -jobname $(basename $(@F)) '$(XML_DEFS)\documentclass[fykos]{fksbase}\setcounter{year}{37}\input{fks-web-xml}'
	fks-web-xml.sh $@ > $@.tmp
	cat $@.tmp | tr -d '\n' | sed 's/<\(.*\)>\s*<\/\1>//g' > $@
	rm -f $@.tmp
