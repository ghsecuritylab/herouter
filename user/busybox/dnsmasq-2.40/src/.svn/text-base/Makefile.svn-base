NAME = dnsmasq
OBJS = cache.o rfc1035.o util.o option.o forward.o isc.o network.o \
       dnsmasq.o dhcp.o lease.o rfc2131.o netlink.o dbus.o bpf.o \
       helper.o tftp.o log.o

CFLAGS += -lnvram -L$(ROOTDIR)/lib/lib

CFLAGS += -I$(ROOTDIR)/lib/libnvram -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/$(LINUXDIR)/include 

CFLAGS += -O2 -D__uClinux__ -DNO_GETTEXT #-DNO_IPV6

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(CFLAGS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)

clean:
	rm -f *~ *.mo *~ *.pot 
	rm -f *.o dnsmasq.a dnsmasq core */core
	