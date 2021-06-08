.PHONY = all

PDFs = NA01.pdf NA02.pdf NA03.pdf NSD01.pdf
TARGETS = $(PDFs)

all: $(TARGETS)

%.pdf: %.md
	@echo Compiling report $^ into $@
	@pandoc $^ -o $@