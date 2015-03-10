all:    libgsp-air.so

%.o: %.c
	$(CC) $(CFLAGS) -c $+

libgsp-air.so: gsp-air.o
	$(CC) -o $@ $(LDFLAGS) -shared $+ -ldlog -lpthread

clean:
	-rm *.o *.so