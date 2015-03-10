all: libadsap2.a adsap2client

adsap2.h: adsap2.xml
	gsp-cair.py -service adsap2.xml .

adsap2.c: adsap2.h

adsap2.o: adsap2.c
	$(CC) $(CFLAGS) -c $+

adsap2_fn.o: adsap2_fn.c adsap2.h
	$(CC) $(CFLAGS) -c adsap2_fn.c
	
libadsap2.a: adsap2.o adsap2_fn.o
	$(AR) rvu $@ $+
	$(RANLIB) $@

adsap2client.o: adsap2client.c adsap2.h
	$(CC) $(CFLAGS) -c adsap2client.c
	
adsap2client: libadsap2.a adsap2client.o
	$(CC) -o adsap2client adsap2client.o -L . -ladsap2 $(LDFLAGS) -lgsp-air -ldlog

clean:
	-rm adsap2.h adsap2.c *.o libadsap2.a adsap2client
