EXEC = slt
SRCS = slt.c
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char

#EXTRA_CFLAGS += -DDBG

all: $(EXEC)

$(EXEC): slt.o
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $@.c

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/slt_test.sh
	$(ROMFSINST) /bin/slt_link_partner.sh
	$(ROMFSINST) /etc_ro/Wireless/RT2860AP/RT2860AP.dat

clean:
	-rm -f $(EXEC) *.o

