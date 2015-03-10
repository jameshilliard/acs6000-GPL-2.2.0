#
# File: Makefile
#
# Copyright (C) 2004-2005 Cyclades Corporation
#
# www.cyclades.com
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#
# Author: Erik Lotspeich <erik.lotspeich@cyclades.com>
#
.SUFFIXES:
.SUFFIXES: .c .cc .d .o .so .a
.PHONY:

SHELL=/bin/sh
RANLIB=ranlib

include libcyc.mk

SHELL=/bin/sh
CFLAGS+=-I. -I../include -D_REENTRANT
CXXFLAGS+=-I. -I../include -D_REENTRANT
LDFLAGS=-s -lpthread

LIB_DSO=libcyc.so
LIB_A=libcyc.a
sources=libcyc_main.c libcyc_malloc.c libcyc_ll.c \
	libcyc_ipc.c libcyc_util.c \
	libcyc_thread.c md5.c
objects=$(sources:.c=.o)

LIBPP_DSO=libcyc++.so
LIBPP_A=libcyc++.a
pp_sources=libcyc++_main.cc libcyc++_signals.cc \
	libcyc++_ipc.cc libcyc++_thread.cc \
	libavo++_util.cc
pp_objects=$(pp_sources:.cc=.o)

all: $(LIB_A) $(LIB_DSO) $(LIBPP_A) $(LIBPP_DSO)

debug: CFLAGS+=-g -ggdb
debug: all

-include $(sources:.c=.d)
-include $(pp_sources:.cc=.d)

LIB_NAME=libcyc
LIB_MAJOR=3
LIB_MINOR=0
$(LIB_DSO): $(objects)
	$(CC) -shared -Wl,-soname,$(LIB_NAME).so.$(LIB_MAJOR) \
                -o $(@).$(LIB_MAJOR).$(LIB_MINOR) $^ $(LDFLAGS)
$(LIB_A): $(objects)
LIBPP_NAME=libcyc++
LIBPP_MAJOR=0
LIBPP_MINOR=1
$(LIBPP_DSO): $(pp_objects)
	$(CXX) -shared -Wl,-soname,$(LIBPP_NAME).so.$(LIBPP_MAJOR) \
                -o $(@).$(LIBPP_MAJOR).$(LIBPP_MINOR) $^ $(LDFLAGS)
$(LIBPP_A): $(pp_objects)

clean:
	-rm -f *.so *.o *.a *~

distclean: clean
	-rm -f *.d

install: all
