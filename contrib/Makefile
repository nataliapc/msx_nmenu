.PHONY: all clean lvgm msx2ansi clean_msx2ansi clean_lvgm

SDCC_VER := 4.5.0
DOCKER_IMG = nataliapc/sdcc:$(SDCC_VER)
DOCKER_RUN = docker run -i --rm -u $(shell id -u):$(shell id -g) -v .:/src -w /src $(DOCKER_IMG)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	COL_RED = \e[1;31m
	COL_YELLOW = \e[1;33m
	COL_ORANGE = \e[1;38:5:208m
	COL_BLUE = \e[1;34m
	COL_GRAY = \e[1;30m
	COL_WHITE = \e[1;37m
	COL_RESET = \e[0m
endif

ROOTDIR = ..
BINDIR = $(ROOTDIR)/bin
LIBDIR = $(ROOTDIR)/libs
INCDIR = $(ROOTDIR)/includes
DSKDIR = $(ROOTDIR)/dsk
EXTERNALS = $(ROOTDIR)/externals
LIB_GUARD=@mkdir -p $(LIBDIR)

MAKE = make -s --no-print-directory


all: lvgm msx2ansi


##################################### lvgm

lvgm: $(LIBDIR)/lvgm.lib

$(LIBDIR)/lvgm.lib:
	@echo "$(COL_YELLOW)######## Creating $@$(COL_RESET)"
	@$(MAKE) -C lvgm

##################################### msx2ansi

msx2ansi: $(LIBDIR)/msx2ansi.lib $(INCDIR)/msx2ansi.h

$(INCDIR)/msx2ansi.h:
	@echo "$(COL_YELLOW)######## Creating $@$(COL_RESET)"
	@cp msx2ansi/msx2ansi.h $(INCDIR)/msx2ansi.h

$(LIBDIR)/msx2ansi.lib: msx2ansi/msx2ansi.h
	@echo "$(COL_YELLOW)######## Creating $@$(COL_RESET)"
	@$(LIB_GUARD)
	@cp msx2ansi/msx2ansi.lib $(LIBDIR)/msx2ansi.lib

##################################### clean

clean: clean_msx2ansi clean_lvgm

clean_msx2ansi:
	@echo "$(COL_ORANGE)#### Cleaning $@$(COL_RESET)"
	@rm -f $(LIBDIR)/msx2ansi.lib
	@rm -f $(INCDIR)/msx2ansi.h

clean_lvgm:
	@echo "$(COL_ORANGE)#### Cleaning $@$(COL_RESET)"
	@$(MAKE) -C lvgm clean