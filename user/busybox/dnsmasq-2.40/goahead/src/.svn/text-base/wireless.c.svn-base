/* vi: set sw=4 ts=4 sts=4: */
/*
 *	wireless.c -- Wireless Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/MP/SDK_4_2_0_0/RT288x_SDK/source/user/goahead/src/wireless.c#3 $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	"../../autoconf.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
  #include	<linux/types.h>
  #include	<linux/socket.h>
  #include	<linux/if.h>
#endif
#include	<linux/wireless.h>

#include	"internet.h"
#include	"nvram.h"
#include	"utils.h"
#include	"webs.h"
#include	"wireless.h"
#define AP_MODE
#include	"oid.h"
#include	"stapriv.h"			//for statistics

#include	"linux/autoconf.h"  //kernel config

#include "wsIntrn.h"

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
#include	<dirent.h>
#endif

#define ENHANCED_AP_STATUS_INFO		// Display enhanced status information (STA info and PER/PLR statistics). Internal use only.

/*
 * RT2860, RTINIC, RT2561
 */
static int default_shown_mbssid[3]  = {0,0,0};

extern int g_wsc_configured;
#if defined (RTDEV_SUPPORT)
extern int g_Raix_wsc_configured;
#endif

static int  getRaxWAPIBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getDLSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getDFSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getRax11nDraft3Built(int eid, webs_t wp, int argc, char_t **argv);
static int  getMaxBssidNum(int eid, webs_t wp, int argc, char_t **argv);
static int	getMeshBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getHSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRVTBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getTxBfBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	get16MBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getAutoProvisionBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getWSCBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getUniWPSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getVideoTurbineBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11bChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanChannel(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv);
static int	getWAPIASCertList(int eid, webs_t wp, int argc, char_t **argv);
static int	getWAPIUserCertList(int eid, webs_t wp, int argc, char_t **argv);
static int	getAPDriverVer(int eid, webs_t wp, int argc, char_t **argv);
static int  getRaxGBand(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaxABand(int eid, webs_t wp, int argc, char_t **argv);
static int  getRaxAC(int eid, webs_t wp, int argc, char_t **argv);
static int  getVoWBuilt(int eid, webs_t wp, int argc, char_t **argv);
static void getVoW(webs_t wp, char_t *path, char_t *query);
static void setVoW(webs_t wp, char_t *path, char_t *query);
static void setVoWRadio(webs_t wp, char_t *path, char_t *query);

static void wirelessBasic(webs_t wp, char_t *path, char_t *query);
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query);
#ifdef DARE_CUSTOMER_WEB
static void wirelessmobset(webs_t wp, char_t *path, char_t *query);
static void wirelessSet(webs_t wp, char_t *path, char_t *query);
static void wirelessQuick(webs_t wp, char_t *path, char_t *query);
static void doInitInternet(webs_t wp, char_t *path, char_t *query);
static void GetAccessControlList(int nvram, webs_t wp, char_t *path, char_t *query);
static void wirelessGetMAClist(webs_t wp, char_t *path, char_t *query);
static int GetAccessPolicy(int eid, webs_t wp, int argc, char_t **argv);
static int getdevmac(int eid, webs_t wp, int argc, char_t **argv);
static void wireless_channel(webs_t wp, char_t *path, char_t *query);
static int GetCurrentChannel(int eid, webs_t wp, int argc, char_t **argv);
static int  get11aChannels(int eid, webs_t wp, int argc, char_t **argv);
static int get11bChannels(int eid, webs_t wp, int argc, char_t **argv);
static int get11gChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getChannelSelected(int eid, webs_t wp, int argc, char_t **argv);
static void disconnectbymacfilter(webs_t wp, char_t *path, char_t *query);
static void QuickSetUpSave(webs_t wp, char_t *path, char_t *query);

#endif
#if	defined (RT2860_WDS_SUPPORT)
static void wirelessWds(webs_t wp, char_t *path, char_t *query);
#endif
#if	defined (RT2860_APCLI_SUPPORT)
static void wirelessApcli(webs_t wp, char_t *path, char_t *query);
static int  ApcliScan(int eid, webs_t wp, int argc, char_t **argv);
#endif
static void wirelessWmm(webs_t wp, char_t *path, char_t *query);
static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query);
static void APSecurity(webs_t wp, char_t *path, char_t *query);
#if defined (RT2860_HS_SUPPORT)
static void setHotspot(webs_t wp, char_t *path, char_t *query);
#endif
static int getRaxHTStream(int eid, webs_t wp, int argc, char_t **argv);
static int  isWPSConfiguredASP(int eid, webs_t wp, int argc, char_t **argv);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);		/* for Access Policy list deletion*/
static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query);
static void APBatchDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query);
void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query);
static int  isAntennaDiversityBuilt(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY
static void AntennaDiversity(webs_t wp, char_t *path, char_t *query);
static void getAntenna(webs_t wp, char_t *path, char_t *query);
#endif
#ifdef CONFIG_RT2860V2_AP_MESH
static void wirelessMesh(webs_t wp, char_t *path, char_t *query);
static void meshManualLink(webs_t wp, char_t *path, char_t *query);
static int	ShowMeshState(int eid, webs_t wp, int argc, char_t **argv);
#endif
static void resetApCounters(webs_t wp, char_t *path, char_t *query);
static int	getApStats(int eid, webs_t wp, int argc, char_t **argv);
static int	getApSNR(int eid, webs_t wp, int argc, char_t **argv);
static int	getApTxBFStats(int eid, webs_t wp, int argc, char_t **argv);
static void wirelessRvt(webs_t wp, char_t *path, char_t *query);
static int getExtChListBuilt(int eid, webs_t wp, int argc, char_t **argv);

void formDefineWireless(void) {
	websAspDefine(T("getRaxWAPIBuilt"), getRaxWAPIBuilt);
	websAspDefine(T("getDLSBuilt"), getDLSBuilt);
	websAspDefine(T("getDFSBuilt"), getDFSBuilt);
	websAspDefine(T("getCarrierBuilt"), getCarrierBuilt);
	websAspDefine(T("getWlanM2UBuilt"), getWlanM2UBuilt);
	websAspDefine(T("getMeshBuilt"), getMeshBuilt);
	websAspDefine(T("getHSBuilt"), getHSBuilt);
	websAspDefine(T("getRVTBuilt"), getRVTBuilt);
	websAspDefine(T("getTxBfBuilt"), getTxBfBuilt);
	websAspDefine(T("getWDSBuilt"), getWDSBuilt);
	websAspDefine(T("getMBSSIDBuilt"), getMBSSIDBuilt);
	websAspDefine(T("get16MBSSIDBuilt"), get16MBSSIDBuilt);
	websAspDefine(T("getAutoProvisionBuilt"), getAutoProvisionBuilt);
	websAspDefine(T("getWSCBuilt"), getWSCBuilt);
	websAspDefine(T("getUniWPSBuilt"), getUniWPSBuilt);
	websAspDefine(T("getVideoTurbineBuilt"), getVideoTurbineBuilt);

	websAspDefine(T("getWlanApcliBuilt"), getWlanApcliBuilt);
	websAspDefine(T("getRax11nDraft3Built"), getRax11nDraft3Built);
	websAspDefine(T("getMaxBssidNum"), getMaxBssidNum);
	websAspDefine(T("getWlan11aChannels"), getWlan11aChannels);
	websAspDefine(T("getWlan11bChannels"), getWlan11bChannels);
	websAspDefine(T("getWlan11gChannels"), getWlan11gChannels);
	websAspDefine(T("getWlanChannel"), getWlanChannel);
	websAspDefine(T("getWlanCurrentMac"), getWlanCurrentMac);
	websAspDefine(T("getWlanStaInfo"), getWlanStaInfo);
	websAspDefine(T("getRaxHTStream"), getRaxHTStream);
	websAspDefine(T("isWPSConfiguredASP"), isWPSConfiguredASP);
	websAspDefine(T("isAntennaDiversityBuilt"), isAntennaDiversityBuilt);
#ifdef CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY
	websFormDefine(T("AntennaDiversity"), AntennaDiversity);
	websFormDefine(T("getAntenna"), getAntenna);
#endif
	websFormDefine(T("wirelessBasic"), wirelessBasic);
	websFormDefine(T("wirelessAdvanced"), wirelessAdvanced);
#ifdef DARE_CUSTOMER_WEB
	websFormDefine(T("wirelessQuick"), wirelessQuick);
	websFormDefine(T("doInitInternet"), doInitInternet);
	websFormDefine(T("wirelessSet"), wirelessSet);
	websFormDefine(T("wirelessmobset"), wirelessmobset);
	websFormDefine(T("wirelessGetMAClist"), wirelessGetMAClist);
	websAspDefine(T("GetAccessPolicy"), GetAccessPolicy);
	websAspDefine(T("getdevmac"), getdevmac);
	websFormDefine(T("wireless_channel"), wireless_channel);
	websAspDefine(T("GetCurrentChannel"), GetCurrentChannel);
	websAspDefine(T("get11aChannels"), get11aChannels);
	websAspDefine(T("get11bChannels"), get11bChannels);
	websAspDefine(T("get11gChannels"), get11gChannels);
	websAspDefine(T("getChannelSelected"), getChannelSelected);
	websFormDefine(T("disconnectbymacfilter"), disconnectbymacfilter);
	websFormDefine(T("QuickSetUpSave"), QuickSetUpSave);
#endif
#if	defined (RT2860_WDS_SUPPORT)
	websFormDefine(T("wirelessWds"), wirelessWds);
#endif
#if	defined (RT2860_APCLI_SUPPORT)
	websFormDefine(T("wirelessApcli"), wirelessApcli);
	websAspDefine(T("ApcliScan"), ApcliScan);
#endif
	websFormDefine(T("wirelessWmm"), wirelessWmm);
	websFormDefine(T("wirelessGetSecurity"), wirelessGetSecurity);
	websFormDefine(T("APSecurity"), APSecurity);
#if defined (RT2860_HS_SUPPORT)
	websFormDefine(T("setHotspot"), setHotspot);
#endif
	websFormDefine(T("APDeleteAccessPolicyList"), APDeleteAccessPolicyList);
	websFormDefine(T("APBatchDeleteAccessPolicyList"), APBatchDeleteAccessPolicyList);
#ifdef CONFIG_RT2860V2_AP_MESH
	websAspDefine(T("ShowMeshState"), ShowMeshState);
	websFormDefine(T("wirelessMesh"), wirelessMesh);
	websFormDefine(T("meshManualLink"), meshManualLink);
#endif
	websAspDefine(T("getWAPIASCertList"), getWAPIASCertList);
	websAspDefine(T("getWAPIUserCertList"), getWAPIUserCertList);
	websAspDefine(T("getAPDriverVer"), getAPDriverVer);
	websAspDefine(T("getRaxGBand"), getRaxGBand);
	websAspDefine(T("getRaxABand"), getRaxABand);
	websAspDefine(T("getRaxAC"), getRaxAC);
	websAspDefine(T("getVoWBuilt"), getVoWBuilt);
	websAspDefine(T("getExtChListBuilt"), getExtChListBuilt);
	websAspDefine(T("getApStats"), getApStats);
	websFormDefine(T("resetApCounters"), resetApCounters);
	websAspDefine(T("getApSNR"), getApSNR);
	websAspDefine(T("getApTxBFStats"), getApTxBFStats);
	websFormDefine(T("wirelessRvt"), wirelessRvt);
#if defined (RT2860_VOW_SUPPORT) || defined (RTDEV_VOW_SUPPORT)
	websFormDefine(T("getVoW"), getVoW);
	websFormDefine(T("setVoW"), setVoW);
	websFormDefine(T("setVoWRadio"), setVoWRadio);
#endif
}


#define STF(nvram, index, flash_key)	STFs(nvram, index, #flash_key, flash_key)
/*
 *   TODO:   move to util.c?
 */
static void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = (char *) nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}


/* LFF means "Load From Flash" ...*/
#define LFF(result, nvram, x, n)	\
							do{		char tmp[128];										\
									if(! ( x  = (char *) nvram_bufget(nvram, #x)) )				\
										tmp[0] = '\0';									\
									else{												\
										if( getNthValueSafe(n, x, ';', tmp, 128) != -1){\
											gstrncat(result, tmp, 4096);				\
										}												\
									}gstrncat(result, "\r", 4096);						\
							}while(0)

/* Load from Web */
#define LFW(x, y)	do{												\
						if(! ( x = websGetVar(wp, T(#y), T(""))))	\
							return;									\
					}while(0)


/* Set the same value into all BSSID */
#define	SetAllValues(bssid_num, nvram, flash_key, value)	do {	char buffer[24]; int i=1;						\
																	strcpy(buffer, value);							\
																	while (i++ < bssid_num)							\
																		sprintf(buffer, "%s;%s", buffer, value);	\
																	nvram_bufset(nvram, #flash_key, buffer);		\
															} while(0)



#define STR_FULL_SPACE "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"


static int get11aChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	
	int  idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM,"CountryRegionABand");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");
//<option value=36 >5180MHz (Channel 36)</option>
//<option value=1 selected>2412MHz (Channel 1)</option
//<li  onClick="pick('0','sz11aChannel','test1')">AutoSelect</li>
	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "7") == 0)) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
											36+4*idx, 5180+20*idx,36+4*idx, 5180+20*idx,36+4*idx, STR_FULL_SPACE);			
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
										  36+4*idx, 5180+20*idx,36+4*idx,5180+20*idx,36+4*idx, STR_FULL_SPACE);
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			 								36+4*idx+1,5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "0") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
										  36+4*idx, 5180+20*idx, 36+4*idx,180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			 								36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "1") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
					            36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "2") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
						          36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "3") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "4") == 0) {
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "5") == 0) {
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                       36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "6") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "8") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "9") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx,  5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx,5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                        36+4*idx+1,5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "10") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                        36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "11") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx, 5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
                       36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "12") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "13") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                         36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                        36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "14") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
			                   36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 136~140 */
		for (idx = 25; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                       36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "15") == 0) {
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                      36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "16") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                       36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "17") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
 											  36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "18") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	} else if (strcmp(value, "19") == 0) {
		/* 56~64 */
		for (idx = 5; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
                         36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "20") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~124 */
		for (idx = 16; idx < 23; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
	                   36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "21") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                   36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 36+4*idx+1, 5180+20*idx+5, 36+4*idx+1,5180+20*idx+5, 36+4*idx+1, STR_FULL_SPACE);
	}

	return 0;
#else
	/* 36~64 */
	for (idx = 0; idx < 8; idx++)
		websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
		                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	/* 100~140 */
	for (idx = 16; idx < 27; idx++)
		websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
		                  36+4*idx, 5180+20*idx, 36+4*idx,5180+20*idx, 36+4*idx, STR_FULL_SPACE);
	/* 149~173 */
	for (idx = 28; idx < 35; idx++)
		websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
		                  36+4*idx+1, 5180+20*idx+5,  36+4*idx+1, 5180+20*idx+5,  36+4*idx+1, STR_FULL_SPACE);
#endif

}
static int get11bChannels(int eid, webs_t wp, int argc, char_t **argv)
{
int idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<li onClick=\"pick('14','select1','test1','2484MHz (Channel 14)')\">2484MHz (Channel 14)</li>\n")); 
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                 idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "31") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "32") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "33") == 0) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
#else
	/* ch1 ~ ch14 */
	for (idx = 0; idx < 14; idx++)
		websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"), 
		                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
#endif

	return 0;	
}
static int get11gChannels(int eid, webs_t wp, int argc, char_t **argv)
{

	int idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1, 2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<li onClick=\"pick('14','select1','test1','2484MHz (Channel 14)')\">2484MHz (Channel 14)</li>\n"));
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	} else if (strcmp(value, "31") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
 
	} else if (strcmp(value, "32") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
 
	} else if (strcmp(value, "33") == 0) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
			                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
	}
#else
	/* ch1 ~ ch14 */
	for (idx = 0; idx < 14; idx++)
		websWrite(wp, T("<li onClick=\"pick('%d','select1','test1','%dMHz (Channel %d)')\">%dMHz (Channel %d) %s</li>\n"),
		                  idx+1, 2412+5*idx, idx+1,2412+5*idx, idx+1, STR_FULL_SPACE);
#endif

	return 0;
	
}

/*
 * description: write 802.11a channels in <select> tag
 */
static int getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int  idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM,"CountryRegionABand");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "7") == 0)) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "5") == 0) {
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "6") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "8") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "9") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "10") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "11") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "12") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "13") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "14") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 136~140 */
		for (idx = 25; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "15") == 0) {
		/* 149~173 */
		for (idx = 28; idx < 35; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "16") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "17") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "18") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "19") == 0) {
		/* 56~64 */
		for (idx = 5; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "20") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~124 */
		for (idx = 16; idx < 23; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "21") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	}

	return 0;
#else
	/* 36~64 */
	for (idx = 0; idx < 8; idx++)
		websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				(36+4*idx == channel)? "selected" : "", 5180+20*idx,
				"MHz (Channel ", 36+4*idx, ")</option>");
	/* 100~140 */
	for (idx = 16; idx < 27; idx++)
		websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				(36+4*idx == channel)? "selected" : "", 5180+20*idx,
				"MHz (Channel ", 36+4*idx, ")</option>");
	/* 149~173 */
	for (idx = 28; idx < 35; idx++)
		websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
				36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
				5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
#endif
}

/*
 * description: write 802.11b channels in <select> tag
 */
static int getWlan11bChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<option value=14 %s>2484MHz (Channel 14)</option>\n"), 
				(14 == channel)? "selected" : "");
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "31") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch14 */
		for (idx = 11; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "32") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch13 */
		for (idx = 11; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "33") == 0) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
#else
	/* ch1 ~ ch14 */
	for (idx = 0; idx < 14; idx++)
		websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
				(idx+1 == channel)? "selected" : "", 2412+5*idx,
				"MHz (Channel ", idx+1, ")</option>");
#endif

	return 0;
}

/*
 * description: write 802.11g channels in <select> tag
 */
static int getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
#if ! defined CONFIG_EXT_CHANNEL_LIST
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<option value=14 %s>2484MHz (Channel 14)</option>\n"), 
				(14 == channel)? "selected" : "");
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "31") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch14 */
		for (idx = 11; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "32") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#if 0
		/* ch12 ~ ch13 */
		for (idx = 11; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
#endif
	} else if (strcmp(value, "33") == 0) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
#else
	/* ch1 ~ ch14 */
	for (idx = 0; idx < 14; idx++)
		websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
				(idx+1 == channel)? "selected" : "", 2412+5*idx,
				"MHz (Channel ", idx+1, ")</option>");
#endif

	return 0;
}

static int getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if	defined (RT2860_APCLI_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRax11nDraft3Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_11NDRAFT3_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMaxBssidNum(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_NEW_MBSS_SUPPORT)
	return websWrite(wp, T("16"));
#else
	return websWrite(wp, T("8"));
#endif
}

static int getChannelSelected(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *value = nvram_bufget(RT2860_NVRAM, "ChannelSelected");
	
	if(0 == strcmp(value, ""))
		return websWrite(wp, T("AutoSelect"));
	else
		return websWrite(wp, T("%s"), value);
}

/*
 * description: write channel number or 0 if auto-select
 */
static int getWlanChannel(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *value = nvram_bufget(RT2860_NVRAM, "AutoChannelSelect");

	if (NULL == value)
		return websWrite(wp, T("9"));
	if (!strncmp(value, "1", 2))
		return websWrite(wp, T("0"));

	value = nvram_bufget(RT2860_NVRAM, "Channel");
	if (NULL == value)
		return websWrite(wp, T("9"));
	else
		return websWrite(wp, T("%s"), value);
}

/*
 * description: write MAC address from interface 'ra0'
 */
static int getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_hw[18] = {0};

	if (-1 == getIfMac("ra0", if_hw))
		return websWrite(wp, T(" "));
	return websWrite(wp, T("%s"), if_hw);
}

typedef struct _RT_802_11_MAC_ENTRY {
#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
	unsigned char			ApIdx;
#endif
	unsigned char           Addr[6];
	unsigned char           Aid;
	unsigned char           Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char           MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char                    AvgRssi0;
	char                    AvgRssi1;
	char                    AvgRssi2;
	unsigned int            ConnectedTime;
	MACHTTRANSMIT_SETTING	TxRate;
	unsigned int			LastRxRate;
#if 0
	int					StreamSnr[3];
	int					SoundingRespSnr[3];
#else
	short					StreamSnr[3];
	short					SoundingRespSnr[3];
#endif
#if 0
	short					TxPER;
	short					reserved;
#endif
#if defined (RT2860_VOW_SUPPORT) || defined (RTDEV_VOW_SUPPORT)
//	char					Tx_Per;
#endif
} RT_802_11_MAC_ENTRY;

#if	defined (CONFIG_FIRST_IF_MT7612E)
#define MAX_NUMBER_OF_MAC               116
#elif defined (RT2860_WAPI_SUPPORT)
#define MAX_NUMBER_OF_MAC               96
#else
#define MAX_NUMBER_OF_MAC               32 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#endif

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long            Num;
	RT_802_11_MAC_ENTRY      Entry[MAX_NUMBER_OF_MAC]; //MAX_LEN_OF_MAC_TABLE = 32
} RT_802_11_MAC_TABLE;

static int getWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, s;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	char tmpBuff[32];
	char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#endif
	char *ifname;

	if (ejArgs(argc, argv, T("%s"), &ifname) < 1) {
		return 0;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		websError(wp, 500, "ioctl sock failed!");
		return -1;
	}

#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
		websError(wp, 500, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		websError(wp, 500, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
#endif
		close(s);
		return -1;
	}

	close(s);
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	for (i = 0; i < table.Num; i++) {
		RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
		unsigned int lastRxRate = pe->LastRxRate;
		unsigned int mcs = pe->LastRxRate & 0x7F;
		int hr, min, sec;

		hr = pe->ConnectedTime/3600;
		min = (pe->ConnectedTime % 3600)/60;
		sec = pe->ConnectedTime - hr*3600 - min*60;

		// MAC Address
		websWrite(wp, T("<tr><td>%02X:%02X:%02X:<br>%02X:%02X:%02X</td>"),
				pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3],
				pe->Addr[4], pe->Addr[5]);

		// AID, Power Save mode, MIMO Power Save
		websWrite(wp, T("<td align=\"center\">%d</td><td align=\"center\">%d</td><td align=\"center\">%d</td>"),
				pe->Aid, pe->Psm, pe->MimoPs);

		// TX Rate
		websWrite(wp, T("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>"),
				pe->TxRate.field.MCS,
				pe->TxRate.field.BW? 40: 20,
				pe->TxRate.field.ShortGI? 'S': 'L',
				phyMode[pe->TxRate.field.MODE],
				pe->TxRate.field.STBC? ", STBC": " ");

		// TxBF configuration
		websWrite(wp, T("<td align=\"center\">%c %c</td>"),
				pe->TxRate.field.iTxBF? 'I': '-',
				pe->TxRate.field.eTxBF? 'E': '-');

		// RSSI
		websWrite(wp, T("<td align=\"center\">%d<br>%d<br>%d</td>"),
				(int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));

		// Per Stream SNR
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[0]*0.25);
		websWrite(wp, T("<td>%s<br>"), tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[1]*0.25); //mcs>7? pe->StreamSnr[1]*0.25: 0.0);
		websWrite(wp, T("%s<br>"), tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[2]*0.25); //mcs>15? pe->StreamSnr[2]*0.25: 0.0);
		websWrite(wp, T("%s</td>"), tmpBuff);

		// Sounding Response SNR
		if (pe->TxRate.field.eTxBF) {
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[0]*0.25);
			websWrite(wp, T("<td>%s<br>"), tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[1]*0.25);
			websWrite(wp, T("%s<br>"), tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[2]*0.25);
			websWrite(wp, T("%s</td>"), tmpBuff);
		}
		else {
			websWrite(wp, T("<td align=\"center\">-<br>-<br>-</td>"));
		}

		// Last RX Rate
		websWrite(wp, T("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>"),
				mcs,  ((lastRxRate>>7) & 0x1)? 40: 20,
				((lastRxRate>>8) & 0x1)? 'S': 'L',
				phyMode[(lastRxRate>>14) & 0x3],
				((lastRxRate>>9) & 0x3)? ", STBC": " ");

		// Connect time
		//websWrite(wp, T("<td align=\"center\">%02d:%02d:%02d<br>PER=%d%%</td>"), hr, min, sec, pe->TxPER);
		websWrite(wp, T("<td align=\"center\">%02d:%02d:%02d</td>"), hr, min, sec);
		websWrite(wp, T("</tr>"));
	}
#elif defined (RT2860_VOW_SUPPORT) || defined (RTDEV_VOW_SUPPORT)
	for (i = 0; i < table.Num; i++) {
		char color[8];
		switch (table.Entry[i].Aid) {
		case 0:
			strcpy(color, "#edc240");
			break;
		case 1:
			strcpy(color, "#afd8f8");
			break;
		case 2:
			strcpy(color, "#cb4b4b");
			break;
		case 3:
			strcpy(color, "#4da74d");
			break;
		default:
			strcpy(color, "#ffffff");
		}
		websWrite(wp, T("<tr bgcolor=%s>"), color);
		websWrite(wp, T("<td>%02X:%02X:%02X:%02X:%02X:%02X</td>"),
				table.Entry[i].Addr[0], table.Entry[i].Addr[1],
				table.Entry[i].Addr[2], table.Entry[i].Addr[3],
				table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
		websWrite(wp, T("<td>%d</td><td>%d</td><td>%d</td>"),
				table.Entry[i].Aid, table.Entry[i].Psm, table.Entry[i].MimoPs);
		websWrite(wp, T("<td>%d</td><td>%s</td><td>%d</td><td>%d</td>"),
				table.Entry[i].TxRate.field.MCS,
				(table.Entry[i].TxRate.field.BW == 0)? "20M":"40M",
				table.Entry[i].TxRate.field.ShortGI, table.Entry[i].TxRate.field.STBC);
		websWrite(wp, T("<td>%d</td></tr>"), table.Entry[i].Tx_Per);
		websWrite(wp, T("</tr>"));
	}
#else
	for (i = 0; i < table.Num; i++) {
		websWrite(wp, T("<tr><td>%02X:%02X:%02X:%02X:%02X:%02X</td>"),
				table.Entry[i].Addr[0], table.Entry[i].Addr[1],
				table.Entry[i].Addr[2], table.Entry[i].Addr[3],
				table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
		websWrite(wp, T("<td>%d</td><td>%d</td><td>%d</td>"),
				table.Entry[i].Aid, table.Entry[i].Psm, table.Entry[i].MimoPs);
		websWrite(wp, T("<td>%d</td><td>%s</td><td>%d</td><td>%d</td></tr>"),
				table.Entry[i].TxRate.field.MCS,
				(table.Entry[i].TxRate.field.BW == 0)? "20M":"40M",
				table.Entry[i].TxRate.field.ShortGI, table.Entry[i].TxRate.field.STBC);
	}
#endif
	return 0;
}

static int getRaxWAPIBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_WAPI_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDLSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_DLS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDFSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_DFS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_CARRIER_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_IGMPSNOOP_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVideoTurbineBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AP_VIDEO_TURBINE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static void revise_mbss_value(int old_num, int new_num)
{
	/* {{{ The parameters that support multiple BSSID is listed as followed,
	   1.) SSID,                 char SSID[33];
	   2.) AuthMode,             char AuthMode[14];
	   3.) EncrypType,           char EncrypType[8];
	   4.) WPAPSK,               char WPAPSK[65];
	   5.) DefaultKeyID,         int  DefaultKeyID;
	   6.) Key1Type,             int  Key1Type;
	   7.) Key1Str,              char Key1Str[27];
	   8.) Key2Type,             int  Key2Type;
	   9.) Key2Str,              char Key2Str[27];
	   10.) Key3Type,            int  Key3Type;
	   11.) Key3Str,             char Key3Str[27];
	   12.) Key4Type,            int  Key4Type;
	   13.) Key4Str,             char Key4Str[27];
	   14.) AccessPolicy,
	   15.) AccessControlList,
	   16.) NoForwarding,
	   17.) IEEE8021X,           int  IEEE8021X;
	   18.) TxRate,              int  TxRate;
	   19.) HideSSID,            int  HideSSID;
	   20.) PreAuth,             int  PreAuth;
	   21.) WmmCapable
	                             int  SecurityMode;
                             	 char VlanName[20];
	                             int  VlanId;
	                             int  VlanPriority;
	}}} */
	char new_value[264], *p;
	const char *old_value;
	int i;

#define MBSS_INIT(field, default_value) \
	do { \
		old_value = nvram_bufget(RT2860_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value + strlen(old_value); \
		for (i = old_num; i < new_num; i++) { \
			snprintf(p, 264 - (p - new_value), ";%s", default_value); \
			p += 1 + strlen(default_value); \
		} \
		nvram_bufset(RT2860_NVRAM, #field, new_value); \
	} while (0)

#define MBSS_REMOVE(field) \
	do { \
		old_value = nvram_bufget(RT2860_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value; \
		for (i = 0; i < new_num; i++) { \
			if (0 == i) \
				p = strchr(p, ';'); \
			else \
				p = strchr(p+1, ';'); \
			if (NULL == p) \
				break; \
		} \
		if (p) \
			*p = '\0'; \
		nvram_bufset(RT2860_NVRAM, #field, new_value); \
	} while (0)

	if (new_num > old_num) {
		//MBSS_INIT(SSID, "ssid");
		MBSS_INIT(AuthMode, "OPEN");
		MBSS_INIT(EncrypType, "NONE");
		//MBSS_INIT(WPAPSK, "12345678");
		MBSS_INIT(DefaultKeyID, "1");
		MBSS_INIT(Key1Type, "0");
		//MBSS_INIT(Key1Str, "");
		MBSS_INIT(Key2Type, "0");
		//MBSS_INIT(Key2Str, "");
		MBSS_INIT(Key3Type, "0");
		//MBSS_INIT(Key3Str, "");
		MBSS_INIT(Key4Type, "0");
		//MBSS_INIT(Key4Str, "");
/*		MBSS_INIT(AccessPolicy0, "0");
		MBSS_INIT(AccessControlList0, "");
		MBSS_INIT(AccessPolicy1, "0");
		MBSS_INIT(AccessControlList1, "");
		MBSS_INIT(AccessPolicy2, "0");
		MBSS_INIT(AccessControlList2, "");
		MBSS_INIT(AccessPolicy3, "0");
		MBSS_INIT(AccessControlList3, ""); */
		MBSS_INIT(NoForwarding, "0");
		MBSS_INIT(NoForwardingBTNBSSID, "0");
		MBSS_INIT(IEEE8021X, "0");
		MBSS_INIT(RADIUS_Server, "0");
		MBSS_INIT(RADIUS_Port, "1812");
		MBSS_INIT(TxRate, "0");
		//MBSS_INIT(HideSSID, "0");
		MBSS_INIT(PreAuth, "0");
		MBSS_INIT(WmmCapable, "1");
		for (i = old_num + 1; i <= new_num; i++) {
			nvram_bufset(RT2860_NVRAM, racat("WPAPSK", i), "12345678");
			nvram_bufset(RT2860_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RT2860_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RT2860_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
	else if (new_num < old_num) {
		//MBSS_REMOVE(SSID);
		MBSS_REMOVE(AuthMode);
		MBSS_REMOVE(EncrypType);
		//MBSS_REMOVE(WPAPSK);
		MBSS_REMOVE(DefaultKeyID);
		MBSS_REMOVE(Key1Type);
		//MBSS_REMOVE(Key1Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key3Type);
		//MBSS_REMOVE(Key3Str);
		MBSS_REMOVE(Key4Type);
		//MBSS_REMOVE(Key4Str);
/*		MBSS_REMOVE(AccessPolicy0);
		MBSS_REMOVE(AccessControlList0);
		MBSS_REMOVE(AccessPolicy1);
		MBSS_REMOVE(AccessControlList1);
		MBSS_REMOVE(AccessPolicy2);
		MBSS_REMOVE(AccessControlList2);
		MBSS_REMOVE(AccessPolicy3);
		MBSS_REMOVE(AccessControlList3); */
		MBSS_REMOVE(NoForwarding);
		MBSS_REMOVE(NoForwardingBTNBSSID);
		MBSS_REMOVE(IEEE8021X);
		MBSS_REMOVE(RADIUS_Server);
		MBSS_REMOVE(RADIUS_Port);
		MBSS_REMOVE(TxRate);
		MBSS_REMOVE(HideSSID);
		MBSS_REMOVE(PreAuth);
		MBSS_REMOVE(WmmCapable);
		for (i = new_num + 1; i <= old_num; i++) {
			nvram_bufset(RT2860_NVRAM, racat("SSID", i), "");
			nvram_bufset(RT2860_NVRAM, racat("WPAPSK", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RT2860_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RT2860_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RT2860_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
}
static void wireless_channel(webs_t wp, char_t *path, char_t *query)
{
	char_t	*sz11aChannel, *sz11bChannel, *sz11gChannel, *ChannelSelected;
	
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T("")); 
	sz11bChannel = websGetVar(wp, T("sz11bChannel"), T("")); 
	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T(""));
	ChannelSelected = websGetVar(wp, T("select1"), T(""));
	
	if((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
			(0 == strlen(sz11gChannel))) {
		websError(wp, 403, T("'Channel' should not be empty!"));
		return;
	}
	
	nvram_bufset(RT2860_NVRAM, "ChannelSelected", ChannelSelected);
			
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
			!strncmp(sz11gChannel, "0", 2))
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	else
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
		
	if (0 != strlen(sz11aChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11aChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11aChannel);
	}
	if (0 != strlen(sz11bChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11bChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11bChannel);
	}
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11gChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);
	}
	
	nvram_commit(RT2860_NVRAM);
	initInternet();
	websRedirect(wp, "herouter/wirelesschannel.asp");
}	
	
/*
 * Get the current channel.
 */
static int GetCurrentChannel(int eid, webs_t wp, int argc, char_t **argv)
{
	/*auto channel*/	
	
	struct iwreq ireq;
	int channel = 1,sock_fd;
	char ChannelStr[64];
	
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&ireq, 0, sizeof ireq);
	strncpy(ireq.ifr_ifrn.ifrn_name, "ra0",3);
	ireq.ifr_ifrn.ifrn_name[3] = 0;
	if (ioctl(sock_fd, 0x8B05, &ireq) == -1) 
		printf("get channel error! use default value 1\n");
	else
		channel = ireq.u.freq.m;	
/*
	switch(channel)
	{
		case 1:
			sprintf(ChannelStr,"%s(Channel 1)","2412MHz");
			break;
		case 2:
			sprintf(ChannelStr,"%s(Channel 2)","2417MHz");
			break;
		case 3:
			sprintf(ChannelStr,"%s(Channel 3)","2422MHz");
			break;
		case 4:
			sprintf(ChannelStr,"%s(Channel 4)","2427MHz");
			break;
		case 5:
			sprintf(ChannelStr,"%s(Channel 5)","2432MHz");
			break;
		case 6:
			sprintf(ChannelStr,"%s(Channel 6)","2437MHz");
			break;
		case 7:
			sprintf(ChannelStr,"%s(Channel 7)","2442MHz");
			break;
		case 8:
			sprintf(ChannelStr,"%s(Channel 8)","2447MHz");
			break;
		case 9:
			sprintf(ChannelStr,"%s(Channel 9)","2452MHz");
			break;
		case 10:
			sprintf(ChannelStr,"%s(Channel 10)","2457MHz");
			break;
		case 11:
			sprintf(ChannelStr,"%s(Channel 11)","2462MHz");
			break;
		case 12:
			sprintf(ChannelStr,"%s(Channel 12)","2467MHz");
			break;																			
		case 13:
			sprintf(ChannelStr,"%s(Channel 13)","2472MHz");
			break;
		case 14:
			sprintf(ChannelStr,"%s(Channel 14)","2477MHz");
			break;
		default:
			sprintf(ChannelStr,"%s(Channel 1)","2412MHz");
			break;
	}
	printf("current channel is %s\n",ChannelStr);
*/
	return websWrite(wp, T("%d"),channel);
}
/* goform/wirelessBasic */
static void wirelessBasic(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wirelessmode;
	char_t	*mssid_0, *mssid_1, *mssid_2, *mssid_3, *mssid_4, *mssid_5, *mssid_6,
			*mssid_7, *bssid_num, *hssid, *isolated_ssid, *mbssidapisolated;
#if defined (RT2860_NEW_MBSS_SUPPORT)
	char_t  *mssid_8, *mssid_9, *mssid_10, *mssid_11, *mssid_12, *mssid_13, *mssid_14,*mssid_15;
#endif
	char_t	*sz11aChannel, *sz11bChannel, *sz11gChannel, *abg_rate;
	char_t  *n_mode, *n_bandwidth, *n_gi, *n_mcs, *n_rdg, *n_extcha, *n_stbc;
	char_t  *n_amsdu, *n_autoba, *n_badecline, *n_disallow_tkip, *n_2040_coexit, *n_ldpc;
	char_t  *vht_bandwidth, *vht_stbc, *vht_sgi, *vht_bw_signal, *vht_ldpc;
	char_t	*tx_stream, *rx_stream;
	char_t	*radio;
#if defined (RT2860_NEW_MBSS_SUPPORT)
	char	hidden_ssid[32], noforwarding[32];
#else
	char	hidden_ssid[16], noforwarding[16];
#endif
	int i = 0, is_n = 0, is_ac = 0, new_bssid_num, old_bssid_num = 1;

	radio = websGetVar(wp, T("radiohiddenButton"), T("2"));
	if (!strncmp(radio, "0", 2)) {
		doSystem("iwpriv ra0 set RadioOn=0");
		nvram_set(RT2860_NVRAM, "RadioOff", "1");
		websRedirect(wp, "wireless/basic.asp");
		return;
	}
	else if (!strncmp(radio, "1", 2)) {
		doSystem("iwpriv ra0 set RadioOn=1");
		nvram_set(RT2860_NVRAM, "RadioOff", "0");
		websRedirect(wp, "wireless/basic.asp");
		return;
	}

	radio = websGetVar(wp, T("wifihiddenButton"), T("2"));
	if (!strncmp(radio, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT)
		int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		do {
			bssid_num--;
			doSystem("ifconfig ra%d down", bssid_num);
		} while (bssid_num > 0);
#else
		doSystem("ifconfig ra0 down");
#endif
		doSystem("rmmod rt2860v2_ap");
		doSystem("reg s b0180000");
		FILE *pp = popen("reg p 400", "r");
		char reg[11];
		fscanf(pp, "%s\n", reg);
		pclose(pp);
		doSystem("reg w 400 %x", strtoul(reg, NULL, 16)&(~(7<<9)));
		doSystem("reg w 1204 8");
		doSystem("reg w 1004 3");
		nvram_set(RT2860_NVRAM, "WiFiOff", "1");
		websRedirect(wp, "wireless/basic.asp");
		return;
	}
	else if (!strncmp(radio, "1", 2)) {
		doSystem("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT)
		int idx = 0;
		int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		do {
			doSystem("ifconfig ra%d up", idx);
			idx++;
		} while (idx < bssid_num);
#else
		doSystem("ifconfig ra0 up");
#endif
		nvram_set(RT2860_NVRAM, "WiFiOff", "0");
		websRedirect(wp, "wireless/basic.asp");
		return;
	}

	//fetch from web input
	wirelessmode = websGetVar(wp, T("wirelessmode"), T("9")); //9: bgn mode
	mssid_0 = websGetVar(wp, T("mssid_0"), T("")); 
	mssid_1 = websGetVar(wp, T("mssid_1"), T("")); 
	mssid_2 = websGetVar(wp, T("mssid_2"), T("")); 
	mssid_3 = websGetVar(wp, T("mssid_3"), T("")); 
	mssid_4 = websGetVar(wp, T("mssid_4"), T("")); 
	mssid_5 = websGetVar(wp, T("mssid_5"), T("")); 
	mssid_6 = websGetVar(wp, T("mssid_6"), T("")); 
	mssid_7 = websGetVar(wp, T("mssid_7"), T("")); 
#if defined (RT2860_NEW_MBSS_SUPPORT)
	mssid_8 = websGetVar(wp, T("mssid_8"), T(""));
	mssid_9 = websGetVar(wp, T("mssid_9"), T(""));
	mssid_10 = websGetVar(wp, T("mssid_10"), T(""));
	mssid_11 = websGetVar(wp, T("mssid_11"), T(""));
	mssid_12 = websGetVar(wp, T("mssid_12"), T(""));
	mssid_13 = websGetVar(wp, T("mssid_13"), T(""));
	mssid_14 = websGetVar(wp, T("mssid_14"), T(""));
	mssid_15 = websGetVar(wp, T("mssid_15"), T(""));
#endif
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	hssid = websGetVar(wp, T("hssid"), T("")); 
	isolated_ssid = websGetVar(wp, T("isolated_ssid"), T(""));
	mbssidapisolated = websGetVar(wp, T("mbssidapisolated"), T("0"));
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T("")); 
	sz11bChannel = websGetVar(wp, T("sz11bChannel"), T("")); 
	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T("")); 
	abg_rate = websGetVar(wp, T("abg_rate"), T("")); 
	n_mode = websGetVar(wp, T("n_mode"), T("0"));
	n_bandwidth = websGetVar(wp, T("n_bandwidth"), T("0"));
	n_gi = websGetVar(wp, T("n_gi"), T("0"));
	n_mcs = websGetVar(wp, T("n_mcs"), T("33"));
	n_rdg = websGetVar(wp, T("n_rdg"), T("0"));
	n_extcha = websGetVar(wp, T("n_extcha"), T("0"));
	n_stbc = websGetVar(wp, T("n_stbc"), T("0"));
	n_amsdu = websGetVar(wp, T("n_amsdu"), T("0"));
	n_autoba = websGetVar(wp, T("n_autoba"), T("0"));
	n_badecline = websGetVar(wp, T("n_badecline"), T("0"));
	n_disallow_tkip = websGetVar(wp, T("n_disallow_tkip"), T("0"));
	n_2040_coexit = websGetVar(wp, T("n_2040_coexit"), T("0"));
	n_ldpc = websGetVar(wp, T("n_ldpc"), T("0"));
	vht_bandwidth = websGetVar(wp, T("vht_bandwidth"), T("0"));
	vht_stbc = websGetVar(wp, T("vht_stbc"), T("0"));
	vht_sgi = websGetVar(wp, T("vht_sgi"), T("0"));
	vht_bw_signal = websGetVar(wp, T("vht_bw_signal"), T("0"));
	vht_ldpc = websGetVar(wp, T("vht_ldpc"), T("0"));
	tx_stream = websGetVar(wp, T("tx_stream"), T("0"));
	rx_stream = websGetVar(wp, T("rx_stream"), T("0"));

	old_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
	new_bssid_num = atoi(bssid_num);

	nvram_bufset(RT2860_NVRAM, "WirelessMode", wirelessmode);
	//BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) //g, gn
		nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "1", 2)) //b
		nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	else //bg,bgn,n
		nvram_bufset(RT2860_NVRAM, "BasicRate", "15");

	if (!strncmp(wirelessmode, "8", 2) || !strncmp(wirelessmode, "9", 2) ||
		!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 3))
		is_n = 1;
	else if (!strncmp(wirelessmode, "14", 2) || !strncmp(wirelessmode, "15", 2))
		is_ac = 1;

	//SSID, Multiple SSID
	if (0 == strlen(mssid_0)) {
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	} else {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_0);
		if (strchr(hssid, '0') != NULL)
			sprintf(hidden_ssid, "%s", "1");
		else
			sprintf(hidden_ssid, "%s", "0");
		if (strchr(isolated_ssid, '0') != NULL)
			sprintf(noforwarding, "%s", "1");
		else
			sprintf(noforwarding, "%s", "0");
	}

//#WPS
	if (!strcmp(hidden_ssid, "1")) {
		nvram_bufset(RT2860_NVRAM, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else {
		const char *wordlist= nvram_bufget(RT2860_NVRAM, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				doSystem("iwpriv ra0 set WscConfStatus=1");
			nvram_bufset(RT2860_NVRAM, "WscConfigured", "1");
			g_wsc_configured = 1;
		}
	}

//#WPS
	default_shown_mbssid[RT2860_NVRAM] = 0;

	if (0 != strlen(mssid_1)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_1);
		if (strchr(hssid, '1') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '1') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_2)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_2);
		if (strchr(hssid, '2') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '2') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_3)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_3);
		if (strchr(hssid, '3') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '3') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_4)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_4);
		if (strchr(hssid, '4') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '4') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_5)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_5);
		if (strchr(hssid, '5') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '5') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_6)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_6);
		if (strchr(hssid, '6') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '6') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}
	if (0 != strlen(mssid_7)) {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_7);
		if (strchr(hssid, '7') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '7') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
	}

#if defined (RT2860_NEW_MBSS_SUPPORT)
	if (0 != strlen(mssid_8)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_8);
		if (strchr(hssid, '8') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '8') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_9)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_9);
		if (strchr(hssid, '9') != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '9') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_10)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_10);
		if (strstr(hssid, "10") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '10') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_11)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_11);
		if (strstr(hssid, "11") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '11') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_12)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_12);
		if (strstr(hssid, "12") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '12') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_13)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_13);
		if (strstr(hssid, "13") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '13') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_14)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_14);
		if (strstr(hssid, "14") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '14') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
	if (0 != strlen(mssid_15)) {
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_15);
		if (strstr(hssid, "15") != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, ";0");
		if (strchr(isolated_ssid, '15') != NULL)
			sprintf(noforwarding, "%s%s", noforwarding, ";1");
		else
			sprintf(noforwarding, "%s%s", noforwarding, ";0");
		i++;
	}
#endif

	nvram_bufset(RT2860_NVRAM, "BssidNum", bssid_num);
#if defined (RT2860_NEW_MBSS_SUPPORT)
	if (new_bssid_num < 1 || new_bssid_num > 16) {
#else
	if (new_bssid_num < 1 || new_bssid_num > 8) {
#endif
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'bssid_num' %s is out of range!"), bssid_num);
		return;
	}
	revise_mbss_value(old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);

	// NoForwarding and NoForwardingBTNBSSID
	nvram_bufset(RT2860_NVRAM, "NoForwarding", noforwarding);
	nvram_bufset(RT2860_NVRAM, "NoForwardingBTNBSSID", mbssidapisolated);

	//11abg Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
			(0 == strlen(sz11gChannel))) {
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'Channel' should not be empty!"));
		return;
	}
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
			!strncmp(sz11gChannel, "0", 2))
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "1");
	else
		nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
	if (0 != strlen(sz11aChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11aChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11aChannel);
	}
	if (0 != strlen(sz11bChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11bChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11bChannel);
	}
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(RT2860_NVRAM, "Channel", sz11gChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);
	}
	sleep(1);

	//Rate for a, b, g
	if (strncmp(abg_rate, "", 1)) {
		int rate = atoi(abg_rate);
		if (!strncmp(wirelessmode, "0", 2) || !strncmp(wirelessmode, "2", 2) || !strncmp(wirelessmode, "4", 2)) {
			if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
				nvram_bufset(RT2860_NVRAM, "FixedTxMode", "CCK");
			else
				nvram_bufset(RT2860_NVRAM, "FixedTxMode", "OFDM");

			if (rate == 1)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "2");
			else if (rate == 6)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "0");
			else if (rate == 9)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "1");
			else if (rate == 11)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "3");
			else if (rate == 12)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "2");
			else if (rate == 18)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "3");
			else if (rate == 24)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "4");
			else if (rate == 36)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "5");
			else if (rate == 48)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "6");
			else if (rate == 54)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "7");
			else
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "33");
		}
		else if (!strncmp(wirelessmode, "1", 2)) {
			nvram_bufset(RT2860_NVRAM, "FixedTxMode", "CCK");
			if (rate == 1)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "2");
			else if (rate == 11)
				SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, "3");
		}
		else //shall not happen
			error(E_L, E_LOG, T("wrong configurations from web UI"));
	}
	else
		nvram_bufset(RT2860_NVRAM, "FixedTxMode", "HT");

	//HT_OpMode, HT_BW, HT_GI, HT_MCS, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_n | is_ac) {
		nvram_bufset(RT2860_NVRAM, "HT_OpMode", n_mode);
		nvram_bufset(RT2860_NVRAM, "HT_BW", n_bandwidth);
		nvram_bufset(RT2860_NVRAM, "HT_GI", n_gi);
		SetAllValues(new_bssid_num, RT2860_NVRAM, HT_MCS, n_mcs);
		nvram_bufset(RT2860_NVRAM, "HT_RDG", n_rdg);
		nvram_bufset(RT2860_NVRAM, "HT_EXTCHA", n_extcha);
		nvram_bufset(RT2860_NVRAM, "HT_STBC", n_stbc);
		nvram_bufset(RT2860_NVRAM, "HT_AMSDU", n_amsdu);
		nvram_bufset(RT2860_NVRAM, "HT_AutoBA", n_autoba);
		nvram_bufset(RT2860_NVRAM, "HT_BADecline", n_badecline);
		nvram_bufset(RT2860_NVRAM, "HT_DisallowTKIP", n_disallow_tkip);
		nvram_bufset(RT2860_NVRAM, "HT_BSSCoexistence", n_2040_coexit);
		nvram_bufset(RT2860_NVRAM, "HT_LDPC", n_ldpc);
	}
	if (is_ac) {
		nvram_bufset(RT2860_NVRAM, "VHT_BW", vht_bandwidth);
		nvram_bufset(RT2860_NVRAM, "VHT_STBC", vht_stbc);
		nvram_bufset(RT2860_NVRAM, "VHT_SGI", vht_sgi);
		nvram_bufset(RT2860_NVRAM, "VHT_BW_SIGNAL", vht_bw_signal);
		nvram_bufset(RT2860_NVRAM, "VHT_LDPC", vht_ldpc);
	}
	nvram_bufset(RT2860_NVRAM, "HT_TxStream", tx_stream);
	nvram_bufset(RT2860_NVRAM, "HT_RxStream", rx_stream);

	if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 2))
	{
		int mbssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		int i = 0;

		while (i < mbssid_num)
		{
			char tmp[128]; 
			char *EncrypType = (char *) nvram_bufget(RT2860_NVRAM, "EncrypType");
			getNthValueSafe(i, EncrypType, ';', tmp, 128);
			// fprintf(stderr, "SSID%d's EncrypType: %s\n", i, tmp);
			if (!strcmp(tmp, "NONE") || !strcmp(tmp, "AES"))
			{
				memset(tmp, 0, 128);
				char *IEEE8021X = (char *) nvram_bufget(RT2860_NVRAM, "IEEE8021X");
				getNthValueSafe(i, IEEE8021X, ';', tmp, 128);
				// fprintf(stderr, "SSID%d's IEEE8021X: %s\n", i, tmp);
				if (!strcmp(tmp, "0"))
				{
					i++;
					continue;
				}
			}
			STFs(RT2860_NVRAM, i, "AuthMode", "OPEN");
			STFs(RT2860_NVRAM, i, "EncrypType", "NONE");
			STFs(RT2860_NVRAM, i, "IEEE8021X", "0");
			i++;
		}
	}

	nvram_commit(RT2860_NVRAM);
//#if CONFIG_RT2860V2_AP == CONFIG_MIPS
	/* this is a workaround:
	 *  when AP is built as kernel
	 *  if more ssids are created, driver won't exe RT28xx_MBSS_Init again
	 *  therefore, we reboot to make it available
	 *  (PS. CONFIG_MIPS would be "y")
	 */
	if (new_bssid_num > old_bssid_num)
		doSystem("reboot");
//#endif
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h2>mode: %s</h2><br>\n"), wirelessmode);
	websWrite(wp, T("mssid_0: %s, bssid_num: %s<br>\n"), mssid_0, bssid_num);
	websWrite(wp, T("mssid_1: %s, mssid_2: %s, mssid_3: %s<br>\n"),
			mssid_1, mssid_2, mssid_3);
	websWrite(wp, T("mssid_4: %s, mssid_5: %s, mssid_6: %s, mssid_7: %s<br>\n"),
			mssid_4, mssid_5, mssid_6, mssid_7);
#if defined (RT2860_NEW_MBSS_SUPPORT)
	websWrite(wp, T("mssid_8: %s, mssid_9: %s, mssid_10: %s, mssid_11: %s<br>\n"),
			mssid_8, mssid_9, mssid_10), mssid_11;
	websWrite(wp, T("mssid_12: %s, mssid_13: %s, mssid_14: %s, mssid_15: %s<br>\n"),
			mssid_12, mssid_13, mssid_14, mssid_15);
#endif
	websWrite(wp, T("hssid: %s<br>\n"), hssid);
	websWrite(wp, T("isolated_ssid: %s<br>\n"), isolated_ssid);
	websWrite(wp, T("mbssidapisolated: %s<br>\n"), mbssidapisolated);
	websWrite(wp, T("sz11aChannel: %s<br>\n"), sz11aChannel);
	websWrite(wp, T("sz11bChannel: %s<br>\n"), sz11bChannel);
	websWrite(wp, T("sz11gChannel: %s<br>\n"), sz11gChannel);
	if (strncmp(abg_rate, "", 1)) {
		websWrite(wp, T("abg_rate: %s<br>\n"), abg_rate);
	}
	if (is_n) {
		websWrite(wp, T("n_mode: %s<br>\n"), n_mode);
		websWrite(wp, T("n_bandwidth: %s<br>\n"), n_bandwidth);
		websWrite(wp, T("n_gi: %s<br>\n"), n_gi);
		websWrite(wp, T("n_mcs: %s<br>\n"), n_mcs);
		websWrite(wp, T("n_rdg: %s<br>\n"), n_rdg);
		websWrite(wp, T("n_extcha: %s<br>\n"), n_extcha);
		websWrite(wp, T("n_stbc: %s<br>\n"), n_stbc);
		websWrite(wp, T("n_amsdu: %s<br>\n"), n_amsdu);
		websWrite(wp, T("n_autoba: %s<br>\n"), n_autoba);
		websWrite(wp, T("n_badecline: %s<br>\n"), n_badecline);
		websWrite(wp, T("n_disallow_tkip: %s<br>\n"), n_disallow_tkip);
		websWrite(wp, T("n_2040_coexit: %s<br>\n"), n_2040_coexit);
		websWrite(wp, T("n_ldpc: %s<br>\n"), n_ldpc);
	}
	if (is_ac) {
		websWrite(wp, T("vht_bandwidth: %s<br>\n"), vht_bandwidth);
		websWrite(wp, T("vht_stbc: %s<br>\n"), vht_stbc);
		websWrite(wp, T("vht_sgi: %s<br>\n"), vht_sgi);
		websWrite(wp, T("vht_bw_signal: %s<br>\n"), vht_bw_signal);
		websWrite(wp, T("vht_ldpc: %s<br>\n"), vht_ldpc);
	}
	websWrite(wp, T("tx_stream: %s<br>\n"), tx_stream);
	websWrite(wp, T("rx_stream: %s<br>\n"), rx_stream);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/wirelessAdvanced */
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query)
{
	char_t	*bg_protection, /**basic_rate,*/ *beacon, *dtim, *fragment, *rts,
			*tx_power, *short_preamble, *short_slot, *tx_burst, *pkt_aggregate,
			*ieee_80211h, *wmm_capable, *apsd_capable, *dls_capable, 
			*countrycode, *countryregion;
	char_t	*m2u_enable, *rd_region, *carrier_detect;
	int		i, ssid_num, wlan_mode;
	char	wmm_enable[16];
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	char_t  *auto_provision;
#endif
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	char_t	*rvt, *oldrx, *oldtx;
	int		oldrvt;
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT)
	char_t	*txbf;
#endif // TXBF //

	//fetch from web input
	bg_protection = websGetVar(wp, T("bg_protection"), T("0"));
	//basic_rate = websGetVar(wp, T("basic_rate"), T("15"));
	beacon = websGetVar(wp, T("beacon"), T("100"));
	dtim = websGetVar(wp, T("dtim"), T("1"));
	fragment = websGetVar(wp, T("fragment"), T("2346"));
	rts = websGetVar(wp, T("rts"), T("2347"));
	tx_power = websGetVar(wp, T("tx_power"), T("100"));
	short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
	short_slot = websGetVar(wp, T("short_slot"), T("0"));
	tx_burst = websGetVar(wp, T("tx_burst"), T("0"));
	pkt_aggregate = websGetVar(wp, T("pkt_aggregate"), T("0"));
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	auto_provision = websGetVar(wp, T("auto_provision"), T("0"));
#endif
	ieee_80211h = websGetVar(wp, T("ieee_80211h"), T("0"));
	rd_region = websGetVar(wp, T("rd_region"), T(""));
	carrier_detect = websGetVar(wp, T("carrier_detect"), T("0"));
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	rvt = websGetVar(wp, T("rvt"), T("0"));
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT)
	txbf = websGetVar(wp, T("txbf"), T("0"));
#endif // TXBF //
	wmm_capable = websGetVar(wp, T("wmm_capable"), T("0"));
	apsd_capable = websGetVar(wp, T("apsd_capable"), T("0"));
	dls_capable = websGetVar(wp, T("dls_capable"), T("0"));
	m2u_enable = websGetVar(wp, T("m2u_enable"), T("0"));
	countrycode = websGetVar(wp, T("country_code"), T("NONE"));

	if (NULL != nvram_bufget(RT2860_NVRAM, "BssidNum"))
		ssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
	else
		ssid_num = 1;
	wlan_mode = atoi(nvram_bufget(RT2860_NVRAM, "WirelessMode"));

#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (NULL != nvram_bufget(RT2860_NVRAM, "RVT"))
		oldrvt = atoi(nvram_bufget(RT2860_NVRAM, "RVT"));
	else
		oldrvt = 0;
#endif // getRVTBuilt //

	//set to nvram
	nvram_bufset(RT2860_NVRAM, "BGProtection", bg_protection);
	//nvram_bufset(RT2860_NVRAM, "BasicRate", basic_rate);
	nvram_bufset(RT2860_NVRAM, "BeaconPeriod", beacon);
	nvram_bufset(RT2860_NVRAM, "DtimPeriod", dtim);
	nvram_bufset(RT2860_NVRAM, "FragThreshold", fragment);
	nvram_bufset(RT2860_NVRAM, "RTSThreshold", rts);
	nvram_bufset(RT2860_NVRAM, "TxPower", tx_power);
	nvram_bufset(RT2860_NVRAM, "TxPreamble", short_preamble);
	nvram_bufset(RT2860_NVRAM, "ShortSlot", short_slot);
	nvram_bufset(RT2860_NVRAM, "TxBurst", tx_burst);
	nvram_bufset(RT2860_NVRAM, "PktAggregate", pkt_aggregate);
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	nvram_bufset(RT2860_NVRAM, "AutoProvisionEn", auto_provision);
#endif
	nvram_bufset(RT2860_NVRAM, "IEEE80211H", ieee_80211h);
	nvram_bufset(RT2860_NVRAM, "RDRegion", rd_region);
	nvram_bufset(RT2860_NVRAM, "CarrierDetect", carrier_detect);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(rvt, "1", 2) && oldrvt != 1) {
		//backup the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(RT2860_NVRAM, "HT_RxStream");
		oldtx = nvram_bufget(RT2860_NVRAM, "HT_TxStream");
		nvram_bufset(RT2860_NVRAM, "HT_RxStream_old", oldrx);
		nvram_bufset(RT2860_NVRAM, "HT_TxStream_old", oldtx);

		nvram_bufset(RT2860_NVRAM, "RVT", "1");
		nvram_bufset(RT2860_NVRAM, "HT_RxStream", "1");
		nvram_bufset(RT2860_NVRAM, "HT_TxStream", "1");
		nvram_bufset(RT2860_NVRAM, "HT_GI", "0");
		nvram_bufset(RT2860_NVRAM, "VideoTurbine", "1");
	}
	else if (!strncmp(rvt, "0", 2) && oldrvt != 0) {
		//restore the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(RT2860_NVRAM, "HT_RxStream_old");
		oldtx = nvram_bufget(RT2860_NVRAM, "HT_TxStream_old");

		nvram_bufset(RT2860_NVRAM, "RVT", "0");
		nvram_bufset(RT2860_NVRAM, "HT_RxStream", oldrx);
		nvram_bufset(RT2860_NVRAM, "HT_TxStream", oldtx);
		nvram_bufset(RT2860_NVRAM, "HT_GI", "1");
		nvram_bufset(RT2860_NVRAM, "VideoTurbine", "0");
	}
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(txbf, "3", 2)) {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "1");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "0");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "1");
	}
	else if (!strncmp(txbf, "2", 2)) {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "1");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "0");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "0");
	}
	else if (!strncmp(txbf, "1", 2)) {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "0");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "1");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "1");
	}
	else {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "0");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "1");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "0");
	}
#endif // TXBF //
	nvram_bufset(RT2860_NVRAM, "APSDCapable", apsd_capable);
	nvram_bufset(RT2860_NVRAM, "DLSCapable", dls_capable);
	nvram_bufset(RT2860_NVRAM, "M2UEnabled", m2u_enable);

	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d", atoi(wmm_capable));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	nvram_bufset(RT2860_NVRAM, "WmmCapable", wmm_enable);

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(RT2860_NVRAM, "TxBurst", "0");
	}

#if ! defined CONFIG_EXT_CHANNEL_LIST
	if (wlan_mode == 0 || wlan_mode == 1 || wlan_mode == 4 ||
		wlan_mode == 9 || wlan_mode == 6) {
		countryregion = websGetVar(wp, T("country_region"), T("5"));
		if (!strcmp(countryregion, "8"))
			nvram_bufset(RT2860_NVRAM, "CountryRegion", "31");
		else if (!strcmp(countryregion, "9"))
			nvram_bufset(RT2860_NVRAM, "CountryRegion", "32");
		else if (!strcmp(countryregion, "10"))
			nvram_bufset(RT2860_NVRAM, "CountryRegion", "33");
		else
			nvram_bufset(RT2860_NVRAM, "CountryRegion", countryregion);
	} else if (wlan_mode == 2 || wlan_mode == 8 || wlan_mode == 11 ||
			   wlan_mode == 14 || wlan_mode == 15) {
		countryregion = websGetVar(wp, T("country_region"), T("7"));
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", countryregion);
	}
#endif
	nvram_bufset(RT2860_NVRAM, "CountryCode", countrycode);

	nvram_commit(RT2860_NVRAM);
	initInternet();

#if defined (RT2860_IGMPSNOOP_SUPPORT)
	if (!strncmp(m2u_enable, "1", 2))
		doSystem("iwpriv ra0 set IgmpSnEnable=1");
	else
		doSystem("iwpriv ra0 set IgmpSnEnable=0");
#endif

	//debug print
	websHeader(wp);
	websWrite(wp, T("bg_protection: %s<br>\n"), bg_protection);
	//websWrite(wp, T("basic_rate: %s<br>\n"), basic_rate);
	websWrite(wp, T("beacon: %s<br>\n"), beacon);
	websWrite(wp, T("dtim: %s<br>\n"), dtim);
	websWrite(wp, T("fragment: %s<br>\n"), fragment);
	websWrite(wp, T("rts: %s<br>\n"), rts);
	websWrite(wp, T("tx_power: %s<br>\n"), tx_power);
	websWrite(wp, T("short_preamble: %s<br>\n"), short_preamble);
	websWrite(wp, T("short_slot: %s<br>\n"), short_slot);
	websWrite(wp, T("tx_burst: %s<br>\n"), tx_burst);
	websWrite(wp, T("pkt_aggregate: %s<br>\n"), pkt_aggregate);
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	websWrite(wp, T("auto_provision: %s<br>\n"), auto_provision);
#endif
	websWrite(wp, T("ieee_80211h: %s<br>\n"), ieee_80211h);
	websWrite(wp, T("rd_region: %s<br>\n"), rd_region);
	websWrite(wp, T("carrier_detect: %s<br>\n"), carrier_detect);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	websWrite(wp, T("RVT: %s<br>\n"), rvt);
#endif
#if defined (RT2860_TXBF_SUPPORT)
	websWrite(wp, T("txbf: %s<br>\n"), txbf);
#endif // TXBF //
	websWrite(wp, T("wmm_capable: %s<br>\n"), wmm_capable);
	websWrite(wp, T("apsd_capable: %s<br>\n"), apsd_capable);
	websWrite(wp, T("dls_capable: %s<br>\n"), dls_capable);
	websWrite(wp, T("countrycode: %s<br>\n"), countrycode);
#if defined (RT2860_IGMPSNOOP_SUPPORT)
	websWrite(wp, T("m2u_enable: %s<br>\n"), m2u_enable);
#endif
	websFooter(wp);
	websDone(wp, 200);
}

#if	defined (RT2860_WDS_SUPPORT)
/* goform/wirelessWds */
static void wirelessWds(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wds_mode, *wds_phy_mode, *wds_encryp_type, *wds_encryp_key0,
			*wds_encryp_key1,*wds_encryp_key2, *wds_encryp_key3, *wds_list;

	wds_mode = websGetVar(wp, T("wds_mode"), T("0"));
	wds_phy_mode = websGetVar(wp, T("wds_phy_mode"), T(""));
	wds_encryp_type = websGetVar(wp, T("wds_encryp_type"), T(""));
	wds_encryp_key0 = websGetVar(wp, T("wds_encryp_key0"), T(""));
	wds_encryp_key1 = websGetVar(wp, T("wds_encryp_key1"), T(""));
	wds_encryp_key2 = websGetVar(wp, T("wds_encryp_key2"), T(""));
	wds_encryp_key3 = websGetVar(wp, T("wds_encryp_key3"), T(""));
	wds_list = websGetVar(wp, T("wds_list"), T(""));

	nvram_bufset(RT2860_NVRAM, "WdsEnable", wds_mode);
	if (strncmp(wds_mode, "0", 2)) {
		nvram_bufset(RT2860_NVRAM, "WdsPhyMode", wds_phy_mode);
		nvram_bufset(RT2860_NVRAM, "WdsEncrypType", wds_encryp_type);
		nvram_bufset(RT2860_NVRAM, "Wds0Key", wds_encryp_key0);
		nvram_bufset(RT2860_NVRAM, "Wds1Key", wds_encryp_key1);
		nvram_bufset(RT2860_NVRAM, "Wds2Key", wds_encryp_key2);
		nvram_bufset(RT2860_NVRAM, "Wds3Key", wds_encryp_key3);
		if (!strncmp(wds_mode, "2", 2) || !strncmp(wds_mode, "3", 2)) {
			if (0 != strlen(wds_list))
				nvram_bufset(RT2860_NVRAM, "WdsList", wds_list);
		}
	}
	nvram_commit(RT2860_NVRAM);

	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("wds_mode: %s<br>\n"), wds_mode);
	websWrite(wp, T("wds_phy_mode: %s<br>\n"), wds_phy_mode);
	websWrite(wp, T("wds_encryp_type: %s<br>\n"), wds_encryp_type);
	websWrite(wp, T("wds_encryp_key0: %s<br>\n"), wds_encryp_key0);
	websWrite(wp, T("wds_encryp_key1: %s<br>\n"), wds_encryp_key1);
	websWrite(wp, T("wds_encryp_key2: %s<br>\n"), wds_encryp_key2);
	websWrite(wp, T("wds_encryp_key3: %s<br>\n"), wds_encryp_key3);
	websWrite(wp, T("wds_list: %s<br>\n"), wds_list);
	websFooter(wp);
	websDone(wp, 200);
}
#endif


#ifdef DARE_CUSTOMER_WEB

void quickConfWEP(int nvram, webs_t wp, int mbssid)
{
	char_t *DefaultKeyID, *Key1Type, *Key1Str;

	LFW(DefaultKeyID, wep_default_key);
	LFW(Key1Str, wep_key_1);
	LFW(Key1Type, WEP1Select);

	STF(nvram, mbssid, DefaultKeyID);
	STF(nvram, mbssid, Key1Type);

	if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
			|| (RTDEV_NVRAM == nvram)
#endif
			){
				
		nvram_bufset(nvram, racat("Key1Str", mbssid+1), Key1Str);
    printf("######## Key1Str1:%s\n",Key1Str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	} else if (RTDEV_NVRAM == nvram) {
		
		STF(nvram, mbssid, Key1Str);
		
#endif
	}
}


void quickConfWPAGeneral(int nvram, webs_t wp, int mbssid)
{
	char *key_renewal_interval;
	char *wepdefaultkey;

	LFW(key_renewal_interval, keyRenewalInterval);
	LFW(wepdefaultkey, wep_default_key);

	STFs(nvram, mbssid, "EncrypType", "AES");

	STFs(nvram, mbssid, "DefaultKeyID", wepdefaultkey);	// DefaultKeyID is 1
	STFs(nvram, mbssid, "RekeyInterval", key_renewal_interval);
	STFs(nvram, mbssid, "RekeyMethod", "TIME");		
	STFs(nvram, mbssid, "IEEE8021X", "0");
}


void quickSecurity(int nvram, webs_t wp, char_t *path, char_t *query)
{
	char_t *SSID;
	int mbssid;
	char_t *security_mode;
	//char_t *wifi_strength;
	//char_t *tx_power;

	LFW(SSID, ssidIndex);
	if(!gstrlen(SSID))
		return;

	mbssid = atoi(SSID);

	default_shown_mbssid[nvram] = mbssid;

	LFW(security_mode, security_mode);
	//LFW(wifi_strength, wifi_strength);

#ifndef CONFIG_RALINK_RT2880
	// RT2880: GPIO13 is belong to WPS PBC indicator.

	if (!strcmp(security_mode, "Disable") || !strcmp(security_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)
#endif

/*
	if (!strcmp(wifi_strength, "0"))
		tx_power = "100";
	else if(!strcmp(wifi_strength, "1"))
		tx_power = "60";
	else if(!strcmp(wifi_strength, "2"))
		tx_power = "40";
			
	if (tx_power != NULL)		
		nvram_bufset(RT2860_NVRAM, "TxPower", tx_power);
*/

	if (!strcmp(security_mode, "OPEN")) {				// Open-WEP Mode
		quickConfWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else if (!strcmp(security_mode, "WPAPSK")) {		// WPA Personal Mode
		char *pass_phrase_str;

		quickConfWPAGeneral(nvram, wp, mbssid);

		LFW(pass_phrase_str, passphrase);

		STFs(nvram, mbssid, "AuthMode", security_mode);

		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				){
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			// nvram_bufset(nvram, "WPAPSK", pass_phrase_str);
			STFs(nvram, mbssid, "WPAPSK", pass_phrase_str);
#endif
		}
	} else if (!strcmp(security_mode, "WPA2PSK") ||			// WPA2 Personal Mode
			   !strcmp(security_mode, "WPAPSKWPA2PSK")) { 	// WPA PSK WPA2 PSK mixed
		char *pass_phrase_str;

		quickConfWPAGeneral(nvram, wp, mbssid);
		LFW(pass_phrase_str, passphrase);

		STFs(nvram, mbssid, "AuthMode", security_mode);

		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) {
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			// nvram_bufset(nvram, "WPAPSK", pass_phrase_str);
			STFs(nvram, mbssid, "WPAPSK", pass_phrase_str);
#endif
		}
	}else {													// Open-None Mode
		STFs(nvram, mbssid, "AuthMode", "OPEN");
		STFs(nvram, mbssid, "EncrypType", "NONE");
	}
	
}


int needWiFiModuleUp(){
	
	FILE *fp;
	char buf[60] = {0};
	int isNeed=1;
	
	fp = fopen("/proc/modules", "r");
	if (NULL == fp)
		return -1;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strncmp(buf, "rt2860v2_ap", 11) != 0)
			continue;
		else{
			isNeed = 0;
			break;
		}
	}
	fclose(fp);
	return isNeed;
}

static void wirelessmobset(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mssid_0, *key, *hssid, *authmode;
	char_t	*radio;
#if defined (RT2860_NEW_MBSS_SUPPORT)
	char	hidden_ssid[32];
#else
	char	hidden_ssid[16];
#endif
	char	new_authmode[128];
	int     nBssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));

	int i = 0,new_bssid_num, old_bssid_num = 1;
	radio = websGetVar(wp, T("wifihiddenButton"), T("2"));
	mssid_0 = websGetVar(wp, T("wifiname"), T("")); 
	hssid = websGetVar(wp, T("hssid"), T("")); 
	authmode = websGetVar(wp, T("hiddentype"), T("")); 
	key = websGetVar(wp, T("wifipasswd"), T(""));
//wifi off or on
	if (!strncmp(radio, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT)
		int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		do {
			bssid_num--;
			doSystem("ifconfig ra%d down", bssid_num);
		} while (bssid_num > 0);
#else
		doSystem("ifconfig ra0 down");
#endif
		doSystem("rmmod rt2860v2_ap");
		doSystem("reg s b0180000");
		FILE *pp = popen("reg p 400", "r");
		char reg[11];
		fscanf(pp, "%s\n", reg);
		pclose(pp);
		doSystem("reg w 400 %x", strtoul(reg, NULL, 16)&(~(7<<9)));
		doSystem("reg w 1204 8");
		doSystem("reg w 1004 3");
		nvram_set(RT2860_NVRAM, "WiFiOff", "1");
		websRedirect(wp, "herouter/mobwifisetting.asp");
		return;

	}
	else if (!strncmp(radio, "1", 2)) {
			
			doSystem("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT)
			int idx = 0;
			int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
			do {
				doSystem("ifconfig ra%d up", idx);
				idx++;
			} while (idx < bssid_num);
#else
			doSystem("ifconfig ra0 up");
#endif
			nvram_set(RT2860_NVRAM, "WiFiOff", "0");
	}
	sleep(2);
	
//ssid and ssidhidden
	if (0 == strlen(mssid_0)) {
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	} else {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_0);
		if (strchr(hssid, '1') != NULL)
			sprintf(hidden_ssid, "%s", (nBssid_num >= 2) ? "1;0" : "1");
		else
			sprintf(hidden_ssid, "%s", (nBssid_num >= 2) ? "0;0" : "0");
		nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);
	}
	if(strcmp(authmode,"OPEN")==0)
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", (nBssid_num >= 2) ? "NONE;AES" : "NONE" );
	}
	else
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", (nBssid_num >= 2) ? "AES;AES" : "AES");
	}
	memset(new_authmode, 0x0, sizeof(new_authmode));
	if (nBssid_num >= 2)
	{
		sprintf(new_authmode, "%s;WPAPSKWPA2PSK", authmode);
	}
	else
	{
		sprintf(new_authmode, "%s", authmode);
	}
	nvram_bufset(RT2860_NVRAM, "AuthMode", new_authmode);
	nvram_bufset(RT2860_NVRAM, "WPAPSK1", key);
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
	websRedirect(wp, "herouter/mobwifisetting.asp");
}
/*wireless set  */
static void wirelessSet(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mssid_0, *bssid_num, *hssid, *mbssidapisolated;
	char_t	*radio;
#if defined (RT2860_NEW_MBSS_SUPPORT)
	char	hidden_ssid[32];
#else
	char	hidden_ssid[16];
#endif
	char	web_bssid_num[16];

	int i = 0,new_bssid_num, old_bssid_num = 1;


  printf("########start wirelessSet########\n");

	radio = websGetVar(wp, T("wifihiddenButton"), T("2"));
	if (!strncmp(radio, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT)
		int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		do {
			bssid_num--;
			doSystem("ifconfig ra%d down", bssid_num);
		} while (bssid_num > 0);
#else
		doSystem("ifconfig ra0 down");
#endif
		doSystem("rmmod rt2860v2_ap");
		doSystem("reg s b0180000");
		FILE *pp = popen("reg p 400", "r");
		char reg[11];
		fscanf(pp, "%s\n", reg);
		pclose(pp);
		doSystem("reg w 400 %x", strtoul(reg, NULL, 16)&(~(7<<9)));
		doSystem("reg w 1204 8");
		doSystem("reg w 1004 3");
		nvram_set(RT2860_NVRAM, "WiFiOff", "1");
		websRedirect(wp, "herouter/wifisetting1.asp");
		return;

	}
	else if (!strncmp(radio, "1", 2)) {
		
      doSystem("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT)
      int idx = 0;
			int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
      do {
          doSystem("ifconfig ra%d up", idx);
          idx++;
      } while (idx < bssid_num);
#else
      doSystem("ifconfig ra0 up");
#endif
      nvram_set(RT2860_NVRAM, "WiFiOff", "0");

  }
  sleep(2);
  
	//fetch from web input
	mssid_0 = websGetVar(wp, T("mssid_0"), T("")); 
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	hssid = websGetVar(wp, T("hssid"), T("")); 

	old_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
	/* in our webpage, it is no need set bssid_num, so set it as old */
	// new_bssid_num = atoi(bssid_num);
	new_bssid_num = old_bssid_num;
	memset(web_bssid_num, 0x0, sizeof(web_bssid_num));
	sprintf(web_bssid_num, "%d", new_bssid_num);

	//SSID, Multiple SSID
	if (0 == strlen(mssid_0)) {
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	} else {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_0);
		if (strchr(hssid, '0') != NULL)
			sprintf(hidden_ssid, "%s", (old_bssid_num >= 2) ? "1;0" : "1");
		else
			sprintf(hidden_ssid, "%s", (old_bssid_num >= 2) ? "0;0" : "0");
	}

//#WPS
	if (!strncmp(hidden_ssid, "1", 1)) {
		nvram_bufset(RT2860_NVRAM, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else {
		const char *wordlist= nvram_bufget(RT2860_NVRAM, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				doSystem("iwpriv ra0 set WscConfStatus=1");
			nvram_bufset(RT2860_NVRAM, "WscConfigured", "1");
			g_wsc_configured = 1;
		}
	}

//#WPS
	default_shown_mbssid[RT2860_NVRAM] = 0;

	nvram_bufset(RT2860_NVRAM, "BssidNum", web_bssid_num);
#if defined (RT2860_NEW_MBSS_SUPPORT)
	if (new_bssid_num < 1 || new_bssid_num > 16) {
#else
	if (new_bssid_num < 1 || new_bssid_num > 8) {
#endif
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'bssid_num' %s is out of range!"), web_bssid_num);
		return;
	}
	revise_mbss_value(old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);

	char_t *wifi_strength;
	char_t *tx_power;
	LFW(wifi_strength, wifi_strength);
		
	if (!strcmp(wifi_strength, "0"))
		tx_power = "100";
	else if(!strcmp(wifi_strength, "1"))
		tx_power = "60";
	else if(!strcmp(wifi_strength, "2"))
		tx_power = "40";
			
	if (tx_power != NULL)		
		nvram_bufset(RT2860_NVRAM, "TxPower", tx_power);
		
  quickSecurity(RT2860_NVRAM, wp, path, query);
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
    
	if (new_bssid_num > old_bssid_num)
		doSystem("reboot");
 
	
	initInternet();
	

  //if need redirect
  websRedirect(wp, "herouter/wifisetting1.asp");
  
/*
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("alert(\"设置成功!\");"));
	websWrite(wp, T("window.location.reload();"));
	websWrite(wp, T("</script></head></html>"));
  websDone(wp, 200);	
*/  

}

extern   void QuickSetUp_first(webs_t wp);
extern void setSysAdm(webs_t wp, char_t *path, char_t *query);

/* goform/wirelessQuick */
static void wirelessQuick(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mssid_0, *bssid_num, *hssid, *mbssidapisolated;
	char_t	*radio;
	char_t  *needWizard = NULL;
#if defined (RT2860_NEW_MBSS_SUPPORT)
	char	hidden_ssid[32];
#else
	char	hidden_ssid[16];
#endif
	char	web_bssid_num[16];

	int i = 0,new_bssid_num, old_bssid_num = 1;
  //printf("########保存上一步操作########\n");

   //QuickSetUp_first( wp);

  printf("########start wirelessQuick########\n");

	radio = websGetVar(wp, T("wifihiddenButton"), T("2"));
	if (!strncmp(radio, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT)
		int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
		do {
			bssid_num--;
			doSystem("ifconfig ra%d down", bssid_num);
		} while (bssid_num > 0);
#else
		doSystem("ifconfig ra0 down");
#endif
		doSystem("rmmod rt2860v2_ap");
		doSystem("reg s b0180000");
		FILE *pp = popen("reg p 400", "r");
		char reg[11];
		fscanf(pp, "%s\n", reg);
		pclose(pp);
		doSystem("reg w 400 %x", strtoul(reg, NULL, 16)&(~(7<<9)));
		doSystem("reg w 1204 8");
		doSystem("reg w 1004 3");
		nvram_set(RT2860_NVRAM, "WiFiOff", "1");
		
		//wizard complete
		needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
		if (NULL != needWizard)
		{
		    if (!strcmp(needWizard, "1"))	
		    {
		        nvram_bufset(RT2860_NVRAM, "needWizard", "2");
		        nvram_commit(RT2860_NVRAM);
		    }
		}
		
		websRedirect(wp, "herouter/wifisetting1.asp");
		return;
	}
	else if (!strncmp(radio, "1", 2)) {
		
      doSystem("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT)
      int idx = 0;
			int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
      do {
          doSystem("ifconfig ra%d up", idx);
          idx++;
      } while (idx < bssid_num);
#else
      doSystem("ifconfig ra0 up");
#endif
      nvram_set(RT2860_NVRAM, "WiFiOff", "0");

  }
  //sleep(2);
  
	//fetch from web input
	mssid_0 = websGetVar(wp, T("mssid_0"), T("")); 
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	hssid = websGetVar(wp, T("hssid"), T("")); 

	old_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
	/* in our webpage, it is no need set bssid_num, so set it as old */
	// new_bssid_num = atoi(bssid_num);
	new_bssid_num = old_bssid_num;
	memset(web_bssid_num, 0x0, sizeof(web_bssid_num));
	sprintf(web_bssid_num, "%d", new_bssid_num);

	//SSID, Multiple SSID
	if (0 == strlen(mssid_0)) {
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	} else {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_0);
		if (strchr(hssid, '0') != NULL)
			sprintf(hidden_ssid, "%s", (old_bssid_num >= 2) ? "1;0" : "1");
		else
			sprintf(hidden_ssid, "%s", (old_bssid_num >= 2) ? "0;0" : "0");
	}

//#WPS
	if (!strncmp(hidden_ssid, "1", 1)) {
		nvram_bufset(RT2860_NVRAM, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else {
		const char *wordlist= nvram_bufget(RT2860_NVRAM, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				doSystem("iwpriv ra0 set WscConfStatus=1");
			nvram_bufset(RT2860_NVRAM, "WscConfigured", "1");
			g_wsc_configured = 1;
		}
	}

//#WPS
	default_shown_mbssid[RT2860_NVRAM] = 0;

	nvram_bufset(RT2860_NVRAM, "BssidNum", web_bssid_num);
#if defined (RT2860_NEW_MBSS_SUPPORT)
	if (new_bssid_num < 1 || new_bssid_num > 16) {
#else
	if (new_bssid_num < 1 || new_bssid_num > 8) {
#endif
		nvram_commit(RT2860_NVRAM);
		websError(wp, 403, T("'bssid_num' %s is out of range!"), web_bssid_num);
		return;
	}
	revise_mbss_value(old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);

  quickSecurity(RT2860_NVRAM, wp, path, query);
  
  //wizard complete
  needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
  if (NULL != needWizard)
  {
      if (!strcmp(needWizard, "1"))	
      {
          nvram_bufset(RT2860_NVRAM, "needWizard", "2");
      }
  }
		
  nvram_commit(RT2860_NVRAM);
    
	if (new_bssid_num > old_bssid_num)
		doSystem("reboot");
 
	setSysAdm(wp, path, query);
	//initInternet();
	

  //if need redirect
  //websRedirect(wp, "herouter/wifisetting1.asp");
  
/*
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("alert(\"设置成功!\");"));
	websWrite(wp, T("window.location.reload();"));
	websWrite(wp, T("</script></head></html>"));
  websDone(wp, 200);	
*/  

}
#endif

static void doInitInternet(webs_t wp, char_t *path, char_t *query)
{
	printf("========initInternet========\n");
	initInternet();
}

#if	defined (RT2860_APCLI_SUPPORT)
/* goform/wirelessApcli */
static void wirelessApcli(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ssid, *bssid, *mode, *enc, *wpapsk, *keyid, *keytype,
			*key1, *key2, *key3, *key4;
	char_t	*key1type, *key2type, *key3type, *key4type;

	//fetch from web input
	ssid = websGetVar(wp, T("apcli_ssid"), T(""));
	bssid = websGetVar(wp, T("apcli_bssid"), T(""));
	mode = websGetVar(wp, T("apcli_mode"), T("OPEN"));
	enc = websGetVar(wp, T("apcli_enc"), T("NONE"));
	wpapsk = websGetVar(wp, T("apcli_wpapsk"), T("12345678"));
	keyid = websGetVar(wp, T("apcli_default_key"), T("1"));
	key1type = websGetVar(wp, T("apcli_key1type"), T("1"));
	key2type = websGetVar(wp, T("apcli_key2type"), T("1"));
	key3type = websGetVar(wp, T("apcli_key3type"), T("1"));
	key4type = websGetVar(wp, T("apcli_key4type"), T("1"));
	key1 = websGetVar(wp, T("apcli_key1"), T(""));
	key2 = websGetVar(wp, T("apcli_key2"), T(""));
	key3 = websGetVar(wp, T("apcli_key3"), T(""));
	key4 = websGetVar(wp, T("apcli_key4"), T(""));

	if (gstrlen(ssid) == 0) {
		websError(wp, 200, "SSID is empty");
		return;
	}

	nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	nvram_bufset(RT2860_NVRAM, "ApCliSsid", ssid);
	nvram_bufset(RT2860_NVRAM, "ApCliBssid", bssid);
	nvram_bufset(RT2860_NVRAM, "ApCliAuthMode", mode);
	nvram_bufset(RT2860_NVRAM, "ApCliEncrypType", enc);
	nvram_bufset(RT2860_NVRAM, "ApCliWPAPSK", wpapsk);
	nvram_bufset(RT2860_NVRAM, "ApCliDefaultKeyID", keyid);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Type", key1type);
	nvram_bufset(RT2860_NVRAM, "ApCliKey1Str", key1);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Type", key2type);
	nvram_bufset(RT2860_NVRAM, "ApCliKey2Str", key2);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Type", key3type);
	nvram_bufset(RT2860_NVRAM, "ApCliKey3Str", key3);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Type", key4type);
	nvram_bufset(RT2860_NVRAM, "ApCliKey4Str", key4);
	nvram_commit(RT2860_NVRAM);
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("ssid: %s<br>\n"), ssid);
	websWrite(wp, T("bssid: %s<br>\n"), bssid);
	websWrite(wp, T("mode: %s<br>\n"), mode);
	websWrite(wp, T("enc: %s<br>\n"), enc);
	if (!strcmp(mode, "WPAPSK") || !strcmp(mode, "WPA2PSK")) {
		websWrite(wp, T("wpapsk: %s<br>\n"), wpapsk);
	}
	if (!strcmp(mode, "OPEN") || !strcmp(mode, "SHARED")) {
		websWrite(wp, T("keyid: %s<br>\n"), keyid);
		websWrite(wp, T("key1type: %s<br>\n"), key1type);
		websWrite(wp, T("key2type: %s<br>\n"), key2type);
		websWrite(wp, T("key3type: %s<br>\n"), key3type);
		websWrite(wp, T("key4type: %s<br>\n"), key4type);
		websWrite(wp, T("key1: %s<br>\n"), key1);
		websWrite(wp, T("key2: %s<br>\n"), key2);
		websWrite(wp, T("key3: %s<br>\n"), key3);
		websWrite(wp, T("key4: %s<br>\n"), key4);
	}
	websFooter(wp);
	websDone(wp, 200);
}

static int  ApcliScan(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[4096], *ptr;
	char channel[4], ssid[186], bssid[20], security[23];
	char signal[9], mode[7], ext_ch[7], net_type[3];
	int i, space_start;

	doSystem("iwpriv ra0 set SiteSurvey=1");
	if(!(fp = popen("iwpriv ra0 get_site_survey", "r"))) {
		fprintf(stderr, "%s: execute get_site_survey failed !\n", __func__);
		websError(wp, 500, "%s: execute get_site_survey failed !\n", __func__);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);
	while (fgets(buf, sizeof(buf), fp)) {
		if (strlen(buf) < 4)
			break;
#if 0
		for (i=0; buf[i] != '\n'; i++) {
			if (i%10 == 0)
				fprintf(stderr, "\n%3d:", i);

			//fprintf(stderr, "%2c ", buf[i]);
			fprintf(stderr, "%2x ", buf[i]);
		}
#endif
		ptr = buf;
		sscanf(ptr, "%s ", channel);
		ptr += 37;
		sscanf(ptr, "%s %s %s %s %s %s", bssid, security, signal, mode, ext_ch, net_type);
		ptr = buf+4;
		i = 0;
		while (i < 33) {
			if (ptr[i] == 0x20)
				if (i == 0 || ptr[i-1] != 0x20)
					space_start = i;
			i++;
		}
		ptr[space_start] = '\0';
		strcpy(ssid, buf+4);
		ConverterStringToDisplay(ssid);

		websWrite(wp, "<tr>\n");
		websWrite(wp, "<td>%s</td><td>%s</td>\n", channel, ssid);
		websWrite(wp, "<td>%s</td><td>%s</td>\n", bssid, security);
		websWrite(wp, "<td>%s</td><td>%s</td>\n", signal, mode);
		websWrite(wp, "<td>%s</td><td>%s</td>\n", ext_ch, net_type);
		websWrite(wp, "</tr>\n");
	}
	pclose(fp);
}
#endif

/* goform/wirelessWmm */
static void wirelessWmm(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ap_aifsn_all, *ap_cwmin_all, *ap_cwmax_all, *ap_txop_all,
			*ap_acm_all, *ap_ackpolicy_all,
			*sta_aifsn_all, *sta_cwmin_all, *sta_cwmax_all, *sta_txop_all,
			*sta_acm_all;

	ap_aifsn_all = websGetVar(wp, T("ap_aifsn_all"), T(""));
	ap_cwmin_all = websGetVar(wp, T("ap_cwmin_all"), T(""));
	ap_cwmax_all = websGetVar(wp, T("ap_cwmax_all"), T(""));
	ap_txop_all = websGetVar(wp, T("ap_txop_all"), T(""));
	ap_acm_all = websGetVar(wp, T("ap_acm_all"), T(""));
	ap_ackpolicy_all = websGetVar(wp, T("ap_ackpolicy_all"), T(""));
	sta_aifsn_all = websGetVar(wp, T("sta_aifsn_all"), T(""));
	sta_cwmin_all = websGetVar(wp, T("sta_cwmin_all"), T(""));
	sta_cwmax_all = websGetVar(wp, T("sta_cwmax_all"), T(""));
	sta_txop_all = websGetVar(wp, T("sta_txop_all"), T(""));
	sta_acm_all = websGetVar(wp, T("sta_acm_all"), T(""));

	if (0 != strlen(ap_aifsn_all))
		nvram_bufset(RT2860_NVRAM, "APAifsn", ap_aifsn_all);
	if (0 != strlen(ap_cwmin_all))
		nvram_bufset(RT2860_NVRAM, "APCwmin", ap_cwmin_all);
	if (0 != strlen(ap_cwmax_all))
		nvram_bufset(RT2860_NVRAM, "APCwmax", ap_cwmax_all);
	if (0 != strlen(ap_txop_all))
		nvram_bufset(RT2860_NVRAM, "APTxop", ap_txop_all);
	if (0 != strlen(ap_acm_all))
		nvram_bufset(RT2860_NVRAM, "APACM", ap_acm_all);
	if (0 != strlen(ap_ackpolicy_all))
		nvram_bufset(RT2860_NVRAM, "AckPolicy", ap_ackpolicy_all);
	if (0 != strlen(sta_aifsn_all))
		nvram_bufset(RT2860_NVRAM, "BSSAifsn", sta_aifsn_all);
	if (0 != strlen(sta_cwmin_all))
		nvram_bufset(RT2860_NVRAM, "BSSCwmin", sta_cwmin_all);
	if (0 != strlen(sta_cwmax_all))
		nvram_bufset(RT2860_NVRAM, "BSSCwmax", sta_cwmax_all);
	if (0 != strlen(sta_txop_all))
		nvram_bufset(RT2860_NVRAM, "BSSTxop", sta_txop_all);
	if (0 != strlen(sta_acm_all))
		nvram_bufset(RT2860_NVRAM, "BSSACM", sta_acm_all);

	nvram_commit(RT2860_NVRAM);

	initInternet();

	websHeader(wp);
	websWrite(wp, T("ap_aifsn_all: %s<br>\n"), ap_aifsn_all);
	websWrite(wp, T("ap_cwmin_all: %s<br>\n"), ap_cwmin_all);
	websWrite(wp, T("ap_cwmax_all: %s<br>\n"), ap_cwmax_all);
	websWrite(wp, T("ap_txop_all: %s<br>\n"), ap_txop_all);
	websWrite(wp, T("ap_acm_all: %s<br>\n"), ap_acm_all);
	websWrite(wp, T("ap_ackpolicy_all: %s<br>\n"), ap_ackpolicy_all);
	websWrite(wp, T("sta_aifsn_all: %s<br>\n"), sta_aifsn_all);
	websWrite(wp, T("sta_cwmin_all: %s<br>\n"), sta_cwmin_all);
	websWrite(wp, T("sta_cwmax_all: %s<br>\n"), sta_cwmax_all);
	websWrite(wp, T("sta_txop_all: %s<br>\n"), sta_txop_all);
	websWrite(wp, T("sta_acm_all: %s<br>\n"), sta_acm_all);
	websFooter(wp);
	websDone(wp, 200);
}

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
void restartWAPIDaemon(void)
{
	const char *auth_mode = nvram_bufget(nvram, "AuthMode");

	doSystem("killall wapid 1>/dev/null 2>&1");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT"))
		doSystem("wapid");
	auth_mode = nvram_bufget(RTDEV_NVRAM, "AuthMode");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT"))
		doSystem("wapid -i rai0");
}
#endif

void restart8021XDaemon(int nvram)
{
	int i, num, apd_flag = 0;
	char *auth_mode = (char *) nvram_bufget(nvram, "AuthMode");
	char *ieee8021x = (char *)nvram_bufget(nvram, "IEEE8021X");
	const char *num_s = nvram_bufget(nvram, "BssidNum");
	if(!num_s)
		return;
	num = atoi(num_s);

	if(nvram == RT2860_NVRAM)
		doSystem("killall rt2860apd 1>/dev/null 2>&1");
#if defined (RTDEV_SUPPORT)
	else if(nvram == RTDEV_NVRAM)
		doSystem("killall rtinicapd 1>/dev/null 2>&1");
#elif defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	else if(nvram == RTDEV_NVRAM)
		doSystem("killall rt61apd 1>/dev/null 2>&1");
#endif
	
	/*
	 * In fact we only support mbssid[0] to use 802.1x radius settings.
	 */
	for(i=0; i<num; i++){
		char tmp_auth[128];
		if( getNthValueSafe(i, auth_mode, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "WPA") || !strcmp(tmp_auth, "WPA2") || !strcmp(tmp_auth, "WPA1WPA2")){
				apd_flag = 1;
				break;
			}
		}

		if( getNthValueSafe(i, ieee8021x, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "1")){
				apd_flag = 1;
				break;
			}
		}
	}

	if(apd_flag){
		if(nvram == RT2860_NVRAM)
			doSystem("rt2860apd");	
#if defined (RTDEV_SUPPORT)
		else if(nvram == RTDEV_NVRAM)
			doSystem("rtinicapd");
#elif defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		else if(nvram == RTDEV_NVRAM)
			doSystem("rt61apd");
#endif
	}
}
#ifdef DARE_CUSTOMER_WEB
static void GetAccessControlList(int nvram, webs_t wp, char_t *path, char_t *query)
{
	char_t result[4096];
	
	memset(result,0,sizeof(result));
	result[0] = '&';
	gstrncat(result, nvram_bufget(nvram, "AccessControlList0"), 4096);
	gstrncat(result, "&", 4096);
	gstrncat(result, nvram_bufget(nvram, "AccessPolicy0"), 4096);
	gstrncat(result, "&", 4096);
	
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"),result);
	websDone(wp, 200);
//printf("return 200 ok \nresult is %s\n",result);
}
#endif
void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int num_ssid, i;
	const char *num_s = nvram_bufget(nvram, "BssidNum");
	char_t result[4096];
	
	char_t *PreAuth, *AuthMode, *EncrypType, *DefaultKeyID, *Key1Type, *Key2Type,
		   *Key3Type, *Key4Type, *RekeyMethod, *RekeyInterval, *PMKCachePeriod, *IEEE8021X;
	char_t *RADIUS_Server, *RADIUS_Port, *session_timeout_interval;
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	// char_t *Wapiifname;
	char_t *WapiPskType, *WapiAsIpAddr, *WapiAsPort, *WapiAsCertPath, *WapiUserCertPath;
#endif

//	printf("***** nvram = %d\n", nvram);

	if(num_s)
		num_ssid = atoi(num_s);
	else
		num_ssid = 1;

	result[0] = '\0';

	// deal with shown MBSSID
	if(default_shown_mbssid[nvram] > atoi(num_s))
		default_shown_mbssid[nvram] = 0;
	sprintf(result, "%d",  default_shown_mbssid[nvram]);

	if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
			|| (RTDEV_NVRAM == nvram)
#endif
			) {
		for(i=0; i<num_ssid; i++) {
			gstrncat(result, "\n", 4096);
			//LFF(result, nvram, SSID, i);
			gstrncat(result, nvram_bufget(nvram, racat("SSID", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, PreAuth, i);
			LFF(result, nvram, AuthMode, i);
			LFF(result, nvram, EncrypType, i);
			LFF(result, nvram, DefaultKeyID, i);
			LFF(result, nvram, Key1Type, i);
			//LFF(result, nvram, Key1Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key1Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key2Type, i);
			//LFF(result, nvram, Key2Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key2Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key3Type, i);
			//LFF(result, nvram, Key3Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key3Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key4Type, i);
			//LFF(result, nvram, Key4Str, i);
			gstrncat(result, nvram_bufget(nvram, racat("Key4Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			//LFF(result, nvram, WPAPSK, i);
			gstrncat(result, nvram_bufget(nvram, racat("WPAPSK", i+1)), 4096);
			gstrncat(result, "\r", 4096);

			LFF(result, nvram, RekeyMethod, i);
			LFF(result, nvram, RekeyInterval, i);
			LFF(result, nvram, PMKCachePeriod, i);
			LFF(result, nvram, IEEE8021X, i);
			LFF(result, nvram, RADIUS_Server, i);
			LFF(result, nvram, RADIUS_Port, i);
			gstrncat(result, nvram_bufget(nvram, racat("RADIUS_Key", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, session_timeout_interval, i);

			// access control related.
			gstrncat(result, nvram_bufget(nvram, racat("AccessPolicy", i)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, nvram_bufget(nvram, racat("AccessControlList", i)), 4096);
			gstrncat(result, "\r", 4096);
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
			LFF(result, nvram, WapiPskType, i);
			gstrncat(result, nvram_bufget(nvram, racat("WapiPsk", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, WapiAsIpAddr, i);
			LFF(result, nvram, WapiAsPort, i);
			LFF(result, nvram, WapiAsCertPath, i);
			LFF(result, nvram, WapiUserCertPath, i);
#endif
		}
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	} else if (RTDEV_NVRAM == nvram) {
		char_t *SSID, *Key1Str, *Key2Str, *Key3Str, *Key4Str, *WPAPSK;
		for(i=0; i<num_ssid; i++) {
			gstrncat(result, "\n", 4096);
			LFF(result, nvram, SSID, i);
			LFF(result, nvram, PreAuth, i);
			LFF(result, nvram, AuthMode, i);
			LFF(result, nvram, EncrypType, i);
			LFF(result, nvram, DefaultKeyID, i);
			LFF(result, nvram, Key1Type, i);
			LFF(result, nvram, Key1Str, i);
			LFF(result, nvram, Key2Type, i);
			LFF(result, nvram, Key2Str, i);
			LFF(result, nvram, Key3Type, i);
			LFF(result, nvram, Key3Str, i);
			LFF(result, nvram, Key4Type, i);
			LFF(result, nvram, Key4Str, i);
			LFF(result, nvram, WPAPSK, i);

			LFF(result, nvram, RekeyMethod, i);
			LFF(result, nvram, RekeyInterval, i);
			LFF(result, nvram, PMKCachePeriod, i);
			LFF(result, nvram, IEEE8021X, i);
			LFF(result, nvram, RADIUS_Server, i);
			LFF(result, nvram, RADIUS_Port, i);
			LFF(result, nvram, RADIUS_Key, i);
			LFF(result, nvram, session_timeout_interval, i);

		/* access control related.
		   gstrncat(result, nvram_bufget(nvram, racat("AccessPolicy", i)), 4096);
		   gstrncat(result, "\r", 4096);
		   gstrncat(result, nvram_bufget(nvram, racat("AccessControlList", i)), 4096);
		   gstrncat(result, "\r", 4096);
		*/
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
			char_t *WapiPsk;
			LFF(result, nvram, WapiPskType, i);
			LFF(result, nvram, WapiPsk, i);
			LFF(result, nvram, WapiAsIpAddr, i);
			LFF(result, nvram, WapiAsPort, i);
			LFF(result, nvram, WapiAsCertPath, i);
			LFF(result, nvram, WapiUserCertPath, i);
#endif
		}
#endif
	}

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), result);
	websDone(wp, 200);

}
#ifdef DARE_CUSTOMER_WEB
static int GetAccessPolicy(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"),nvram_bufget(RT2860_NVRAM, "AccessPolicy0"));
}

static int getdevmac(int eid, webs_t wp, int argc, char_t **argv)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
  return websWrite(wp, T("%s"), myMAC);
}

static void wirelessGetMAClist(webs_t wp, char_t *path, char_t *query)
{
	return GetAccessControlList(RT2860_NVRAM, wp, path, query);
}
#endif
static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query)
{
	return getSecurity(RT2860_NVRAM, wp, path, query);
}

void updateFlash8021x(int nvram)
{
	char lan_if_addr[16];
	const char *RADIUS_Server;
	const char *operation_mode;

	// if(! (operation_mode = nvram_bufget(nvram, "OperationMode")))
	if(! (operation_mode = nvram_get(RT2860_NVRAM, "OperationMode")))
		return;

	if(! (RADIUS_Server = nvram_bufget(nvram, "RADIUS_Server")))
		return;

	if(!strlen(RADIUS_Server))
		return;

	if(*operation_mode == '0'){ // Bridge
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			nvram_bufset(nvram, "Ethifname", getLanIfName());
#endif
		}
	}else if(*operation_mode == '1'){	// Gateway
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			nvram_bufset(nvram, "Ethifname", getLanIfName());
#endif
		}
	}else if(*operation_mode == '2'){	// Wireless gateway
		if (getIfIp(getLanIfName(), lan_if_addr) == -1)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) {
			nvram_bufset(nvram, "EAPifname", getLanIfName());
			nvram_bufset(nvram, "PreAuthifname", getLanIfName());
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			nvram_bufset(nvram, "Ethifname", getLanIfName());
#endif
		}
	}else{
		printf("not op mode\n");
		return;
	}
	nvram_commit(nvram);
}


int AccessPolicyHandle(int nvram, webs_t wp, int mbssid)
{
	char_t *apselect, *newap_list;
	char str[32];
	char ap_list[2048];

	if(mbssid > 8 || mbssid < 0)
		return -1;

	sprintf(str, "apselect_%d", mbssid);	// UI on web page
	apselect = websGetVar(wp, str, T(""));
	if(!apselect){
		printf("cant find %s\n", apselect);
		return -1;
	}

	sprintf(str, "newap_text_%d", mbssid);
	newap_list = websGetVar(wp, str, T(""));
	if(!newap_list)
		return -1;
	if(!gstrlen(newap_list))
		return 0;
	sprintf(str, "AccessControlList%d", mbssid);
	sprintf(ap_list, "%s", nvram_bufget(nvram, str));
	if(strlen(ap_list))
		sprintf(ap_list, "%s;%s", ap_list, newap_list);
	else
		sprintf(ap_list, "%s", newap_list);
	nvram_bufset(nvram, str, ap_list);
	
	sprintf(str, "AccessPolicy%d", mbssid);
	nvram_bufset(nvram, str, apselect);
	
	return 0;
}



void conf8021x(int nvram, webs_t wp, int mbssid)
{
	char_t *RadiusServerIP, *RadiusServerPort, *RadiusServerSecret, *RadiusServerSessionTimeout;//, *RadiusServerIdleTimeout;

	LFW(RadiusServerIP, RadiusServerIP);
	LFW(RadiusServerPort, RadiusServerPort);
	LFW(RadiusServerSecret, RadiusServerSecret);
	LFW(RadiusServerSessionTimeout, RadiusServerSessionTimeout);
	// LFW(RadiusServerIdleTimeout, RadiusServerIdleTimeout);
	if(!gstrlen(RadiusServerSessionTimeout))
		RadiusServerSessionTimeout = "0";

	STFs(nvram, mbssid, "RADIUS_Server", RadiusServerIP);
	STFs(nvram, mbssid, "RADIUS_Port", RadiusServerPort);
	nvram_bufset(nvram, racat("RADIUS_Key", mbssid+1), RadiusServerSecret);
	STFs(nvram, mbssid, "session_timeout_interval", RadiusServerSessionTimeout);

	updateFlash8021x(nvram);
	restart8021XDaemon(nvram);
}

void confWEP(int nvram, webs_t wp, int mbssid)
{
	char_t *DefaultKeyID, *Key1Type, *Key1Str, *Key2Type, *Key2Str, *Key3Type, *Key3Str, *Key4Type, *Key4Str;

	LFW(DefaultKeyID, wep_default_key);
	LFW(Key1Str, wep_key_1);
	LFW(Key2Str, wep_key_2);
	LFW(Key3Str, wep_key_3);
	LFW(Key4Str, wep_key_4);
	LFW(Key1Type, WEP1Select);
	LFW(Key2Type, WEP2Select);
	LFW(Key3Type, WEP3Select);
	LFW(Key4Type, WEP4Select);

	STF(nvram, mbssid, DefaultKeyID);
	STF(nvram, mbssid, Key1Type);
	STF(nvram, mbssid, Key2Type);
	STF(nvram, mbssid, Key3Type);
	STF(nvram, mbssid, Key4Type);
	if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
			|| (RTDEV_NVRAM == nvram)
#endif
			){
		nvram_bufset(nvram, racat("Key1Str", mbssid+1), Key1Str);
		nvram_bufset(nvram, racat("Key2Str", mbssid+1), Key2Str);
		nvram_bufset(nvram, racat("Key3Str", mbssid+1), Key3Str);
		nvram_bufset(nvram, racat("Key4Str", mbssid+1), Key4Str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	} else if (RTDEV_NVRAM == nvram) {
		// nvram_bufset(nvram, "Key1Str", Key1Str);
		// nvram_bufset(nvram, "Key2Str", Key2Str);
		// nvram_bufset(nvram, "Key3Str", Key3Str);
		// nvram_bufset(nvram, "Key4Str", Key4Str);
		STF(nvram, mbssid, Key1Str);
		STF(nvram, mbssid, Key2Str);
		STF(nvram, mbssid, Key3Str);
		STF(nvram, mbssid, Key4Str);
#endif
	}
}

void confWPAGeneral(int nvram, webs_t wp, int mbssid)
{
	char *cipher_str;
	char *key_renewal_interval;

	LFW(cipher_str, cipher);
	LFW(key_renewal_interval, keyRenewalInterval);

	switch(cipher_str[0]){
	case '0':
		STFs(nvram, mbssid, "EncrypType", "TKIP");
		break;
	case '1':
		STFs(nvram, mbssid, "EncrypType", "AES");
		break;
	case '2':
		STFs(nvram, mbssid, "EncrypType", "TKIPAES");
	}
	STFs(nvram, mbssid, "DefaultKeyID", "");	// DefaultKeyID is 1
	STFs(nvram, mbssid, "RekeyInterval", key_renewal_interval);
	STFs(nvram, mbssid, "RekeyMethod", "TIME");		
	STFs(nvram, mbssid, "IEEE8021X", "0");
}

inline void clearRadiusSetting(int nvram, int mbssid)
{
	char *RADIUS_Server, *RADIUS_Port;
	//char *session_timeout_interval = NULL;

	RADIUS_Server = (char *)nvram_bufget(nvram, "RADIUS_Server");
	RADIUS_Port = (char *)nvram_bufget(nvram, "RADIUS_Port");
	nvram_bufget(nvram, racat("RADIUS_Key", mbssid+1));
	//session_timeout_interval = nvram_bufget(nvram, "session_timeout_interval");

	STFs(nvram, mbssid, "IEEE8021X", "0");
	nvram_bufset(nvram, "RADIUS_Server", setNthValue(mbssid, RADIUS_Server, "0"));
	nvram_bufset(nvram, "RADIUS_Port", setNthValue(mbssid, RADIUS_Port, "1812"));
	nvram_bufset(nvram, racat("RADIUS_Key", mbssid+1), "ralink");
	//nvram_bufset(nvram, "session_timeout_interval", setNthValue(mbssid, session_timeout_interval, ""));
            
	return;
}


void Security(int nvram, webs_t wp, char_t *path, char_t *query)
{
	char_t *SSID;
	int mbssid;
	char_t *security_mode;

	LFW(SSID, ssidIndex);
	if(!gstrlen(SSID))
		return;

	mbssid = atoi(SSID);
	default_shown_mbssid[nvram] = mbssid;

	LFW(security_mode, security_mode);

#ifndef CONFIG_RALINK_RT2880
	// RT2880: GPIO13 is belong to WPS PBC indicator.

	if (!strcmp(security_mode, "Disable") || !strcmp(security_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)
#endif

	//clear Radius settings
	clearRadiusSetting(nvram, mbssid);

	if (!strcmp(security_mode, "OPEN")) {				// Open-WEP Mode
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else if (!strcmp(security_mode, "SHARED")) {		// Shared-WEP Mode
		char *security_shared_mode;
		confWEP(nvram, wp, mbssid);

		LFW(security_shared_mode, security_shared_mode);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else if (!strcmp(security_mode, "WEPAUTO")) {		// WEP Auto Mode
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
#ifdef MINIUPNPD_SUPPORT
		doSystem("miniupnpd.sh init");
#else
		doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else if (!strcmp(security_mode, "WPA")) {			// WPA Enterprise Mode
		conf8021x(nvram, wp, mbssid);
		confWPAGeneral(nvram, wp, mbssid);

		STFs(nvram, mbssid, "AuthMode", security_mode);
	} else if (!strcmp(security_mode, "WPAPSK")) {		// WPA Personal Mode
		char *pass_phrase_str;

		confWPAGeneral(nvram, wp, mbssid);

		LFW(pass_phrase_str, passphrase);

		STFs(nvram, mbssid, "AuthMode", security_mode);

		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				){
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			// nvram_bufset(nvram, "WPAPSK", pass_phrase_str);
			STFs(nvram, mbssid, "WPAPSK", pass_phrase_str);
#endif
		}
	} else if (!strcmp(security_mode, "WPA2") ||			// WPA2 Enterprise Mode
			   !strcmp(security_mode, "WPA1WPA2")) {		// WPA Enterprise Mode
		char *PMKCachePeriod;
		char *PreAuth;

		conf8021x(nvram, wp, mbssid);
		confWPAGeneral(nvram, wp, mbssid);

		LFW(PMKCachePeriod, PMKCachePeriod);
		LFW(PreAuth, PreAuthentication);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STF(nvram, mbssid, PMKCachePeriod);
		STF(nvram, mbssid, PreAuth);
	} else if (!strcmp(security_mode, "WPA2PSK") ||			// WPA2 Personal Mode
			   !strcmp(security_mode, "WPAPSKWPA2PSK")) { 	// WPA PSK WPA2 PSK mixed
		char *pass_phrase_str;

		confWPAGeneral(nvram, wp, mbssid);
		LFW(pass_phrase_str, passphrase);

		STFs(nvram, mbssid, "AuthMode", security_mode);

		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) {
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			// nvram_bufset(nvram, "WPAPSK", pass_phrase_str);
			STFs(nvram, mbssid, "WPAPSK", pass_phrase_str);
#endif
		}
	} else if (!strcmp(security_mode, "IEEE8021X")) {		// 802.1X WEP Mode
		char *ieee8021x_wep;

		conf8021x(nvram, wp, mbssid);
		STFs(nvram, mbssid, "IEEE8021X", "1");
		STFs(nvram, mbssid, "AuthMode", "OPEN");

		LFW(ieee8021x_wep, ieee8021x_wep);
		if(ieee8021x_wep[0] == '0')
			STFs(nvram, mbssid, "EncrypType", "NONE");
		else
			STFs(nvram, mbssid, "EncrypType", "WEP");
	}
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	else if (!strcmp(security_mode, "WAICERT")) {			// WAPI-CERT Mode
		// char_t *Wapiifname, *WapiAsPort;
		char_t *WapiAsIpAddr, *WapiAsCertPath, *WapiUserCertPath;

		LFW(WapiAsIpAddr, wapicert_asipaddr);
		// LFW(WapiAsPort, wapicert_asport);
		LFW(WapiAsCertPath, wapicert_ascert);
		LFW(WapiUserCertPath, wapicert_usercert);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "SMS4");
		nvram_bufset(nvram, "Wapiifname", "br0");
		nvram_bufset(nvram, "WapiAsIpAddr", WapiAsIpAddr);
		nvram_bufset(nvram, "WapiAsPort", "3810");
		nvram_bufset(nvram, "WapiAsCertPath", WapiAsCertPath);
		nvram_bufset(nvram, "WapiUserCertPath", WapiUserCertPath);
	} else if (!strcmp(security_mode, "WAIPSK")) {			// WAPI-PSK Mode
		char *preshared_key, *key_type;

		LFW(preshared_key, wapipsk_prekey);
		LFW(key_type, wapipsk_keytype);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "SMS4");
		STFs(nvram, mbssid, "WapiPskType", key_type);

		if ((RT2860_NVRAM == nvram) 
#if defined (RTDEV_SUPPORT)
				|| (RTDEV_NVRAM == nvram)
#endif
				) 
		{
			nvram_bufset(nvram, racat("WapiPsk", mbssid+1), preshared_key);
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
		} else if (RTDEV_NVRAM == nvram) {
			STFs(nvram, mbssid, "WapiPsk", preshared_key);
#endif
		}
	}
#endif
	else {													// Open-None Mode
		STFs(nvram, mbssid, "AuthMode", "OPEN");
		STFs(nvram, mbssid, "EncrypType", "NONE");
	}

//# Access Policy
	if(AccessPolicyHandle(nvram, wp, mbssid) == -1)
		trace(0, "** error in AccessPolicyHandle()\n");

//# WPS
	{
		const char *wordlist= nvram_bufget(nvram, "WscModeOption");

		if (wordlist && mbssid == 0) {		// only ra0 supports WPS now.
			if (nvram == RT2860_NVRAM) {
				if (strcmp(wordlist, "0"))
					doSystem("iwpriv ra0 set WscConfStatus=1");
				g_wsc_configured = 1;
			}
#if defined (RTDEV_SUPPORT)
			else if (nvram == RTDEV_NVRAM) {
				if (strcmp(wordlist, "0"))
					doSystem("iwpriv rai0 set WscConfStatus=1");
				g_Raix_wsc_configured = 1;
			}
#endif
			nvram_bufset(nvram, "WscConfigured", "1");
		}
	}

	nvram_commit(nvram);

	initInternet();
#ifdef DARE_CUSTOMER_WEB
	char *reloadweb = NULL;
	LFW(reloadweb, reloadweb);
	if(strcmp(reloadweb,"reloadwebture") == 0 )
	{
		websRedirect(wp, "herouter/wirelessMAC.asp");
	}
	else
#endif
	{	
		//debug print
		websHeader(wp);
		websWrite(wp, T("<h2>MBSSID index: %d, Security Mode: %s Done</h2><br>\n"),
				mbssid, security_mode);
		websFooter(wp);
		websDone(wp, 200);	
	}
}

static void APSecurity(webs_t wp, char_t *path, char_t *query)
{
	//Security(RT2860_NVRAM, wp, path, query);
	char_t *SSID;
	int mbssid;
	char_t *security_mode;

	LFW(SSID, ssidIndex);
	if(!gstrlen(SSID))
		return;

	mbssid = atoi(SSID);
	LFW(security_mode, security_mode);
	
	sleep(3); // wait for APBatchDeleteAccessPolicyList
	
	//# Access Policy	
	if(AccessPolicyHandle(RT2860_NVRAM, wp, mbssid) == -1)		
		trace(0, "** error in AccessPolicyHandle()\n");

	nvram_commit(RT2860_NVRAM);
	//initInternet();
	
	#ifdef DARE_CUSTOMER_WEB
	char *reloadweb = NULL;
	LFW(reloadweb, reloadweb);
	if(strcmp(reloadweb,"reloadwebture") == 0 )
	{
		//websRedirect(wp, "herouter/wirelessMAC.asp");
		websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
		websDone(wp, 200);			
	}
	else
#endif
	{	
		//debug print
		websHeader(wp);
		websWrite(wp, T("<h2>MBSSID index: %d, Security Mode: %s Done</h2><br>\n"),
				mbssid, security_mode);
		websFooter(wp);
		websDone(wp, 200);	
	}
}


void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int mbssid, aplist_num;
	char str[32], apl[64*20];
	const char *tmp;
	sscanf(query, "%d,%d", &mbssid, &aplist_num);

	sprintf(str, "AccessControlList%d", mbssid);
	if(!(tmp = nvram_bufget(nvram, str)))
		return;
	strcpy(apl, tmp);

	deleteNthValueMulti(&aplist_num, 1, apl, ';');

	nvram_set(nvram, str, apl);

	default_shown_mbssid[nvram] = mbssid;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("ok done"));
	websDone(wp, 200);
	
}
#ifdef DARE_CUSTOMER_WEB
static void QuickSetUpSave(webs_t wp, char_t *path, char_t *query)
{
	char *AccessControlList,*AccessPolicy;

	AccessPolicy = nvram_bufget(RT2860_NVRAM, "AccessPolicy_temp");
	AccessControlList = nvram_bufget(RT2860_NVRAM, "AccessControlList_temp");
	if(!gstrcmp(AccessPolicy, "default") || !gstrcmp(AccessControlList, "default"))
	{	
		websRedirect(wp, "/herouter/devicemag.asp");
		return;
	}	
	printf("AccessPolicy0 = %s \nAccessControlList_temp is %s\nAccessControlList0 is %s\n",AccessPolicy,AccessControlList,nvram_bufget(RT2860_NVRAM, "AccessControlList0"));
	nvram_bufset(RT2860_NVRAM, "AccessPolicy0", AccessPolicy);
	nvram_bufset(RT2860_NVRAM, "AccessControlList0", AccessControlList);
	initInternet();
	nvram_commit(RT2860_NVRAM);
	websRedirect(wp, "/herouter/devicemag.asp");
	
}
	
static void disconnectbymacfilter(webs_t wp, char_t *path, char_t *query)
{
	char_t str[4096];
	char *AccessPolicy ,*AccessControlList, *AccessControTemp,*flag,*p,*q,*s;
	
	if(!strlen(query))
	{printf("query is NULL .return \n");
		return;	
	}	 
	
	AccessPolicy =  nvram_bufget(RT2860_NVRAM, "AccessPolicy0");
	q = AccessControlList = nvram_bufget(RT2860_NVRAM, "AccessControlList0");
	AccessControTemp = nvram_bufget(RT2860_NVRAM, "AccessControlList_temp");
	
	if(!gstrcmp(AccessControTemp, "default"))
	{
		printf("this is first coming...%s\n",AccessControTemp);	
	}
	else
	{
		AccessControlList = AccessControTemp;
		printf("no the first time coming...AccessControlList = %s\n",AccessControlList);		
	}		
			
	sprintf(str, "%s", AccessControlList);
	printf("AccessPolicy = 2 str=%s\n",str);
	if(strlen(str))
		sprintf(str, "%s;%s", str, query);
	else
		sprintf(str,"%s",query);
	nvram_bufset(RT2860_NVRAM, "AccessPolicy0", "2");
	nvram_bufset(RT2860_NVRAM, "AccessControlList0", str);
	
	//nvram_bufset(RT2860_NVRAM, "AllowDisconnect", "yes");	
	nvram_commit(RT2860_NVRAM);
	initInternet();
	
	printf("===========%s============\n",AccessPolicy);
	printf("query is %s\n Str is %s\n",query,str);
	printf("=======================\n");
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"),query);
	websDone(wp, 200);
}
#endif
static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query)
{
	DeleteAccessPolicyList(RT2860_NVRAM, wp, path, query);
}

static void APBatchDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query)
{
	char *delim = ";";
	char *p, *q;

	int	mbssid,	aplist_num;
	char str[32],	apl[64*20];
	const char *tmp;

	mbssid = 0;
	p = strtok(query,	delim);	
	do
	{
		q = p;
		//sscanf(q, "%d,%d", &mbssid, &aplist_num);
		sscanf(q, "%d", &aplist_num);
		printf("====>mbssid: %d, aplist_num: %d<====\n", mbssid, aplist_num);
		
		sprintf(str, "AccessControlList%d", mbssid);
		if(!(tmp = nvram_bufget(RT2860_NVRAM, str)))
			return;
		
		strcpy(apl, tmp);
		deleteNthValueMulti(&aplist_num, 1, apl, ';');
		nvram_set(RT2860_NVRAM, str, apl);
		default_shown_mbssid[RT2860_NVRAM] = mbssid;
	}while((p = strtok(NULL, delim)));

	sprintf(str, "AccessControlList%d", mbssid);
	tmp = nvram_bufget(RT2860_NVRAM, str);
	printf("====>AccessControlList%d: %s<====\n", mbssid, tmp);
	if(strcmp(tmp, "") == 0)
	{
		printf("====>disable AccessPolicy<====\n");
		nvram_bufset(RT2860_NVRAM, "AccessPolicy0", "0");
		nvram_commit(RT2860_NVRAM);
	}
	
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("ok	done"));
	websDone(wp, 200);	
}

#ifdef CONFIG_RT2860V2_AP_MESH
/* goform/wirelessMesh */
static void wirelessMesh(webs_t wp, char_t *path, char_t *query)
{
	char_t	*meshenable, *mid, *hostname;
	char_t	*autolink;
	char_t	*mode, *encrypt_type, *wepkey, *wep_select, *wpakey;
	char defaultkey[2];

	//fetch from web input
	meshenable = websGetVar(wp, T("MeshEnable"), T("")); 
	mid = websGetVar(wp, T("MeshID"), T("")); 
	hostname = websGetVar(wp, T("HostName"), T("")); 
	autolink = websGetVar(wp, T("AutoLinkEnable"), T(""));
	mode = websGetVar(wp, T("security_mode"), T(""));
	strcpy(defaultkey, "");
	if (0 == strcmp(mode, "OPEN"))
	{
		encrypt_type = websGetVar(wp, T("open_encrypt_type"), T(""));
		if (0 == strcmp(encrypt_type, "WEP"))
			strcpy(defaultkey, "1");
	}
	else if (0 == strcmp(mode, "WPANONE"))
	{
		encrypt_type = websGetVar(wp, T("wpa_cipher"), T(""));
	}
	wepkey = websGetVar(wp, T("wep_key"), T(""));
	wep_select = websGetVar(wp, T("wep_select"), T(""));
	wpakey = websGetVar(wp, T("passphrase"), T(""));

	// store to flash
	nvram_bufset(RT2860_NVRAM, "MeshEnabled", meshenable);
	nvram_bufset(RT2860_NVRAM, "MeshId", mid);
	nvram_bufset(RT2860_NVRAM, "MeshHostName", hostname);
	nvram_bufset(RT2860_NVRAM, "MeshAutoLink", autolink);
	nvram_bufset(RT2860_NVRAM, "MeshAuthMode", mode);
	nvram_bufset(RT2860_NVRAM, "MeshEncrypType", encrypt_type);
	nvram_bufset(RT2860_NVRAM, "MeshDefaultkey", defaultkey);
	nvram_bufset(RT2860_NVRAM, "MeshWEPKEY", wepkey);
	nvram_bufset(RT2860_NVRAM, "MeshWEPKEYType", wep_select);
	nvram_bufset(RT2860_NVRAM, "MeshWPAKEY", wpakey);

	nvram_commit(RT2860_NVRAM);
	
	// restart network
	initInternet();

	// debug print
	websHeader(wp);
	websWrite(wp, T("MeshEnable: %s<br>\n"), meshenable);
	websWrite(wp, T("MeshID: %s<br>\n"), mid);
	websWrite(wp, T("HostName: %s<br>\n"), hostname);
	websWrite(wp, T("AutoLinkEnable: %s<br>\n"), autolink);
	websWrite(wp, T("security_mode: %s<br>\n"), mode);
	websWrite(wp, T("encrypt_type: %s<br>\n"), encrypt_type);
	websWrite(wp, T("defaultkey: %s<br>\n"), defaultkey);
	websWrite(wp, T("wep_key: %s<br>\n"), wepkey);
	websWrite(wp, T("wep_select: %s<br>\n"), wep_select);
	websWrite(wp, T("passphrase: %s<br>\n"), wpakey);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/meshManualLink */
static void meshManualLink(webs_t wp, char_t *path, char_t *query)
{
	char_t *action, *mpmac;

	// fetch from web input
	action = websGetVar(wp, T("link_action"), T(""));
	mpmac = websGetVar(wp, T("mpmac"), T(""));
	
	// link action
	if (0 == strcmp(action, "add"))
		doSystem("iwpriv mesh0 set MeshAddLink=%s", mpmac);
	if (0 == strcmp(action, "del"))
		doSystem("iwpriv mesh0 set MeshDelLink=%s", mpmac);
	sleep(1);

	websRedirect(wp, "wireless/mesh.asp");
}

typedef struct _MESH_NEIGHBOR_ENTRY_INFO {
	char			Rssi;
	unsigned char	HostName[64 + 1];
	unsigned char	MacAddr[6];
	unsigned char	MeshId[32 + 1];
	unsigned char	Channel;
	unsigned char	Status;
	unsigned char	MeshEncrypType;
} MESH_NEIGHBOR_ENTRY_INFO;

typedef struct _MESH_NEIGHBOR_INFO {
	MESH_NEIGHBOR_ENTRY_INFO	Entry[64];
	unsigned char				num;
} MESH_NEIGHBOR_INFO;

/* goform/ShowMeshState */
static int ShowMeshState(int eid, webs_t wp, int argc, char_t **argv)
{
	int socket_id, ret, i;
	struct iwreq wrq;
	MESH_NEIGHBOR_INFO *neighbor;

	if (NULL == (neighbor = (MESH_NEIGHBOR_INFO *) malloc(sizeof(MESH_NEIGHBOR_INFO))))
	{
		fprintf(stderr, "can't allocat memory in MESH_NEIGHBOR_INFO !\n");
		return -1;
	}
	memset(neighbor, 0, sizeof(MESH_NEIGHBOR_INFO));

	if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		free(neighbor);
		fprintf(stderr, "ShowMeshState: open socket error !\n");
		websError(wp, 500, "ioctl sockey failed !");
		return -1;
	}

	strcpy(wrq.ifr_name, "mesh0");
	wrq.u.data.length = sizeof(MESH_NEIGHBOR_INFO);
	wrq.u.data.pointer = (caddr_t) neighbor;
	wrq.u.data.flags = OID_802_11_MESH_LIST;
	if ((ret = ioctl(socket_id, RT_PRIV_IOCTL, &wrq)) < 0)
	{
		fprintf(stderr, "ShowMeshState: ioctl -> OID_802_11_MESH_LIST error !\n");
		websError(wp, 500, "ioctl -> OID_802_11_MESH_LIST failed!");
		free(neighbor);
		close(socket_id);
		return -1;
	}
	
	for(i=0;i<neighbor->num;i++)
	{
		websWrite(wp, T("<tr align=\"center\">"));
		if (1 == neighbor->Entry[i].Status)
			websWrite(wp, T("<td>%s</td>"), "<img src=\"/graphics/handshake.gif\">");
		else
			websWrite(wp, T("<td>%s</td>"), "<br />");
		websWrite(wp, T("<td>%02X:%02X:%02X:%02X:%02X:%02X</td>"), 
					  neighbor->Entry[i].MacAddr[0],
					  neighbor->Entry[i].MacAddr[1],
					  neighbor->Entry[i].MacAddr[2],
					  neighbor->Entry[i].MacAddr[3],
					  neighbor->Entry[i].MacAddr[4],
					  neighbor->Entry[i].MacAddr[5]);
		websWrite(wp, T("<td>%d</td>"), neighbor->Entry[i].Rssi);
		websWrite(wp, T("<td>%s</td>"), neighbor->Entry[i].MeshId);
		websWrite(wp, T("<td>%s</td>"), neighbor->Entry[i].HostName);
		websWrite(wp, T("<td>%d</td>"), neighbor->Entry[i].Channel);
		if (neighbor->Entry[i].MeshEncrypType == 1)
			websWrite(wp, T("<td>%s</td>"), "OPEN-WEP");
		else if (neighbor->Entry[i].MeshEncrypType == 2)
			websWrite(wp, T("<td>%s</td>"), "WPANONE-TKIP");
		else if (neighbor->Entry[i].MeshEncrypType == 3)
			websWrite(wp, T("<td>%s</td>"), "WPANONE-AES");
		else
			websWrite(wp, T("<td>%s</td>"), "OPEN-NONE");
		websWrite(wp, T("</tr>"));
	}
	free(neighbor);
	close(socket_id);

	return 0;
}
#endif

static int getRaxHTStream(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_3T3R_SUPPORT)
		websWrite(wp, T("3"));
#elif defined (RT2860_1T1R_SUPPORT)
		websWrite(wp, T("1"));	
#else
		websWrite(wp, T("2"));	
#endif
	return 0;
}

static int isWPSConfiguredASP(int eid, webs_t wp, int argc, char_t **argv)
{
	if(g_wsc_configured){
		websWrite(wp, T("1"));
	}else
		websWrite(wp, T("0"));	
	return 0;
}

#ifdef CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY
void AntennaDiversityInit(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "AntennaDiversity");

	if(!gstrcmp(mode, "Disable")){						// Disable
		doSystem("echo 0 > /proc/AntDiv/AD_RUN");
	}else if(!gstrcmp(mode, "Enable_Algorithm1")){
		doSystem("echo 1 > /proc/AntDiv/AD_ALGORITHM"); // Algorithm1
		doSystem("echo 1 > /proc/AntDiv/AD_RUN");
	}else if(!gstrcmp(mode, "Enable_Algorithm2")){
		doSystem("echo 2 > /proc/AntDiv/AD_ALGORITHM"); // Algorithm2
		doSystem("echo 1 > /proc/AntDiv/AD_RUN");
	}else if(!gstrcmp(mode, "Antenna0")){				// fix Ant0
		doSystem("echo 0 > /proc/AntDiv/AD_RUN");
		doSystem("echo 0 > /proc/AntDiv/AD_FORCE_ANTENNA");
	}else if(!gstrcmp(mode, "Antenna2")){				// fix Ant2
		doSystem("echo 0 > /proc/AntDiv/AD_RUN");
		doSystem("echo 2 > /proc/AntDiv/AD_FORCE_ANTENNA");
	}else{
		doSystem("echo 0 > /proc/AntDiv/AD_RUN");
	return;
	}

	return;
}

static void AntennaDiversity(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mode;

	mode = websGetVar(wp, T("ADSelect"), T(""));
	if(!mode || !strlen(mode))
		return;
	
	nvram_bufset(RT2860_NVRAM, "AntennaDiversity", mode);
	nvram_commit(RT2860_NVRAM);

	// re-init
	AntennaDiversityInit();

	//debug print
	websHeader(wp);
	websWrite(wp, T("mode:%s"), mode);
	websFooter(wp);
	websDone(wp, 200);	
}

static void getAntenna(webs_t wp, char_t *path, char_t *query)
{
	char buf[32];
	FILE *fp = fopen("/proc/AntDiv/AD_CHOSEN_ANTENNA", "r");
	if(!fp){
		strcpy(buf, "not support\n");
	}else{
		fgets(buf, 32, fp);
		fclose(fp);
	}
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, "%s", buf);
	websDone(wp, 200);
}

static int isAntennaDiversityBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("1"));
	return 0;
}
#else
static int isAntennaDiversityBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("0"));
	return 0;
}
#endif

static int getWAPIASCertList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *p = popen("nvram_get wapi ASCertFile", "r");
	char path[128];

	memset(path, 0, 128);
	fscanf(p, "%s", path);
	pclose(p);

	if (strlen(path) > 0)
		websWrite(wp, T("<option value=\"%s\">%s</option>"), path, path);
	else
		websWrite(wp, T("<option value=\"\"></option>"));

	return 0;
}

static int getWAPIUserCertList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *p = popen("nvram_get wapi UserCertFile", "r");
	char path[128];

	memset(path, 0, 128);
	fscanf(p, "%s", path);
	pclose(p);

	if (strlen(path) > 0)
		websWrite(wp, T("<option value=\"%s\">%s</option>"), path, path);
	else
		websWrite(wp, T("<option value=\"\"></option>"));
	
	return 0;
}

int ApOidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

/*
 * description: write AP driver version
 */
static int getAPDriverVer(int eid, webs_t wp, int argc, char_t **argv)
{
	//RT_VERSION_INFO DriverVersionInfo;
	unsigned char DriverVersionInfo[16];
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	//Driver
	if (ApOidQueryInformation(RT_OID_VERSION_INFO, s, *argv, &DriverVersionInfo, sizeof(DriverVersionInfo)) >= 0) {
		websWrite(wp, "%s", DriverVersionInfo);
	}
	else
		websWrite(wp, "&nbsp;");

	close(s);
	return 0;
}

/*
 * description: display AP TX/RX statistics
 */
static int getApStats(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *t;
	NDIS_802_11_STATISTICS stat;
	int s, ret;
	float txCount;
#ifdef ENHANCED_AP_STATUS_INFO
	char  tmpStatistics[256];
#endif

	if (ejArgs(argc, argv, T("%s"), &t) < 1) {
		return websWrite(wp, T("Insufficient args"));
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ApOidQueryInformation(OID_802_11_STATISTICS, s, "ra0", &stat, sizeof(stat));
	close(s);
	if (ret >= 0) {
		txCount = stat.TransmittedFragmentCount.QuadPart;

		if (!strncmp(t, "TxSucc", 7))
			return websWrite(wp, T("%ld"), stat.TransmittedFragmentCount.QuadPart);
#ifdef ENHANCED_AP_STATUS_INFO
		else if (!strncmp(t, "TxRetry", 8))
		{
			sprintf(tmpStatistics, "%lld,  PER=%0.1f%%", stat.RetryCount.QuadPart,
				txCount==0? 0.0: 100.0f*(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart)/(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart+txCount));
			return websWrite(wp, T("%s"), tmpStatistics);
		}
		else if (!strncmp(t, "TxFail", 7))
		{
			sprintf(tmpStatistics, "%lld,  PLR=%0.1e", stat.FailedCount.QuadPart,
			txCount==0? 0.0: (float)stat.FailedCount.QuadPart/(stat.FailedCount.QuadPart+txCount));
			return websWrite(wp, T("%s"), tmpStatistics);
		}
#else
		else if (!strncmp(t, "TxRetry", 8))
			return websWrite(wp, T("%ld"), stat.RetryCount.QuadPart);
		else if (!strncmp(t, "TxFail", 7))
			return websWrite(wp, T("%ld"), stat.FailedCount.QuadPart);
#endif
		else if (!strncmp(t, "RTSSucc", 8))
			return websWrite(wp, T("%ld"), stat.RTSSuccessCount.QuadPart);
		else if (!strncmp(t, "RTSFail", 8))
			return websWrite(wp, T("%ld"), stat.RTSFailureCount.QuadPart);
		else if (!strncmp(t, "RxSucc", 7))
			return websWrite(wp, T("%ld"), stat.ReceivedFragmentCount.QuadPart);
		else if (!strncmp(t, "FCSError", 9))
		{
#ifdef ENHANCED_AP_STATUS_INFO
			sprintf(tmpStatistics, "%lld, PER=%0.1f%%", stat.FCSErrorCount.QuadPart,
					stat.ReceivedFragmentCount.QuadPart==0?
						0.0: 100.0f*stat.FCSErrorCount.QuadPart/(stat.FCSErrorCount.QuadPart+stat.ReceivedFragmentCount.QuadPart));
			return websWrite(wp, T("%s"), tmpStatistics);
#else
			return websWrite(wp, T("%ld"), stat.FCSErrorCount.QuadPart);
#endif
		}
		else
			return websWrite(wp, T("type not supported"));
	}
	else
		return websWrite(wp, T("ioctl %d"), ret);
}

/* goform/resetApCounters */
static void resetApCounters(webs_t wp, char_t *path, char_t *query)
{
	system("iwpriv ra0 set ResetCounter=0");
	websRedirect(wp, "wireless/apstatistics.asp");
	return;
}

/*
 * description: write AP SNR
 */
static int getApSNR(int eid, webs_t wp, int argc, char_t **argv)
{
	int s, n, ret;
	unsigned long SNR;

	if (ejArgs(argc, argv, T("%d"), &n) < 1) {
		return websWrite(wp, T("Insufficient args\n"));
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (n == 0)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_0, s, "ra0", &SNR, sizeof(SNR));
	else if (n == 1)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_1, s, "ra0", &SNR, sizeof(SNR));
	else if (n == 2)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_2, s, "ra0", &SNR, sizeof(SNR));
	else
		ret = -1;
	close(s);

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0)
		return websWrite(wp, "n/a");
	else
		return websWrite(wp, "%ld", SNR);
}

static int getApTxBFStats(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_TXBF_SUPPORT)
	int s, ret, i;
	RT_802_11_TXBF_TABLE table = {0};
	RT_COUNTER_TXBF *pCnt;
	int hdr = 0;
	char  tmpStatistics[256];
	unsigned long totalNBF, totalEBF, totalIBF, totalTx, totalRetry, totalSuccess;
	char *ifname;

	if (ejArgs(argc, argv, T("%s"), &ifname) < 1) {
		return 0;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ApOidQueryInformation(RT_OID_802_11_QUERY_TXBF_TABLE, s, ifname, &table, sizeof(table));
	close(s);
	if (ret < 0)
		return websWrite(wp, "ioctl error %d", ret);

	for (i = 0; i < table.Num; i++) {
		pCnt = &(table.Entry[i]);
		totalNBF = pCnt->TxSuccessCount + pCnt->TxFailCount;
		totalEBF = pCnt->ETxSuccessCount + pCnt->ETxFailCount;
		totalIBF = pCnt->ITxSuccessCount + pCnt->ITxFailCount;
		totalTx = totalNBF + totalEBF + totalIBF;
		totalRetry = pCnt->TxRetryCount + pCnt->ETxRetryCount + pCnt->ITxRetryCount;
		totalSuccess = pCnt->TxSuccessCount + pCnt->ETxSuccessCount + pCnt->ITxSuccessCount;

		if (totalTx == 0)
			continue;
		if (hdr == 0) {
			websWrite(wp, T("<tr><td class=\"title\" colspan=\"2\">Detailed TX Statistics (Retry count is approximate)</td></tr>"));
			hdr = 1;
		}

		websWrite(wp, T("<tr><td class=\"head2\">%d</td><td><kbd>"), i);
		websWrite(wp, T("%s"), replaceSpaces(T("              Success    Retry/PER    Fail/PLR<br>")) );
		if (totalNBF!=0) {
			sprintf(tmpStatistics, T("NonBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalNBF/totalTx, pCnt->TxSuccessCount,
				pCnt->TxRetryCount, 100*pCnt->TxRetryCount/(pCnt->TxSuccessCount+pCnt->TxRetryCount),
				pCnt->TxFailCount, 100*pCnt->TxFailCount/totalNBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatistics));
		}
		if (totalEBF!=0) {
			sprintf(tmpStatistics, T("ETxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalEBF/totalTx, pCnt->ETxSuccessCount,
				pCnt->ETxRetryCount, 100*pCnt->ETxRetryCount/(pCnt->ETxSuccessCount+pCnt->ETxRetryCount),
				pCnt->ETxFailCount, 100*pCnt->ETxFailCount/totalEBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatistics));
		}

		if (totalIBF!=0) {
			sprintf(tmpStatistics, T("ITxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)<br>"),
				100*totalIBF/totalTx, pCnt->ITxSuccessCount,
				pCnt->ITxRetryCount, 100*pCnt->ITxRetryCount/(pCnt->ITxSuccessCount+pCnt->ITxRetryCount),
				pCnt->ITxFailCount, 100*pCnt->ITxFailCount/totalIBF);
			websWrite(wp, T("%s"), replaceSpaces(tmpStatistics));
		}

		sprintf(tmpStatistics, T("Total         %7lu  %7lu (%2lu%%) %5lu (%1lu%%)"),
			totalSuccess, totalRetry, 100*totalRetry/(totalSuccess + totalRetry),
			pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount,
			100*(pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount)/totalTx);
		websWrite(wp, T("%s"), replaceSpaces(tmpStatistics));
		websWrite(wp, T("</kbd></td></tr>"));
	}
#endif
	return 0;
}

/* goform/wirelessRvt */
static void wirelessRvt(webs_t wp, char_t *path, char_t *query)
{
	char_t	*etxbf, *itxbf;
	//char_t	*itxbfto, *etxbfto;
	char_t	*macenhance, *classifier;

	itxbf = websGetVar(wp, T("itxbf"), T("0"));
	//itxbfto = websGetVar(wp, T("itxbfto"), T("0"));
	etxbf = websGetVar(wp, T("etxbf"), T("0"));
	//etxbfto = websGetVar(wp, T("etxbfto"), T("0"));
	macenhance = websGetVar(wp, T("macenhance"), T("0"));
	classifier = websGetVar(wp, T("classifier"), T("0"));

	nvram_bufset(RT2860_NVRAM, "ITxBfEn", itxbf);
	/*
	if (strncmp(itxbfto, "0", 2))
		nvram_bufset(RT2860_NVRAM, "ITxBfTimeout", itxbfto);
	*/
	nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", etxbf);
	nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", strncmp(etxbf, "1", 2)==0? "0": "1");
	/*
	if (strncmp(etxbfto, "0", 2))
		nvram_bufset(RT2860_NVRAM, "ETxBfTimeout", etxbfto);
	*/
	nvram_bufset(RT2860_NVRAM, "VideoTurbine", macenhance);
	nvram_commit(RT2860_NVRAM);

	if (!strncmp(classifier, "0", 2))
		doSystem("rmmod cls");
	else
		doSystem("insmod -q cls");
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("itxbf: %s<br>\n"), itxbf);
	//websWrite(wp, T("itxbfto: %s<br>\n"), itxbfto);
	websWrite(wp, T("etxbf: %s<br>\n"), etxbf);
	//websWrite(wp, T("etxbfto: %s<br>\n"), etxbfto);
	websWrite(wp, T("macenhance: %s<br>\n"), macenhance);
	websWrite(wp, T("classifier: %s<br>\n"), classifier);
	websFooter(wp);
	websDone(wp, 200);
	return;
}

#if defined (RT2860_HS_SUPPORT)
/* goform/setHotspot */
static void setHotspot(webs_t wp, char_t *path, char_t *query)
{
	int	index, wmode;
	char_t	*hs_enabled, *internet, *hessid, *roaming_consortium_oi;
	char_t	*proxy_arp, *dgaf_disabled, *l2_filter, *icmpv4_deny, *nai_enabled, *plmn_enabled;
	char_t	*realm1, *eap1, *realm2, *eap2, *realm3, *eap3, *realm4, *eap4;
	char_t	*mcc1, *mnc1, *mcc2, *mnc2, *mcc3, *mnc3, *mcc4, *mnc4, *mcc5, *mnc5, *mcc6, *mnc6;

	index = strtol(websGetVar(wp, T("wifiIndex"), T("0")), NULL, 10);
	wmode = strtol(nvram_bufget(RT2860_NVRAM,"WirelessMode"), NULL, 10);
	hs_enabled = websGetVar(wp, T("hs_enabled"), T("0"));
	internet = websGetVar(wp, T("internet"), T("0"));
	hessid = websGetVar(wp, T("hessid"), T(""));
	roaming_consortium_oi = websGetVar(wp, T("roaming_consortium_oi"), T(""));
	proxy_arp = websGetVar(wp, T("proxy_arp"), T("0"));
	dgaf_disabled = websGetVar(wp, T("dgaf_disabled"), T("0"));
	l2_filter = websGetVar(wp, T("l2_filter"), T("0"));
	icmpv4_deny = websGetVar(wp, T("icmpv4_deny"), T("0"));
	nai_enabled = websGetVar(wp, T("nai_enabled"), T("0"));
	realm1 = websGetVar(wp, T("realm1"), T(""));
	eap1 = websGetVar(wp, T("eap_method1"), T(""));
	realm2 = websGetVar(wp, T("realm2"), T(""));
	eap2 = websGetVar(wp, T("eap_method2"), T(""));
	realm3 = websGetVar(wp, T("realm3"), T(""));
	eap3 = websGetVar(wp, T("eap_method3"), T(""));
	realm4 = websGetVar(wp, T("realm4"), T(""));
	eap4 = websGetVar(wp, T("eap_method4"), T(""));
	plmn_enabled = websGetVar(wp, T("plmn_enabled"), T(""));
	mcc1 = websGetVar(wp, T("mcc1"), T(""));
	mnc1 = websGetVar(wp, T("mnc1"), T(""));
	mcc2 = websGetVar(wp, T("mcc2"), T(""));
	mnc2 = websGetVar(wp, T("mnc2"), T(""));
	mcc3 = websGetVar(wp, T("mcc3"), T(""));
	mnc3 = websGetVar(wp, T("mnc3"), T(""));
	mcc4 = websGetVar(wp, T("mcc4"), T(""));
	mnc4 = websGetVar(wp, T("mnc4"), T(""));
	mcc5 = websGetVar(wp, T("mcc5"), T(""));
	mnc5 = websGetVar(wp, T("mnc5"), T(""));
	mcc6 = websGetVar(wp, T("mcc6"), T(""));
	mnc6 = websGetVar(wp, T("mnc6"), T(""));

	STFs(RT2860_NVRAM, index, "HS_enabled", hs_enabled);
	STFs(RT2860_NVRAM, index, "HS_internet", internet);
	STFs(RT2860_NVRAM, index, "HS_hessid", hessid);
	STFs(RT2860_NVRAM, index, "HS_roaming_consortium_oi", roaming_consortium_oi);
	// STFs(RT2860_NVRAM, index, "HS_roaming_consortium_oi", "50-6F-9A,00-1B-C5-04-BD");
	if (wmode == 10) {
		STFs(RT2860_NVRAM, index, "HS_operating_class", "81,115");
	} else if (wmode == 2 || wmode == 8 || wmode == 11) {
		STFs(RT2860_NVRAM, index, "HS_operating_class", "115");
	} else {
		STFs(RT2860_NVRAM, index, "HS_operating_class", "81");
	}

	STFs(RT2860_NVRAM, index, "HS_proxy_arp", proxy_arp);
	STFs(RT2860_NVRAM, index, "HS_dgaf_disabled", dgaf_disabled);
	STFs(RT2860_NVRAM, index, "HS_l2_filter", l2_filter);
	STFs(RT2860_NVRAM, index, "HS_icmpv4_deny", icmpv4_deny);
	STFs(RT2860_NVRAM, index, "HS_nai", nai_enabled);
	STFs(RT2860_NVRAM, index, "HS_nai1_realm", realm1);
	STFs(RT2860_NVRAM, index, "HS_nai1_eap_method", eap1);
	STFs(RT2860_NVRAM, index, "HS_nai2_realm", realm2);
	STFs(RT2860_NVRAM, index, "HS_nai2_eap_method", eap2);
	STFs(RT2860_NVRAM, index, "HS_nai3_realm", realm3);
	STFs(RT2860_NVRAM, index, "HS_nai3_eap_method", eap3);
	STFs(RT2860_NVRAM, index, "HS_nai4_realm", realm4);
	STFs(RT2860_NVRAM, index, "HS_nai4_eap_method", eap4);
	STFs(RT2860_NVRAM, index, "HS_capacity", "0");
#ifdef CONFIG_MAC_TO_MAC_MODE
	STFs(RT2860_NVRAM, index, "HS_link_status", (getIfIsUp(getWanIfName()) == 1)? "1" : "0");
	STFs(RT2860_NVRAM, index, "HS_dl_speed", "1000000");
	STFs(RT2860_NVRAM, index, "HS_ul_speed", "1000000");
#else
#if ! defined (CONFIG_RALINK_RT6855A)
	doSystem("reg s b0110000");
#else
	doSystem("reg s bfb50000");
#endif
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350)
	FILE *pp = popen("reg p 80", "r");
	char reg[11];
#ifdef CONFIG_WAN_AT_P0
	int wan_link_status = 25;
	int wan_speed = 0;
#else
	int wan_link_status = 29;
	int wan_speed = 4;
#endif
	fscanf(pp, "%s\n", reg);
	pclose(pp);
	STFs(RT2860_NVRAM, index, "HS_link_status", 
		 (((strtoul(reg, NULL, 16)>>wan_link_status)&(0x1)) == 0x1)? "1" : "0");
	if (((strtoul(reg, NULL, 16)>>wan_speed)&(0x1)) == 0x1) {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "100000");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "100000");
	} else if (((strtoul(reg, NULL, 16)>>wan_link_status)&(0x1)) == 0x1) {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "10000");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "10000");
	} else {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "0");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "0");
	}
#else
#ifdef CONFIG_WAN_AT_P0
	FILE *pp = popen("reg p 3008", "r");
	char reg[11];
#else
	FILE *pp = popen("reg p 3408", "r");
	char reg[11];
#endif
	fscanf(pp, "%s\n", reg);
	pclose(pp);
	STFs(RT2860_NVRAM, index, "HS_link_status", 
		 ((strtoul(reg, NULL, 16)&(0x1)) == 0x1)? "1" : "0");
	if (((strtoul(reg, NULL, 16)>>2)&(0x3)) == 0x2) {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "1000000");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "1000000");
	} else if (((strtoul(reg, NULL, 16)>>2)&(0x3)) == 0x1) {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "100000");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "100000");
	} else if ((strtoul(reg, NULL, 16)&(0x1)) == 0x1) {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "10000");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "10000");
	} else {
		STFs(RT2860_NVRAM, index, "HS_dl_speed", "0");
		STFs(RT2860_NVRAM, index, "HS_ul_speed", "0");
	}
#endif
#endif
	STFs(RT2860_NVRAM, index, "HS_dl_load", "0");
	STFs(RT2860_NVRAM, index, "HS_ul_load", "0");
	STFs(RT2860_NVRAM, index, "HS_lmd", "0");
	STFs(RT2860_NVRAM, index, "HS_capacity", "0");
	STFs(RT2860_NVRAM, index, "HS_plmn", plmn_enabled);
	STFs(RT2860_NVRAM, index, "HS_plmn1_mcc", mcc1);
	STFs(RT2860_NVRAM, index, "HS_plmn1_mnc", mnc1);
	STFs(RT2860_NVRAM, index, "HS_plmn2_mcc", mcc2);
	STFs(RT2860_NVRAM, index, "HS_plmn2_mnc", mnc2);
	STFs(RT2860_NVRAM, index, "HS_plmn3_mcc", mcc3);
	STFs(RT2860_NVRAM, index, "HS_plmn3_mnc", mnc3);
	STFs(RT2860_NVRAM, index, "HS_plmn4_mcc", mcc4);
	STFs(RT2860_NVRAM, index, "HS_plmn4_mnc", mnc4);
	STFs(RT2860_NVRAM, index, "HS_plmn5_mcc", mcc5);
	STFs(RT2860_NVRAM, index, "HS_plmn5_mnc", mnc5);
	STFs(RT2860_NVRAM, index, "HS_plmn6_mcc", mcc6);
	STFs(RT2860_NVRAM, index, "HS_plmn6_mnc", mnc6);

	nvram_commit(RT2860_NVRAM);
	initInternet();
	//debug print
	websHeader(wp);
	websWrite(wp, T("ra%d<br>\n"), index);
	websWrite(wp, T("hs_enabled: %s<br>\n"), hs_enabled);
	websWrite(wp, T("internet: %s<br>\n"), internet);
	websWrite(wp, T("hessid: %s<br>\n"), hessid);
	websWrite(wp, T("roaming_consortium_oi: %s<br>\n"), roaming_consortium_oi);
	websWrite(wp, T("proxy_arp: %s<br>\n"), proxy_arp);
	websWrite(wp, T("dgaf_disabled: %s<br>\n"), dgaf_disabled);
	websWrite(wp, T("l2_filter: %s<br>\n"), l2_filter);
	websWrite(wp, T("icmpv4_deny: %s<br>\n"), icmpv4_deny);
	websWrite(wp, T("nai_enabled: %s<br>\n"), nai_enabled);
	websWrite(wp, T("realm1: %s<br>\n"), realm1);
	websWrite(wp, T("eap1: %s<br>\n"), eap1);
	websWrite(wp, T("realm2: %s<br>\n"), realm2);
	websWrite(wp, T("eap2: %s<br>\n"), eap2);
	websWrite(wp, T("realm3: %s<br>\n"), realm3);
	websWrite(wp, T("eap3: %s<br>\n"), eap3);
	websWrite(wp, T("realm4: %s<br>\n"), realm4);
	websWrite(wp, T("eap4: %s<br>\n"), eap4);
	websWrite(wp, T("plmn_enabled: %s<br>\n"), plmn_enabled);
	websWrite(wp, T("mcc1: %s<br>\n"), mcc1);
	websWrite(wp, T("mnc1: %s<br>\n"), mnc1);
	websWrite(wp, T("mcc2: %s<br>\n"), mcc2);
	websWrite(wp, T("mnc2: %s<br>\n"), mnc2);
	websWrite(wp, T("mcc3: %s<br>\n"), mcc3);
	websWrite(wp, T("mnc3: %s<br>\n"), mnc3);
	websWrite(wp, T("mcc4: %s<br>\n"), mcc4);
	websWrite(wp, T("mnc4: %s<br>\n"), mnc4);
	websWrite(wp, T("mcc5: %s<br>\n"), mcc5);
	websWrite(wp, T("mnc5: %s<br>\n"), mnc5);
	websWrite(wp, T("mcc6: %s<br>\n"), mcc6);
	websWrite(wp, T("mnc6: %s<br>\n"), mnc6);
	websFooter(wp);
	websDone(wp, 200);
}
#endif

static int getMeshBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_AP_MESH || defined CONFIG_RT2860V2_STA_MESH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getHSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_HS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRVTBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getTxBfBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_TXBF_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if	defined (RT2860_WDS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWSCBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_WSC_SUPPORT) || defined (CONFIG_RT2860V2_STA_WSC)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUniWPSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_UNIQUE_WPS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_MBSS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int get16MBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef RT2860_NEW_MBSS_SUPPORT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getAutoProvisionBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaxGBand(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_GBAND_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

#if defined (RT2860_VOW_SUPPORT) || defined (RTDEV_VOW_SUPPORT)
#define STATUS_OK           0
#define STATUS_ERROR        1

#define OID_GET_SET_TOGGLE      0x8000

#define RT_OID_BW_ENABLE        0x0680
#define RT_OID_BW_SET_CFG       0x0681
#define RT_OID_BW_GET_STATE     0x0682
#define RT_OID_BW_SET_ROOT		0x0683

static int vow_debug = 0;

typedef struct _BW_UI_CFG
{
	unsigned char	Priority;
	unsigned int	G_Rate;
	unsigned int	M_Rate;
} BW_UI_CFG, *PBW_UI_CFG;

typedef struct _BW_UI_INFO
{
	long long   Tx_Byte_Cnt;
	long long	Time_Stamp;
} BW_UI_INFO, *PBW_UI_INFO;

int QueryRalinkOid(char *pIntfName,
		unsigned short ralink_oid,
		char *arg,
		unsigned short BufLen,
		void *pOutBuf)
{
	int skfd;
	struct iwreq lwreq;
	int rv = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Open socket failed.\n");
		return -1;
	}

	if ((arg != NULL) && (strlen(arg) < BufLen))
		strncpy(pOutBuf, arg, BufLen);

	sprintf(lwreq.ifr_ifrn.ifrn_name, pIntfName, strlen(pIntfName));
	lwreq.u.data.flags = ralink_oid;
	lwreq.u.data.pointer = (caddr_t) pOutBuf;
	lwreq.u.data.length = BufLen;

	if(ioctl(skfd, RT_PRIV_IOCTL, &lwreq) < 0)
	{
		fprintf(stderr, "QueryRalinkOid:: Interface doesn't accept private ioctl...(0x%04x)\n", ralink_oid);
		rv = -1;
	}

	close(skfd);

	return rv;
}

int SetRalinkOid(char *pIntfName,
		unsigned short ralink_oid,
		unsigned short BufLen,
		void *pInBuf)
{
	int skfd;
	struct iwreq lwreq;
	int rv = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Open socket failed.\n");
		return -1;
	}

	sprintf(lwreq.ifr_ifrn.ifrn_name, pIntfName, strlen(pIntfName));
	lwreq.u.data.flags = ralink_oid | OID_GET_SET_TOGGLE;
	lwreq.u.data.pointer = (caddr_t) pInBuf;
	lwreq.u.data.length = BufLen;

	if(ioctl(skfd, RT_PRIV_IOCTL, &lwreq) < 0)
	{
		fprintf(stderr, "SetRalinkOid:: Interface doesn't accept private ioctl...(0x%04x)\n", ralink_oid);
		rv = -1;
	}

	close(skfd);

	return rv;
}

static void getVoW(webs_t wp, char_t *path, char_t *query)
{
	int i;
	BW_UI_INFO info[4];
	unsigned int data[5];

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	if (vow_debug == 1)
		fprintf(stderr, "query: %s\n", query);
	QueryRalinkOid(query, RT_OID_BW_GET_STATE, NULL, sizeof(info), info);

	for (i = 0; i < 4; i++) {
		if (vow_debug == 1)
			fprintf(stderr, "BSS%d tx byte %lld, time %lld\n", i, info[i].Tx_Byte_Cnt, info[i].Time_Stamp);
		data[i] = info[i].Tx_Byte_Cnt*8/(1024*1024);
	}
	data[i] = info[0].Time_Stamp/(1000*1000*1000);

	if (vow_debug == 1)
		fprintf(stderr, "%d;%d;%d;%d;%d\n", data[0], data[1], data[2], data[3], data[4]);

	websWrite(wp, T("%d;%d;%d;%d;%d"), data[0], data[1], data[2], data[3], data[4]);
	websDone(wp, 200);
	return;
}

static void setVoW(webs_t wp, char_t *path, char_t *query)
{
	int i;
	BW_UI_CFG Cfg[4];
	char_t *ifname;
	char_t *g_rate1, *g_rate2, *g_rate3, *g_rate4;
	char_t *ceil_rate1, *ceil_rate2, *ceil_rate3, *ceil_rate4;
	char_t *priority1, *priority2, *priority3, *priority4;
	char setData[1024];
	int nvram;
	unsigned int root_rate;
	
	ifname = websGetVar(wp, T("interface"), T("")); 
	if (!strcmp(ifname, "ra0"))
		nvram = RT2860_NVRAM;
	else
		nvram = RTDEV_NVRAM;
	vow_debug = strtol(websGetVar(wp, T("debug_mode"), T("0")), NULL, 10);
	root_rate = strtoul(websGetVar(wp, T("root_rate"), T("")), NULL, 10) * 1024 * 1024; 
	if (root_rate == 0)
		root_rate = strtoul(nvram_bufget(nvram, "BW_Root"), NULL, 10);
	if (root_rate > 0) {
		SetRalinkOid(ifname, RT_OID_BW_SET_ROOT, sizeof(root_rate),	&root_rate);
		sprintf(setData, "%lu", root_rate);
		nvram_bufset(nvram, "BW_Root", setData);
		if (vow_debug == 1)
			fprintf(stderr, "set RootRate: %s\n", setData); 
	}
	g_rate1 = websGetVar(wp, T("g_rate1"), T("")); 
	g_rate2 = websGetVar(wp, T("g_rate2"), T("")); 
	g_rate3 = websGetVar(wp, T("g_rate3"), T("")); 
	g_rate4 = websGetVar(wp, T("g_rate4"), T("")); 
	ceil_rate1 = websGetVar(wp, T("ceil_rate1"), T("")); 
	ceil_rate2 = websGetVar(wp, T("ceil_rate2"), T("")); 
	ceil_rate3 = websGetVar(wp, T("ceil_rate3"), T("")); 
	ceil_rate4 = websGetVar(wp, T("ceil_rate4"), T("")); 
	priority1 = websGetVar(wp, T("priority1"), T("")); 
	priority2 = websGetVar(wp, T("priority2"), T("")); 
	priority3 = websGetVar(wp, T("priority3"), T("")); 
	priority4 = websGetVar(wp, T("priority4"), T("")); 

	Cfg[0].Priority = strtol(priority1, NULL, 10); /* highest */
	Cfg[0].G_Rate = strtol(g_rate1, NULL, 10)*1024*1024;	 /* guarantee rate */
	Cfg[0].M_Rate = strtol(ceil_rate1, NULL, 10)*1024*1024; /* peak rate */

	Cfg[1].Priority = strtol(priority2, NULL, 10); /* highest */
	Cfg[1].G_Rate = strtol(g_rate2, NULL, 10)*1024*1024;	 /* guarantee rate */
	Cfg[1].M_Rate = strtol(ceil_rate2, NULL, 10)*1024*1024; /* peak rate */

	Cfg[2].Priority = strtol(priority3, NULL, 10); /* highest */
	Cfg[2].G_Rate = strtol(g_rate3, NULL, 10)*1024*1024;	 /* guarantee rate */
	Cfg[2].M_Rate = strtol(ceil_rate3, NULL, 10)*1024*1024; /* peak rate */

	Cfg[3].Priority = strtol(priority4, NULL, 10); /* highest */
	Cfg[3].G_Rate = strtol(g_rate4, NULL, 10)*1024*1024;	 /* guarantee rate */
	Cfg[3].M_Rate = strtol(ceil_rate4, NULL, 10)*1024*1024; /* peak rate */
	if (vow_debug == 1)
		for (i=0;i<4;i++)
			fprintf(stderr, "set SSID%d Priority: %c, G_Rate: %d, M_Rate: %d\n", 
					i+1, Cfg[i].Priority, Cfg[i].G_Rate, Cfg[i].M_Rate);

	SetRalinkOid(ifname, RT_OID_BW_SET_CFG, sizeof(Cfg), Cfg);
	sprintf(setData, "%ld;%ld;%ld;%ld", Cfg[0].G_Rate, Cfg[1].G_Rate, Cfg[2].G_Rate, Cfg[3].G_Rate);
	nvram_bufset(nvram, "BW_Guarantee_Rate", setData);
	sprintf(setData, "%ld;%ld;%ld;%ld", Cfg[0].M_Rate, Cfg[1].M_Rate, Cfg[2].M_Rate, Cfg[3].M_Rate);
	nvram_bufset(nvram, "BW_Maximum_Rate", setData);
	sprintf(setData, "%d;%d;%d;%d", Cfg[0].Priority, Cfg[1].Priority, Cfg[2].Priority, Cfg[3].Priority);
	nvram_bufset(nvram, "BW_Priority", setData);
	nvram_commit(nvram);

	if (!strcmp(ifname, "ra0"))
		if (vow_debug == 1)
			websRedirect(wp, "wireless/vow_setting2.asp");
		else
			websRedirect(wp, "wireless/vow_setting.asp");
	else
		if (vow_debug == 1)
			websRedirect(wp, "inic/vow_setting2.asp");
		else
			websRedirect(wp, "inic/vow_setting.asp");

	return;
}

static void setVoWRadio(webs_t wp, char_t *path, char_t *query)
{
	//char_t *rf;
	char_t *vow, *ifname;
	int nvram;

	ifname = websGetVar(wp, T("interface"), T(""));
	//rf = websGetVar(wp, T("rf"), T(""));
	vow = websGetVar(wp, T("vow"), T(""));
	if (!strcmp(ifname, "ra0"))
		nvram = RT2860_NVRAM;
	else
		nvram = RTDEV_NVRAM;
#if 0
	if (!strncmp(rf, "0", 2)) {
		doSystem("iwpriv ra0 set RadioOn=0");
		nvram_set(nvram, "RadioOff", "1");
	} else if (!strncmp(rf, "1", 2)) {
		doSystem("iwpriv ra0 set RadioOn=1");
		nvram_set(nvram, "RadioOff", "0");
	}
#endif
	nvram_set(nvram, "BW_Enable", vow);
	SetRalinkOid(ifname, RT_OID_BW_ENABLE, 1, vow);
	if (!strcmp(ifname, "ra0"))
		websRedirect(wp, "wireless/vow_setting.asp");
	else
		websRedirect(wp, "inic/vow_setting.asp");
	return;
}
#endif

static int getRaxABand(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_ABAND_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaxAC(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RT2860_AC_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVoWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	if (!strcmp(*argv, "ra0"))
#if defined (RT2860_VOW_SUPPORT)
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
	else
#if defined (RTDEV_VOW_SUPPORT)
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
}

static int getExtChListBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_EXT_CHANNEL_LIST
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

