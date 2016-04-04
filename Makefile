PRODUCT= libfozzlib.a
# If PRODUCT= line is missing, be sure to insert one
# $(PRODUCT).c will be automatically compiled, so it
# doesn't need to be inserted below
CFILES= getargs.c math.c evalexpr.c gdate.c socket.c files.c strings.c shell.c ttlock.c database.c
OTHER_CFLAGS= -DFOZZLIB
CFLAGS= -O -g -Wall -I$(HOME)/Unix/$(OSTYPE)/include -dD
MAKEFILEDIR=/usr/local/Makefiles
MAKEFILE=lib.make

-include Makefile.preamble

include $(MAKEFILEDIR)/$(MAKEFILE)

-include Makefile.postamble

-include Makefile.dependencies
