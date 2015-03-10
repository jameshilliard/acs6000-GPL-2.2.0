all:    libavct-protocol-core.a

.c:
	$(CC) $(CFLAGS) $@.c

libavct-protocol-core.a: avct-protocol-core.o
	$(AR) rvu $@ avct-protocol-core.o
	$(RANLIB) $@

clean:
	-rm *.o libavct-protocol-core.a
