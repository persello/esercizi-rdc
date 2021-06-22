PDFs = NA01.pdf NA02.pdf NA03.pdf NA04.pdf NSD01.pdf
TARGETS = $(PDFs)

.PHONY: all clean

all: $(TARGETS)

%.pdf: %.md
	@echo Compiling report $^ into $@
	pandoc $^ -o $@

clean: $(EXERCISESDIRS)
	rm -rf *.pdf
