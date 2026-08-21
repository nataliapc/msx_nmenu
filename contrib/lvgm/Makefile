.PHONY: all clean lvgm clean

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

ROOTDIR = .
LIBDIR = $(ROOTDIR)/libs
INCDIR = $(ROOTDIR)/includes
SRCDIR = $(ROOTDIR)/src
OBJDIR = $(ROOTDIR)/obj
ROOTLIBDIR = ../../libs
ROOTINCDIR = ../../includes
LIB_GUARD=@mkdir -p $(LIBDIR)
DIR_GUARD=@mkdir -p $(OBJDIR)

AS = $(DOCKER_RUN) sdasz80
AR = $(DOCKER_RUN) sdar
CC = $(DOCKER_RUN) sdcc
HEX2BIN = hex2bin
MAKE = make -s --no-print-directory

FULLOPT :=  --max-allocs-per-node 200000
LDFLAGS = -rc
OPFLAGS = --std-sdcc2x --less-pedantic --opt-code-size -pragma-define:CRT_ENABLE_STDIO=0
WRFLAGS = --disable-warning 196 --disable-warning 84
CCFLAGS = --code-loc 0x0180 --data-loc 0 -mz80 --no-std-crt0 --out-fmt-ihx $(OPFLAGS) $(WRFLAGS) $(DEFINES) $(DEBUG)


all: lvgm


##################################### msx2ansi

lvgm: $(ROOTLIBDIR)/lvgm.lib

$(OBJDIR)/%.c.rel: $(SRCDIR)/%.c
	@echo "$(COL_BLUE)#### CC $@$(COL_RESET)"
	@$(DIR_GUARD)
	@$(CC) $(CCFLAGS) $(FULLOPT) -I$(INCDIR) -c -o $@ $^ ;

$(ROOTLIBDIR)/lvgm.lib: $(LIBDIR)/lvgm.lib
	@echo "$(COL_YELLOW)#### Copying $@$(COL_RESET)"
	@$(LIB_GUARD)
	@cp $(LIBDIR)/lvgm.lib $(ROOTLIBDIR)/

$(LIBDIR)/lvgm.lib: $(patsubst $(SRCDIR)/%, $(OBJDIR)/%.rel, $(wildcard $(SRCDIR)/*.c))
	@echo "$(COL_WHITE)######## Creating $@$(COL_RESET)"
	@$(LIB_GUARD)
	@$(AR) $(LDFLAGS) $@ $^ ;


clean:
	@echo "$(COL_ORANGE)#### Cleaning lvgm$(COL_RESET)"
	@rm -f $(ROOTLIBDIR)/lvgm.lib
	@rm -rf $(LIBDIR) $(OBJDIR)
