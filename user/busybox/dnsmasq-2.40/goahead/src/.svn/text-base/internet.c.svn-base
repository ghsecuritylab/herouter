/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Internet Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
#include	"internet.h"
#include	"nvram.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"wireless.h"
#include  "wsIntrn.h"

#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config
#include	"user/busybox/include/autoconf.h" //busybox config

#ifdef CONFIG_RALINKAPP_SWQOS
#include      "qos.h"
#endif
#ifdef CONFIG_RALINKAPP_HWQOS
#include      "qos.h"
#endif
static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv6DSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getiTunesBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int get3GBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPktFilterBuilt(int eid, webs_t wp, int argc, char_t **argv);

static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv);
#ifdef DARE_CUSTOMER_WEB
static int getCliList(int eid, webs_t wp, int argc, char_t **argv);
static int getDhcpList(int eid, webs_t wp, int argc, char_t **argv);
static int getClientIPMAC(int eid, webs_t wp, int argc, char_t **argv);
static void MACCloneCfg(webs_t wp, char_t *path, char_t *query);
static int getCliNum(int eid, webs_t wp, int argc, char_t **argv);
static int getDhcpCliList4He(int eid, webs_t wp, int argc, char_t **argv);
/*
static int getstartnum(int eid, webs_t wp, int argc, char_t **argv);
static int getendnum(int eid, webs_t wp, int argc, char_t **argv);
static int getClientnum(int eid, webs_t wp, int argc, char_t **argv);
*/
#endif
static int getDns(int eid, webs_t wp, int argc, char_t **argv);
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv);
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv);
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv);
static int getWanPortStatus(int eid, webs_t wp, int argc, char_t **argv);
static int getPingStatus(int eid, webs_t wp, int argc, char_t **argv);
static void setLan(webs_t wp, char_t *path, char_t *query);
#ifdef DARE_CUSTOMER_WEB
static void entconfig(webs_t wp, char_t *path, char_t *query);
static void dhcpbanding(webs_t wp, char_t *path, char_t *query);
#endif
#if defined (CONFIG_IPV6)
static void setIPv6(webs_t wp, char_t *path, char_t *query);
#endif
static void setVpnPaThru(webs_t wp, char_t *path, char_t *query);
static void setWan(webs_t wp, char_t *path, char_t *query);
static void getMyMAC(webs_t wp, char_t *path, char_t *query);
static void addRouting(webs_t wp, char_t *path, char_t *query);
static void delRouting(webs_t wp, char_t *path, char_t *query);
static void dynamicRouting(webs_t wp, char_t *path, char_t *query);
static void UpdateCert(webs_t wp, char_t *path, char_t *query);
#ifdef DARE_CUSTOMER_WEB
static void netDiagnosis(webs_t wp, char_t *path, char_t *query);
static void netState(webs_t wp, char_t *path, char_t *query);
static void MobWanSet(webs_t wp, char_t *path, char_t *query);
static void MobPppoeSet(webs_t wp, char_t *path, char_t *query);
static void QuickSetUp(webs_t wp, char_t *path, char_t *query);
#endif

static int getSoftwareVersion(int eid, webs_t wp, int argc, char_t **argv);

static int getSysTime(int eid, webs_t wp, int argc, char_t **argv);
static void setSysTime(webs_t wp, char_t *path, char_t *query);

extern NETWORK_STATUS herouterStatus;
extern int hertUtil_upEthrtPortStatus();

inline void zebraRestart(void);
void ripdRestart(void);

void formDefineInternet(void) {
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
#ifdef DARE_CUSTOMER_WEB
	websAspDefine(T("getCliList"), getCliList);	
	websAspDefine(T("getDhcpList"), getDhcpList);	
	websAspDefine(T("getClientIPMAC"), getClientIPMAC);	
	websFormDefine(T("MACCloneCfg"), MACCloneCfg);
	websAspDefine(T("getCliNum"), getCliNum);		
		
/*	
	websAspDefine(T("getendnum"), getendnum);
	websAspDefine(T("getstartnum"), getstartnum);
	websAspDefine(T("getClientnum"), getClientnum);
*/
#endif	
  websAspDefine(T("getDhcpCliList4He"), getDhcpCliList4He);
	websAspDefine(T("getDns"), getDns);
	websAspDefine(T("getHostSupp"), getHostSupp);
	websAspDefine(T("getIfLiveWeb"), getIfLiveWeb);
	websAspDefine(T("getIfIsUpWeb"), getIfIsUpWeb);
	websAspDefine(T("getIgmpProxyBuilt"), getIgmpProxyBuilt);
	websAspDefine(T("getVPNBuilt"), getVPNBuilt);
	websAspDefine(T("getLanIp"), getLanIp);
	websAspDefine(T("getLanMac"), getLanMac);
	websAspDefine(T("getWiFiMac"), getWiFiMac);
	websAspDefine(T("getLanIfNameWeb"), getLanIfNameWeb);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getDnsmasqBuilt"), getDnsmasqBuilt);
	websAspDefine(T("getGWBuilt"), getGWBuilt);
	websAspDefine(T("getLltdBuilt"), getLltdBuilt);
	websAspDefine(T("getPppoeRelayBuilt"), getPppoeRelayBuilt);
	websAspDefine(T("getUpnpBuilt"), getUpnpBuilt);
	websAspDefine(T("getRadvdBuilt"), getRadvdBuilt);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanPortStatus"), getWanPortStatus);
	websAspDefine(T("getPingStatus"), getPingStatus);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWanIfNameWeb"), getWanIfNameWeb);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanGateway"), getWanGateway);
	websAspDefine(T("getRoutingTable"), getRoutingTable);
	websAspDefine(T("getUSBBuilt"), getUSBBuilt);
	websAspDefine(T("getIPv6Built"), getIPv6Built);
	websAspDefine(T("getIPv66rdBuilt"), getIPv66rdBuilt);
	websAspDefine(T("getIPv6DSBuilt"), getIPv6DSBuilt);
	websAspDefine(T("getStorageBuilt"), getStorageBuilt);
	websAspDefine(T("getFtpBuilt"), getFtpBuilt);
	websAspDefine(T("getSmbBuilt"), getSmbBuilt);
	websAspDefine(T("getMediaBuilt"), getMediaBuilt);
	websAspDefine(T("getWebCamBuilt"), getWebCamBuilt);
	websAspDefine(T("getPrinterSrvBuilt"), getPrinterSrvBuilt);
	websAspDefine(T("getiTunesBuilt"), getiTunesBuilt);
	websFormDefine(T("setLan"), setLan);
	websAspDefine(T("getSoftwareVersion"), getSoftwareVersion);
	websAspDefine(T("getSysTime"), getSysTime);
	websFormDefine(T("setSysTime"), setSysTime);
#ifdef DARE_CUSTOMER_WEB
	websFormDefine(T("entconfig"), entconfig);
	websFormDefine(T("dhcpbanding"), dhcpbanding);
#endif
#if defined (CONFIG_IPV6)
	websFormDefine(T("setIPv6"), setIPv6);
#endif
	websFormDefine(T("setVpnPaThru"), setVpnPaThru);
	websFormDefine(T("setWan"), setWan);
	websFormDefine(T("getMyMAC"), getMyMAC);
	websFormDefine(T("addRouting"), addRouting);
	websFormDefine(T("delRouting"), delRouting);
	websFormDefine(T("dynamicRouting"), dynamicRouting);
	websFormDefine(T("UpdateCert"), UpdateCert);
#ifdef DARE_CUSTOMER_WEB
	websFormDefine(T("netDiagnosis"), netDiagnosis);
	websFormDefine(T("netState"), netState);
	websFormDefine(T("MobWanSet"), MobWanSet);
	websFormDefine(T("MobPppoeSet"), MobPppoeSet);
	websFormDefine(T("QuickSetUp"), QuickSetUp);
#endif
	websAspDefine(T("getDynamicRoutingBuilt"), getDynamicRoutingBuilt);
	websAspDefine(T("getSWQoSBuilt"), getSWQoSBuilt);
	websAspDefine(T("getDATEBuilt"), getDATEBuilt);
	websAspDefine(T("getDDNSBuilt"), getDDNSBuilt);
	websAspDefine(T("getSysLogBuilt"), getSysLogBuilt);
	websAspDefine(T("getETHTOOLBuilt"), getETHTOOLBuilt);
	websAspDefine(T("get3GBuilt"), get3GBuilt);
	websAspDefine(T("getPktFilterBuilt"), getPktFilterBuilt);
}

/*
 * arguments: ifname  - interface name
 * description: test the existence of interface through /proc/net/dev
 * return: -1 = fopen error, 1 = not found, 0 = found
 */
int getIfLive(char *ifname)
{
	FILE *fp;
	char buf[256], *p;
	int i;

	if (NULL == (fp = fopen("/proc/net/dev", "r"))) {
		error(E_L, E_LOG, T("getIfLive: open /proc/net/dev error"));
		return -1;
	}

	fgets(buf, 256, fp);
	fgets(buf, 256, fp);
	while (NULL != fgets(buf, 256, fp)) {
		i = 0;
		while (isspace(buf[i++]))
			;
		p = buf + i - 1;
		while (':' != buf[i++])
			;
		buf[i-1] = '\0';
		if (!strcmp(p, ifname)) {
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	error(E_L, E_LOG, T("getIfLive: device %s not found"), ifname);
	return 1;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 18-byte buffer to store mac address
 * description: fetch mac address according to given interface name
 */
int getIfMac(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfMac: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfMac: ioctl SIOCGIFHWADDR error for %s"), ifname);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfIp: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfIp: ioctl SIOCGIFADDR error for %s"), ifname);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

/*
 * arguments: ifname - interface name
 * description: return 1 if interface is up
 *              return 0 if interface is down
 */
int getIfIsUp(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		perror("socket");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		close(skfd);
		return -1;
	}
	close(skfd);
	if (ifr.ifr_flags & IFF_UP)
		return 1;
	else
		return 0;
}

/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
int getIfNetmask(char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfNetmask: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfNetmask: ioctl SIOCGIFNETMASK error for %s\n"), ifname);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

/*
 * description: return WAN interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
char* getWanIfName(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P0)
		if_name = "eth2.1";
#else
		if_name = "eth2.5";
#endif
#elif defined (CONFIG_RAETH_GMAC2)
		if_name = "eth3";
#else
		if_name = "eth2.2";
#endif
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if_name = "eth3";
#else /* MARVELL & CONFIG_ICPLUS_PHY */
		if_name = "eth2";
#endif
	}
	else if (!strncmp(mode, "2", 2))
		if_name = "ra0";
	else if (!strncmp(mode, "3", 2))
		if_name = "apcli0";
	return if_name;
}

char* getWanIfNamePPP(void)
{
    const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
    if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
		|| !strncmp(cm, "3G", 3)
#endif
	){
        return "ppp0";
	}

    return getWanIfName();
}


/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "br0";
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1)	// multiple ssid
			if_name = "br0";
		else
			if_name = "ra0";
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if_name = "br0";
#else
		if_name = "ra0";
#endif
	}
	else if (!strncmp(mode, "2", 2)) {
		if_name = "br0";
	}
	else if (!strncmp(mode, "3", 2)) {
		if_name = "br0";
	}
	return if_name;
}

/*
 * description: get the value "WAN" or "LAN" the interface is belong to.
 */
char *getLanWanNamebyIf(char *ifname)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");

	if (NULL == mode)
		return "Unknown";

	if (!strcmp(mode, "0")){	// bridge mode
		if(!strcmp(ifname, "br0"))
			return "LAN";
		return ifname;
	}

	if (!strcmp(mode, "1")) {	// gateway mode
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if(!strcmp(ifname, "br0"))
			return "LAN";
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P0)
		if(!strcmp(ifname, "eth2.1") || !strcmp(ifname, "ppp0"))
#else
		if(!strcmp(ifname, "eth2.5") || !strcmp(ifname, "ppp0"))
#endif
#else
		if(!strcmp(ifname, "eth2.2") || !strcmp(ifname, "ppp0"))
#endif
			return "WAN";
		return ifname;
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1 && !strcmp(ifname, "br0") )	// multiple ssid
			return "LAN";
		if(atoi(num_s) == 1 && !strcmp(ifname, "ra0"))
			return "LAN";
		if (!strcmp(ifname, "eth2") || !strcmp(ifname, "ppp0"))
			return "WAN";
		return ifname;
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if(!strcmp(ifname, "br0"))
			return "LAN";
		if(!strcmp(ifname, "eth3"))
			return "WAN";
#else
		if(!strcmp(ifname, "ra0"))
			return "LAN";
		return ifname;
#endif
	}else if (!strncmp(mode, "2", 2)) {	// ethernet convertor
		if(!strcmp("eth2", ifname))
			return "LAN";
		if(!strcmp("ra0", ifname))
			return "WAN";
		return ifname;
	}else if (!strncmp(mode, "3", 2)) {	// apcli mode
		if(!strcmp("br0", ifname))
			return "LAN";
		if(!strcmp("apcli0", ifname))
			return "WAN";
		return ifname;
	}
	return ifname;
}

#define CHECK_EMPTY_MAC(macarray) \
	if ((macarray[0]==0) && (macarray[1]==0) && (macarray[2]==0) && \
		(macarray[3]==0) && (macarray[4]==0) && (macarray[5]==0)) \
	{ \
		continue; \
	}
	

#ifdef DARE_CUSTOMER_WEB

/*
static int getstartnum(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%u"),1);
}
static int getendnum(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%u"),1 >count?1:count);
}
static int getClientnum(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%u"),count);
}
*/
static int getClientIPMAC(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int i = 0,j;
	struct in_addr addr;
	char fs[1024*3] = {0};
	//comment it for no valid: doSystem("killall -q -USR1 udhcpd");
	char_t *Co_Mac;
	
	Co_Mac = nvram_bufget(RT2860_NVRAM, "macCloneMac");
	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return websWrite(wp, T("<input class=\"selectinp\" type=\"text\" id=\"macCloneMac\" name=\"macCloneMac\" value=\"%s\"/></td>"),Co_Mac==NULL?"输入MAC地址":Co_Mac);
	sprintf(fs+strlen(fs), "   <td><span>MAC 地址</span></td>\n");
	sprintf(fs+strlen(fs), "              	   <td><div class=\"fix\" style=\" z-index:999;\">\n");	
	sprintf(fs+strlen(fs), "              	     <div class=\"ganyin\" onClick=\"hide('test1')\" style=\" width:250px;\"></div>\n");	
	sprintf(fs+strlen(fs), "              	     <div id=\"test1\" style=\"display: none; \" class=\"bm\">\n");	
	sprintf(fs+strlen(fs), "              	     <ul style=\" width:250px;\">\n");	

	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) 
	{
		CHECK_EMPTY_MAC(lease.mac);
		addr.s_addr = lease.ip;
		sprintf(fs+strlen(fs), "              	       <li  onClick=\"pick('%02X",lease.mac[0]);
		for (j = 1; j < 6; j++)
			sprintf(fs+strlen(fs), T(":%02X"), lease.mac[j]);
		sprintf(fs+strlen(fs),"','macCloneMac','test1')\" >%s</li> \n",inet_ntoa(addr));		
		i ++ ;
	}	
//		printf("########fs is %s \n",fs);
	if(i == 0)
	{
		websWrite(wp, T("<td><span>MAC 地址</span></td>\n"));
		websWrite(wp, T("<td><input class=\"selectinp\" type=\"text\" id=\"macCloneMac\" name=\"macCloneMac\" value=\"%s\"/></td>\n"),Co_Mac==NULL?"输入MAC地址":Co_Mac);
		fclose(fp);
		return 0;
	}
	else
	{	
		websWrite(wp, T("%s\n"),fs);
		websWrite(wp, T("</ul>\n"));	
		websWrite(wp, T("</div>\n"));
		websWrite(wp, T("</div>\n"));
		websWrite(wp, T("<input class=\"selectinp\" type=\"text\" id=\"macCloneMac\" name=\"macCloneMac\" value=\"%s\"/></td>\n"),Co_Mac==NULL?"输入MAC地址":Co_Mac);							 
	}
	fclose(fp);
	return 0;
}

static void MACCloneCfg(webs_t wp, char_t *path, char_t *query)
{
	char_t	*clone_en, *clone_mac;
	clone_en = websGetVar(wp, T("macCloneEnbl"), T("0"));
	clone_mac = websGetVar(wp, T("macCloneMac"), T(""));
	nvram_bufset(RT2860_NVRAM, "macCloneEnabled", clone_en);
	if (!strncmp(clone_en, "1", 2))
	{	
		printf("nvran bufset macclone\n");
		nvram_bufset(RT2860_NVRAM, "macCloneMac", clone_mac);
	}
	nvram_commit(RT2860_NVRAM);
	
	initInternet();	
	websRedirect(wp, "herouter/MACClone.asp");
}

static int getCliNum_old(int eid, webs_t wp, int argc, char_t **argv)
{
	struct dhcpOfferedAddr 
	{
			unsigned char hostname[16];
			unsigned char mac[16];
			unsigned long ip;
			unsigned long expires;
	} lease;
	FILE *fp;
	unsigned count =0;

	//comment it for no valid: doSystem("killall -q -USR1 udhcpd");
	
	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return websWrite(wp, T("N/A"));
		
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) 
	{
		CHECK_EMPTY_MAC(lease.mac);
		count++;
	}	
	
	fclose(fp);
	printf("return...\n");
	websWrite(wp,T("%d"),count);
	return 	0;
}

static int getCliNum(int eid, webs_t wp, int argc, char_t **argv)
{
	int count =0;

    count = hertUtil_GetDeviceList_DoInit(1);

    if ( count < 0 )
    {
		return websWrite(wp, T("N/A"));
    }

    hertUtil_GetDeviceList_DoUnInit();

	websWrite(wp,T("%d"),count);
	return 	0;
}

static int getCliList_old(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
		struct dhcpOfferedAddr {
			unsigned char hostname[16];
			unsigned char mac[16];
			unsigned long ip;
			unsigned long expires;
		} lease;
		int i,type;
		struct in_addr addr;
		unsigned long expires;
		unsigned count =0,displayNum = 1;
		
		//comment it for no valid: doSystem("killall -q -USR1 udhcpd");
	
		nvram_bufset(RT2860_NVRAM, "AccessControlList_temp", "default");
		nvram_bufset(RT2860_NVRAM, "AccessPolicy_temp", "default");
		
		nvram_commit(RT2860_NVRAM);
		
		fp = fopen("/var/udhcpd.leases", "r");
		if (NULL == fp)
			return websWrite(wp, T(""));
			
		websWrite(wp,T("   <div class='data_detail'>\n"));	
			
		while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) 
		{
			CHECK_EMPTY_MAC(lease.mac);
			addr.s_addr = lease.ip;
		//	printf("====================\n");
		//	printf("%s\n%s\n%d\n%d\n",lease.hostname,lease.mac,lease.ip,lease.expires);
		//	printf("====================\n");
			
			/* display 5 Clients info in page*/
			if(displayNum <= 5)
				websWrite(wp, T("			<ul id = 'display%d' style='display:'> \n"),displayNum);
			else 
				websWrite(wp, T("			<ul id = 'display%d' style='display:none'> \n"),displayNum);
			//class device2 is wifi logo 	
			type = 2;
			if (strlen(lease.hostname) > 0)
			{
				if(strstr(lease.hostname,"android"))
					type = 4;
				else if(strstr(lease.hostname,"iPhone")) 
					type = 3;
				else if(strstr(lease.hostname,"Mac")) 
					type = 1;
				else if(strstr(lease.hostname,"PC")||strstr(lease.hostname,"WINDOWS")) 
					type = 5;
				else
				{
					if (!hertUtil_IsWirelessClientMac(&lease.mac[0]))
					{
					    type = 5; /* if it is not connected with wireless, think it as PC */
					}
				}
			}
				
			websWrite(wp, T("				<li class='l1'><span id = 'mac%d' class='device%d'>%02X"), displayNum,type,lease.mac[0]);
			for (i = 1; i < 6; i++)
				websWrite(wp, T(":%02X"), lease.mac[i]);			
			expires = ntohl(lease.expires);
			websWrite(wp, T("</span></li> \n				<li class='l2'>%s</li> \n"), inet_ntoa(addr));			
			if (strlen(lease.hostname) > 0)
				websWrite(wp, T("				<li class='l3'>%-16s</li>\n"), lease.hostname);
			else
				websWrite(wp, T("				<li class='l3'></li> \n"));
			
//			websWrite(wp, T("				<li class='l4'>128 KB/S<br />10KB/S</li>\n"));	
//			websWrite(wp, T("				<li class='l5'><strong>无</strong></li>\n"));	
			websWrite(wp, T("				<li class='l6'><em id = 'emid_%d' style='display:' onclick=\"disconnect('mac%d','emid_%d','Ocemid_%d');\">加入黑名单</em><em id ='Ocemid_%d' style='display:none'>正在断开</em></li>\n"),displayNum,displayNum,displayNum,displayNum,displayNum);	
			websWrite(wp, T("			</ul> \n"));
			
			count++;displayNum++;
		}		
		websWrite(wp,T("	  </div>\n"));
		websWrite(wp,T("	  <div class='countinfo'>\n"));
		websWrite(wp,T("	  	<ul>\n"));
		websWrite(wp,T("				<li>接入无线设备总数量：<em>%u</em></li>\n"),count);
		websWrite(wp,T("	      <li class='pagechange'><strong class='nextpage'  Onclick='NextDisplay(%u);'>下一页</strong></li>\n"),count);
		websWrite(wp,T("	      <li class='pagechange'><strong class='prepage'   Onclick='PreDisplay(%u);'>上一页</strong></li>\n"),count);
		websWrite(wp,T("	    </ul>\n"));
		websWrite(wp,T("	   </div>\n"));
		fclose(fp);
		return 0;
}
static int getCliList(int eid, webs_t wp, int argc, char_t **argv)
{
    char szMac[64];
    char szDevName[128];
    char szIpAddr[64];
    int  nDevNum = 0;
    int i,type;
    unsigned count =0,displayNum = 1;

    //comment it for no valid: doSystem("killall -q -USR1 udhcpd");
    
    nvram_bufset(RT2860_NVRAM, "AccessControlList_temp", "default");
    nvram_bufset(RT2860_NVRAM, "AccessPolicy_temp", "default");

    nvram_commit(RT2860_NVRAM);

    nDevNum = hertUtil_GetDeviceList_DoInit(0);

    websWrite(wp,T("   <div class='data_detail'>\n"));	
    for(i = 0; i < nDevNum; i++)
    {
        memset(szMac, 0x0, sizeof(szMac));
        memset(szDevName, 0x0, sizeof(szDevName));
        memset(szIpAddr, 0x0, sizeof(szIpAddr));
        if (hertUtil_GetDeviceList_GetItem(i, szMac, szDevName, szIpAddr) < 0 )
        {
            continue;
        }

        /* display 5 Clients info in page*/
        if(displayNum <= 5)
            websWrite(wp, T("			<ul id = 'display%d' style='display:'> \n"),displayNum);
        else 
            websWrite(wp, T("			<ul id = 'display%d' style='display:none'> \n"),displayNum);

        //class device2 is wifi logo
        type = 2;
        if (strlen(szDevName) > 0)
        {
            if(strstr(szDevName,"android"))
                type = 4;
            else if(strstr(szDevName,"iPhone")) 
                type = 3;
            else if(strstr(szDevName,"Mac")) 
                type = 1;
            else if(strstr(szDevName,"PC")||strstr(szDevName,"WINDOWS")) 
                type = 5;
            else if(strstr(szDevName,"iPad")) 
                type = 6;                
            else
            {
                if (!hertUtil_IsWirelessClientMac(szMac))
                {
                    type = 5; /* if it is not connected with wireless, think it as PC */
                }
            }
        }

        websWrite(wp, T("				<li class='l1'><span id = 'mac%d' class='device%d'>"), displayNum,type);
        websWrite(wp, T("%s"), szMac);			
        websWrite(wp, T("</span></li> \n				<li class='l2'>%s</li> \n"), szIpAddr);
        if (strlen(szDevName) > 0)
            websWrite(wp, T("				<li class='l3'>%-16s</li>\n"), szDevName);
        else
            websWrite(wp, T("				<li class='l3'></li> \n"));

        // websWrite(wp, T("				<li class='l4'>128 KB/S<br />10KB/S</li>\n"));
        // websWrite(wp, T("				<li class='l5'><em id = 'emidbitrate_%d' style='display:' onclick=\"doratelimit('mac%d');\">无</em></li>\n"),displayNum);
        websWrite(wp, T("				<li class='l6'><em id = 'emid_%d' style='display:' onclick=\"disconnect('mac%d','emid_%d','Ocemid_%d');\">加入黑名单</em><em id ='Ocemid_%d' style='display:none'>正在断开</em></li>\n"),displayNum,displayNum,displayNum,displayNum,displayNum);
        websWrite(wp, T("			</ul> \n"));

        count++;displayNum++;
    }
    websWrite(wp,T("	  </div>\n"));
    websWrite(wp,T("	  <div class='countinfo'>\n"));
    websWrite(wp,T("	  	<ul>\n"));
    websWrite(wp,T("				<li>接入无线设备总数量：<em>%u</em></li>\n"),count);
    websWrite(wp,T("	      <li class='pagechange'><strong class='nextpage'  Onclick='NextDisplay(%u);'>下一页</strong></li>\n"),count);
    websWrite(wp,T("	      <li class='pagechange'><strong class='prepage'   Onclick='PreDisplay(%u);'>上一页</strong></li>\n"),count);
    websWrite(wp,T("	    </ul>\n"));
    websWrite(wp,T("	   </div>\n"));

    hertUtil_GetDeviceList_DoUnInit();

    return 0;
}

typedef struct tagdhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip;
        unsigned long expires;
        unsigned int  nReachable;
} DHCPITEM;

static int getDhcpList(int eid, webs_t wp, int argc, char_t **argv)
{
    char szMac[64];
    char szLowMac[64];
    char szDevName[128];
    char szIpAddr[64];
    int  nDevNum = 0;
    int i,type;
    unsigned count =0,displayNum = 1;
    DHCPITEM tDhcpItem;
    struct in_addr addr;

    //comment it for no valid: doSystem("killall -q -USR1 udhcpd");
    
    nvram_bufset(RT2860_NVRAM, "AccessControlList_temp", "default");
    nvram_bufset(RT2860_NVRAM, "AccessPolicy_temp", "default");

    nvram_commit(RT2860_NVRAM);

    //* get wireless client items */
    hertUtil_UpdateWirelessClientInfo();

    //* get dhcp client items */
    nDevNum = hertUtil_UpdateIpReachInfo(0);

    //* we empty it if there is no lan1 and lan2 connect */
    hertUtil_upEthrtPortStatus();
    if ((strcmp(herouterStatus.lan1PortStatus,STRING_FAIL) == 0) &&
        (strcmp(herouterStatus.lan2PortStatus,STRING_FAIL) == 0))
    {
        nDevNum = 0;
    }

    websWrite(wp,T("   <div class='data_detail'>\n"));	
    for(i = 0; i < nDevNum; i++)
    {
        memset(szMac, 0x0, sizeof(szMac));
        memset(szLowMac, 0x0, sizeof(szLowMac));
        memset(szDevName, 0x0, sizeof(szDevName));
        memset(szIpAddr, 0x0, sizeof(szIpAddr));
		
        if (hertUtil_GetDhcpClientLeaseItem(i, &tDhcpItem) <= 0 )
        {
            continue;
        }
        sprintf(szMac, "%02X:%02X:%02X:%02X:%02X:%02X", tDhcpItem.mac[0],
            tDhcpItem.mac[1], tDhcpItem.mac[2], tDhcpItem.mac[3],
            tDhcpItem.mac[4], tDhcpItem.mac[5]);

        if (hertUtil_IsWirelessClientMac(szMac))
        {
            continue; /* it is in connect-wireless-client, not show it */
        }

        sprintf(szLowMac, "%02x:%02x:%02x:%02x:%02x:%02x", tDhcpItem.mac[0],
            tDhcpItem.mac[1], tDhcpItem.mac[2], tDhcpItem.mac[3],
            tDhcpItem.mac[4], tDhcpItem.mac[5]);
        if (hertUtil_IsInSaveWirelessClientMac(szLowMac))
        {
            continue; /* it is wireless client, but it is disconnect, not show it */
        }

        addr.s_addr = tDhcpItem.ip;
        sprintf(szIpAddr, "%s", inet_ntoa(addr));

        /* display 5 Clients info in page*/
        if(displayNum <= 5)
            websWrite(wp, T("			<ul id = 'display%d' style='display:'> \n"),displayNum);
        else 
            websWrite(wp, T("			<ul id = 'display%d' style='display:none'> \n"),displayNum);

        //class device5 is pc logo
        type = 5;

        websWrite(wp, T("				<li class='l1'><span id = 'dhcpmac%d' class='device%d'>"), displayNum,type);
        websWrite(wp, T("%s"), szMac);			
        websWrite(wp, T("</span></li> \n				<li class='l2'>%s</li> \n"), szIpAddr);
        if (strlen(tDhcpItem.hostname) > 0)
            websWrite(wp, T("				<li class='l3'>%-16s</li>\n"), tDhcpItem.hostname);
        else
            websWrite(wp, T("				<li class='l3'></li> \n"));

        // websWrite(wp, T("				<li class='l4'>128 KB/S<br />10KB/S</li>\n"));
        // websWrite(wp, T("				<li class='l5'><em id = 'emidbitrate_%d' style='display:' onclick=\"doratelimit('mac%d');\">无</em></li>\n"),displayNum);
        websWrite(wp, T("				<li class='l6'>无</li>\n"));
        websWrite(wp, T("			</ul> \n"));

        count++;displayNum++;
    }
	
    websWrite(wp,T("	  </div>\n"));
    websWrite(wp,T("	  <div class='countinfo'>\n"));
    websWrite(wp,T("	  	<ul>\n"));
    websWrite(wp,T("				<li>接入有线设备总数量：<em>%u</em></li>\n"),count);
    websWrite(wp,T("	      <li class='pagechange'><strong class='nextpage'  Onclick='NextDisplay(%u);'>下一页</strong></li>\n"),count);
    websWrite(wp,T("	      <li class='pagechange'><strong class='prepage'   Onclick='PreDisplay(%u);'>上一页</strong></li>\n"),count);
    websWrite(wp,T("	    </ul>\n"));
    websWrite(wp,T("	   </div>\n"));

    return 0;
}
#endif

/*
 * description: write DHCP client list
 */
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
	unsigned d, h, m;

	//comment it for no valid: doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		CHECK_EMPTY_MAC(lease.mac);
		addr.s_addr = lease.ip;
		//CHECK_LANIP_REACHABLE(&lease);
		if (strlen(lease.hostname) > 0)
			websWrite(wp, T("<tr><td>%-16s</td>"), lease.hostname);
		else
			websWrite(wp, T("<tr><td><br /></td>"));
		websWrite(wp, T("<td>%02X"), lease.mac[0]);
		for (i = 1; i < 6; i++)
			websWrite(wp, T(":%02X"), lease.mac[i]);
		expires = ntohl(lease.expires);
		websWrite(wp, T("</td><td>%s</td><td>"), inet_ntoa(addr));
		d = expires / (24*60*60); expires %= (24*60*60);
		h = expires / (60*60); expires %= (60*60);
		m = expires / 60; expires %= 60;
		if (d) websWrite(wp, T("%u days "), d);
		websWrite(wp, T("%02u:%02u:%02u\n"), h, m, (unsigned)expires);
	}
	fclose(fp);
	return 0;
}


/*
 * description: write DHCP client list for Herouter
 */

static int getDhcpCliList4He(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int i;
	struct in_addr addr;

	//comment it for no valid: doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		CHECK_EMPTY_MAC(lease.mac);
		websWrite(wp, T("<dl>"));
		websWrite(wp, T("<dt></dt>"));
		
		if (strlen(lease.hostname) > 0)
			websWrite(wp, T("<dd>%-16s</dd>"), lease.hostname);
		else
			websWrite(wp, T("<dd><br /></dd>"));
		websWrite(wp, T("<dd>%02X"), lease.mac[0]);
		for (i = 1; i < 6; i++)
			websWrite(wp, T(":%02X"), lease.mac[i]);
		addr.s_addr = lease.ip;
		websWrite(wp, T("</dd><dd>%s</dd>"), inet_ntoa(addr));
		
		websWrite(wp, T("</dl>"));

	}
	fclose(fp);
	return 0;
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strncmp(buf, "nameserver", 10) != 0)
			continue;
		sscanf(buf, "%s%s", ns_str, dns);
		idx++;
		if (idx == req)
			break;
	}
	fclose(fp);

	return websWrite(wp, T("%s"), dns);
}

/*
 * arguments: 
 * description: return 1 if hostname is supported
 */
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GA_HOSTNAME_SUPPORT
	ejSetResult(eid, "1");
#else
	ejSetResult(eid, "0");
#endif
	return 0;
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc dosen't exist, 1 = ifc exists
 */
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char exist[2] = "0";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	exist[0] = (getIfLive(name) == 0)? '1' : '0';
	return websWrite(wp, T("%s"), exist);
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc is down, 1 = ifc is up
 */
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char up[2] = "1";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	up[0] = (getIfIsUp(name) == 1)? '1' : '0';
	return websWrite(wp, T("%s"), up);
}

static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_NF_CONNTRACK_PPTP || defined CONFIG_NF_CONNTRACK_PPTP_MODULE || \
    defined CONFIG_IP_NF_PPTP        || defined CONFIG_IP_NF_PPTP_MODULE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined CONFIG_USB) || (defined CONFIG_MMC)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_SIT_6RD)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6DSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_TUNNEL)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_PROFTPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_SAMBA
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getiTunesBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_MTDAAPD 
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ZEBRA
    return websWrite(wp, T("1"));
#else
    return websWrite(wp, T("0"));
#endif
}

static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RALINKAPP_SWQOS || defined CONFIG_RALINKAPP_HWQOS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif

}

static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_DATE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_INADYN
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ETHTOOL
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int get3GBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_3G
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPktFilterBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_IP_NF_FILTER
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getLanIfName(), if_addr)) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getLanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write WiFi MAC address accordingly
 */
static int getWiFiMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];
	char *ifname;

	if (ejArgs(argc, argv, T("%s"), &ifname) < 1) {
		return 0;
	}
	if (-1 == getIfMac(ifname, if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return LAN interface name (default)
 *                   1 = write LAN interface name
 * description: return or write LAN interface name accordingly
 */
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getLanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (-1 == getIfNetmask(getLanIfName(), if_net)) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE || \
	(defined CONFIG_GE1_RGMII_AN && defined CONFIG_GE2_RGMII_AN) || \
	(defined CONFIG_GE1_RGMII_FORCE_1000 && defined CONFIG_GE2_INTERNAL_GPHY) || \
	(defined CONFIG_GE1_TRGMII_FORCE_1200 && defined CONFIG_GE2_INTERNAL_GPHY)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DNSMASQ
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_LLTD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RPPPPOE_RELAY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined CONFIG_USER_UPNP_IGD) || (defined CONFIG_USER_MINIUPNPD)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RADVD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getWanIfNamePPP(), if_addr)) {
		//websError(wp, 500, T("getWanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}


/*
 * description: get Wan Port Status
 */
static int getWanPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	char cableResult[8] = {0};

  strcpy(cableResult,herouterStatus.wanPortStatus);

	return websWrite(wp, T("%s"), cableResult);
}
static int getPingStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	printf("getPingStatus : %s\n",herouterStatus.pingStatus);
	return websWrite(wp, T("%s"), herouterStatus.pingStatus);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getWanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}




/*
 * description: get FW version
 */
static int getSoftwareVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	char FWVersion[32] = {0};

  sprintf(FWVersion, "%s", hertUtil_getFirmwareVersion()); 

	return websWrite(wp, T("%s"), FWVersion);
}

static int getSysTime(int eid, webs_t wp, int argc, char_t **argv)
{
	char systime[32] = {0};

       sprintf(systime, "%s", hertUtil_GetDateTime()); 

	return websWrite(wp, T("%s"), systime);
}

static void setSysTime(webs_t wp, char_t *path, char_t *query)
{
	char *date, *tmp;
	int year = 0;
	int month = 0;
	int day = 0; 
	int hour = 0;
	int minute = 0;

	char *tz, *ntpServer, *ntpSync;
	char *setMode;

	setMode = websGetVar(wp, T("time_radio"), T(""));

	doSystem("killall ntpclient");

	if (strcmp(setMode, "0") == 0) // manual
	{
		nvram_bufset(RT2860_NVRAM, "timeSetMode", "0");
		date = websGetVar(wp, T("ymd"), T(""));
		tmp = websGetVar(wp, T("hour"), T(""));
		hour = atoi(tmp);
		tmp = websGetVar(wp, T("minute"), T(""));
		minute = atoi(tmp);
	
       	sscanf(date, "%04d-%02d-%02d", &year, &month, &day);

		hertUtil_SetDateTime(year, month, day, hour, minute);
	}
	else if (strcmp(setMode, "1") == 0) // NTP
	{
		nvram_bufset(RT2860_NVRAM, "timeSetMode", "1");
		
		tz = "CST_008";
		ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));
		ntpSync = websGetVar(wp, T("NTPSync"), T(""));

		if(!ntpServer || !ntpSync)
			return;

		if(!strlen(ntpServer)){
			// user choose to make  NTP server disable
			nvram_bufset(RT2860_NVRAM, "NTPServerIP", "");
			nvram_bufset(RT2860_NVRAM, "NTPSync", "");
		}else{
			if(checkSemicolon(ntpServer))
				return;
			if(!strlen(ntpSync))
				return;
			if(atoi(ntpSync) > 300)
				return;
			nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
			nvram_bufset(RT2860_NVRAM, "NTPSync", ntpSync);
		}
		nvram_bufset(RT2860_NVRAM, "TZ", tz);
		nvram_commit(RT2860_NVRAM);

		doSystem("ntp.sh");
	}
	
	websRedirect(wp, "herouter/systemtime.asp");
}

/*
 * arguments: type - 0 = return WAN interface name (default)
 *                   1 = write WAN interface name
 * description: return or write WAN interface name accordingly
 */
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getWanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];
	const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");

	if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
			|| !strncmp(cm, "3G", 3)
#endif
	){ //fetch ip from ppp0
		if (-1 == getIfNetmask("ppp0", if_net)) {
			return websWrite(wp, T(""));
		}
	}
	else if (-1 == getIfNetmask(getWanIfName(), if_net)) {
		//websError(wp, 500, T("getWanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;

	char sgw[16];

	FILE *fp = fopen("/proc/net/route", "r");

	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
						&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				fclose(fp);
				return websWrite(wp, T("format error"));
			}

			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr==0 ? "" : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}
		nl++;
	}
	fclose(fp);

	if (find_default_flag == 1)
		return websWrite(wp, T("%s"), sgw);
	else
		return websWrite(wp, T(""));
}


#define DD printf("%d\n", __LINE__);fflush(stdout);

/*
 *
 */
int getIndexOfRoutingRule(char *dest, char *netmask, char *interface)
{
	int index=0;
	char *rrs, one_rule[256];
	char dest_f[32], netmask_f[32], interface_f[32];

	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return -1;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		if((getNthValueSafe(0, one_rule, ',', dest_f, sizeof(dest_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask_f, sizeof(netmask_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', interface_f, sizeof(interface_f)) == -1)){
			index++;
			continue;
		}
		//printf("@@@@@ %s %s %s\n", dest_f, netmask_f, interface_f);
		//printf("----- %s %s %s\n", dest, netmask, interface);
		if( (!strcmp(dest, dest_f)) && (!strcmp(netmask, netmask_f)) && (!strcmp(interface, interface_f))){
			return index;
		}
		index++;
	}

	return -1;
}

static void removeRoutingRule(char *dest, char *netmask, char *ifname)
{
	char cmd[1024];
	strcpy(cmd, "route del ");
	
	// host or net?
	if(!strcmp(netmask, "255.255.255.255") )
		strcat(cmd, "-host ");
	else
		strcat(cmd, "-net ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(strcmp(netmask, "255.255.255.255"))
		sprintf(cmd, "%s netmask %s", cmd, netmask);

	//interface
	sprintf(cmd, "%s dev %s ", cmd, ifname);
	doSystem(cmd);
}

void staticRoutingInit(void)
{
	int index=0;
	char one_rule[256];
	char *rrs;
	struct in_addr dest_s, gw_s, netmask_s;
	char dest[32], netmask[32], gw[32], interface[32], true_interface[32], custom_interface[32], comment[32];
	int	flgs, ref, use, metric, nl=0;
	unsigned long int d,g,m;
	int isGatewayMode = (!strcmp("1", nvram_bufget(RT2860_NVRAM, "OperationMode"))) ? 1 : 0 ;

	// delete old user rules
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return;

	while (fgets(one_rule, sizeof(one_rule), fp) != NULL) {
		if (nl) {
			if (sscanf(one_rule, "%s%lx%lx%X%d%d%d%lx",
					interface, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				fclose(fp);
				return;
			}
			dest_s.s_addr = d;
			gw_s.s_addr = g;
			netmask_s.s_addr = m;

			strncpy(dest, inet_ntoa(dest_s), sizeof(dest));
			strncpy(gw, inet_ntoa(gw_s), sizeof(gw));
			strncpy(netmask, inet_ntoa(netmask_s), sizeof(netmask));

			// check if internal routing rules
			if( (index=getIndexOfRoutingRule(dest, netmask, interface)) != -1){
				removeRoutingRule(dest, netmask, interface);
			}
		}
		nl++;
	}
	fclose(fp);

	index = 0;
	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs|| !strlen(rrs))
		return;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		char cmd[1024];

		if((getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', true_interface, sizeof(true_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(5, one_rule, ',', custom_interface, sizeof(custom_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)){
			index++;
			continue;
		}

		strcpy(cmd, "route add ");
		
		// host or net?
		if(!strcmp(netmask, "255.255.255.255") )
			strcat(cmd, "-host ");
		else
			strcat(cmd, "-net ");

		// destination
		strcat(cmd, dest);
		strcat(cmd, " ");

		// netmask
		if(strcmp(netmask, "255.255.255.255") )
			sprintf(cmd, "%s netmask %s", cmd, netmask);

		// gateway
		if(strlen(gw) && strcmp(gw, "0.0.0.0"))
			sprintf(cmd, "%s gw %s", cmd, gw);

		//interface
//		if (!strcmp(interface, "WAN")){
//			true_interface = getWanIfName();
//		}else if (!gstrcmp(interface, "Custom")){
//			true_interface = custom_interface;
//		}else	// LAN & unknown
//			true_interface = getLanIfName();

		sprintf(cmd, "%s dev %s ", cmd, true_interface);

		strcat(cmd, "2>&1 ");

		if(strcmp(interface, "WAN") || (!strcmp(interface, "WAN") && isGatewayMode)  ){
			doSystem(cmd);
		}else{
			printf("Skip WAN routing rule in the non-Gateway mode: %s\n", cmd);
		}

		index++;
	}
	return;
}

void dynamicRoutingInit(void)
{
	zebraRestart();
	ripdRestart();
}

void RoutingInit(void)
{
	staticRoutingInit();
	dynamicRoutingInit();
}

static inline int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;

    if(!pos || !strlen(pos))
        return 0;
    while( (pos = strchr(pos, delimit))){
        pos = pos+1;
        count++;
    }
    return count;
}

/*
 * description: get routing table
 */
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{
	char   result[4096] = {0};
	char   buff[512];
	int    nl = 0, index;
	char   ifname[32], interface[128];
	struct in_addr dest, gw, netmask;
	char   dest_str[32], gw_str[32], netmask_str[32], comment[32];
	int    flgs, ref, use, metric;
	int	   *running_rules = NULL;
	unsigned long int d,g,m;
	char *rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	int  rule_count;
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return -1;

	rule_count = getNums(rrs, ';');

	if(rule_count){
		running_rules = calloc(1, sizeof(int) * rule_count);
		if(!running_rules) {
			fclose(fp);
			return -1;
		}
	}
		
	strncat(result, "\"", sizeof(result));
	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			if (sscanf(buff, "%s%lx%lx%X%d%d%d%lx",
					ifname, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				free(running_rules);
				fclose(fp);
				return websWrite(wp, T(""));
			}
			dest.s_addr = d;
			gw.s_addr = g;
			netmask.s_addr = m;

			if(! (flgs & 0x1) )	// skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(netmask), sizeof(netmask_str));

			if(nl > 1)
				strncat(result, ";", sizeof(result));
			strncat(result, ifname, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, dest_str, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, gw_str, sizeof(result));			strncat(result, ",", sizeof(result));
			strncat(result, netmask_str, sizeof(result) );	strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%d,%d,%d,%d,", result, flgs, ref, use, metric);

			// check if internal routing rules
			strcpy(comment, " ");
			if( (index=getIndexOfRoutingRule(dest_str, netmask_str, ifname)) != -1){
				char one_rule[256];

				if(index < rule_count)
					running_rules[index] = 1;
				else
					printf("fatal error in %s\n", __FUNCTION__);

				snprintf(result, sizeof(result), "%s%d,", result, index);
				if(rrs && strlen(rrs)){
					if( getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) != -1){

						if( getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) != -1){
							strncat(result, interface, sizeof(result));
							strncat(result, ",", sizeof(result));
						}
						if( getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) != -1){
							// do nothing;
						}
					}
				}
			}else{
				strncat(result, "-1,", sizeof(result));
				strncat(result, getLanWanNamebyIf(ifname), sizeof(result));
				strncat(result, ",", sizeof(result));
			}
			strncat(result, "0,", sizeof(result));	// used rule
			strncat(result, comment, sizeof(result));
		}
		nl++;
	}

	for(index=0; index < rule_count; index++){
		char one_rule[256];

		if(running_rules[index])
			continue;

		if(getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) == -1)
			continue;

		if(getNthValueSafe(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
			continue;

		if(getNthValueSafe(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
			continue;

		if(getNthValueSafe(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
			continue;

		if(getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)
			continue;

		if(getNthValueSafe(4, one_rule, ',', ifname, sizeof(ifname)) == -1)
			continue;

		if(getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)
			continue;

		if(strlen(result))
			strncat(result, ";", sizeof(result));

		snprintf(result, sizeof(result), "%s%s,%s,%s,%s,0,0,0,0,%d,%s,1,%s", result, ifname, dest_str, gw_str, netmask_str, index, interface, comment);
	}

	strcat(result, "\"");
	websLongWrite(wp, result);
	fclose(fp);
	if(running_rules)
		free(running_rules);
	//printf("%s\n", result);
	return 0;
}

static void addRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *dest, *hostnet, *netmask, *gateway, *interface, *true_interface, *custom_interface, *comment;
	char cmd[256] = {0};
	char result[256] = {0};

	FILE *fp;

	dest = websGetVar(wp, T("dest"), T(""));
	hostnet = websGetVar(wp, T("hostnet"), T(""));
	netmask = websGetVar(wp, T("netmask"), T(""));	
	gateway = websGetVar(wp, T("gateway"), T(""));
	interface = websGetVar(wp, T("interface"), T(""));
	custom_interface = websGetVar(wp, T("custom_interface"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));

	if( !dest)
		return;

	strcat(cmd, "route add ");
	
	// host or net?
	if(!gstrcmp(hostnet, "net"))
		strcat(cmd, "-net ");
	else
		strcat(cmd, "-host ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(gstrlen(netmask))
		sprintf(cmd, "%s netmask %s", cmd, netmask);
	else
		netmask = "255.255.255.255";

	//gateway
	if(gstrlen(gateway))
		sprintf(cmd, "%s gw %s", cmd, gateway);
	else
		gateway = "0.0.0.0";

	//interface
	if(gstrlen(interface)){
		if (!gstrcmp(interface, "WAN")){
			true_interface = getWanIfName();
		}else if (!gstrcmp(interface, "Custom")){
			if(!gstrlen(custom_interface))
				return;
			true_interface = custom_interface;
		}else	// LAN & unknown
			true_interface = getLanIfName();
	}else{
		interface = "LAN";
		true_interface = getLanIfName();
	}
	sprintf(cmd, "%s dev %s ", cmd, true_interface);

	strcat(cmd, "2>&1 ");

	printf("%s\n", cmd);
	fp = popen(cmd, "r");
	fgets(result, sizeof(result), fp);
	pclose(fp);


	if(!strlen(result)){
		// success, write down to the flash
		char tmp[1024];
		const char *rrs = nvram_bufget(RT2860_NVRAM, "RoutingRules");
		if(!rrs || !strlen(rrs)){
			memset(tmp, 0, sizeof(tmp));
		}else{
			strncpy(tmp, rrs, sizeof(tmp));
		}
		if(strlen(tmp))
			strcat(tmp, ";");
		sprintf(tmp, "%s%s,%s,%s,%s,%s,%s,%s", tmp, dest, netmask, gateway, interface, true_interface, custom_interface, comment);
		nvram_bufset(RT2860_NVRAM, "RoutingRules", tmp);
		nvram_commit(RT2860_NVRAM);
	}else{
		websHeader(wp);		
		websWrite(wp, T("<h1>Add routing failed:<br> %s<h1>"), result);
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>Add routing table:</h3><br>\n"));
	if(strlen(result)){
		websWrite(wp, T("Success"));
	}else
		websWrite(wp, T("%s"), result);

	websWrite(wp, T("Destination: %s<br>\n"), dest);
	websWrite(wp, T("Host/Net: %s<br>\n"), hostnet);
	websWrite(wp, T("Netmask: %s<br>\n"), netmask);
	websWrite(wp, T("Gateway: %s<br>\n"), gateway);
	websWrite(wp, T("Interface: %s<br>\n"), interface);
	websWrite(wp, T("True Interface: %s<br>\n"), true_interface);
	if(strlen(custom_interface))
		websWrite(wp, T("Custom_interface %s<br>\n"), custom_interface);
	websWrite(wp, T("Comment: %s<br>\n"), comment);
	websFooter(wp);
	websDone(wp, 200);
}

static void delRouting(webs_t wp, char_t *path, char_t *query)
{
	int index, rule_count;
	char_t *value, dest[256], netmask[256], true_interface[256];
	char name_buf[16] = {0};
	char *rrs;
	char *new_rrs;
	int *deleArray, j=0;
	
	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return;

	rule_count = getNums(rrs, ';');
	if(!rule_count)
		return;

	if(!(deleArray = malloc(sizeof(int) * rule_count) ) )
		return;

	if(! (new_rrs = strdup(rrs))){
		free(deleArray);
		return;
	}

	websHeader(wp);

	for(index=0; index< rule_count; index++){
		snprintf(name_buf, sizeof(name_buf), "DR%d", index);
		value = websGetVar(wp, name_buf, NULL);
		if(value){
			deleArray[j++] = index;
			if(strlen(value) > 256)
				continue;
			sscanf(value, "%s%s%s", dest, netmask, true_interface);
			removeRoutingRule(dest, netmask, true_interface);
			websWrite(wp, T("Delete entry: %s,%s,%s<br>\n"), dest, netmask, true_interface);
		}
	}

	if(j>0){
		deleteNthValueMulti(deleArray, j, new_rrs, ';');
		nvram_bufset(RT2860_NVRAM, "RoutingRules", new_rrs);
		nvram_commit(RT2860_NVRAM);
	}

	websFooter(wp);
	websDone(wp, 200);

	free(deleArray);
	free(new_rrs);
}

void ripdRestart(void)
{
	char lan_ip[16], wan_ip[16], lan_mask[16], wan_mask[16];

	const char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char *password = nvram_bufget(RT2860_NVRAM, "Password");
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q ripd");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
        return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/ripd.conf ");
	doSystem("echo \"password %s\" >> /etc/ripd.conf ", password);
	doSystem("echo \"router rip\" >> /etc/ripd.conf ");

	// deal with WAN
	if(getIfIp(getWanIfName(), wan_ip) != -1){
		if(getIfNetmask(getWanIfName(), wan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", wan_ip, netmask_aton(wan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getWanIfName());
		}else
			printf("ripdRestart(): The WAN IP is still undeterminated...\n");
	}else
		printf("ripdRestart(): The WAN IP is still undeterminated...\n");

	// deal with LAN
	if(getIfIp(getLanIfName(), lan_ip) != -1){
		if(getIfNetmask(getLanIfName(), lan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", lan_ip, netmask_aton(lan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getLanIfName());
		}
	}
	doSystem("echo \"version 2\" >> /etc/ripd.conf");
	doSystem("echo \"log syslog\" >> /etc/ripd.conf");
	doSystem("ripd -f /etc/ripd.conf -d");
}

inline void zebraRestart(void)
{
	const char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char *password = nvram_bufget(RT2860_NVRAM, "Password");
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q zebra");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
		return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/zebra.conf ");
	doSystem("echo \"password %s\" >> /etc/zebra.conf ", password);
	doSystem("echo \"enable password rt2880\" >> /etc/zebra.conf ");
	doSystem("echo \"log syslog\" >> /etc/zebra.conf ");
	doSystem("zebra -d -f /etc/zebra.conf");
}

static void dynamicRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *rip;
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");
	rip = websGetVar(wp, T("RIPSelect"), T(""));
	if(!rip || !strlen(rip))
		return;

	if(!RIPEnable || !strlen(RIPEnable))
		RIPEnable = "0";

	if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "0")){
		// nothing changed
	}else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "1")){
		// nothing changed
	}else if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "1")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		doSystem("killall -q ripd");
		doSystem("killall -q zebra");
	}else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "0")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		zebraRestart();
		ripdRestart();
	}else{
		return;
	}

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>Dynamic Routing:</h3><br>\n"));
	websWrite(wp, T("RIPEnable %s<br>\n"), rip);
	websFooter(wp);
	websDone(wp, 200);
}

int flash_read_mac(char *buf)
{
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
#if ! defined (NO_WIFI_SOC)
	lseek(fd, 0x2E, SEEK_SET);
#else
	lseek(fd, 0xE006, SEEK_SET);
#endif
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}


/*
 * description: setup internet according to nvram configurations
 *              (assume that nvram_init has already been called)
 *              return value: 0 = successful, -1 = failed
 */
int initInternet(void)
{
#ifndef CONFIG_RALINK_RT2880
	const char *auth_mode = nvram_bufget(RT2860_NVRAM, "AuthMode");
#endif
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	const char *opmode;
#endif
/*****************set SSID****************/	
	char *isTheFirstStart = nvram_bufget(RT2860_NVRAM, "IS_THE_FIRSTTIME_START");
	if(!strcmp(isTheFirstStart,"1"))
	{	
		char if_mac[32];
		char Strssid[128];
#if 0
		char SsidPwd[128];

		memset(SsidPwd, 0x0, sizeof(SsidPwd));
#endif
		memset(Strssid, 0x0, sizeof(Strssid));
		memset(if_mac, 0x0, sizeof(if_mac));
		flash_read_mac(if_mac);
		printf("if_mac = %s\n", if_mac);
		
		/* MAIN SSID configuration */
		sprintf(Strssid,"CMCC_%02X%02X%02X",0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5]);
		nvram_bufset(RT2860_NVRAM, "SSID1", Strssid);
		printf("MAIN SSID = %s\n", Strssid);
#if 0
		/* HIDE SSID configuration */
		strcat(Strssid,"hid");
		nvram_bufset(RT2860_NVRAM, "SSID2", Strssid);
		printf("HIDE SSID = %s\n", Strssid);

		/* HIDE SSID PSWD configuration */
		websEncode64(SsidPwd, Strssid, sizeof(SsidPwd));
		if (strlen(SsidPwd) <= 8)
		{
			nvram_bufset(RT2860_NVRAM, "WPAPSK2", SsidPwd);
			printf("WPAPSK2 = %s\n", SsidPwd);
		}
		else
		{
			char *pLeft8Chr = SsidPwd + strlen(SsidPwd) - 8;
			nvram_bufset(RT2860_NVRAM, "WPAPSK2", pLeft8Chr);
			printf("WPAPSK2 = %s\n", pLeft8Chr);
		}
#endif
		nvram_bufset(RT2860_NVRAM, "IS_THE_FIRSTTIME_START", "0");
		nvram_commit(RT2860_NVRAM);
	}
	
/***********set end************************/
	system("cp /var/udhcpd.leases /var/bak_udhcpd.leases");

	doSystem("internet.sh");

	system("cp /var/bak_udhcpd.leases /var/udhcpd.leases");

#if defined (CONFIG_IPV6)
	ipv6Config(strtol(nvram_bufget(RT2860_NVRAM, "IPv6OpMode"), NULL, 10));
#endif

	//automatically connect to AP according to the active profile
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	if (!strcmp(opmode, "2") || (!strcmp(opmode, "0") &&
				!strcmp("1", nvram_bufget(RT2860_NVRAM, "ethConvert")))) {
		if (-1 != initStaProfile())
			initStaConnection();
	}
#endif

#if !defined(CONFIG_RALINK_RT2880) && !defined(CONFIG_RALINK_MT7620)
	if (!strcmp(auth_mode, "Disable") || !strcmp(auth_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)
#endif

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	restartWAPIDaemon();	// in wireless.c
#endif
#if ! defined (CONFIG_FIRST_IF_NONE) 
	restart8021XDaemon(RT2860_NVRAM);	// in wireless.c
#endif
#if ! defined (CONFIG_SECOND_IF_NONE)
	restart8021XDaemon(RTDEV_NVRAM);	// in wireless.c
#endif

#ifdef CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY
	AntennaDiversityInit();
#endif

	firewall_init();
	management_init();
	RoutingInit();
#ifdef CONFIG_RALINKAPP_SWQOS
	QoSInit();
#endif
#ifdef CONFIG_RALINKAPP_HWQOS
	QoSInit();
#endif

	return 0;
}

static void getMyMAC(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), myMAC);
	websDone(wp, 200);
}

#ifdef DARE_CUSTOMER_WEB

/* goform/dhcpbanding */
static void dhcpbanding(webs_t wp, char_t *path, char_t *query)
{
	char_t *dhcp_tp, *startip, *endip, *timelease, *lease, *static1, *static2, *static3;
	int isdhcp;
	dhcp_tp = websGetVar(wp, T("dhcphidden"), T(""));
	startip = websGetVar(wp, T("starthidden"), T(""));
	endip = websGetVar(wp, T("endhidden"), T(""));
	timelease = websGetVar(wp, T("realtime"), T(""));
	static1 = websGetVar(wp, T("static1"), T(""));
	static2 = websGetVar(wp, T("static2"), T(""));
	static3 = websGetVar(wp, T("static3"), T(""));
	isdhcp = atoi(dhcp_tp);
	
	if(isdhcp)
	{
		if (-1 == inet_addr(startip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Start IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpStart", startip);
		if (-1 == inet_addr(endip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP End IP");
			return;
		}
		
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", endip);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		nvram_bufset(RT2860_NVRAM, "dhcpLease", timelease);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic1", static1);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic2", static2);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic3", static3);
		
		
		
	}
	else
	{
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
	}
	initInternet();
	websRedirect(wp, "herouter/DHCPbanding.asp");
}
#define STRSTR(s,temp)\ 
while(*(s))\
{\
	 if(*(s) == '.')\
		(temp)++;\
	if(temp == 3)\
	{\		
		*(s) = '\0';\
		break;\
	}\
	(s)++;\
}
/* goform/entconfig */
static void entconfig(webs_t wp, char_t *path, char_t *query)
{
	char_t *lanip,*s;
	char cmd[128],dhcp_start[16],dhcp_end[16];
	int temp = 0 ;
	
	s = lanip = websGetVar(wp, T("entconfiglan"), T(""));
	const char *nowlanip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	printf("[%s,%d]: ########start entconfig ip==%s nowlanip===%s########\n",
			__FUNCTION__, __LINE__, lanip,nowlanip);
	if(0 == strcmp(nowlanip,lanip))
	{
		printf("[%s,%d]: no need do follow\n", __FUNCTION__, __LINE__);
		return;
	}
	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", lanip);
	nvram_bufset(RT2860_NVRAM, "dhcpPriDns", lanip);
	nvram_bufset(RT2860_NVRAM, "dhcpGateway", lanip);
	
	STRSTR(s,temp);
	memset(&dhcp_start,0x0,sizeof(dhcp_start));
	memset(&dhcp_end,0x0,sizeof(dhcp_end));
	sprintf(dhcp_start, "%s.%d",lanip,100);	
	sprintf(dhcp_end, "%s.%d",lanip,200);

	nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_start);
	nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_end);
	
	nvram_commit(RT2860_NVRAM);

	initInternet();
/*
	system("killall dnsd");
	
	memset(cmd, 0x0, sizeof(cmd));
	sprintf(cmd, "echo \"www.heluyou.com $ip\" > /etc/dnsd.conf", lanip);
	//printf("[%s,%d]: cmd(%s)\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
	
	memset(cmd, 0x0, sizeof(cmd));
	sprintf(cmd, "dnsd -i %s &", lanip);
	//printf("[%s,%d]: cmd(%s)\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
*/
	system("killall dnsmasq");
	
	memset(cmd, 0x0, sizeof(cmd));
	sprintf(cmd, "dnsmasq --listen-address=%s --address=/www.heluyou.com/%s &", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"),nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
	printf("internet :%s\n",cmd);
	system(cmd);

	//websRedirect(wp, "herouter/main_fake.asp");
}
#endif

/* goform/setLan */
static void setLan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ip, *nm, *dhcp_tp, *stp_en, *lltd_en, *igmp_en, *upnp_en,
			*radvd_en, *pppoer_en, *dnsp_en;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL;
	char_t *lan2enabled, *lan2_ip, *lan2_nm;
#ifdef GA_HOSTNAME_SUPPORT
	char_t	*host;
#endif
	char_t  *dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l;
	char_t	*dhcp_sl1, *dhcp_sl2, *dhcp_sl3;
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
	const char	*ctype = nvram_bufget(RT2860_NVRAM, "connectionType");

	ip = websGetVar(wp, T("lanIp"), T(""));
	nm = websGetVar(wp, T("lanNetmask"), T(""));
	lan2enabled = websGetVar(wp, T("lan2enabled"), T(""));
	lan2_ip = websGetVar(wp, T("lan2Ip"), T(""));
	lan2_nm = websGetVar(wp, T("lan2Netmask"), T(""));
#ifdef GA_HOSTNAME_SUPPORT
	host = websGetVar(wp, T("hostname"), T("0"));
#endif
	dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
	stp_en = websGetVar(wp, T("stpEnbl"), T("0"));
	lltd_en = websGetVar(wp, T("lltdEnbl"), T("0"));
	igmp_en = websGetVar(wp, T("igmpEnbl"), T("0"));
	upnp_en = websGetVar(wp, T("upnpEnbl"), T("0"));
	radvd_en = websGetVar(wp, T("radvdEnbl"), T("0"));
	pppoer_en = websGetVar(wp, T("pppoeREnbl"), T("0"));
	dnsp_en = websGetVar(wp, T("dnspEnbl"), T("0"));
	dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
	dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
	dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
	dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
	dhcp_sd = websGetVar(wp, T("dhcpSecDns"), T(""));
	dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
	dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));
	dhcp_sl1 = websGetVar(wp, T("dhcpStatic1"), T(""));
	dhcp_sl2 = websGetVar(wp, T("dhcpStatic2"), T(""));
	dhcp_sl3 = websGetVar(wp, T("dhcpStatic3"), T(""));

	/*
	 * check static ip address:
	 * lan and wan ip should not be the same except in bridge mode
	 */
	if (strncmp(ctype, "STATIC", 7)) {
		if (strcmp(opmode, "0") && !strncmp(ip, wan_ip, 15)) {
			websError(wp, 200, "IP address is identical to WAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			if (strcmp(opmode, "0") && !strncmp(lan2_ip, wan_ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to WAN");
				return;
			}
			else if (strcmp(opmode, "0") && !strncmp(lan2_ip, ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to LAN1");
				return;
			}
		}
	}
	// configure gateway and dns (WAN) at bridge mode
	if (!strncmp(opmode, "0", 2)) {
		gw = websGetVar(wp, T("lanGateway"), T(""));
		pd = websGetVar(wp, T("lanPriDns"), T(""));
		sd = websGetVar(wp, T("lanSecDns"), T(""));
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}
	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
	nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
	nvram_bufset(RT2860_NVRAM, "Lan2Enabled", lan2enabled);
	nvram_bufset(RT2860_NVRAM, "lan2_ipaddr", lan2_ip);
	nvram_bufset(RT2860_NVRAM, "lan2_netmask", lan2_nm);
#ifdef GA_HOSTNAME_SUPPORT
	nvram_bufset(RT2860_NVRAM, "HostName", host);
#endif
	if (!strncmp(dhcp_tp, "DISABLE", 8))
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
	else if (!strncmp(dhcp_tp, "SERVER", 7)) {
		if (-1 == inet_addr(dhcp_s)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Start IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_s);
		if (-1 == inet_addr(dhcp_e)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP End IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_e);
		if (-1 == inet_addr(dhcp_m)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpMask", dhcp_m);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		nvram_bufset(RT2860_NVRAM, "dhcpPriDns", dhcp_pd);
		nvram_bufset(RT2860_NVRAM, "dhcpSecDns", dhcp_sd);
		nvram_bufset(RT2860_NVRAM, "dhcpGateway", dhcp_g);
		nvram_bufset(RT2860_NVRAM, "dhcpLease", dhcp_l);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic1", dhcp_sl1);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic2", dhcp_sl2);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic3", dhcp_sl3);
	}
	nvram_bufset(RT2860_NVRAM, "stpEnabled", stp_en);
	nvram_bufset(RT2860_NVRAM, "lltdEnabled", lltd_en);
	nvram_bufset(RT2860_NVRAM, "igmpEnabled", igmp_en);
	nvram_bufset(RT2860_NVRAM, "upnpEnabled", upnp_en);
	nvram_bufset(RT2860_NVRAM, "radvdEnabled", radvd_en);
	nvram_bufset(RT2860_NVRAM, "pppoeREnabled", pppoer_en);
	nvram_bufset(RT2860_NVRAM, "dnsPEnabled", dnsp_en);
	nvram_commit(RT2860_NVRAM);

	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>LAN Interface Setup</h3><br>\n"));
#ifdef GA_HOSTNAME_SUPPORT
	websWrite(wp, T("Hostname: %s<br>\n"), host);
#endif
	websWrite(wp, T("IP: %s<br>\n"), ip);
	websWrite(wp, T("Netmask: %s<br>\n"), nm);
	websWrite(wp, T("LAN2 Enabled: %s<br>\n"), lan2enabled);
	websWrite(wp, T("LAN2 IP: %s<br>\n"), lan2_ip);
	websWrite(wp, T("LAN2 Netmask: %s<br>\n"), lan2_nm);
	if (!strncmp(opmode, "0", 2)) {
		websWrite(wp, T("Gateway: %s<br>\n"), gw);
		websWrite(wp, T("PriDns: %s<br>\n"), pd);
		websWrite(wp, T("SecDns: %s<br>\n"), sd);
	}
	websWrite(wp, T("DHCP type: %s<br>\n"), dhcp_tp);
	if (strncmp(dhcp_tp, "DISABLE", 8)) {
		websWrite(wp, T("--> DHCP start: %s<br>\n"), dhcp_s);
		websWrite(wp, T("--> DHCP end: %s<br>\n"), dhcp_e);
		websWrite(wp, T("--> DHCP mask: %s<br>\n"), dhcp_m);
		websWrite(wp, T("--> DHCP DNS: %s %s<br>\n"), dhcp_pd, dhcp_sd);
		websWrite(wp, T("--> DHCP gateway: %s<br>\n"), dhcp_g);
		websWrite(wp, T("--> DHCP lease: %s<br>\n"), dhcp_l);
		websWrite(wp, T("--> DHCP static 1: %s<br>\n"), dhcp_sl1);
		websWrite(wp, T("--> DHCP static 2: %s<br>\n"), dhcp_sl2);
		websWrite(wp, T("--> DHCP static 3: %s<br>\n"), dhcp_sl3);
	}
	websWrite(wp, T("STP enable: %s<br>\n"), stp_en);
	websWrite(wp, T("LLTD enable: %s<br>\n"), lltd_en);
	websWrite(wp, T("IGMP proxy enable: %s<br>\n"), igmp_en);
	websWrite(wp, T("UPNP enable: %s<br>\n"), upnp_en);
	websWrite(wp, T("RADVD enable: %s<br>\n"), radvd_en);
	websWrite(wp, T("DNS proxy enable: %s<br>\n"), dnsp_en);
	websFooter(wp);
	websDone(wp, 200);
}

#if defined (CONFIG_IPV6)
void ipv6Config(int mode)
{
	const char *wan_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6WANIPAddr");
#if defined (CONFIG_IPV6_SIT_6RD) || defined (CONFIG_IPV6_TUNNEL)
	const char *srv_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6SrvAddr");
#endif
	int prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6PrefixLen"), NULL, 10);
	int wan_prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6WANPrefixLen"), NULL, 10);
	const char *gw_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6GWAddr");
	char *wan_if = getWanIfName();
	char *lan_if = getLanIfName();
	char v6addr[40]; 
#if defined (CONFIG_IPV6_SIT_6RD)
	char wan_addr[16]; 
	char ipv6_ip_addr[20];
	unsigned short temp[8];
	int i, used, shift;
	char *tok = NULL;
#endif

	strcpy(v6addr, nvram_bufget(RT2860_NVRAM, "IPv6IPAddr"));
#if defined (CONFIG_IPV6_SIT_6RD)
	doSystem("ip link set 6rdtun down 1>/dev/null 2>&1");
#endif
	doSystem("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding");

	switch (mode) {
	case 1:
		doSystem("ifconfig %s add  %s/%d", lan_if, v6addr, prefix_len);
		doSystem("ifconfig %s add  %s/%d", wan_if, wan_v6addr, wan_prefix_len);
		//doSystem("route -A inet6 add default gw %s dev %s", gw_v6addr, wan_if);
		doSystem("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		// doSystem("ecmh");
		break;
#if defined (CONFIG_IPV6_SIT_6RD)
	case 2:
		if (getIfIp(getWanIfNamePPP(), wan_addr) < 0) {
			fprintf(stderr, "Can't Query WAN IPv4 Address!\n");
			return;
		}
		memset(temp, 0, sizeof(temp));
		doSystem("ip tunnel add 6rdtun mode sit local %s ttl 64", wan_addr);
		for (i=0, tok = strtok(v6addr, ":"); tok; i++, tok = strtok(NULL, ":"))
			temp[i] = strtol(tok, NULL, 16);
		if ((shift = 16 - (prefix_len % 16)) < 16) {
			temp[i-1] = (temp[i-1] >> shift) << shift;
		}
		sprintf(v6addr, "%x", temp[0]);
		for (used=1; used<i; used++)
			sprintf(v6addr, "%s:%x", v6addr, temp[used]);
		for (tok = strtok(wan_addr, "."); tok; i++, tok = strtok(NULL, ".")) {
			temp[i] = strtol(tok, NULL, 10)<<8;
			tok = strtok(NULL, ".");
			temp[i] += strtol(tok, NULL, 10);
		}
		if (shift < 16) {
			used = prefix_len / 16;
			while (used < i) {
				temp[used] = (temp[used] >> shift) << shift; 
				temp[used] += temp[used+1]>>(16-shift);
				temp[used+1] <<= shift;
				used++;
			}
		} else {
			used = i;
		}
		sprintf(ipv6_ip_addr, "%x", temp[0]);
		for (i=1; i<used; i++) {
			sprintf(ipv6_ip_addr, "%s:%x", ipv6_ip_addr, temp[i]);
		}
		doSystem("ip tunnel 6rd dev 6rdtun 6rd-prefix %s::/%d", v6addr, prefix_len);
		doSystem("ip addr add %s::1/%d dev 6rdtun", ipv6_ip_addr, prefix_len);
		doSystem("ip link set 6rdtun up");
		doSystem("ip route add ::/0 via ::%s dev 6rdtun", srv_v6addr);
		doSystem("ip addr add %s::1/64 dev %s", ipv6_ip_addr, lan_if);
		doSystem("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		doSystem("radvd.sh %s", ipv6_ip_addr);
		break;
#endif
#if defined (CONFIG_IPV6_TUNNEL)
	case 3:
		doSystem("config-dslite.sh %s %s %s", srv_v6addr, v6addr, gw_v6addr);
		break;
#endif
	default:
		break;
	}
	return;
}

/* goform/setIPv6 */
static void setIPv6(webs_t wp, char_t *path, char_t *query)
{
	char_t	*opmode;
	char_t  *ipaddr, *prefix_len, *wan_ipaddr, *wan_prefix_len, *srv_ipaddr;
#if defined (CONFIG_IPV6_TUNNEL)
	char_t  *gw_ipaddr = NULL;
#endif

	ipaddr = prefix_len = wan_ipaddr = wan_prefix_len = srv_ipaddr = NULL;
	opmode = websGetVar(wp, T("ipv6_opmode"), T("0"));
	if (!strcmp(opmode, "1")) {
		ipaddr = websGetVar(wp, T("ipv6_lan_ipaddr"), T(""));
		prefix_len = websGetVar(wp, T("ipv6_lan_prefix_len"), T(""));
		wan_ipaddr = websGetVar(wp, T("ipv6_wan_ipaddr"), T(""));
		wan_prefix_len = websGetVar(wp, T("ipv6_wan_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_static_gw"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6WANIPAddr", wan_ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6WANPrefixLen", wan_prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", srv_ipaddr);
#if defined (CONFIG_IPV6_SIT_6RD)
	} else if (!strcmp(opmode, "2")) {
		ipaddr = websGetVar(wp, T("ipv6_6rd_prefix"), T(""));
		prefix_len = websGetVar(wp, T("ipv6_6rd_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_6rd_border_ipaddr"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
		nvram_bufset(RT2860_NVRAM, "radvdEnabled", "1");
#endif
#if defined (CONFIG_IPV6_TUNNEL)
	} else if (!strcmp(opmode, "3")) {
		ipaddr = websGetVar(wp, T("ipv6_ds_wan_ipaddr"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_ds_aftr_ipaddr"), T(""));
		gw_ipaddr = websGetVar(wp, T("ipv6_ds_gw_ipaddr"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", gw_ipaddr);
#endif
	}
	nvram_bufset(RT2860_NVRAM, "IPv6OpMode", opmode);
	nvram_commit(RT2860_NVRAM);
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>IPv6 Setup</h3><br>\n"));
	websWrite(wp, T("ipv6_opmode: %s<br>\n"), opmode);
	if (!strcmp(opmode, "1")) {
		websWrite(wp, T("ipv6_lan_ipaddr: %s<br>\n"), ipaddr);
		websWrite(wp, T("ipv6_lan_prefix_len: %s<br>\n"), prefix_len);
		websWrite(wp, T("ipv6_wan_ipaddr: %s<br>\n"), wan_ipaddr);
		websWrite(wp, T("ipv6_wan_prefix_len: %s<br>\n"), wan_prefix_len);
		websWrite(wp, T("ipv6_static_gw: %s<br>\n"), srv_ipaddr);
#if defined (CONFIG_IPV6_SIT_6RD)
	} else if (!strcmp(opmode, "2")) {
		websWrite(wp, T("ipv6_6rd_prefix: %s<br>\n"), ipaddr);
		websWrite(wp, T("ipv6_6rd_prefix_len: %s<br>\n"), prefix_len);
		websWrite(wp, T("ipv6_6rd_border_ipaddr: %s<br>\n"), srv_ipaddr);
#endif
#if defined (CONFIG_IPV6_TUNNEL)
	} else if (!strcmp(opmode, "3")) {
		websWrite(wp, T("ipv6_ds_wan_ipaddr: %s<br>\n"), ipaddr);
		websWrite(wp, T("ipv6_ds_aftr_ipaddr: %s<br>\n"), srv_ipaddr);
		websWrite(wp, T("ipv6_ds_gw_ipaddr: %s<br>\n"), gw_ipaddr);
#endif
	}
	websFooter(wp);
	websDone(wp, 200);
}
#endif

/* goform/setVpnPaThru */
static void setVpnPaThru(webs_t wp, char_t *path, char_t *query)
{
	char_t	*l2tp_pt, *ipsec_pt, *pptp_pt;

	l2tp_pt = websGetVar(wp, T("l2tpPT"), T("0"));
	ipsec_pt = websGetVar(wp, T("ipsecPT"), T("0"));
	pptp_pt = websGetVar(wp, T("pptpPT"), T("0"));
	
	nvram_bufset(RT2860_NVRAM, "l2tpPassThru", l2tp_pt);
	nvram_bufset(RT2860_NVRAM, "ipsecPassThru", ipsec_pt);
	nvram_bufset(RT2860_NVRAM, "pptpPassThru", pptp_pt);
	nvram_commit(RT2860_NVRAM);

	doSystem("vpn-passthru.sh");

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>VPN Pass Through</h3><br>\n"));
	websWrite(wp, T("l2tp: %s<br>\n"), l2tp_pt);
	websWrite(wp, T("ipsec: %s<br>\n"), ipsec_pt);
	websWrite(wp, T("pptp: %s<br>\n"), pptp_pt);
	websFooter(wp);
	websDone(wp, 200);
}
#ifdef DARE_CUSTOMER_WEB
/* quick set up*/
static void QuickSetUp(webs_t wp, char_t *path, char_t *query)
{
	
	char_t	*ctype;
	char_t	*ip, *nm, *gw, *pd, *sd;
	char_t	*eth, *user, *pass, *hostname;
	char_t	*clone_en, *clone_mac;
	char_t  *pptp_srv, *pptp_mode;
	char_t  *l2tp_srv, *l2tp_mode;

	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

	printf("########111########\n");

	ctype = ip = nm = gw = pd = sd = eth = user = pass = hostname =
		clone_en = clone_mac = pptp_srv = pptp_mode = l2tp_srv = l2tp_mode =
		NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0")); 
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0")) {
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));

		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15)) {
				nvram_commit(RT2860_NVRAM);
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		struct in_addr addr;
		if (-1 == inet_aton(nm,&addr)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
		/*
		 * in Bridge Mode, lan and wan are bridged together and associated with
		 * the same ip address
		 */
		if (NULL != opmode && !strcmp(opmode, "0")) {
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
		}
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	} else {
		printf("########ctype(%s)########\n", ctype);
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", "");
		nvram_bufset(RT2860_NVRAM, "wan_netmask", "");
		nvram_bufset(RT2860_NVRAM, "wan_gateway", "");
		if (!strncmp(ctype, "DHCP", 5)) {
			hostname = websGetVar(wp, T("hostname"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", hostname);
		}
		else if (!strncmp(ctype, "PPPOE", 6)) {
			char *pppoe_opmode,*pppoe_optime; 
			user = websGetVar(wp, T("pppoeUser"), T(""));
			pass = websGetVar(wp, T("pppoePass"), T(""));


			pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T(""));
			if (0 == strcmp(pppoe_opmode, "OnDemand"))
			    pppoe_optime = websGetVar(wp, T("pppoeIdleTime"), T(""));
			else 
			    pppoe_optime = websGetVar(wp, T("pppoeRedialPeriod"), T(""));
							
			/*for firsttime parameter "pppoeOPMode" maybe empty*/
			if(strlen(pppoe_opmode) < 1)
			{
			    pppoe_opmode = "KeepAlive"; 		
			    pppoe_optime = "60";				
			}

			nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoe_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", pppoe_optime);
			
		}
		else {
			websHeader(wp);
			websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
			websFooter(wp);
			websDone(wp, 200);
			return;
		}
	}
	nvram_commit(RT2860_NVRAM);
    printf("########herouter/intstatus.asp########\n");

	//websRedirect(wp, "herouter/QuickSetwifi.asp");
	websRedirect(wp, "herouter/intstatus.asp?quicksetup=1");
}
#endif
/* goform/setWan */
static void setWan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ctype;
	char_t	*ip, *nm, *gw, *pd, *sd;
	char_t	*eth, *user, *pass, *hostname,*mtu;
	char_t	*clone_en, *clone_mac;
	char_t  *pptp_srv, *pptp_mode;
	char_t  *l2tp_srv, *l2tp_mode;
#ifdef CONFIG_USER_3G
	char_t	*usb3g_dev=NULL, *usb3g_pin=NULL, *usb3g_apn=NULL, *usb3g_dial=NULL, *usb3g_user=NULL, *usb3g_pass=NULL;
	//char_t	*usb3g_mode=NULL;
#endif
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

#ifdef DARE_CUSTOMER_WEB
  const char *conTp_radio = websGetVar(wp, T("conTp_radio"), T("")); 
  int isHerouter = 0;
  if(strlen(conTp_radio) > 0){
      isHerouter = 1;	
  }
#endif

	ctype = ip = nm = gw = pd = sd = eth = user = pass = hostname =
		clone_en = clone_mac = pptp_srv = pptp_mode = l2tp_srv = l2tp_mode =
		NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0")); 
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0")) {
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));

		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15)) {
				nvram_commit(RT2860_NVRAM);
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		struct in_addr addr;
		if (-1 == inet_aton(nm,&addr)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
		/*
		 * in Bridge Mode, lan and wan are bridged together and associated with
		 * the same ip address
		 */
		if (NULL != opmode && !strcmp(opmode, "0")) {
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
		}
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	} else {
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", "");
		nvram_bufset(RT2860_NVRAM, "wan_netmask", "");
		nvram_bufset(RT2860_NVRAM, "wan_gateway", "");
		if (!strncmp(ctype, "DHCP", 5)) {
			hostname = websGetVar(wp, T("hostname"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", hostname);
		}
		else if (!strncmp(ctype, "PPPOE", 6)) {
			char_t *pppoe_opmode, *pppoe_optime;

            mtu = websGetVar(wp, T("MTU"), T(""));
			user = websGetVar(wp, T("pppoeUser"), T(""));
			pass = websGetVar(wp, T("pppoePass"), T(""));
			pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T(""));
			if (0 == strcmp(pppoe_opmode, "OnDemand"))
				pppoe_optime = websGetVar(wp, T("pppoeIdleTime"), T(""));
			else 
				pppoe_optime = websGetVar(wp, T("pppoeRedialPeriod"), T(""));
				
/*for firsttime parameter "pppoeOPMode" maybe empty*/
#ifdef DARE_CUSTOMER_WEB
			if(isHerouter == 1)
			{
					if(strlen(pppoe_opmode) < 1)
					{
							pppoe_opmode = "KeepAlive";			
							pppoe_optime = "60";				
					}
			}
#endif  	
            nvram_bufset(RT2860_NVRAM, "Pppoe_Mtu",mtu);	
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoe_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", pppoe_optime);
		}
		else if (!strncmp(ctype, "L2TP", 5)) {
			char_t *l2tp_opmode, *l2tp_optime;

			l2tp_srv = websGetVar(wp, T("l2tpServer"), T(""));
			user = websGetVar(wp, T("l2tpUser"), T(""));
			pass = websGetVar(wp, T("l2tpPass"), T(""));
			l2tp_mode = websGetVar(wp, T("l2tpMode"), T("0"));
			ip = websGetVar(wp, T("l2tpIp"), T(""));
			nm = websGetVar(wp, T("l2tpNetmask"), T(""));
			gw = websGetVar(wp, T("l2tpGateway"), T(""));
			l2tp_opmode = websGetVar(wp, T("l2tpOPMode"), T(""));
			if (0 == strcmp(l2tp_opmode, "OnDemand"))
				l2tp_optime = websGetVar(wp, T("l2tpIdleTime"), T(""));
			else
				l2tp_optime = websGetVar(wp, T("l2tpRedialPeriod"), T(""));
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_server", l2tp_srv);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_mode", l2tp_mode);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_opmode", l2tp_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_optime", l2tp_optime);
			if (!strncmp(l2tp_mode, "0", 2)) {
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_ip", ip);
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_netmask", nm);
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_gateway", gw);
			}
		}
		else if (!strncmp(ctype, "PPTP", 5)) {
			char_t *pptp_opmode, *pptp_optime;

			pptp_srv = websGetVar(wp, T("pptpServer"), T(""));
			user = websGetVar(wp, T("pptpUser"), T(""));
			pass = websGetVar(wp, T("pptpPass"), T(""));
			pptp_mode = websGetVar(wp, T("pptpMode"), T("0"));
			ip = websGetVar(wp, T("pptpIp"), T(""));
			nm = websGetVar(wp, T("pptpNetmask"), T(""));
			gw = websGetVar(wp, T("pptpGateway"), T(""));
			pptp_opmode = websGetVar(wp, T("pptpOPMode"), T(""));
			if (0 == strcmp(pptp_opmode, "OnDemand"))
				pptp_optime = websGetVar(wp, T("pptpIdleTime"), T(""));
			else
				pptp_optime = websGetVar(wp, T("pptpRedialPeriod"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_server", pptp_srv);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_mode", pptp_mode);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_opmode", pptp_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_optime", pptp_optime);
			if (!strncmp(pptp_mode, "0", 2)) {
				nvram_bufset(RT2860_NVRAM, "wan_pptp_ip", ip);
				nvram_bufset(RT2860_NVRAM, "wan_pptp_netmask", nm);
				nvram_bufset(RT2860_NVRAM, "wan_pptp_gateway", gw);
			}
		}
#ifdef CONFIG_USER_3G
		else if (!strncmp(ctype, "3G", 3)) {
			//usb3g_mode = websGetVar(wp, T("OPMode3G"), T(""));
			usb3g_dev = websGetVar(wp, T("Dev3G"), T(""));
			usb3g_apn = websGetVar(wp, T("APN3G"), T(""));
			usb3g_pin = websGetVar(wp, T("PIN3G"), T(""));
			usb3g_dial = websGetVar(wp, T("Dial3G"), T(""));
			usb3g_user = websGetVar(wp, T("User3G"), T(""));
			usb3g_pass = websGetVar(wp, T("Password3G"), T(""));

			//nvram_bufset(RT2860_NVRAM, "wan_3g_opmode", usb3g_mode);
			nvram_bufset(RT2860_NVRAM, "wan_3g_dev", usb3g_dev);
			nvram_bufset(RT2860_NVRAM, "wan_3g_apn", usb3g_apn);
			nvram_bufset(RT2860_NVRAM, "wan_3g_pin", usb3g_pin);
			nvram_bufset(RT2860_NVRAM, "wan_3g_dial", usb3g_dial);
			nvram_bufset(RT2860_NVRAM, "wan_3g_user", usb3g_user);
			nvram_bufset(RT2860_NVRAM, "wan_3g_pass", usb3g_pass);
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		}
#endif
		else {
			websHeader(wp);
			websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
			websFooter(wp);
			websDone(wp, 200);
			return;
		}
	}

	// mac clone
	clone_en = websGetVar(wp, T("macCloneEnbl"), T("0"));
	clone_mac = websGetVar(wp, T("macCloneMac"), T(""));
	nvram_bufset(RT2860_NVRAM, "macCloneEnabled", clone_en);
	if (!strncmp(clone_en, "1", 2))
		nvram_bufset(RT2860_NVRAM, "macCloneMac", clone_mac);
	//wizard complete
  	char_t  *needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
  if (NULL != needWizard)
  {
      if (!strcmp(needWizard, "1"))	
      {
          nvram_bufset(RT2860_NVRAM, "needWizard", "2");
      }
  }
	nvram_commit(RT2860_NVRAM);

#ifdef DARE_CUSTOMER_WEB
    if(isHerouter != 1)
    {
        initInternet();
    }
    else
    {
        /* redirect */
        websRedirect(wp, "herouter/intstatus.asp?quicksetup=0");
    }
#else
    initInternet();
#endif      


  
  //add dare fangan ,this post came from heroute
#ifdef DARE_CUSTOMER_WEB
  if(isHerouter == 1)
      return;
#endif      
	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>Mode: %s</h2><br>\n"), ctype);
	if (!strncmp(ctype, "STATIC", 7)) {
		websWrite(wp, T("IP Address: %s<br>\n"), ip);
		websWrite(wp, T("Subnet Mask: %s<br>\n"), nm);
		websWrite(wp, T("Default Gateway: %s<br>\n"), gw);
		websWrite(wp, T("Primary DNS: %s<br>\n"), pd);
		websWrite(wp, T("Secondary DNS: %s<br>\n"), sd);
	}
	else if (!strncmp(ctype, "DHCP", 5)) {
		websWrite(wp, T("Hostname: %s<br>\n"), hostname);
	}
	else if (!strncmp(ctype, "PPPOE", 6)) {
		websWrite(wp, T("User Name: %s<br>\n"), user);
		websWrite(wp, T("Password: %s<br>\n"), pass);
	}
	else if (!strncmp(ctype, "L2TP", 5)) {
		websWrite(wp, T("L2TP Server IP Address: %s<br>\n"), l2tp_srv);
		websWrite(wp, T("User Account: %s<br>\n"), user);
		websWrite(wp, T("Password: %s<br>\n"), pass);
		websWrite(wp, T("Address Mode: %s<br>\n"), l2tp_mode);
		if (!strncmp(l2tp_mode, "0", 2)) {
			websWrite(wp, T("IP: %s<br>\n"), ip);
			websWrite(wp, T("Netmask: %s<br>\n"), nm);
			websWrite(wp, T("Gateway: %s<br>\n"), gw);
		}
	}
	else if (!strncmp(ctype, "PPTP", 5)) {
		websWrite(wp, T("PPTP Server IP Address: %s<br>\n"), pptp_srv);
		websWrite(wp, T("User Account: %s<br>\n"), user);
		websWrite(wp, T("Password: %s<br>\n"), pass);
		websWrite(wp, T("Address Mode: %s<br>\n"), pptp_mode);
		if (!strncmp(pptp_mode, "0", 2)) {
			websWrite(wp, T("IP: %s<br>\n"), ip);
			websWrite(wp, T("Netmask: %s<br>\n"), nm);
			websWrite(wp, T("Gateway: %s<br>\n"), gw);
		}
	}
#ifdef CONFIG_USER_3G
	else if (!strncmp(ctype, "3G", 3)) {
		websWrite(wp, T("3G device: %s<br>\n"), usb3g_dev);
		websWrite(wp, T("APN: %s<br>\n"), usb3g_apn);
		websWrite(wp, T("PIN: %s<br>\n"), usb3g_pin);
		websWrite(wp, T("Dial Number: %s<br>\n"), usb3g_dial);
		websWrite(wp, T("Username: %s<br>\n"), usb3g_user);
		websWrite(wp, T("Password: %s<br>\n"), usb3g_pass);
	}
#endif

	websWrite(wp, T("MAC Clone Enable: %s<br>\n"), clone_en);
	if (!strncmp(clone_en, "1", 2))
		websWrite(wp, T("MAC Address: %s<br>\n"), clone_mac);
	websFooter(wp);
	websDone(wp, 200);
}

static void UpdateCert( webs_t wp, char_t *path, char_t *query)
{
	char file[128];
	FILE *pp;
	
	sleep (2);
	if (!strcmp(query, "key"))
		pp = popen("nvram_get cert KeyCertFile", "r");
	else if (!strcmp(query, "client"))
		pp = popen("nvram_get cert CACLCertFile", "r");
	else if (!strcmp(query, "as"))
		pp = popen("nvram_get wapi ASCertFile", "r");
	else if (!strcmp(query, "user"))
		pp = popen("nvram_get wapi UserCertFile", "r");
	else
		return;

	if (!pp) {
		error(E_L, E_LOG, T("Certificate update error"));
		return;
	}
	memset(file, 0, 128);
	fscanf(pp, "%s", file);
	pclose(pp);

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	if (strlen(file) > 0)
		websWrite(wp, T("%s"), file);
	else
		websWrite(wp, T(""));

    websDone(wp, 200);	
	return;
}

#ifdef DARE_CUSTOMER_WEB

static void MobPppoeSet(webs_t wp, char_t *path, char_t *query)
{
	char_t *user, *pass;
	char_t *pppoe_opmode, *pppoe_optime;
	user = websGetVar(wp, T("username"), T(""));
	pass = websGetVar(wp, T("password"), T(""));
	nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", user);
	nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", pass);
	nvram_bufset(RT2860_NVRAM, "wanConnectionMode", "PPPOE");


	pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T(""));
	if(strlen(pppoe_opmode) < 1)
	{
		pppoe_opmode = "KeepAlive";			
		pppoe_optime = "60";				
	}
	nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoe_opmode);
	nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", pppoe_optime);

	//wizard complete
  	char_t  *needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
  if (NULL != needWizard)
  {
      if (!strcmp(needWizard, "1"))	
      {
          nvram_bufset(RT2860_NVRAM, "needWizard", "2");
      }
  }
    nvram_commit(RT2860_NVRAM);
   // system("reboot");
	initInternet();
	websRedirect(wp, "herouter/mobilePPPoE.asp");
}


static void MobWanSet(webs_t wp, char_t *path, char_t *query)
{
		char_t	*ctype;
		ctype = websGetVar(wp, T("hiddenwantype"), T("0")); 
		if (!strncmp(ctype, "DHCP", 5)) {
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		}
		//wizard complete
  	char_t  *needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
  if (NULL != needWizard)
  {
      if (!strcmp(needWizard, "1"))	
      {
          nvram_bufset(RT2860_NVRAM, "needWizard", "2");
      }
  }
		nvram_commit(RT2860_NVRAM);
		initInternet();
		websRedirect(wp, "herouter/mobintsetting.asp");
		
}

static int LANSideDiagnose(){
	  /*always looks as ok in lan side*/
    return 1;	    
}


static int CheckUSBDisk()
{
	return hertUtil_isStorageExist();	
} 

static void netDiagnosis(webs_t wp, char_t *path, char_t *query){
	
  char cableResult[8] = {0},siteResult[8] = {0},lanResult[8] = {0},WanIpResult[8] = {0};
  char WanIP[16] = {0},LAN1[8] = {0},LAN2[8] = {0};
  int USBDiskResult =0;
  
  hertUtil_upEthrtPortStatus();
  strcpy(cableResult,herouterStatus.wanPortStatus);
  strcpy(LAN1,herouterStatus.lan1PortStatus);
  strcpy(LAN2,herouterStatus.lan2PortStatus);
  strcpy(WanIpResult,herouterStatus.WanIpStatus);
  
  if(strncmp(herouterStatus.pingStatus,STRING_PASS,strlen(STRING_PASS)) == 0 )
  {
      strcpy(siteResult,STRING_PASS);
  }
  else
  {
  	  strcpy(siteResult,STRING_FAIL);
  }
  
  if(LANSideDiagnose() == 1){
      strcpy(lanResult,STRING_PASS);
  }else{
      strcpy(lanResult,STRING_FAIL);	
  }
  
  //check USB disk
  if(CheckUSBDisk() == 1)
  	USBDiskResult = 1;

  printf("cablePlug:%s\nWanIp:%s\nsiteAccess:%s\nlanNetwork:%s\nUSBDiskResult:%d\nLAN1:%s\nLAN2:%s\n", \
            cableResult,WanIpResult,siteResult,lanResult ,USBDiskResult,LAN1,LAN2);

	sleep(4);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("cablePlug:%s\n"), cableResult);
	websWrite(wp, T("WanIp:%s\n"), WanIpResult );
	websWrite(wp, T("siteAccess:%s\n"), siteResult);
	websWrite(wp, T("lanNetwork:%s\n"), lanResult);
	websWrite(wp, T("USBDisk:%d\n"), USBDiskResult);
	websWrite(wp, T("LAN1:%s\n"), LAN1);
	websWrite(wp, T("LAN2:%s\n"), LAN2);
	websDone(wp, 200);
	
 
}

static void netState(webs_t wp, char_t *path, char_t *query)
{
	
    char cableResult[8] = {0},siteResult[8] = {0},WanIpResult[8] = {0};
    char WanIP[16] = {0};
  
    hertUtil_upEthrtPortStatus();
    strcpy(cableResult,herouterStatus.wanPortStatus);
    strcpy(WanIpResult,herouterStatus.WanIpStatus);
  
    if(strncmp(herouterStatus.pingStatus,STRING_PASS,strlen(STRING_PASS)) == 0 )
    {
        strcpy(siteResult,STRING_PASS);
    }
    else
    {
        strcpy(siteResult,STRING_FAIL);
    }

    printf("cablePlug:%s\nWanIp:%s\nsiteAccess:%s\n", cableResult,WanIpResult,siteResult);

    //sleep(4);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("cablePlug:%s\n"), cableResult);
    websWrite(wp, T("WanIp:%s\n"), WanIpResult );
    websWrite(wp, T("siteAccess:%s\n"), siteResult);
    websWrite(wp, T("PPPOEStatus:%s\n"), herouterStatus.PPPOEStatus);
    websDone(wp, 200);
}
#endif
