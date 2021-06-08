.PHONY = all

TARGETS = NA01.pdf NA02.pdf NA03.pdf

all: $(TARGETS)

%.pdf: %.md
	@echo Compiling report $^ into $@
	@pandoc $^ -o $@