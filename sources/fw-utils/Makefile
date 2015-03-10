BINDIR = $(DESTDIR)/sbin

CFLAGS += -I./include -I$(STAGING_DIR)/include
LDFLAGS = $(subst -s,,$(subst -S,,-s))

TARGETS = checkImage kernelhdr flash2x avctWDT

all: $(TARGETS)

avctWDT:
	$(CC) $(LDFLAGS) -o $@ avctWDT.c

checkImage:	checkImage.o
	$(CC) $(LDFLAGS) -L$(STAGING_DIR)/lib -l ssl -l crypto -o $@ $^

flash2x:	flash2x.o
	$(CC) $(LDFLAGS) -L$(STAGING_DIR)/lib -l ssl -l crypto -o $@ $^

kernelhdr:	kernelhdr.o
	$(CC) $(LDFLAGS) -o $@ $^

install:
	install -d $(BINDIR)
	install -m 755 $(TARGETS) $(BINDIR)
	@rm -f $(BINDIR)/checkKernel
	ln -s checkImage $(BINDIR)/checkKernel
	@rm -f $(BINDIR)/checkFileSystem
	ln -s checkImage $(BINDIR)/checkFileSystem
	@rm -f $(BINDIR)/checkBoot
	ln -s checkImage $(BINDIR)/checkBoot

