SV_SOURCES=$(shell find qcsrc/server qcsrc/client-server qcsrc/common qcsrc/menu-server -type f -regextype posix-egrep -regex '.*\.(qc|qh|src)')
CL_SOURCES=$(shell find qcsrc/client qcsrc/client-server qcsrc/common qcsrc/client-menu -type f -regextype posix-egrep -regex '.*\.(qc|qh|src|c)')
MENU_SOURCES=$(shell find qcsrc/menu qcsrc/common qcsrc/menu-server qcsrc/client-menu -type f -regextype posix-egrep -regex '.*\.(qc|qh|src|c|inc)')
CFG_SOURSES=$(shell ls cfg/*.in)
SET_CURL_PACKAGE=yes
QCC=rmqcc
QCCFLAGS=-O3
MOD_NAME=1vs1
SV_PROGNAME=$(MOD_NAME).dat
CL_PROGNAME=cl_$(MOD_NAME).dat
CFG_NAME=$(MOD_NAME).cfg
PK3_NAME=cl_$(MOD_NAME)

all: $(SV_PROGNAME) $(CL_PROGNAME) menu.dat cl.pk3 $(CFG_NAME)

$(SV_PROGNAME) : $(SV_SOURCES)
	cd qcsrc/server && $(QCC) $(QCCFLAGS)
	mv -f qcsrc/server/progs.dat $(SV_PROGNAME)

$(CL_PROGNAME) : $(CL_SOURCES)
	cd qcsrc/client && $(QCC) $(QCCFLAGS)
	mv -f qcsrc/client/csprogs.dat $(CL_PROGNAME)

menu.dat : $(MENU_SOURCES)
	cd qcsrc/menu && $(QCC) $(QCCFLAGS)
	mv -f qcsrc/menu/menu.dat menu.dat

$(CFG_NAME) : $(CFG_SOURSES) Makefile
	echo sv_progs $(SV_PROGNAME) > $(CFG_NAME).tmp
	echo csqc_progname $(CL_PROGNAME) >> $(CFG_NAME).tmp
	if test "$(SET_CURL_PACKAGE)" = yes; then echo sv_curl_serverpackages $(CL_PROGNAME) >> $(CFG_NAME).tmp; else echo -n; fi
	for F in $(CFG_SOURSES); do echo "//begin of $$F"; cat $$F; echo "//end of $$F"; done >> $(CFG_NAME).tmp
	mv -f $(CFG_NAME).tmp $(CFG_NAME)

.PHONY: cl.pk3

cl.pk3 : $(CL_PROGNAME)
	HASH=`md5sum $(CL_PROGNAME) | head -c 8`; DATE=`stat -c %y $(CL_PROGNAME) | head -c 19 | sed 's/[ :-]//g'`; NAME="$(PK3_NAME)-$$DATE$$HASH.pk3";\
if test ! -f "$$NAME" -o $(CL_PROGNAME) -nt "$$NAME"; then rm -f $(PK3_NAME)*.pk3; REXEXT_NAME="rexext_$$DATE$$HASH.cfg"; echo // > $$REXEXT_NAME; zip "$$NAME" $(CL_PROGNAME) $$REXEXT_NAME; rm -f $$REXEXT_NAME; fi
