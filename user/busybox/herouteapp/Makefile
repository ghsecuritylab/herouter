EXE = herouteapp
OBJS = hert_des.o hert_msg.o hert_app.o hert_util.o hert_tcp.o  hert_com.o
#OBJS = hert_app.o

MONITOREXE = hertMonitor
MONITOROBJS = hertMonitor.o hert_util.o hert_ping_new.o hert_tcp.o hert_com.o

#stdlibc++ known issue: 
#If your program  needs libstdc++, please turn off code optimization

CFLAGS += -O2 -Wall -g  -lpthread -lnvram -L$(ROOTDIR)/lib/lib -DBUILD_PLAT_COM -L./ -ldes

CFLAGS += -I.\
		-I$(ROOTDIR)/lib/libnvram -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/$(LINUXDIR)/include \
		-I$(ROOTDIR)/$(LINUXDIR)/drivers/flash \
		-I ../../lib/curl-7.36.0/include/curl

all: herouteapp hemonitor

herouteapp: $(OBJS)
	cp -rf "../../lib/curl-7.36.0/lib/.libs/libcurl.a" ./
	$(CC) -o $(EXE) $(OBJS) libcurl.a $(CFLAGS)  $(EXTRA_CFLAGS) 

hemonitor: $(MONITOROBJS)
	$(CC) -o $(MONITOREXE) $(MONITOROBJS) $(CFLAGS)  $(EXTRA_CFLAGS) 

romfs:
	$(ROMFSINST) /bin/$(EXE)
	$(ROMFSINST) /bin/$(MONITOREXE)
clean:
	@rm -rf $(EXE)  *.o
	@rm -rf $(MONITOREXE)  *.o

