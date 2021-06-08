PDFs = NA01.pdf NA02.pdf NA03.pdf NSD01.pdf
TARGETS = $(PDFs)
EXERCISESDIRS := $(wildcard NSD01/*)

.PHONY: all exercises $(EXERCISESDIRS) clean

all: $(TARGETS) exercises

%.pdf: %.md
	@echo Compiling report $^ into $@
	pandoc $^ -o $@

# Building
exercises: $(EXERCISESDIRS)
$(EXERCISESDIRS):
ifneq ($(MAKECMDGOALS),clean)
	@echo Compiling exercise $@
	$(MAKE) -C $@
else
	@echo Cleaning exercise $@
	$(MAKE) -C $@ clean
endif

clean: $(EXERCISESDIRS)
	rm -rf *.pdf
