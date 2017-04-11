/* vi: set sw=4 ts=4: */
/* dhcpd.c
 *
 * udhcp Server
 * Copyright (C) 1999 Matthew Ramsay <matthewr@moreton.com.au>
 *			Chris Trew <ctrew@moreton.com.au>
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "common.h"
#include "dhcpc.h"
#include "dhcpd.h"
#include "options.h"
#include <linux/wireless.h>

/* globals */
struct dhcpOfferedAddr *leases;
/* struct server_config_t server_config is in bb_common_bufsiz1 */

#undef DEBUG
#define DEBUG printf

#define TOADDDEVFILE "/var/toaddDevList"
#define TEMP_TOADDDEVFILE "/var/toaddDevList_Dhcp"
#define ALLOWTOADDLIST "/var/allowAddlist"

//===========================================================
//
// define wireless part start(START)
//
//===========================================================
typedef union _MACHTTRANSMIT_SETTING {
	struct  {
		unsigned short  MCS:7;  // MCS
		unsigned short  BW:1;   //channel bandwidth 20MHz or 40 MHz
		unsigned short  ShortGI:1;
		unsigned short  STBC:2; //SPACE
		unsigned short	eTxBF:1;
		unsigned short	rsv:1;
		unsigned short	iTxBF:1;
		unsigned short  MODE:2; // Use definition MODE_xxx.
	} field;
	unsigned short      word;
} MACHTTRANSMIT_SETTING;

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

#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE				0x8BE0
#endif
#define SIOCIWFIRSTPRIV				SIOCDEVPRIVATE
#endif

#define RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1E)
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif


#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1F)

//===========================================================
//
// define wireless part start(END)
//
//===========================================================

#define REMOVE_CRLN(data) \
        if( (*(data + strlen(data) - 1) == 0x0a) || \
            (*(data + strlen(data) - 1) == 0x0d) ) \
        { \
            *(data + strlen(data) - 1) = 0x0; \
        } \
        if( (*(data + strlen(data) - 2) == 0x0a) || \
            (*(data + strlen(data) - 2) == 0x0d) ) \
        { \
            *(data + strlen(data) - 2) = 0x0; \
        }

int hertUtil_GetNvram_Values(char *pszTitle, char *szOutput, int nBuffSize)
{
    FILE *file;
    char cmd[128];
    const char *tempFileName = "/var/tempnvramval.txt";

    memset(szOutput, 0x0, nBuffSize);
    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"nvram_get %s > %s", pszTitle, tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        DEBUG("unable to open config file: %s\n",tempFileName);
        return 0;
    }   
    if (fgets(szOutput, 64, file) && (strlen(szOutput) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(szOutput);
    }    
    fclose(file);
    unlink(tempFileName);

    return strlen(szOutput);
}

void hertUtil_toUpper(char *pszStr)
{
    char *s1 = pszStr;
    if (!s1) return;
    while (*s1 && (*s1 = toupper(*s1)))
        s1++;
}

void hertUtil_DelNetFiltMac(char *filterMac,uint32_t addr)
{
    char cmdLine[256] = {0};
    struct in_addr  sin_addr;
    sin_addr.s_addr = addr;
	
    DEBUG("######## dhcp delete MacFilter:%s-%s########\n",filterMac,inet_ntoa(sin_addr));
    
    /*for local Network*/
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-dst ! %s -s %s -j DROP 1>/dev/null 2>/dev/null",\
		inet_ntoa(sin_addr),filterMac);
    system(cmdLine);
    system(cmdLine);
    system(cmdLine);
	  
	  
    /*for dhcp*/
    memset(cmdLine,0x0,sizeof(cmdLine));
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-proto 17 --ip-sport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
    system(cmdLine);
    system(cmdLine);
    system(cmdLine);
		
    memset(cmdLine,0x0,sizeof(cmdLine));    
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-proto 17 --ip-dport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
    system(cmdLine); 
    system(cmdLine);
    system(cmdLine);

}

void hertUtil_AddNetFiltMac(char *filterMac,uint32_t addr)
{
    char cmdLine[256] = {0};
	
	  struct in_addr  sin_addr;
	  sin_addr.s_addr = addr;

    DEBUG("######## dhcp add MacFilter:%s-%s########\n",filterMac,inet_ntoa(sin_addr));
    
    /*for local Network*/
	  snprintf(cmdLine,sizeof(cmdLine),"ebtables -A INPUT -p 0x0800 --ip-dst ! %s -s %s -j DROP 1>/dev/null 2>/dev/null",\
		inet_ntoa(sin_addr),filterMac);
		system(cmdLine);
		
    
    /*for dhcp*/
    memset(cmdLine,0x0,sizeof(cmdLine));
	  snprintf(cmdLine,sizeof(cmdLine),"ebtables -I INPUT -p 0x0800 --ip-proto 17 --ip-sport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
		system(cmdLine);
		
		
		memset(cmdLine,0x0,sizeof(cmdLine));    
	  snprintf(cmdLine,sizeof(cmdLine),"ebtables -I INPUT -p 0x0800 --ip-proto 17 --ip-dport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
		system(cmdLine);    
	
}

void QoSRestart(void)
{
	FILE *fp = fopen("/bin/qos_run", "r");
	if(!fp)
        return;
	fclose(fp);
	system("/bin/qos_run");
}

#define MAX_QOS_RULES_SIZE 512*5  /*one rule about less 70 bytes,max 32 rules*/

void hertUtil_AddQosViaMac(char *pszHssMacList)
{
    char szCmd[MAX_QOS_RULES_SIZE];
    char szQoSULRules_Old[MAX_QOS_RULES_SIZE];
    char szQoSDLRules_Old[MAX_QOS_RULES_SIZE];
    char szQosULItem[128];
    char szQosDLItem[128];
    char *pchr = NULL;
    char *pMac = NULL;
    char szHssMac[256]; /* Hide special ssid mac */

    memset(szCmd, 0x0, sizeof(szCmd));
    memset(szHssMac, 0x0, sizeof(szHssMac));
    strncpy(szHssMac, pszHssMacList, sizeof(szHssMac));
    hertUtil_toUpper(szHssMac);

    DEBUG("********** do Qos for Hid ssid connect wifi client(%s) start ********** \n", szHssMac);
    hertUtil_GetNvram_Values("QoSULRules", szQoSULRules_Old, sizeof(szQoSULRules_Old));
    hertUtil_GetNvram_Values("QoSDLRules", szQoSDLRules_Old, sizeof(szQoSDLRules_Old));

    pMac = szHssMac;
    while( pMac && (*pMac != 0x0))
    {
        memset(szQosULItem, 0x0, sizeof(szQosULItem));
        memset(szQosDLItem, 0x0, sizeof(szQosDLItem));
        pchr = strstr(pMac, ",");
        if (pchr)
        {
            *pchr = 0x0;
            pchr++;
        }
        /* update Upload list */
        sprintf(szQosULItem,"IPTV_UL_%c%c%c%c%c%c,5,1,%s,,,,,,,,,,,,,N/A", 
			*(pMac+9), *(pMac+10), *(pMac+12), *(pMac+13), *(pMac+15), *(pMac+16), pMac);

        if (0x0 == szQoSULRules_Old[0])
        {
            sprintf(szQoSULRules_Old, "%s", szQosULItem);
        }
        else if ( NULL == strstr(szQoSULRules_Old, szQosULItem))
        {
            strcat(szQoSULRules_Old, ";");
            strcat(szQoSULRules_Old, szQosULItem);
        }

        /* update Download list */
        sprintf(szQosDLItem,"IPTV_DL_%c%c%c%c%c%c,5,1,%s,,,,,,,,,,,,,N/A", 
			*(pMac+9), *(pMac+10), *(pMac+12), *(pMac+13), *(pMac+15), *(pMac+16), pMac);

        if (0x0 == szQoSDLRules_Old[0])
        {
            sprintf(szQoSDLRules_Old, "%s", szQosDLItem);
        }
        else if ( NULL == strstr(szQoSDLRules_Old, szQosDLItem))
        {
            strcat(szQoSDLRules_Old, ";");
            strcat(szQoSDLRules_Old, szQosDLItem);
        }

        pMac = pchr;
    };
    if (szQoSULRules_Old[0] != 0x0)
    {
        memset(szCmd, 0x0, sizeof(szCmd));
        sprintf(szCmd,"nvram_set QoSULRules \"%s\"", szQoSULRules_Old);
        DEBUG("Do command(%s)\n", szCmd);
        system(szCmd);
    }
    if (szQoSDLRules_Old[0] != 0x0)
    {
        memset(szCmd, 0x0, sizeof(szCmd));
        sprintf(szCmd,"nvram_set QoSDLRules \"%s\"", szQoSDLRules_Old);
        DEBUG("Do command(%s)\n", szCmd);
        system(szCmd);
    }
    DEBUG("__________ do Qos for Hid ssid connect wifi client(%s) end   __________ \n", szHssMac);

    if(szCmd[0] != 0x0)
    {
        /* restart Qos */
        QoSRestart();

        /* killall goahead */
        system("echo restartgoahead > /var/rstgoahead");
    }
}

int hertUtil_GetNeedFilterMac(char *pszFilterMac, int nBufSize)
{
    int s;
    struct iwreq iwr;
    char *ifname = "ra0";
    char szWpsMac[256] = { 0x0 }; /* wps mac */
    char szHssMac[256] = { 0x0 }; /* Hide special ssid mac */
	
    memset(szWpsMac, 0x0, sizeof(szWpsMac));
    memset(szHssMac, 0x0, sizeof(szHssMac));
    memset(pszFilterMac, 0x0, nBufSize);

    /* get if name */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) 
    {
        DEBUG("hertUtil_GetWPSMac: ioctl sock failed!\n");
        return 1;
    }

    /* get wps mac for filter */
    *szWpsMac = 0x01;
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = szWpsMac;
    if (ioctl(s, RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT, &iwr) < 0) 
    {
        DEBUG("hertUtil_GetWPSMac: ioctl -> RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT failed!\n");
        close(s);
        return 1;
    }
    DEBUG("________ szWpsMac(%s) ________\n", szWpsMac);

    /* get Hide special ssid mac for filter */
    *szHssMac = 0x02;
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = szHssMac;
    if (ioctl(s, RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT, &iwr) < 0) 
    {
        DEBUG("hertUtil_GetHssMac: ioctl -> RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT failed!\n");
        close(s);
        return 1;
    }
    DEBUG("________ szHssMac(%s) ________\n", szHssMac);
    close(s);

    strcpy(pszFilterMac, szWpsMac);
#if 0
    if (*pszFilterMac != 0x0)
    {
        sprintf(pszFilterMac + strlen(pszFilterMac), ",%s", szHssMac);
    }
    else
    {
        sprintf(pszFilterMac + strlen(pszFilterMac), "%s", szHssMac);
    }
#else
    if (*szHssMac != 0x0)
    {
        hertUtil_AddQosViaMac(szHssMac);
    }
#endif
    DEBUG("________ pszFilterMac(%s) ________\n", pszFilterMac);

    return 0;
}

int hertUtil_IsInFile(const char *szFile, const char *szfindstr)
{
   FILE *fs = NULL;
   char line[256];
   int nFind = 0;
   char *ptr= NULL;

   if ( !szfindstr || (*szfindstr == 0x0) )
   {
      return nFind;
   }
   
   fs =  fopen(szFile, "r");

   if(fs != NULL)
   {
      while ( fgets(line, 256, fs) != NULL ) 
      {
         ptr = strstr(line, szfindstr);
         if(ptr)
         {
            nFind = 1; 
            break;
         }
      }
      fclose(fs);
   }
   return nFind;
}
void  hertUtil_debugData(const unsigned char *pData, unsigned char length)
{
    int i = 0;
    char szBuf[512];

    DEBUG("[%s,%d]: length = %x\n", __FUNCTION__, __LINE__, 0xff & length);
    memset(szBuf, 0x0, sizeof(szBuf));
    for(i = 0; i < length; i++)
    {
        if (0 == i)
        {
            sprintf(szBuf + strlen(szBuf), "%s", 
                "======= 00 01 02 03 04 05 06 07    08 09 0A 0B 0C 0D 0E 0F =======\n");
        }
        if (0 == (i%16))
        {
            sprintf(szBuf + strlen(szBuf), "%s", "-------");
        }
        if (8 == (i%9) && 0 == (i%2))
        {
            sprintf(szBuf + strlen(szBuf), "%s", "   ");
        }
        sprintf(szBuf + strlen(szBuf), " %02x", 0xff & (*(pData+i)));
        if (15 == (i%16))
        {
            sprintf(szBuf + strlen(szBuf), "%s", "\n");
        }
    }
    DEBUG("[%s,%d]: \n%s\n", __FUNCTION__, __LINE__, szBuf);
}

typedef struct tagToAddDev
{
    unsigned long curtime;
    char szProduct[64];
    char szProdType[64];
    char szProdManu[64];
    char szProdSerial[64];
    char szMac[64];
}TOADDDEV;

typedef TOADDDEV DEVINFO;

unsigned long hertUtil_getSeconds()
{
    struct timeval  tv;
    struct timezone tz;

    memset(&tv, 0x0, sizeof(tv));
    memset(&tz, 0x0, sizeof(tz));

    if (gettimeofday(&tv, &tz))
    {
        DEBUG("No memory, failed to print msg log!");
        return -1;
    }
    return tv.tv_sec;
}

int  hertUtil_ProcessSaveToAddInfo(const unsigned char *pData, unsigned char length, char *szMac)
{
    int i = 0;
    char *szParseData[4];
    int iSession = 0;
    unsigned char *pStart;
    TOADDDEV toAdd;
    int nItemLength = 0;
	int leftBufLength = 0;
    TOADDDEV tempData;
    FILE* fr = NULL;
    FILE* fw = NULL;
    unsigned long curTime = 0;
    char szCmd[256];
    int  bAdd = 0;

    memset(&toAdd, 0x0, sizeof(toAdd));

    szParseData[0] = toAdd.szProduct;
    szParseData[1] = toAdd.szProdType;
    szParseData[2] = toAdd.szProdManu;
    szParseData[3] = toAdd.szProdSerial;

    pStart = pData;
    /* parse option 60 data product name|product type|manual|serial| */
    for( i = 0; (i < length) && (iSession < 4); i++)
    {
        if ((*(pData+i)) == 0x0 )
        {
            nItemLength = ((pData + i - pStart) > 64) ? 64 : (pData + i - pStart);
            DEBUG("iSession:%d,nItemLength:%d\n",iSession,nItemLength);
            memcpy(szParseData[iSession], pStart, nItemLength);
            DEBUG("after copy-");
            DEBUG("%s\n",szParseData[iSession]);
            pStart = pData + i + 1;
			leftBufLength = length - i;
            iSession++;
        }

		if(iSession == 3){
            memcpy(szParseData[iSession], pStart, leftBufLength);
            DEBUG("after copy-");
            DEBUG("%s\n",szParseData[iSession]);
            iSession++;
			break;
	    }
    }
	
	  DEBUG("totle iSession:%d\n",iSession);
    if (!hertUtil_IsInFile("/var/testdhcpd", "DEBUG"))
    {
        // no enough data
        if ( iSession < 4)
        {
            DEBUG("[%s,%d]: iSession(%d)\n", __FUNCTION__, __LINE__, iSession);
            return -1;
        }
    }
    else
    {
        strcpy(toAdd.szProduct, "he-route");
        strcpy(toAdd.szProdType, "route");
        strcpy(toAdd.szProdManu, "dare");
        strcpy(toAdd.szProdSerial, "0123456");

    }

    strcpy(toAdd.szMac, szMac);
    curTime = hertUtil_getSeconds();
    toAdd.curtime = curTime;
    fr  = fopen(TOADDDEVFILE, "rb");
    if ( !fr)
    {
        DEBUG("[%s,%d]: Failed to open TOADDDEVFILE(%s)\n", __FUNCTION__, __LINE__, TOADDDEVFILE);
        fw = fopen(TOADDDEVFILE, "wb");
        if ( fw)
        {
            if ( 1 != fwrite(&toAdd, sizeof(toAdd), 1, fw))
            {
                DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
                perror("fwrite error");
            }
            fclose(fw);
        }
        else
        {
            DEBUG("[%s,%d]: TOADDDEVFILE(%s)\n", __FUNCTION__, __LINE__, TOADDDEVFILE);
        }
        return -2;
    }

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm %s", TEMP_TOADDDEVFILE);
    system(szCmd);

    fw = fopen(TEMP_TOADDDEVFILE, "wb");
    if ( !fw)
    {
        DEBUG("[%s,%d]: TEMP_TOADDDEVFILE(%s)\n", __FUNCTION__, __LINE__, TEMP_TOADDDEVFILE);
        return -2;
    }

    /* File was opened successfully. */
    memset(&tempData, 0x0, sizeof(tempData));

    /* Attempt to read => write */
    while (fread(&tempData, 1, sizeof(tempData), fr) == sizeof(tempData))
    {
        if (strstr(tempData.szMac, szMac))
        {
            if ( 1 != fwrite(&toAdd, sizeof(toAdd), 1, fw))
            {
                DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
            }
            bAdd = 1;
        }
        else
        {
            if (curTime < (tempData.curtime + 120))/* only back for 2 min */
            {
                if ( 1 != fwrite(&tempData, sizeof(tempData), 1, fw))
                {
                    DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
                }
            }
            else
            {
                DEBUG("[%s,%d]: we drop the data, for it is too old\n", __FUNCTION__, __LINE__);
            }
        }
        memset(&tempData, 0x0, sizeof(tempData));
    }

    if (!bAdd)
    {
        if ( 1 != fwrite(&toAdd, sizeof(toAdd), 1, fw))
        {
            DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
        }
        bAdd = 1;
    }

    fclose(fr);
    fclose(fw);

    /* copy file */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm %s", TOADDDEVFILE);
    system(szCmd);

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "cp %s %s", TEMP_TOADDDEVFILE, TOADDDEVFILE);
    system(szCmd);
	
    return 0;
}

int  hertUtil_ProcessOption60Data(const unsigned char *pData, unsigned char length, char *szMac)
{
    int ret = -1;	
    // print debug info
    hertUtil_debugData(pData, length);
    ret = hertUtil_ProcessSaveToAddInfo(pData, length, szMac);
    if(ret == -1)
    {
        return ret;
    }	
    
    return hertUtil_IsInFile(ALLOWTOADDLIST, szMac);
}

int hertUtil_FilterGuestSsidMac(char *szMac, uint32_t addr)
{
    int s;
    struct iwreq iwr;
    char *ifname = "ra0";
    char szGuestMac[256] = { 0x0 }; /* Guest special ssid mac */
    char cmdLine[256] = {0};
    struct in_addr  sin_addr;
    sin_addr.s_addr = addr;
  
    memset(szGuestMac, 0x0, sizeof(szGuestMac));
    
    /* get if name */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {        
        DEBUG("hertUtil_GetWPSMac: ioctl sock failed!\n");
        return 1;
    }     
    
    /* get guest special ssid mac for filter */
    *szGuestMac = 0x03;
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = szGuestMac;
    if (ioctl(s, RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT, &iwr) < 0)
    {
        DEBUG("hertUtil_GetHssMac: ioctl -> RTPRIV_IOCTL_GET_FILTER_MAC_TABLE_STRUCT failed!\n");
        close(s);
        return 1;
    }
    DEBUG("________ szGuestMac(%s) ________\n", szGuestMac);
    close(s);
    
    if (strstr(szGuestMac, szMac))
    {
        snprintf(cmdLine, sizeof(cmdLine), "iptables -D INPUT -m mac --mac-source %s -d %s/24 -j DROP",\
          szMac, inet_ntoa(sin_addr));
        DEBUG("________ cmdLine(%s) ________\n", cmdLine);
        system(cmdLine);
        
        memset(cmdLine, 0x0, sizeof(cmdLine));
        snprintf(cmdLine, sizeof(cmdLine), "iptables -A INPUT -m mac --mac-source %s -d %s/24 -j DROP",\
          szMac, inet_ntoa(sin_addr));
        DEBUG("________ cmdLine(%s) ________\n", cmdLine);
        system(cmdLine);        
    }
    else
    {
        snprintf(cmdLine, sizeof(cmdLine), "iptables -D INPUT -m mac --mac-source %s -d %s/24 -j DROP",\
          szMac, inet_ntoa(sin_addr));
        DEBUG("________ cmdLine(%s) ________\n", cmdLine);
        system(cmdLine);    	
    }
    
    return 0;
}

/* this function is only used to save option60 relate data
DHCP OPTION60
   +-----+-----+--------+--------+--------+------+--
   |  60 |  n  |产品名称|产品类型|制造厂商|序列号|…
   +-----+-----+--------+--------+--------+------+--
*/
int  hertUtil_SaveDevInfo(const unsigned char *pData, unsigned char length, char *szMac)
{
    int i = 0;
    char *szParseData[4];
    int iSession = 0;
    unsigned char *pStart;
    DEVINFO tDevInf;
    int nItemLength = 0;
    int leftBufLength = 0;
    DEVINFO tmpDevInf;
    FILE* fr = NULL;
    FILE* fw = NULL;
    unsigned long curTime = 0;
    char szCmd[256];
    int  bAdd = 0;
#define DEVINFO_FILE      "/var/dhcpdevinf"
#define TEMPW_DEVINFO_FILE "/var/dhcpdevinftempw"
    DEBUG("[%s,%d]: >>>>>>>>>>>>>>>>>>>>>>>>>\n", __FUNCTION__, __LINE__);
    memset(&tDevInf, 0x0, sizeof(tDevInf));

    szParseData[0] = tDevInf.szProduct;
    szParseData[1] = tDevInf.szProdType;
    szParseData[2] = tDevInf.szProdManu;
    szParseData[3] = tDevInf.szProdSerial;

    pStart = pData;
    /* parse option 60 data product name|product type|manual|serial| */
    for( i = 0; (i < length) && (iSession < 4); i++)
    {
        DEBUG("[%s,%d]: parse the option60\n", __FUNCTION__, __LINE__);
        if ((*(pData+i)) == 0x0 )
        {
            nItemLength = ((pData + i - pStart) > 64) ? 64 : (pData + i - pStart);
            DEBUG("iSession:%d,nItemLength:%d\n",iSession,nItemLength);
            memcpy(szParseData[iSession], pStart, nItemLength);
            DEBUG("after copy-");
            DEBUG("%s\n",szParseData[iSession]);
            pStart = pData + i + 1;
            leftBufLength = length - i;
            iSession++;
        }

        if(iSession == 3)
        {
            memcpy(szParseData[iSession], pStart, leftBufLength);
            DEBUG("after copy-");
            DEBUG("%s\n",szParseData[iSession]);
            iSession++;
            break;
        }
    }
	
    DEBUG("totle iSession:%d\n",iSession);
    if (!hertUtil_IsInFile("/var/testDevinf", "DEBUG"))
    {
        // no enough data
        if ( iSession < 4)
        {
            DEBUG("[%s,%d]: iSession(%d)\n", __FUNCTION__, __LINE__, iSession);
            return -1;
        }
    }
    else
    {
        strcpy(tDevInf.szProduct, "he-route");
        strcpy(tDevInf.szProdType, "route");
        strcpy(tDevInf.szProdManu, "dare");
        strcpy(tDevInf.szProdSerial, "0123456");

    }

    /* ----------------------------------- */
    char szMobileBoxId[128] = { 0x0 };
    hertUtil_GetNvram_Values("HE_ROUTE_MOBILEBOXID", szMobileBoxId, sizeof(szMobileBoxId));
    DEBUG("[%s,%d]: szMobileBoxId=(%s)\n", __FUNCTION__, __LINE__, szMobileBoxId);
    if ( *szMobileBoxId == 0x0 )
    {
        strcpy(szMobileBoxId, "mobilebox");
    }
    DEBUG("[%s,%d]: szParseData[1]=(%s)\n", __FUNCTION__, __LINE__, szParseData[1]);
    if ( strstr(szParseData[1], szMobileBoxId) )
    {
        /* it is mobile box */
        hertUtil_AddQosViaMac(szMac);
    }
    /* ----------------------------------- */
	
	
    DEBUG("[%s,%d]: Start to add the item to file...\n", __FUNCTION__, __LINE__);
    strcpy(tDevInf.szMac, szMac);
    fr  = fopen(DEVINFO_FILE, "rb");
    if ( !fr)
    {
        DEBUG("[%s,%d]: File(%s) is not exist, add it directly.\n", __FUNCTION__, __LINE__, DEVINFO_FILE);
        fw = fopen(DEVINFO_FILE, "w");
        if ( fw)
        {
            if ( 1 != fwrite(&tDevInf, sizeof(tDevInf), 1, fw))
            {
                DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
                perror("fwrite error");
            }
            fclose(fw);
        }
        else
        {
            DEBUG("[%s,%d]: DEVINFO_FILE(%s)\n", __FUNCTION__, __LINE__, DEVINFO_FILE);
        }
        return -2;
    }
    fw = fopen(TEMPW_DEVINFO_FILE, "w");
    if ( !fw)
    {
        DEBUG("[%s,%d]: TEMPW_DEVINFO_FILE(%s)\n", __FUNCTION__, __LINE__, TEMPW_DEVINFO_FILE);
        return -2;
    }

    /* File was opened successfully. */
    memset(&tmpDevInf, 0x0, sizeof(tmpDevInf));

    /* Attempt to read => write */
    DEBUG("[%s,%d]: Attempt to read => write\n", __FUNCTION__, __LINE__);
    while (fread(&tmpDevInf, 1, sizeof(tmpDevInf), fr) == sizeof(tmpDevInf))
    {
        if (strstr(tmpDevInf.szMac, szMac))
        {
            if ( 1 != fwrite(&tDevInf, sizeof(tDevInf), 1, fw))
            {
                DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
            }
            bAdd = 1;
        }
        else
        {
            if ( 1 != fwrite(&tmpDevInf, sizeof(tmpDevInf), 1, fw))
            {
                DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
            }
        }
        memset(&tmpDevInf, 0x0, sizeof(tmpDevInf));
    }

    DEBUG("[%s,%d]: bAdd(%d), check whether to add it\n", __FUNCTION__, __LINE__, bAdd);
    if (!bAdd)
    {
        if ( 1 != fwrite(&tDevInf, sizeof(tDevInf), 1, fw))
        {
            DEBUG("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
        }
    }
    DEBUG("[%s,%d]: cp temp file to real file\n", __FUNCTION__, __LINE__);

    fclose(fr);
    fclose(fw);

    /* copy file */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm %s", DEVINFO_FILE);
    system(szCmd);

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "cp %s %s", TEMPW_DEVINFO_FILE, DEVINFO_FILE);
    system(szCmd);

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm %s", TEMPW_DEVINFO_FILE);
    system(szCmd);
    DEBUG("[%s,%d]: <<<<<<<<<<<<<<<<<<<\n", __FUNCTION__, __LINE__);
}


int udhcpd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int udhcpd_main(int argc UNUSED_PARAM, char **argv)
{
	fd_set rfds;
	struct timeval tv;
	int server_socket = -1, bytes, retval, max_sock;
	struct dhcpMessage packet;
	uint8_t *state, *server_id, *requested;
	uint32_t server_id_align, requested_align, static_lease_ip;
	unsigned timeout_end;
	unsigned num_ips;
	unsigned opt;
	struct option_set *option;
	struct dhcpOfferedAddr *lease, static_lease;
	USE_FEATURE_UDHCP_PORT(char *str_P;)

	unsigned char *vendor = NULL;
	char szMac[64];
  char szFilterMacList[512];
  int rst = 0;

#if ENABLE_FEATURE_UDHCP_PORT
	SERVER_PORT = 67;
	CLIENT_PORT = 68;
#endif

	opt = getopt32(argv, "fS" USE_FEATURE_UDHCP_PORT("P:", &str_P));
	argv += optind;

	if (!(opt & 1)) { /* no -f */
		bb_daemonize_or_rexec(0, argv);
		logmode &= ~LOGMODE_STDIO;
	}

	if (opt & 2) { /* -S */
		openlog(applet_name, LOG_PID, LOG_LOCAL0);
		logmode |= LOGMODE_SYSLOG;
	}
#if ENABLE_FEATURE_UDHCP_PORT
	if (opt & 4) { /* -P */
		SERVER_PORT = xatou16(str_P);
		CLIENT_PORT = SERVER_PORT + 1;
	}
#endif
	/* Would rather not do read_config before daemonization -
	 * otherwise NOMMU machines will parse config twice */
	read_config(argv[0] ? argv[0] : DHCPD_CONF_FILE);

	/* Make sure fd 0,1,2 are open */
	bb_sanitize_stdio();
	/* Equivalent of doing a fflush after every \n */
	setlinebuf(stdout);

	/* Create pidfile */
	write_pidfile(server_config.pidfile);
	/* if (!..) bb_perror_msg("cannot create pidfile %s", pidfile); */

	bb_info_msg("%s (v"BB_VER") started", applet_name);

	option = find_option(server_config.options, DHCP_LEASE_TIME);
	server_config.lease = LEASE_TIME;
	if (option) {
		memcpy(&server_config.lease, option->data + 2, 4);
		server_config.lease = ntohl(server_config.lease);
	}

	/* Sanity check */
	num_ips = server_config.end_ip - server_config.start_ip + 1;
	if (server_config.max_leases > num_ips) {
		bb_error_msg("max_leases=%u is too big, setting to %u",
			(unsigned)server_config.max_leases, num_ips);
		server_config.max_leases = num_ips;
	}

	leases = xzalloc(server_config.max_leases * sizeof(*leases));
	read_leases(server_config.lease_file);

	if (read_interface(server_config.interface, &server_config.ifindex,
			   &server_config.server, server_config.arp)) {
		retval = 1;
		goto ret;
	}

	/* Setup the signal pipe */
	udhcp_sp_setup();

	timeout_end = monotonic_sec() + server_config.auto_time;
	while (1) { /* loop until universe collapses */

		if (server_socket < 0) {
			server_socket = listen_socket(/*INADDR_ANY,*/ SERVER_PORT,
					server_config.interface);
		}

		max_sock = udhcp_sp_fd_set(&rfds, server_socket);
		if (server_config.auto_time) {
			tv.tv_sec = timeout_end - monotonic_sec();
			tv.tv_usec = 0;
		}
		retval = 0;
		if (!server_config.auto_time || tv.tv_sec > 0) {
			retval = select(max_sock + 1, &rfds, NULL, NULL,
					server_config.auto_time ? &tv : NULL);
		}
		if (retval == 0) {
			write_leases();
			timeout_end = monotonic_sec() + server_config.auto_time;
			continue;
		}
		if (retval < 0 && errno != EINTR) {
			DEBUG("error on select");
			continue;
		}

		switch (udhcp_sp_read(&rfds)) {
		case SIGUSR1:
			bb_info_msg("Received a SIGUSR1");
			write_leases();
			/* why not just reset the timeout, eh */
			timeout_end = monotonic_sec() + server_config.auto_time;
			continue;
		case SIGTERM:
			bb_info_msg("Received a SIGTERM");
			goto ret0;
		case 0: break;		/* no signal */
		default: continue;	/* signal or error (probably EINTR) */
		}

		bytes = udhcp_recv_kernel_packet(&packet, server_socket); /* this waits for a packet - idle */
		if (bytes < 0) {
			if (bytes == -1 && errno != EINTR) {
				DEBUG("error on read, %s, reopening socket", strerror(errno));
				close(server_socket);
				server_socket = -1;
			}
			continue;
		}

		state = get_option(&packet, DHCP_MESSAGE_TYPE);
		if (state == NULL) {
			bb_error_msg("cannot get option from packet, ignoring");
			continue;
		}
		memset(szMac, 0x0, sizeof(szMac));
		sprintf(szMac, "%02x:%02x:%02x:%02x:%02x:%02x", 0xff & packet.chaddr[0], 0xff & packet.chaddr[1], 0xff & packet.chaddr[2], 
				0xff & packet.chaddr[3], 0xff & packet.chaddr[4], 0xff & packet.chaddr[5]);
		
		DEBUG("[%s,%d]: ___ szMac: %s ___\n", __FUNCTION__, __LINE__, szMac);
		/* Look for a static lease */
		static_lease_ip = getIpByMac(server_config.static_leases, &packet.chaddr);

		if (static_lease_ip) {
			bb_info_msg("Found static lease: %x", static_lease_ip);

			memcpy(&static_lease.chaddr, &packet.chaddr, 16);
			static_lease.yiaddr = static_lease_ip;
			static_lease.expires = 0;

			lease = &static_lease;
		} else {
			lease = find_lease_by_chaddr(packet.chaddr);
		}

		vendor = NULL;
		switch (state[0]) {
		case DHCPDISCOVER:
			DEBUG("Received DISCOVER");
           if (!hertUtil_IsInFile("/var/wpsMacDebug", "Debug"))/* DEBUG, START, default it will check the mac of wps */
           {
		       if ( (packet.op == BOOTREQUEST) && (! hertUtil_GetNeedFilterMac(szFilterMacList, sizeof(szFilterMacList))) && strstr(szFilterMacList, szMac))
		       {
		           vendor = get_option(&packet, DHCP_VENDOR);
		           if (vendor) 
		           {
		               rst = 	hertUtil_ProcessOption60Data(vendor, *(vendor - 1), szMac);
					   DEBUG("######## rst:%d ########\n",rst);
		               if ( rst == 0) /* not allow by user */
		               {
		                   //bb_error_msg("[%s,%d]-DHCPDISCOVER: Pls provision valid option60 data if you connect with PIN", __FUNCTION__, __LINE__);
		                   //break;
		                   //hertUtil_DelNetFiltMac(szMac,server_config.server);
		                   //hertUtil_AddNetFiltMac(szMac,server_config.server);
						   
		               }
		               else if(rst == -1)
		               {
		                   DEBUG("%s is normal home Device!",szMac);		
		               }
		               else
		               {
                           //hertUtil_DelNetFiltMac(szMac,server_config.server);   
		               }
		           }
		           else /* no option60 */
		           {
		               //bb_error_msg("[%s,%d]-DHCPDISCOVER: Pls provision your option60 data if you connect with PIN", __FUNCTION__, __LINE__);
		               //break;
		           }
		       }
            }/* DEBUG, END */

			/*----------- zhuzhh@dare, 20140703, add for save dev info, start */
			if ( vendor || (!vendor) && (vendor = get_option(&packet, DHCP_VENDOR)))
			{
				DEBUG("[%s,%d]: save option60 data to file\n", __FUNCTION__, __LINE__);
				if (vendor)
				{
					hertUtil_SaveDevInfo(vendor, *(vendor - 1), szMac);
				}
			}
			/*----------- zhuzhh@dare, 20140703, add for save dev info, end */

			
			if (send_offer(&packet) < 0) {
				bb_error_msg("send OFFER failed");
			}
			break;
		case DHCPREQUEST:
			DEBUG("received REQUEST");
			if (!hertUtil_IsInFile("/var/wpsMacDebug", "Debug"))/* DEBUG, START, default it will check the mac of wps */
			{
				if ( (packet.op == BOOTREQUEST) && (! hertUtil_GetNeedFilterMac(szFilterMacList, sizeof(szFilterMacList))) && strstr(szFilterMacList, szMac))
				{
					vendor = get_option(&packet, DHCP_VENDOR);
					if (vendor) 
					{
						if ( !hertUtil_ProcessOption60Data(vendor, *(vendor - 1), szMac)) /* not allow by user */
						{
							//bb_error_msg("[%s,%d]-DHCPDISCOVER: Pls provision valid option60 data if you connect with PIN", __FUNCTION__, __LINE__);
							//break;
							//hertUtil_DelNetFiltMac(szMac,server_config.server);
							//hertUtil_AddNetFiltMac(szMac,server_config.server);
						}
						else
						{
							//hertUtil_DelNetFiltMac(szMac,server_config.server);   
						}
					}
					else /* no option60 */
					{
						//bb_error_msg("[%s,%d]-DHCPDISCOVER: Pls provision your option60 data if you connect with PIN", __FUNCTION__, __LINE__);
						//break;
					}
				}
			 }/* DEBUG, END */

			/*----------- zhuzhh@dare, 20140703, add for save dev info, start */
			if ( vendor || (!vendor) && (vendor = get_option(&packet, DHCP_VENDOR)))
			{
				DEBUG("[%s,%d]: save option60 data to file\n", __FUNCTION__, __LINE__);
				if (vendor)
				{
					hertUtil_SaveDevInfo(vendor, *(vendor - 1), szMac);
				}
			}
			/*----------- zhuzhh@dare, 20140703, add for save dev info, end */

			/*----------- zhuzhh@dare, 20141013, add for wifi client info, start */
			int nDcNum = 0;
			nDcNum = hertUtil_UpdateWirelessClientInfo();
			if (hertUtil_IsWirelessClientMac(szMac))
			{
				hertUtil_SaveWirelessClientMac(szMac);
			}
			/*----------- zhuzhh@dare, 20141013, add for wifi client info, end */
			
			requested = get_option(&packet, DHCP_REQUESTED_IP);
			server_id = get_option(&packet, DHCP_SERVER_ID);

			if (requested) memcpy(&requested_align, requested, 4);
			if (server_id) memcpy(&server_id_align, server_id, 4);

			hertUtil_FilterGuestSsidMac(szMac, server_config.server);
			if (lease) {
				if (server_id) {
					/* SELECTING State */
					DEBUG("server_id = %08x", ntohl(server_id_align));
					if (server_id_align == server_config.server && requested
					 && requested_align == lease->yiaddr
					) {
						send_ACK(&packet, lease->yiaddr);
					}
				} else if (requested) {
					/* INIT-REBOOT State */
					if (lease->yiaddr == requested_align)
						send_ACK(&packet, lease->yiaddr);
					else
						send_NAK(&packet);
				} else if (lease->yiaddr == packet.ciaddr) {
					/* RENEWING or REBINDING State */
					send_ACK(&packet, lease->yiaddr);
				} else { /* don't know what to do!!!! */
					send_NAK(&packet);
				}

			/* what to do if we have no record of the client */
			} else if (server_id) {
				/* SELECTING State */

			} else if (requested) {
				/* INIT-REBOOT State */
				lease = find_lease_by_yiaddr(requested_align);
				if (lease) {
					if (lease_expired(lease)) {
						/* probably best if we drop this lease */
						memset(lease->chaddr, 0, 16);
					/* make some contention for this address */
					} else
						send_NAK(&packet);
				} else {
					uint32_t r = ntohl(requested_align);
					if (r >= server_config.start_ip
				         && r <= server_config.end_ip
					) {
						send_NAK(&packet);
					}
					/* else remain silent */
				}

			} else {
				/* RENEWING or REBINDING State */
			}
			break;
		case DHCPDECLINE:
			DEBUG("Received DECLINE");
			if (lease) {
				memset(lease->chaddr, 0, 16);
				lease->expires = time(0) + server_config.decline_time;
			}
			break;
		case DHCPRELEASE:
			DEBUG("Received RELEASE");
			if (lease)
				lease->expires = time(0);
			break;
		case DHCPINFORM:
			DEBUG("Received INFORM");
			send_inform(&packet);
			break;
		default:
			bb_info_msg("Unsupported DHCP message (%02x) - ignoring", state[0]);
		}
	}
 ret0:
	retval = 0;
 ret:
	/*if (server_config.pidfile) - server_config.pidfile is never NULL */
		remove_pidfile(server_config.pidfile);
	return retval;
}


#if 1  /* this parts used for save mac address of wireless */

static RT_802_11_MAC_TABLE g_rtTable = {0};

#define HERT_LOGERR   DEBUG
#define HERT_LOGDEBUG DEBUG
#define HERT_LOGINFO  DEBUG

/*
** get wireless client info, 
** for some client will not be include in dhcp client
**
** return < 0 for error, else return the table number
*/
int hertUtil_UpdateWirelessClientInfo()
{
    char *ifname = "ra0";
    struct iwreq iwr;
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
    char tmpBuff[32];
    char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#endif
    int s = 0;

    memset(&g_rtTable, 0x0, sizeof(g_rtTable));

   /* get if name */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) &g_rtTable;

    if (s < 0) 
    {
        HERT_LOGERR("ioctl sock failed!");
        return -1;
    }

#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
    if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) 
    {
        HERT_LOGERR("ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
#else
    if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) 
    {
        HERT_LOGERR("ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
#endif
        close(s);
        return -1;
    }

    close(s);

    return g_rtTable.Num;
}

int hertUtil_IsWirelessClientMac(unsigned char *pszMac)
{
    int  i = 0;
    char szTempMac[64];

    for(i = 0; i < g_rtTable.Num; i++)
    {
        memset(szTempMac, 0x0, sizeof(szTempMac));
        sprintf(szTempMac, "%02X:%02X:%02X:%02X:%02X:%02X",
                g_rtTable.Entry[i].Addr[0], g_rtTable.Entry[i].Addr[1],
                g_rtTable.Entry[i].Addr[2], g_rtTable.Entry[i].Addr[3],
                g_rtTable.Entry[i].Addr[4], g_rtTable.Entry[i].Addr[5]);
		HERT_LOGDEBUG("szTempMac(%s), pszMac(%s)", szTempMac, pszMac);
        if ( 0 == strcasecmp(szTempMac, pszMac) )
        {
            return 1;
        }
    }
    return 0;
}

int hertUtil_SaveWirelessClientMac(unsigned char *pszMac)
{
    FILE *file;
    char cmd[128];
    char line[256];
    const char *pszFileName = "/var/wirelessclient.txt";
    char *ptr = NULL;

    if (!(file = fopen(pszFileName, "r"))) 
    {
        memset(cmd, 0x0, sizeof(cmd));
        sprintf(cmd,"echo %s >> %s", pszMac, pszFileName);
        system(cmd);
        HERT_LOGINFO("cmd: %s\n",cmd);
        return 0;
    }
    while ( fgets(line, 256, file) != NULL ) 
    {
        ptr = strstr(line, pszMac);
        if(ptr)
        {
            break;
        }
    }
    fclose(file);

    if ( NULL == ptr )
    {
        memset(cmd, 0x0, sizeof(cmd));
        sprintf(cmd,"echo %s >> %s", pszMac, pszFileName);
        system(cmd);
        HERT_LOGINFO("cmd: %s\n",cmd);
    }
	
    HERT_LOGINFO("pszFileName(%s)", pszFileName);
    return 0;
}


#endif /* this parts used for save mac address of wireless */




