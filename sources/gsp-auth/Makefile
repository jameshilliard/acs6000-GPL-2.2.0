.SUFFIXES:
.SUFFIXES: .c .cc .d .o .so .a

SHELL=/bin/sh

include gsp_aaa.mk

PAM_LIBS=-lpam -lpam_misc -ldl
CYC_LIBS=-lcyc -lcyc++
ADSAP_LIBS=-lgsp-air -ladsap2
GSP_LIBS=-ldlog

LIBAUTH_NAME=libgsp_auth
LIBAUTH_MAJOR=0
LIBAUTH_MINOR=0

LIBGSP_AUTH_SOURCES_CC=gsp_auth.cc gsp_auth_session.cc \
	gsp_auth_pam.cc gsp_auth_util.cc gsp_auth_pubapi.cc
LIBGSP_AUTH_SOURCES=$(LIBGSP_AUTH_SOURCES_CC)
LIBGSP_AUTH_DSO=$(LIBAUTH_NAME).so
LIBGSP_AUTH_STATIC=$(LIBGSP_AUTH_DSO:.so=.a)
LIBGSP_AUTH_OBJS_CC=$(LIBGSP_AUTH_SOURCES_CC:.cc=.o)
LIBGSP_AUTH_OBJS=$(LIBGSP_AUTH_OBJS_CC)

TARGETS=$(LIBGSP_AUTH_DSO) $(LIBGSP_AUTH_STATIC)

all: $(TARGETS)

-include $(LIBGSP_AUTH_SOURCES_CC:.cc=.d)

$(LIBGSP_AUTH_DSO): $(LIBGSP_AUTH_OBJS)
	$(CXX) -shared -Wl,-soname,$(LIBAUTH_NAME).so.$(LIBAUTH_MAJOR) 	\
		-o $@ $^ $(LDFLAGS) $(PAM_LIBS) $(CYC_LIBS) $(ADSAP_LIBS) $(GSP_LIBS)

$(LIBGSP_AUTH_STATIC): $(LIBGSP_AUTH_OBJS)

install: all

clean:
	rm -f *.o *~ $(TARGETS)

distclean: clean
	-rm -f *.d

