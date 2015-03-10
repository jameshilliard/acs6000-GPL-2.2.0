# $Id: Makefile,v 1.7 2006/06/03 00:51:04 livio Exp $

PLATFORM = LINUX_LC6

# Uncomment for debugging
DEBUG = -Wall
INSTALL = /usr/bin/install -c

DEFINES = -D$(PLATFORM) $(DEBUG)
BINDIR = $(DESTDIR)/bin
LIBDIR = $(DESTDIR)/usr/lib
this_INCLUDEDIR = $(DESTDIR)/usr/include/libcyevent

CFLAGS += $(DEFINES) 

TARGETS = sh_write_evt
TARGET_LIBS = libcyevent
TARGET_LIBS_DSO=$(TARGET_LIBS).so
TARGET_LIBS_MAJOR=0
TARGET_LIBS_MINOR=0
TARGETS_INCLUDE = event_connect.h

all: $(TARGET_LIBS_DSO) $(TARGETS)

event_lib.o: event_lib.c
	$(CC) -fPIC -c $(CFLAGS) $(TARGET_ARCH) -o $@ $<

$(TARGET_LIBS_DSO): event_lib.o
	$(CC) -shared -Wl,-soname,$(TARGET_LIBS).so.$(TARGET_LIBS_MAJOR)   \
		-o $@ $< $(LDFLAGS)
	ln -sf $(TARGET_LIBS_DSO) $(TARGET_LIBS_DSO).$(TARGET_LIBS_MAJOR)
	ln -sf $(TARGET_LIBS_DSO) $(TARGET_LIBS_DSO).$(TARGET_LIBS_MAJOR).$(TARGET_LIBS_MINOR)

sh_write_evt: sh_write_evt.o $(TARGET_LIBS_DSO)
	$(CC) -o $@ $< -L. -lcyevent -ldlog $(LDFLAGS)

install:
	$(INSTALL) -d $(BINDIR)
	$(INSTALL) -d $(LIBDIR)
	$(INSTALL) -d $(this_INCLUDEDIR)
	$(INSTALL) -m 755 $(TARGETS) $(BINDIR)
	$(INSTALL) -m 755 $(TARGET_LIBS_DSO) \
		$(LIBDIR)/$(TARGET_LIBS_DSO).$(TARGET_LIBS_MAJOR).$(TARGET_LIBS_MINOR)
	$(INSTALL) -m 644 $(TARGETS_INCLUDE) $(this_INCLUDEDIR)

clean:
	rm -f $(TARGETS) *.o *.so* core *~

distclean: clean
