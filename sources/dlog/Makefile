all:    libdlog.a libdlog.so dlogcfg

%.o:	%.c
	$(CC) $(CFLAGS) -c $+

libdlog.a: dlog.o
	$(AR) rvu $@ dlog.o
	$(RANLIB) $@

libdlog.so: dlog.o
	$(CC) -o $@ $(LDFLAGS) -shared dlog.o

dlogcfg: dlog.o dlogcfg.o
	$(CC) -o $@ $(LDFLAGS)  dlog.o dlogcfg.o

clean:
	-rm *.o libdlog.a libdlog.so dlogcfg
