include ../common.mk
ROOT=..
BUILD_LATEX_DIR=$(BUILD)/slides
LATEX_SRC=$(wildcard *.tex)
LATEX_OPTS=-shell-escape --output-directory=.
LATEX_OBJ=$(patsubst %.tex,$(BUILD_LATEX_DIR)/%.pdf,$(LATEX_SRC))
INCLUDED_OBJ_FILES=\
	$(wildcard $(BUILD)/images/*.png) \
	$(wildcard $(BUILD)/images/*.eps)
BIB_SRC=gpn22.bib
BIB_COPIED=$(patsubst %,$(BUILD_LATEX_DIR)/%,$(BIB_SRC))
all: $(BUILD_LATEX_DIR) $(LATEX_OBJ)

$(BUILD_LATEX_DIR):
	mkdir -p $@

$(BUILD_LATEX_DIR)/%: %
	cp -pf $< $@

$(BUILD_LATEX_DIR)/%: ../bib/%
	cp -pf $< $@

$(BUILD_LATEX_DIR)/%.tex: %.tex
	cp -pf $< $@

$(BUILD_LATEX_DIR)/%.pdf: $(BUILD_LATEX_DIR)/%.tex $(INCLUDED_OBJ_FILES) $(BIB_COPIED)
	(cd $(BUILD_LATEX_DIR); lualatex $(LATEX_OPTS) $(<F)) # create .aux
	(cd $(BUILD_LATEX_DIR); biber `basename "$(<F)" .tex`) # use .aux for bibliography
	(cd $(BUILD_LATEX_DIR); lualatex $(LATEX_OPTS) $(<F)) # recompile with bib
	(cd $(BUILD_LATEX_DIR); lualatex $(LATEX_OPTS) $(<F)) # recompile with bib
	(cd $(BUILD_LATEX_DIR); lualatex $(LATEX_OPTS) $(<F)) # recompile with bib

objclean:
	rm -rf $(BUILD_LATEX_DIR)

clean: objclean

bkpclean:
	rm -f *~

distclean: clean bkpclean

.PHONY: all clean

#  Local Variables:
#    coding:utf-8
#    mode:Makefile
#  End:
