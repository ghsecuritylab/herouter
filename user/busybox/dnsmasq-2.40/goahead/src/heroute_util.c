#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/vfs.h> 
#include <sys/statfs.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/wireless.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <ctype.h>

#include "md5.h"
#include	"internet.h"
#include	"nvram.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include  "wsIntrn.h"


#include "webs.h"
#include "uemf.h"

extern NETWORK_STATUS herouterStatus;
extern char InternetAccessRules[32][64];
extern char *repeatStart;
int g_esw_fd;

#ifdef SUPPORT_SEM
#include <semaphore.h>

char SEM_TASK_HEROUTE_PLATFORM[]= "heroute_platform_task";
char SEM_TASK_HEROUTE_APP[]= "heroute_app_task";
sem_t *g_sem_want_read = NULL;
sem_t *g_sem_finish_read = NULL;
#endif

#include "hert_com.h"
#include "herouter.h"

#define MAX_DEV_NUM  12

extern int g_nWpsState;
extern int g_nRemainTime;

#define MAC_LEN 17

extern char currMsgType[128];
extern int toAddDevNum;
extern int firstTime;


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


int vpcom_Printf(VPCOM_LOGLEVEL level, const char*szFile, int nLine, char *fmt, va_list vList);

char g_logTitle[255];
static int g_logDestMode = -1; /* 0: console, 1: telnet */
static int g_LogTelnetFd = -1;

VPCOM_LOGLEVEL g_logLevel = VPCOM_LOG_ERROR;

char *g_loglevelTxt[] = { "LOGOFF", "EMERG", "CRIT", "ALERT", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

int vpcom_LogInit(int nDestLog)
{
   g_logDestMode = nDestLog;
   return 0;
}

VPCOM_LOGLEVEL hertUtil_getLoglevel()
{
    FILE *file;
    char loglevel[64];
    char cmd[128];
    const char *tempFileName = "/var/tempwebloglevel.txt";

    memset(loglevel, 0x0, sizeof(loglevel));
    sprintf(cmd,"nvram_get HE_ROUTE_WEBAPP_LOGLEVEL > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return VPCOM_LOG_ERROR; /* default for 30 seconds */
    }   
    if (fgets(loglevel, 64, file) && (strlen(loglevel) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(loglevel);
    }    
    fclose(file);
    unlink(tempFileName);

    if ( 0 == strcmp(loglevel, "NOTICE"))
    {
        return VPCOM_LOG_NOTICE;
    }
    else if ( 0 == strcmp(loglevel, "DEBUG"))
    {
        return VPCOM_LOG_DEBUG;
    }
    else
    {
        return VPCOM_LOG_ERROR;
    }
}

int vpcom_Printf(VPCOM_LOGLEVEL level, const char*szFile, int nLine, char *fmt, va_list vList)
{
   struct tm *tm_ptr;
   time_t curtime;
   char   strModule[64] = { 0x0 };
   char   strLog1[1024] = { 0x0 };
#ifdef DESKTOP_LINUX
   int logTelnetFd = -1;
#endif

   time( &curtime );
   tm_ptr = gmtime( &curtime );

   sprintf(strModule, "%s", "HERT_HTTP_LOG");
   
   snprintf(strLog1, sizeof(strLog1), "[%s %02d:%02d:%02d] %s - %s, %d: ", strModule, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, 
           VPCOM_LOG_DEBUG >= level ? g_loglevelTxt[level] : "UNKNOW", szFile, nLine);

   if ( 1 == g_logDestMode )
   {
   #ifdef DESKTOP_LINUX
      /* Fedora Desktop Linux */
      g_LogTelnetFd = open("/dev/pts/1", 2);
   #else
      /* CPE use ptyp0 as the first pesudo terminal */
      //g_LogTelnetFd = open("/dev/ttyp0", 2);
   #endif
      if(g_LogTelnetFd != -1)
      {
/* here, we temp to comment it for telnet, zhuzhh, 2013/01/29 start */
#if 0
         fprintf(g_LogTelnetFd, strLog1);
         vfprintf(g_LogTelnetFd, fmt, vList);
         fprintf(g_LogTelnetFd, "\n");
         return 0;
#endif
/* here, we temp to comment it for telnet, zhuzhh, 2013/01/29 end */
      }
   }
   printf(strLog1);
   vprintf(fmt, vList);
   printf("\n");
   return 0;
}


/* Highest Log level */
void vpcom_log(const char*szFile, int nLine, const char *format,...)
{                          
   va_list varArgList;
   if ( VPCOM_LOG_DEBUG <= g_logLevel )
   {
      va_start (varArgList, format);
      vpcom_Printf(VPCOM_LOG_DEBUG, szFile, nLine, (char*)format, varArgList);
      va_end (varArgList);
   }
}

/* Medium Log level */
void vpcom_logInfo(const char*szFile, int nLine,  const char *format,...)
{
   va_list varArgList;
   
   if ( VPCOM_LOG_NOTICE <= g_logLevel )
   {
      va_start (varArgList, format);
      vpcom_Printf(VPCOM_LOG_INFO, szFile, nLine, (char*)format, varArgList);
      va_end (varArgList);
   }
}

/* Lowest Log level */
void vpcom_logErr(const char*szFile, int nLine, const char *format,...)
{                          
   va_list varArgList;
   
   if ( VPCOM_LOG_ERROR <= g_logLevel )
   {
      va_start (varArgList, format);
      vpcom_Printf(VPCOM_LOG_ERROR, szFile, nLine, (char*)format, varArgList);
      va_end (varArgList);
   }
}

VPCOM_LOGLEVEL vpcom_GetLogLevel()
{
   return g_logLevel;
}

void vpcom_SetLogLevel(VPCOM_LOGLEVEL level)
{  
   g_logLevel = level;

   if ( level <= VPCOM_LOG_DEBUG )
   {
      strcpy(g_logTitle, g_loglevelTxt[level]);
   }
   else
   {
      strcpy(g_logTitle, "UNKOW");
   }
}


void hertUtil_setLoglevel()
{
    VPCOM_LOGLEVEL nloglevel = hertUtil_getLoglevel();
    vpcom_SetLogLevel(nloglevel);
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





/* {?°version?±:0,?± msgType?±:?±json?±, ?°msgSeq?±:0, ?°errorCode?±,?± description?±:?±errdesc?±:?±abcdefg, */
CHAR *ParseJsonString(CHAR *pData, CHAR *pszItemName, CHAR *pszItemValue)
{
    CHAR    szName[128];
    CHAR    szValue[128];
    CHAR    *pchrR = NULL;
    CHAR    *pchrL = NULL;
    CHAR    *pchrEnd = NULL;
    CHAR    *pchrTmp = NULL;
    int      nItemLength = 0;
	int      arryJsonFlag = 0;

#define STR_QUOTATION "\""
#define STR_SEPRATION ":"
#define STR_END1      ","
#define STR_END2      "}"
#define STR_ARRY_START   "[{"
#define STR_ARRY_END1     "},"
#define STR_ARRY_END2     "],"


    memset(szName, 0x0, sizeof(szName));
    memset(szValue, 0x0, sizeof(szValue));
    
    //if point to repeat json,just game over,not good ios
    if((repeatStart != NULL) && (repeatStart < pData))
    {
        HERT_LOGINFO(">>>>Repeat json no need parse<<<<!!!!!!(%s)", pData);
        return NULL; 
    }

    /* -------- get item name -------- */
    pchrL = strstr(pData, STR_QUOTATION);
    if (!pchrL)
    {
        HERT_LOGINFO("Can not find item name(%s)", pData);
        return NULL;
    }
    pchrL += strlen(STR_QUOTATION);

    pchrR = strstr(pchrL, STR_QUOTATION);
    if (!pchrR)
    {
        HERT_LOGINFO("Can not find item name(%s)", pData);
        return NULL;
    }
    nItemLength = pchrR - pchrL;
    nItemLength = (nItemLength > (sizeof(szName) - 1)) ? (sizeof(szName) - 1) : nItemLength;
    memcpy(szName, pchrL, nItemLength);

    /* -------- get item name -------- */
	
    /* 1.get left possition */
    pchrL = strstr(pchrR, STR_SEPRATION);
    if (!pchrL)
    {
        HERT_LOGINFO("Can not find item name(%s)", pData);
        return NULL;
    }
    pchrL += strlen(STR_SEPRATION);

    /*this item is arry json,should set new itemName*/
	if(strncmp(pchrL,STR_ARRY_START,strlen(STR_ARRY_START)) == 0)
    {

        memset(szName, 0x0, sizeof(szName));
		
        pchrL = strstr(pchrL, STR_QUOTATION);
        if (!pchrL)
        {
            HERT_LOGINFO("Can not find item name(%s)", pData);
            return NULL;
        }
        pchrL += strlen(STR_QUOTATION);
		
        pchrR = strstr(pchrL, STR_QUOTATION);
        if (!pchrR)
        {
            HERT_LOGINFO("Can not find item name(%s)", pData);
            return NULL;
        }
        nItemLength = pchrR - pchrL;
        nItemLength = (nItemLength > (sizeof(szName) - 1)) ? (sizeof(szName) - 1) : nItemLength;
        memcpy(szName, pchrL, nItemLength);
        /* 1.get left possition */
        pchrL = strstr(pchrR, STR_SEPRATION);
        if (!pchrL)
        {
            HERT_LOGINFO("Can not find item name(%s)", pData);
            return NULL;
        }
        pchrL += strlen(STR_SEPRATION);


    }
    /* 2.get end possition */
    pchrEnd = strstr(pchrL, STR_END1);
    
    if(pchrEnd)	
    {
        pchrTmp = pchrEnd - 1;
        if(strncmp(pchrTmp,STR_ARRY_END1,strlen(STR_ARRY_END1)) == 0 || \
            strncmp(pchrTmp,STR_ARRY_END2,strlen(STR_ARRY_END2)) == 0)
        {
            arryJsonFlag = 1;
        }
    }
    if(pchrEnd && arryJsonFlag != 1)
    {
        pchrR = pchrEnd;
        pchrEnd += strlen(STR_END1);
    }
    else
    {
        pchrEnd = strstr(pchrL, STR_END2);
        if(pchrEnd)
        {
            pchrR = pchrEnd;
            pchrEnd += strlen(STR_END2);
        }
    }
    if (!pchrEnd)
    {
        HERT_LOGINFO("Invalid msg data format(%s)", pData);
        return NULL;
    }
    /* 3.modify left and right possition if it is string */
    pchrTmp = strstr(pchrL, STR_QUOTATION);
    if (pchrTmp && (pchrTmp < pchrEnd) )
    {
        pchrL = pchrTmp + strlen(STR_SEPRATION);
        pchrR = NULL;
        pchrTmp = strstr(pchrL, STR_QUOTATION);
        if (pchrTmp && (pchrTmp < pchrEnd) )
        {
            pchrR = pchrTmp;
        }
    }
    if ((!pchrL) || (!pchrR))
    {
        HERT_LOGINFO("Invalid msg data format(%s)", pData);
        return NULL;
    }
    nItemLength = pchrR - pchrL;
    nItemLength = (nItemLength > (sizeof(szValue) - 1)) ? (sizeof(szValue) - 1) : nItemLength;

    memcpy(szValue, pchrL, nItemLength);

    strcpy(pszItemName, szName);

    strcpy(pszItemValue, szValue);

    //count set devNumber for first time
    if(strncmp(currMsgType,"MSG_SET_ROUTER_ADD_DEV_REQ",strlen("MSG_SET_ROUTER_ADD_DEV_REQ")) == 0 && firstTime == 1)
    {
        if((pchrL = strstr(pData,"[")) && (pchrR = strstr(pData,"]")))    
        {
            pchrTmp = pchrL;
            for(pchrTmp = pchrL;pchrTmp < pchrR; pchrTmp++)
            {
                if(*pchrTmp == '{')
                toAddDevNum++;
            }
        }
        else
        {
            toAddDevNum = 0;
        }
        firstTime = 0;
    }
    return pchrEnd;
}

char *hertUtil_getPassword()
{
	const char *pwd = nvram_bufget(RT2860_NVRAM, "Password");
/*	
	FILE *fp;
	memset(pass_buf, 0, sizeof(pass_buf));
	if(!(fp = popen("nvram_get Password", "r")))
		return NULL;
	fread(pass_buf, 1, sizeof(pass_buf), fp);
	pclose(fp);
*/
	return pwd;
}

int hertUtil_setNetConfig4App(HERT_WEBS_MSG_REQ_VAR_SETNET_BODY * NetConfig)
{
	char_t	*mssid_0 = NULL;
	char_t	bssid_num[16];
	int i = 0,new_bssid_num, old_bssid_num = 1;
	int mbssid;
		
	if(NetConfig == NULL)
	    return HERT_ERR_INVALID_JSON;
	    
	/*WAN config start*/
	nvram_bufset(RT2860_NVRAM, "wan_ipaddr", "");
	nvram_bufset(RT2860_NVRAM, "wan_netmask", "");
	nvram_bufset(RT2860_NVRAM, "wan_gateway", "");
		
	//NetConfig->netType ===> 0:DHCP,1:PPPOE,2:PPPOE
	if (NetConfig->netType == 0) {
	    nvram_bufset(RT2860_NVRAM, "wanConnectionMode", "DHCP");
		  nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", "");
	}
	else if (NetConfig->netType == 1) {
      if(NetConfig->netAccount == NULL || NetConfig->netPassword == NULL)
      {
          return HERT_ERR_INVALID_JSON;	
      }
		  nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", NetConfig->netAccount);
		  nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", NetConfig->netPassword);
		  printf("@@@@@@@@ NetConfig->netAccount:%s @@@@@@@@\n",NetConfig->netAccount);
		  printf("@@@@@@@@ NetConfig->netPassword:%s @@@@@@@@\n",NetConfig->netPassword);
		  nvram_bufset(RT2860_NVRAM, "wanConnectionMode", "PPPOE");
		  nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", "pppoeRedialPeriod");
		  nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", "60");
  }
  else if (NetConfig->netType == 2) {

    	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	    const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	    const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

      nvram_bufset(RT2860_NVRAM, "wanConnectionMode", "STATIC");
      if (-1 == inet_addr(NetConfig->ipv4Address)) {
          nvram_commit(RT2860_NVRAM);
          return HERT_ERR_INTERNEL_FALSE;
      }
      /*
      * lan and wan ip should not be the same except in bridge mode
      */
      if (NULL != opmode && strcmp(opmode, "0") && !strncmp(NetConfig->ipv4Address, lan_ip, 15)) {
          nvram_commit(RT2860_NVRAM);
          return HERT_ERR_INVALID_JSON;
      }
		  if (!strcmp(lan2enabled, "1"))
		  {
		  	const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			  if (NULL != opmode && strcmp(opmode, "0") && !strncmp(NetConfig->ipv4Address, lan2_ip, 15)) {
				  nvram_commit(RT2860_NVRAM);
				  return HERT_ERR_INVALID_JSON;
			  }
		  }
		  nvram_bufset(RT2860_NVRAM, "wan_ipaddr", NetConfig->ipv4Address);
		  struct in_addr addr;
		  if (-1 == inet_aton(NetConfig->subnetMask,&addr)) {
			  nvram_commit(RT2860_NVRAM);
			  return HERT_ERR_INTERNEL_FALSE;
		  }
		  nvram_bufset(RT2860_NVRAM, "wan_netmask", NetConfig->subnetMask);
		  /*
		   * in Bridge Mode, lan and wan are bridged together and associated with
		   * the same ip address
		   */
		  if (NULL != opmode && !strcmp(opmode, "0")) {
			  nvram_bufset(RT2860_NVRAM, "lan_ipaddr", NetConfig->ipv4Address);
			  nvram_bufset(RT2860_NVRAM, "lan_netmask", NetConfig->subnetMask);
		  }
		  nvram_bufset(RT2860_NVRAM, "wan_gateway", NetConfig->gateway);
		  nvram_bufset(RT2860_NVRAM, "wan_primary_dns", NetConfig->dnsServer);
		  nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", NetConfig->dnsServerBackup);
  }
  else {
		return HERT_ERR_UNKNOW_MSGTYPE;
	}
	/*WAN config end*/

	memset(bssid_num, 0x0, sizeof(bssid_num));
	mbssid = 0;
	/*WIFI basic config start*/
	if(NetConfig->wifiName != NULL)
	    mssid_0 = NetConfig->wifiName;
	printf("@@@@@@@@ NetConfig->wifiName:%s @@@@@@@@\n",NetConfig->wifiName);
	printf("@@@@@@@@ mssid_0:%s @@@@@@@@\n",mssid_0);
	old_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));

	/* we set ssid number as before, it is no need changed */
	sprintf(bssid_num, "%d", old_bssid_num);

	new_bssid_num = atoi(bssid_num);
	//set SSID
	if (NULL == mssid_0 || 0 == strlen(mssid_0)) {
		return HERT_ERR_INVALID_JSON;
	} else {
		i++;
		nvram_bufset(RT2860_NVRAM, racat("SSID", i), mssid_0);
	}
	nvram_bufset(RT2860_NVRAM, "BssidNum", bssid_num);
	revise_mbss_value(old_bssid_num, new_bssid_num);
	/*WIFI basic config end*/
	
	
	
  /*WIFI Security config start*/
	if (strlen(NetConfig->wifiPassword) > 0) { 	// WPA PSK WPA2 PSK mixed

		STFs(RT2860_NVRAM, mbssid, "EncrypType", "AES");
		STFs(RT2860_NVRAM, mbssid, "DefaultKeyID", "2");	// DefaultKeyID is 1
		STFs(RT2860_NVRAM, mbssid, "RekeyInterval", "1");
		STFs(RT2860_NVRAM, mbssid, "RekeyMethod", "TIME");		
		STFs(RT2860_NVRAM, mbssid, "IEEE8021X", "0");
		STFs(RT2860_NVRAM, mbssid, "AuthMode", "WPAPSKWPA2PSK");

		nvram_bufset(RT2860_NVRAM, racat("WPAPSK", mbssid+1), NetConfig->wifiPassword);
		printf("@@@@@@@@ NetConfig->wifiPassword:%s @@@@@@@@\n",NetConfig->wifiPassword);
	}else {						// Open-None Mode
		STFs(RT2860_NVRAM, mbssid, "AuthMode", "OPEN");
		STFs(RT2860_NVRAM, mbssid, "EncrypType", "NONE");
	}
	/*WIFI Security config end*/
	
  return 0;
}

#define MAX_ENCRYPT_LEN 1024
#define HASH_SIZE 130

int hertUtil_aes_ecb_128_Encrypt(const unsigned char *sMsg, int cbMsg, unsigned char *sEncryptMsg, int *cbEncryptMsg)
{
	  
	  
    MD5_CONTEXT		md5ctx;    
    unsigned char	hash[HASH_SIZE] = {0};    	
	  unsigned char SessionKey[18] = {0};
	  char    md5_aes_key[64] = {0};
	  
	  
    OpenSSL_add_all_algorithms();

    // 产生会话密钥(aec-128)
    //RAND_bytes(SessionKey,16);
    memset(SessionKey,0x0,sizeof(SessionKey));
    memcpy(SessionKey,AES_APP_KEY,strlen(AES_APP_KEY)); 
    SessionKey[strlen(AES_APP_KEY)] = '\0';
    
    MD5Init(&md5ctx);     
    MD5Update(&md5ctx, SessionKey, (unsigned int)strlen(SessionKey));    
    MD5Final(hash, &md5ctx);
		printf("@@@@@@@@ Encrypt_SessionKey:%s @@@@@@@@\n",SessionKey); 
		memset(md5_aes_key,0x0,sizeof(md5_aes_key));
    //snprintf(md5_aes_key,sizeof(md5_aes_key), "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
					//hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);
    snprintf(md5_aes_key,sizeof(md5_aes_key), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
					hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);
     
    herUtil_dumpAesHexString(__FUNCTION__, __LINE__, md5_aes_key, strlen(md5_aes_key)); 
    printf("@@@@@@@@ Encrypt_md5_aes_key:%s @@@@@@@@\n",md5_aes_key); 
    printf("@@@@@@@@ Be_Encrypt_text(%d):%s @@@@@@@@\n",cbMsg,sMsg); 
     
    // 加密
    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_CIPHER_CTX_set_padding(&ctx, 1); // 1- padding, 0 - No Padding
    if(EVP_EncryptInit_ex(&ctx,EVP_get_cipherbynid(NID_aes_128_ecb),NULL,md5_aes_key,NULL))
    {
        int offseti=0;
        int offseto=0;
        int offsett=0;
        for(;;)
        {
            if(cbMsg-offseti<=MAX_ENCRYPT_LEN)
            {
                EVP_EncryptUpdate(&ctx, sEncryptMsg+offseto, &offsett, sMsg+offseti, cbMsg-offseti);
                offseto += offsett;
                break;
            }
            else
            {
                EVP_EncryptUpdate(&ctx, sEncryptMsg+offseto, &offsett, sMsg+offseti, MAX_ENCRYPT_LEN);
                offseti += MAX_ENCRYPT_LEN;
                offseto += offsett;
            }
        }
        int test = cbMsg>>4;

        if(cbMsg != test<<4)
        {
            HERT_AES_DEBUG(HERT_LOGERR("@@@@@@@@ EVP_EncryptFinal_ex(%d):%s @@@@@@@@\n",cbMsg,sMsg)); 
            EVP_EncryptFinal_ex(&ctx, sEncryptMsg+offseto, &offsett);
            offseto+=offsett;
        }

        EVP_EncryptFinal_ex(&ctx, sEncryptMsg+offseto, &offsett);
        offseto+=offsett;
        *cbEncryptMsg=offseto;
    }
    EVP_CIPHER_CTX_cleanup(&ctx);

    return 0;
}


int hertUtil_aes_ecb_128_Decrypt(unsigned char* inBuf,int len,unsigned char* outBuf)
{
	const unsigned char* sMsg = inBuf;
	int cbMsg = len;
	int cbEncryptMsg;
	char SessionKey[18] = {0};
	char    md5_aes_key[64] = {0};
	unsigned char	hash[HASH_SIZE] = {0};  
	MD5_CONTEXT		md5ctx; 
	
	OpenSSL_add_all_algorithms();
	

	// 产生会话密钥(aec-128)
	memset(SessionKey,0x0,sizeof(SessionKey));
	memcpy(SessionKey,AES_ONENET_KEY,strlen(AES_ONENET_KEY)); 
	SessionKey[strlen(AES_ONENET_KEY)] = '\0';
	
	MD5Init(&md5ctx);     
	MD5Update(&md5ctx, SessionKey, (unsigned int)strlen(SessionKey));    
	MD5Final(hash, &md5ctx);

  printf("@@@@@@@@ Decrypt_SessionKey:%s @@@@@@@@\n",SessionKey); 

  snprintf(md5_aes_key,sizeof(md5_aes_key), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
					hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);
	//snprintf(md5_aes_key,sizeof(md5_aes_key), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
				//hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);
	 
	printf("@@@@@@@@ Decrypt_md5_aes_key:%s @@@@@@@@\n",md5_aes_key); 
	
	//解密
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_CIPHER_CTX_set_padding(&ctx, 1); 
	if(EVP_DecryptInit_ex(&ctx,EVP_get_cipherbynid(NID_aes_128_ecb),NULL,md5_aes_key,NULL))
	{
		int offseti=0;//in
		int offseto=0;//out
		int offsett=0;//temp
		for(;;)
		{
			if(cbMsg-offseti<=MAX_ENCRYPT_LEN)
			{
				EVP_DecryptUpdate(&ctx, outBuf+offseto, &offsett, sMsg+offseti, cbMsg-offseti);
				offseto+=offsett;
				break;
			}
			else
			{
				EVP_DecryptUpdate(&ctx, outBuf+offseto, &offsett, sMsg+offseti, MAX_ENCRYPT_LEN);
				offseti+=MAX_ENCRYPT_LEN;
				offseto+=offsett;
			}
		}
	  EVP_DecryptFinal_ex(&ctx, outBuf+offseto, &offsett);
	
	  offseto+=offsett;
	  cbEncryptMsg=offseto;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 0;
}


void hertUtil_BuildErrMsgRsp(DWORD errorCode,const char* msgType,char *pszOutputData, int nBufferLen,HERT_WEBS_MSG_REQ_BODY *pReqBody){

	char errTmp[128] = {0};
	char tErrorCode[8] = {0};
	char tMsgType[32] = {0};
	char tVersion[4] = {0};
	char tMsgSeq[16] = {0};
	
	HERT_LOGINFO("Enter in buildErrorMsgRsp\n");	
	
  snprintf(tErrorCode, sizeof(tErrorCode), "%ld",errorCode);
  
  snprintf(tMsgType, sizeof(tMsgType), "%s",msgType);
  
  snprintf(tVersion, sizeof(tVersion), "%ld",pReqBody->version);
  
  if(strlen(pReqBody->msgSeq) > 0)
  	snprintf(tMsgSeq, sizeof(tMsgSeq), "%s",pReqBody->msgSeq);	
  
  snprintf(errTmp, sizeof(errTmp),"{\"version\":%s,\"msgSeq\":\"%s\",\"errorCode\":%s,\"msgType\":\"%s\"}",\
  		tVersion,tMsgSeq,tErrorCode,msgType);
  
  memset(pszOutputData,0x0,nBufferLen);
  strncpy(pszOutputData,errTmp,nBufferLen);
  
}

void hertUtil_DelNetFiltMac(char *filterMac)
{
    char cmdLine[256] = {0};
    char lan_if_addr[16] = {0};

    if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
        return;
    }

    HERT_LOGINFO("######## app delete MacFilter:%s-%s########\n",filterMac,lan_if_addr);
    
    /*for local Network*/
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-dst ! %s -s %s -j DROP 1>/dev/null 2>/dev/null",\
        lan_if_addr,filterMac);
    system(cmdLine);
	  system(cmdLine);
	  system(cmdLine);
	  
    HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
	  
    /*for dhcp*/
    memset(cmdLine,0x0,sizeof(cmdLine));
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-proto 17 --ip-sport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
    system(cmdLine);
	  system(cmdLine);
	  system(cmdLine);
		
    HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
		
    memset(cmdLine,0x0,sizeof(cmdLine));    
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -D INPUT -p 0x0800 --ip-proto 17 --ip-dport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
    system(cmdLine);
	  system(cmdLine);
	  system(cmdLine);
		
    HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
	  
}


void hertUtil_AddNetFiltMac(char *filterMac)
{
    char cmdLine[256] = {0};	
    char lan_if_addr[16] = {0};

    if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
        return;
    }

    HERT_LOGINFO("######## app add MacFilter:%s-%s########\n",filterMac,lan_if_addr);
    
    /*for local Network*/
    snprintf(cmdLine,sizeof(cmdLine),"ebtables -A INPUT -p 0x0800 --ip-dst ! %s -s %s -j DROP 1>/dev/null 2>/dev/null",\
          lan_if_addr,filterMac);
    system(cmdLine);
    
    HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
    
    /*for dhcp*/
    memset(cmdLine,0x0,sizeof(cmdLine));
	  snprintf(cmdLine,sizeof(cmdLine),"ebtables -I INPUT -p 0x0800 --ip-proto 17 --ip-sport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
		system(cmdLine);
		
		HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
		
		memset(cmdLine,0x0,sizeof(cmdLine));    
	  snprintf(cmdLine,sizeof(cmdLine),"ebtables -I INPUT -p 0x0800 --ip-proto 17 --ip-dport 67 -s %s -j ACCEPT 1>/dev/null 2>/dev/null",\
		    filterMac);
		system(cmdLine);  
		
		HERT_LOGINFO("######## cmdLine:%s########\n",cmdLine);
}

int hertUtil_getHerouteUpImg()
{
    FILE *file;
    char szHeUpImg[64];
    char cmd[128];
    const char *tempFileName = "/var/tempheupimg.txt";

    memset(szHeUpImg, 0x0, sizeof(szHeUpImg));
    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"nvram_get HE_ROUTE_UPIMAG > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 0;
    }   
    if (fgets(szHeUpImg, 64, file) && (strlen(szHeUpImg) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(szHeUpImg);
    }    
    fclose(file);
    unlink(tempFileName);

    return atoi(szHeUpImg);
}

/* 0: no need upgrade, 1:need upgrade */
int hertUtil_getIsNeedUpgrade()
{
    FILE* fr = NULL;
    /* 发送数据-BODY_REPONSE_VAR_GET_UPDATE */
    typedef struct tagRspPushDataVarPart_GetUpdate
    {
        CHAR  needUpdate;                /* 是否需要升级, 0：无需升级, 1：强制升级, 2：非强制升级 */
        CHAR  updateUrl[256];            /* 升级文件URL, 无需升级时，无此字段 */
        CHAR  updateDescription[256];    /* 升级原因说明, 各项说明通过竖线（|）分割 */
    }HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE;
    HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE tmpData;
    int ret = 0; /*default it is no need upgrade */

#define TEMP_NEEDUPGRADEINFO "/var/upgradeinfo"

    fr  = fopen(TEMP_NEEDUPGRADEINFO, "rb");
    if ( fr)
    {
        if ( fread(&tmpData, 1, sizeof(tmpData), fr) == sizeof(tmpData))
        {
            if (tmpData.needUpdate == 0)
            {
                ret = 0;
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            HERT_LOGINFO("TEMP_NEEDUPGRADEINFO(%s)\n", TEMP_NEEDUPGRADEINFO);
        }
        fclose(fr);
    }
    else
    {
        HERT_LOGINFO("Failed to open TEMP_NEEDUPGRADEINFO(%s)\n", TEMP_NEEDUPGRADEINFO);
    }
    return ret;
}


HERT_WEBS_MSG_RSP_STATUS_BODY g_tWebRspBody;

typedef struct tagMacIpAddr
{
    unsigned char mac[32];             /* Mac */
    unsigned long ip;          /* Ip */
}MAC_IP_MAP;

#define MAX_MACIP_MAP_NUMBER 32
int g_nNumOfMacIpMap = 0;
MAC_IP_MAP g_MacIpArray[MAX_MACIP_MAP_NUMBER];

/* return device list number if success, else return < 0 */
int hertUtil_GetDeviceList_DoInit(int nIsOnlyGetNum)
{
    memset(&g_tWebRspBody, 0x0, sizeof(g_tWebRspBody));

    hertUtil_GetDeviceList(&g_tWebRspBody.pDev, &g_tWebRspBody.dwDevNumber);

    if(nIsOnlyGetNum)
    {
        HERT_LOGINFO("g_tWebRspBody.dwDevNumber(%ld)", g_tWebRspBody.dwDevNumber);
        return (int)g_tWebRspBody.dwDevNumber;
    }

    FILE *fp;
    struct dhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip;
        unsigned long expires;
    } lease;

    g_nNumOfMacIpMap = 0;
    memset(&g_MacIpArray[0], 0x0, sizeof(MAC_IP_MAP) * MAX_MACIP_MAP_NUMBER);

    system("cp /var/udhcpd.leases /var/tempwebudcpdlease 2>/dev/null");
    fp = fopen("/var/tempwebudcpdlease", "r");
    if (fp)
    {
        while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) 
        {
            memcpy(&g_MacIpArray[g_nNumOfMacIpMap].mac[0], &lease.mac[0], sizeof(lease.mac));
            g_MacIpArray[g_nNumOfMacIpMap].ip = lease.ip;			
            g_nNumOfMacIpMap++;
        }
        fclose(fp);
    }
    system("rm -rf /var/tempwebudcpdlease 2>/dev/null");

    HERT_LOGINFO("g_tWebRspBody.dwDevNumber(%ld)", g_tWebRspBody.dwDevNumber);
    return (int)g_tWebRspBody.dwDevNumber;
}

void hertUtil_GetDeviceList_DoUnInit()
{
    if (g_tWebRspBody.pDev)
    {
        free(g_tWebRspBody.pDev);
        g_tWebRspBody.pDev = NULL;
    }
}

int hertUtil_GetDeviceList_GetItem(int nIndex, char *szMac, char *szDevName, char *szIpAddr)
{
    HERT_WEBS_MSG_RSP_SUBDEV_BODY *pDev = g_tWebRspBody.pDev;
    struct in_addr addr;
    int i = 0;
    char szTempMac[64];

    if (g_tWebRspBody.dwDevNumber <= nIndex || nIndex < 0 )
    {
        HERT_LOGINFO("g_tWebRspBody.dwDevNumber(%ld), nIndex(%d)", g_tWebRspBody.dwDevNumber, nIndex);
        return -1;
    }

    pDev += nIndex;

    // get mac
    sprintf(szMac, "%s", pDev->mac);
    sprintf(szDevName, "%s", pDev->devName);
    for(i = 0; i < g_nNumOfMacIpMap; i++)
    {
        memset(szTempMac, 0x0, sizeof(szTempMac));
        sprintf(szTempMac, "%02X:%02X:%02X:%02X:%02X:%02X", g_MacIpArray[i].mac[0], g_MacIpArray[i].mac[1], 
            g_MacIpArray[i].mac[2], g_MacIpArray[i].mac[3], g_MacIpArray[i].mac[4], g_MacIpArray[i].mac[5]);
        HERT_LOGINFO("g_MacIpArray[%d].szTempMac(%s), pDev->mac(%s)", i, szTempMac, pDev->mac);
        if ( 0 == strcasecmp(szTempMac, pDev->mac) )
        {
            addr.s_addr = g_MacIpArray[i].ip;
            sprintf(szIpAddr, "%s", inet_ntoa(addr));
            break;
        }
    }
    HERT_LOGINFO("pDev(%p), szMac(%s), szDevName(%s), szIpAddr(%s)", pDev, szMac, szDevName, szIpAddr);
    return 0;
}

void herUtil_isCloseGuestSsid()
{
    int bssid_num = 1;
    int new_bssid_num, old_bssid_num = 1;
    
    int open_time = 0;
    time_t start_sec, cur_sec;
    int usedtime;
      
    open_time = atoi(nvram_bufget(RT2860_NVRAM, "GuestOpenTime"));
    start_sec = atol(nvram_bufget(RT2860_NVRAM, "GuestStartTime"));
    HERT_LOGDEBUG("open_time(%d) start_sec(%ld)", open_time, start_sec);
    if (open_time == -1 || open_time == 0)
    {
        g_nRemainTime = open_time;
        return;
    }

    cur_sec = getUptime();
    HERT_LOGDEBUG("cur_sec(%ld)\n", cur_sec);
    if (start_sec > cur_sec)
    {
        char sec[128];
        memset(sec, 0x0, sizeof(sec));
        sprintf(sec, "%ld", cur_sec);
        nvram_bufset(RT2860_NVRAM, "GuestStartTime", sec);
        return;
    }
    
    usedtime = (int)((cur_sec - start_sec)/3600);
    g_nRemainTime = open_time*60 - (int)((cur_sec - start_sec)/60);

    HERT_LOGDEBUG("Guest Remain Time(%d minutes)\n", g_nRemainTime);
    if (usedtime >= open_time)
    {
        HERT_LOGINFO("Now close guest SSID...\n");  
        bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
        old_bssid_num = bssid_num;
        
        if (bssid_num >= 2)
        {
            nvram_bufset(RT2860_NVRAM, racat("SSID", 2), "");
            nvram_bufset(RT2860_NVRAM, "BssidNum", "1");
        }
                
        new_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
        revise_mbss_value(old_bssid_num, new_bssid_num);
        nvram_bufset(RT2860_NVRAM, "GuestOpenTime", "0");
        nvram_bufset(RT2860_NVRAM, "GuestStartTime", "0"); 
        nvram_commit(RT2860_NVRAM);
        initInternet(); 
    }      
}

void herUtil_checkWps()
{
    static int nCloseWpsCount = 0;

    if (g_nWpsState == 2)
    {
        nCloseWpsCount++;
        if (nCloseWpsCount == 20 )
        {
            doSystem("iwpriv ra0 set WscConfMode=0 1>/dev/null 2>&1");
#if defined (RTDEV_SUPPORT)
        const char *raix_wsc_enable = nvram_bufget(RTDEV_NVRAM, "WscModeOption");
        if (strcmp(raix_wsc_enable, "0") == 0)
#endif
            doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
            nCloseWpsCount = 0;
			g_nWpsState = 0;/* no set */
			HERT_LOGINFO("Now close WPS.");
        }
        HERT_LOGINFO("WPS is openned, wait for close %d...", nCloseWpsCount);
    }
}

int hertUtil_InitStatusThread()
{
    g_esw_fd = socket(AF_INET, SOCK_DGRAM, 0);

    return g_esw_fd;
}

int hertUtil_UnInitStatusThread()
{
    if(g_esw_fd >= 0)
    {
        close(g_esw_fd);
    }

    return 0;
}


#define RTPRIV_IOCTL_SET_FILTER_HIDE_SSID	(SIOCIWFIRSTPRIV + 0x1D)

int hertUtil_SetHideSSidForFilter()
{
    int s;
    struct iwreq iwr;
    char *ifname = "ra0";
    char szHideSsid[256] = { 0x0 }; /* wps mac */
	char *pHideSsid = nvram_bufget(RT2860_NVRAM, "SSID2");

    memset(szHideSsid, 0x0, sizeof(szHideSsid));
    sprintf(szHideSsid, "%s", pHideSsid);

    /* get if name */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) 
    {
        HERT_LOGERR("RTPRIV_IOCTL_SET_FILTER_HIDE_SSID: ioctl sock failed!\n");
        return 1;
    }

    /* get Hide special ssid mac for filter */
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = szHideSsid;
    if (ioctl(s, RTPRIV_IOCTL_SET_FILTER_HIDE_SSID, &iwr) < 0) 
    {
        HERT_LOGERR("hertUtil_GetHssMac: ioctl -> RTPRIV_IOCTL_SET_FILTER_HIDE_SSID failed!\n");
        close(s);
        return 1;
    }
    HERT_LOGINFO("SetFilterHideSsid(%s)\n", szHideSsid);
    close(s);

    return 0;
}

static int hertUtil_reg_read(int offset, int *value,char *portStatus)
{
    struct ifreq ifr;

    if (value == NULL)
        return -1;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = portStatus;
    if (-1 == ioctl(g_esw_fd, RAETH_PORTSTATE_READ, &ifr)) 
    {
        perror("ioctl");
        close(g_esw_fd);
        exit(0);
    }
    return 0;
}

int hertUtil_upEthrtPortStatus()
{
    char portStatus[4] = {0};   
    int  value;
    
    hertUtil_reg_read(REG_ESW_VLAN_ID_BASE, &value,portStatus);  
    if(strlen(portStatus) > 0)
    {
        memset(herouterStatus.wanPortStatus,0x0,sizeof(herouterStatus.wanPortStatus));
        memset(herouterStatus.lan1PortStatus,0x0,sizeof(herouterStatus.lan1PortStatus));
        memset(herouterStatus.lan2PortStatus,0x0,sizeof(herouterStatus.lan2PortStatus));
        
        if(portStatus[0] == 'u')
        {
            strcpy(herouterStatus.wanPortStatus,STRING_PASS);	
        }
        else
        {
            strcpy(herouterStatus.wanPortStatus,STRING_FAIL);	
        }	
        
        if(portStatus[1] == 'u')
        {
            strcpy(herouterStatus.lan1PortStatus,STRING_PASS);	
        }
        else
        {
            strcpy(herouterStatus.lan1PortStatus,STRING_FAIL);	
        }	
        
        if(portStatus[2] == 'u')
        {
            strcpy(herouterStatus.lan2PortStatus,STRING_PASS);	
        }
        else
        {
            strcpy(herouterStatus.lan2PortStatus,STRING_FAIL);	
        }	
    }    	
    HERT_LOGDEBUG("wanPortStatus:%s,lan1PortStatus:%s,lan2PortStatus:%s\n",
        herouterStatus.wanPortStatus,herouterStatus.lan1PortStatus,herouterStatus.lan2PortStatus);
    //printf("wanPortStatus:%s,lan1PortStatus:%s,lan2PortStatus:%s\n",herouterStatus.wanPortStatus,herouterStatus.lan1PortStatus,herouterStatus.lan2PortStatus);
    return 0;
}


int countSubString(char *src,char *tag)
{
    int count=0,ruleSize = 0;
    char *p = NULL,*endp = NULL,*ptr = src,*qtr = tag;
    
    if((ruleSize = strlen(src)) == 0)
    {
    	  count = 0;
        return count;
    }
    endp = src + (ruleSize -1) ;
    do{
        p=strstr(ptr,qtr);
        if(p != NULL) {
           ptr=p+1;
           count=count+1;
        }
    }while((p!=NULL) && (endp >= ptr));
    //QOS end char is not ";",so the number of rules should add 1. 
    if(strcmp(tag,INTERNET_QOS_SEPERATOR) == 0)
    {
        count = count + 1;
    }
    HERT_LOGINFO("Rules Number:%d%s\n", count);	
    return count;
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
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 0;
    }
    if (fread(szOutput, sizeof(char),nBuffSize,file) && (strlen(szOutput) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(szOutput);
    }    
    fclose(file);
    unlink(tempFileName);

    return strlen(szOutput);
}


/*the rules in nvram like "aa:bb:cc:dd:ee:ff(MAC)-1(accessControl)-12:00(startTime)-14:30(endTime)-1(netLimit)," */
int isValidRules(const char *rules)
{
    int flagNum = 0,ret = 0;
    char *ptr = rules,*qtr=NULL,*sep_indx = NULL;
    
    sep_indx = strstr(rules,INTERNET_RULES_SEPERATOR);
    
    if(strstr(rules,INTERNET_TIME_SEPERATOR) != NULL)
    {
        ret++;
    }
        
    
    for(;ptr < sep_indx;)
    { 
        if(((qtr = strstr(ptr,"-")) != NULL) && ((qtr - ptr) >= 1))
        {
            ptr++;
            flagNum++;
            if((ret == 0) && (flagNum == 4))
            {
            	  ret++;
            	  break;
            }
            else if((ret == 1) && (flagNum == 6))
            {
            	  ret++;
            	  break;
            }
        }
        else
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

int hertParse_internetRules(char *interRules,char *mac,int *accCon,int *netLimit,char *timeStart,char *timeEnd)
{
  char *pStart = NULL,*pEnd = NULL;	
  char timeStart2[8],timeEnd2[8];
  int ret = 0,isMultiTime = 0;
  
  if(interRules == NULL)
  {
      ret = -1;
      return ret;
  }
  
  memset(timeStart2,0x0,sizeof(timeStart2));
  memset(timeEnd2,0x0,sizeof(timeEnd2));
  
  
  pStart = interRules; 
  if((pEnd = strstr(pStart,INTERNET_RULES_SEPERATOR)) == NULL)
  {
      ret = -1;
      return ret;
  }
  *(++pEnd) = '\0';
  if((isMultiTime = isValidRules(interRules)) != 0)
  {
    isMultiTime--;
    if(isMultiTime == 1)
    {
        sscanf(pStart, "%17s-%1d-%5s-%5s&%5s-%5s-%1d",mac,accCon,timeStart,timeEnd,timeStart2,timeEnd2,netLimit); 
        strcat(timeStart,",");
        strcat(timeStart,timeStart2);
        strcat(timeEnd,",");
        strcat(timeEnd,timeEnd2);
    }
    else
    {
        sscanf(pStart, "%17s-%1d-%5s-%5s-%1d",mac,accCon,timeStart,timeEnd,netLimit); 
    }
	  ret = 0;
  }
  else
  {
  	ret = -1;
  }
  
  return ret;
}


/*if find and rules is valid return 0*/
int hertUtil_getAccessRulesInNvram(const char *tarMAC, HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY *rulesBody)
{
    int ret = -1,paAccCon = 0,netLimit = 0,timeBodySize = 0;	
    char *ptr = NULL,*pstard1 = NULL,*pend1 = NULL,*pstard2 = NULL,*pend2 = NULL;
    char paTimeStart[32],paTimeEnd[32],paMac[32];
    HERT_WEBS_MSG_TIME_PERIOD_INFO netTime,*tmpTimeBody = NULL;
    const char *nvram_tmp = nvram_get(RT2860_NVRAM, "InternetRules");	
    
    memset(paTimeStart,0x0,sizeof(paTimeStart));
    memset(paTimeEnd,0x0,sizeof(paTimeEnd));
    memset(paMac,0x0,sizeof(paMac));
    
    if((nvram_tmp != NULL) &&((ptr = strstr(nvram_tmp,tarMAC)) != NULL))
    {
        memset(paTimeStart,0x0,sizeof(paTimeStart));
        memset(paTimeEnd,0x0,sizeof(paTimeEnd));
        if(0 != hertParse_internetRules(ptr,paMac,&paAccCon,&netLimit,paTimeStart,paTimeEnd))
        {
            ret = -1;
            return ret;
        }	
             
        timeBodySize = sizeof(netTime.startTime) + sizeof(netTime.endTime);
		    rulesBody->netTime = (HERT_WEBS_MSG_TIME_PERIOD_INFO *)malloc((timeBodySize * MAX_TIME_PERIOD_NUM) + 1);
		    if (!(rulesBody->netTime))
		    {
		    	HERT_LOGERR("No memory!!!!\n");
		    	ret = -1;
		    	return ret;
		    }
		    
		    tmpTimeBody = rulesBody->netTime;
		    if(strstr(paTimeStart,",") && strstr(paTimeEnd,","))
		    {
		    	  /***************get the first start&end time*****************/
		    	  pstard1 = paTimeStart;
		        pend1 = strstr(pstard1,",");
		        *pend1 = '\0';
		        strcpy(tmpTimeBody->startTime,pstard1);	
		        
		    	  pstard2 = paTimeEnd;
		        pend2 = strstr(pstard2,",");
		        *pend2 = '\0';
            strcpy(tmpTimeBody->endTime,pstard2);	
            
            /***************get the second start&end time*****************/
            tmpTimeBody++;
            
            pstard1 = pend1 + 1;
            if((pstard1 != NULL) && (*pstard1 != '\0'))
            {
		            strcpy(tmpTimeBody->startTime,pstard1);	
		        }
		        
            pstard2 = pend2 + 1;
            if((pstard2 != NULL) && (*pstard2 != '\0'))
            {
		            strcpy(tmpTimeBody->endTime,pstard2);	
		        }   
		        rulesBody->periodNum = 2;
            
		    }
		    else
		    {
		        strcpy(tmpTimeBody->startTime,paTimeStart);	
            strcpy(tmpTimeBody->endTime,paTimeEnd);	
        }
        
		    strcpy(rulesBody->devMac,paMac);
		    rulesBody->accessControl = paAccCon;
        rulesBody->netLimit = netLimit;
        
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    
    HERT_LOGINFO("############old rule:%s##############\n", rulesBody);	
    
    return ret;
}

int herUtil_updateInterRules()
{
    FILE *fs = NULL;
    time_t rawtime;
    struct tm * timeinfo;
    char line[128],MACList[50][20],currrule[64],szCmd[128],tmpBuf[32],tmpBuf2[32],tmpBuf3[32];
    char paMac[32],paTimeStart[32],paTimeEnd[32];
    char *pstart = NULL,*psepr = NULL,*pcolon1 = NULL,*pcolon2 = NULL,*cpStart = NULL;
    int ret = 0,i = 0,j = 0,paAccCon = 0,netLimit = 0,isAlreadySet = 0;
    int nowHours = 0,nowMins = 0,dropNum = 0;
    int hourStart1=0,minStart1=0,hourEnd1=0,minEnd1=0,hourStart2=0,minStart2=0,hourEnd2=0,minEnd2=0;
    int nowNum = 0,startNum1 = 0,endNum1 = 0,startNum2 = 0,endNum2 = 0;
    memset(MACList,0x0,sizeof(MACList));
    memset(line,0x0,sizeof(line));
    

    
    
    if(strlen(InternetAccessRules) < 1)
    {
        updateArryRules();
    }

    system("iptables -L FORWARD > /var/iptableList");
    
    for(i = 0;i < 3;i++)
    {
        if((fs = fopen("/var/iptableList", "r")) != NULL)
        {
            break;	
        }
        else
        {
            sleep(1);
        }
    }
    
    if(fs != NULL)
    {
    	 dropNum = 0;
       while ( fgets(line, 128, fs) != NULL ) 
       {
           if((strstr(line,"DROP") != NULL) && ((cpStart = strstr(line,"MAC")) != NULL))
           {
               cpStart = cpStart + strlen("MAC") + 1; //point to MAC Address start
               strncpy(MACList[dropNum],cpStart,MAC_LEN);
               dropNum++;
           }
           if(dropNum >= 50)
           { 	       	
               break;
           }
       }
       fclose(fs);
       
       system("rm /var/iptableList -rf");

       time ( &rawtime );
       timeinfo = localtime ( &rawtime );
	     nowHours = timeinfo->tm_hour;
	     nowMins= timeinfo->tm_min;
	     nowNum = (nowHours*60) + nowMins;
	     
	     HERT_LOGDEBUG("############DEBUG___(nowHours:%d,nowMins:%d,nowNum:%d)##############\n",nowHours,nowMins,nowNum);	
	     
       for(i = 0;i < 32;i++)
       {
           isAlreadySet = 0;
           if(strlen(InternetAccessRules[i]) > 0)
           {
           	   memset(paMac,0x0,sizeof(paMac));
               memset(paTimeStart,0x0,sizeof(paTimeStart));
               memset(paTimeEnd,0x0,sizeof(paTimeEnd));
               memset(currrule,0x0,sizeof(currrule));
               memset(szCmd,0x0,sizeof(szCmd));
               memset(tmpBuf,0x0,sizeof(tmpBuf));
               memset(tmpBuf2,0x0,sizeof(tmpBuf2));
               memset(tmpBuf3,0x0,sizeof(tmpBuf3));
               hourStart1=0;
               minStart1=0;
               hourEnd1=0;
               minEnd1=0;
               hourStart2=0;
               minStart2=0;
               hourEnd2=0;
               minEnd2=0;
               startNum1 = 0;
               endNum1 = 0;
               startNum2 = 0;
               endNum2 = 0;
               
               strcpy(currrule,InternetAccessRules[i]);
               if(0 != hertParse_internetRules(currrule,paMac,&paAccCon,&netLimit,paTimeStart,paTimeEnd))
               {
                   ret = -1;
                   continue;
               }	
               
               /*find if drop rule already set in iptables:isAlreadySet*/
               for(j = 0;j < dropNum;j++)
               {
                   if(strcmp(MACList[j],paMac) == 0)
                   {
                       isAlreadySet = 1;
                       HERT_LOGDEBUG("############paMac:%s already set##############\n", paMac);	
                       break;
                   }
               }
               if(paAccCon == 1)
               {
	               /*****************parse the start time*****************/
	               //for muti time period 
	               pstart = paTimeStart;
	               if((psepr = strstr(pstart,",")) != NULL)
	               {
	                   strncpy(tmpBuf,pstart,(psepr - pstart));
	                   //parse hour and min
	                   pstart = tmpBuf;
	                   if((pcolon1 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon1 - pstart));
	                       hourStart1 = atoi(tmpBuf2);
	                       pcolon1++;
	                       strcpy(tmpBuf3,pcolon1);
	                       minStart1 = atoi(tmpBuf3);
	                   }
	                   
	                   memset(tmpBuf2,0x0,sizeof(tmpBuf2));
	                   memset(tmpBuf3,0x0,sizeof(tmpBuf3));
	                   
	                   pstart = psepr + 1;
	                   if((pcolon2 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon2 - pstart));
	                       hourStart2 = atoi(tmpBuf2);
	                       pcolon2++;
	                       strcpy(tmpBuf3,pcolon2);
	                       minStart2 = atoi(tmpBuf3);
	                   }
	               }
	               else
	               {
	                   if((pcolon1 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon1 - pstart));
	                       hourStart1 = atoi(tmpBuf2);
	                       pcolon1++;
	                       strcpy(tmpBuf3,pcolon1);
	                       minStart1 = atoi(tmpBuf3);
	                   }
	               }
	               
	               memset(tmpBuf,0x0,sizeof(tmpBuf));
	               memset(tmpBuf2,0x0,sizeof(tmpBuf2));
	               memset(tmpBuf3,0x0,sizeof(tmpBuf3));
	               
	               /*****************parse the end time*****************/
	               pstart = paTimeEnd;
	               //for muti time period 
	               if((psepr = strstr(pstart,",")) != NULL)
	               {
	                   strncpy(tmpBuf,pstart,(psepr - pstart));
	                   //parse hour and min
	                   pstart = tmpBuf;
	                   if((pcolon1 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon1 - pstart));
	                       hourEnd1 = atoi(tmpBuf2);
	                       pcolon1++;
	                       strcpy(tmpBuf3,pcolon1);
	                       minEnd1 = atoi(tmpBuf3);
	                   }
	                   
	                   memset(tmpBuf2,0x0,sizeof(tmpBuf2));
	                   memset(tmpBuf3,0x0,sizeof(tmpBuf3));
	                   
	                   pstart = psepr + 1;
	                   if((pcolon2 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon2 - pstart));
	                       hourEnd2 = atoi(tmpBuf2);
	                       pcolon2++;
	                       strcpy(tmpBuf3,pcolon2);
	                       minEnd2 = atoi(tmpBuf3);
	                   }
	               }
	               else
	               {
	                   if((pcolon1 = strstr(pstart,":")) != NULL)
	                   {
	                       strncpy(tmpBuf2,pstart,(pcolon1 - pstart));
	                       hourEnd1 = atoi(tmpBuf2);
	                       pcolon1++;
	                       strcpy(tmpBuf3,pcolon1);
	                       minEnd1 = atoi(tmpBuf3);
	                   }
	               }

               startNum1 = (hourStart1*60) + minStart1;
               endNum1 = (hourEnd1*60) + minEnd1;
               startNum2 = (hourStart2*60) + minStart2;
               endNum2 = (hourEnd2*60) + minEnd2;

               HERT_LOGDEBUG("############startNum1:%d##############\n",startNum1);
               HERT_LOGDEBUG("############endNum1:%d##############\n",endNum1);
               HERT_LOGDEBUG("############startNum2:%d##############\n",startNum2);
               HERT_LOGDEBUG("############endNum2:%d##############\n",endNum2);

               /*****************decide is need to add drop iptable rule*****************/
               if( ((startNum1 <= nowNum) && (endNum1 > nowNum)) || ((startNum2 <= nowNum) && (endNum2 > nowNum)))           
               {
                   if(isAlreadySet == 1)
                   {
                       sprintf(szCmd, "set_iptables.sh deleteDropRule %s",paMac);
                       system(szCmd); 
                   }
               }
               else
               {
                   HERT_LOGDEBUG("############not in free time##############\n");
                   if(isAlreadySet == 1)
                   {
                       /*do nothing as already set rule*/
                   }
                   else
                   {
                       sprintf(szCmd, "set_iptables.sh addDropRule %s",paMac);
                       system(szCmd); 
                   }
               }
             }
             else //not allowed access internet
             {
                   HERT_LOGDEBUG("############is not allowed##############\n");	
                   if(isAlreadySet == 1)
                   {
                       /*do nothing as already set rule*/
                   }
                   else
                   {
                       sprintf(szCmd, "set_iptables.sh addDropRule %s",paMac);
                       system(szCmd); 
                   }    
             }
           }	
           else
           {
               break;	
           }
           memset(currrule,0x0,sizeof(currrule));
       }  
    }
    return 0;
}


void updateArryRules()
{
    char AccessRules[MAX_ACCESS_RULES_SIZE],currRule[64];
    char *pstard = NULL,*pend = NULL,accCon = 0,netLimit = 1;
    int currRuleSize = 0,ruleNum = 0,i = 0,isMultiTime = 0;
    
    memset(AccessRules,0x0,MAX_ACCESS_RULES_SIZE);
    memset(currRule,0x0,sizeof(currRule));
    
    currRuleSize = hertUtil_GetNvram_Values("InternetRules", AccessRules, MAX_ACCESS_RULES_SIZE);
    pstard = AccessRules;
    pend = strstr(AccessRules,INTERNET_RULES_SEPERATOR);
    if(currRuleSize > 0 && (pend != NULL))
    {
        memset(InternetAccessRules,0x0,sizeof(InternetAccessRules));
        ruleNum = countSubString(AccessRules,INTERNET_RULES_SEPERATOR);

        for(i = 0;(i < ruleNum) && (i < 32);i++)
        {
        	  strncpy(currRule,pstard,(pend - pstard + 1));
        	  if((isMultiTime = isValidRules(currRule)) > 0)
        	  {
                strcpy(InternetAccessRules[i],currRule);    
        	  }    
            pstard = pend + 1;
            pend = strstr(pstard,INTERNET_RULES_SEPERATOR);
            memset(currRule,0x0,sizeof(currRule));
        }
    }
    
	
}
                       
int hertUtil_updateAccessRulesInNvram(HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY rulesBody)
{
    char AccessRules[MAX_ACCESS_RULES_SIZE],RulesTmp[MAX_ACCESS_RULES_SIZE],newRule[64];
    char *pStart = NULL,*pEnd = NULL,*pTmp = NULL,*pTmp2 = NULL,oldRule[64],szCmd[128];
    char *pstard1 = NULL,*pend1 = NULL,*pstard2 = NULL,*pend2 = NULL;
    char paTimeStart[32],paTimeEnd[32],paMac[32],paTimeStart2[32],paTimeEnd2[32];
    int ruleNum = 0,ret = 0,currRuleSize = 0,newRuleSize = 0,oldAcc = 0,oldLim = 0;
    HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY newTerminalBody;
    HERT_WEBS_MSG_TIME_PERIOD_INFO netTime,*tmpTimeBody = NULL;
    
    memset(AccessRules,0x0,MAX_ACCESS_RULES_SIZE);
    memset(RulesTmp,0x0,MAX_ACCESS_RULES_SIZE);
    memset(newRule,0x0,sizeof(newRule));
    memset(&newTerminalBody,0x0,sizeof(newTerminalBody));
    memset(oldRule,0x0,sizeof(oldRule));
    memset(paTimeStart,0x0,sizeof(paTimeStart));
    memset(paTimeEnd,0x0,sizeof(paTimeEnd));
    memset(paTimeStart2,0x0,sizeof(paTimeStart2));
    memset(paTimeEnd2,0x0,sizeof(paTimeEnd2));
    memset(paMac,0x0,sizeof(paMac));
    
    currRuleSize = hertUtil_GetNvram_Values("InternetRules", AccessRules, MAX_ACCESS_RULES_SIZE);
    ruleNum = countSubString(AccessRules,INTERNET_RULES_SEPERATOR);
    
    tmpTimeBody = rulesBody.netTime;
    //build new rule format
    if(rulesBody.periodNum == 2)
    {
        strcpy(paTimeStart,tmpTimeBody->startTime);
        strcpy(paTimeEnd,tmpTimeBody->endTime);
        tmpTimeBody++;
        strcpy(paTimeStart2,tmpTimeBody->startTime);
        strcpy(paTimeEnd2,tmpTimeBody->endTime);
        
        snprintf(newRule,sizeof(newRule) - 1,"%s-%d-%s-%s&%s-%s-%d,",rulesBody.devMac,rulesBody.accessControl, \
                       paTimeStart,paTimeEnd,paTimeStart2,paTimeEnd2,rulesBody.netLimit);  
        memset(paTimeStart,0x0,sizeof(paTimeStart));
        memset(paTimeEnd,0x0,sizeof(paTimeEnd));
        memset(paTimeStart2,0x0,sizeof(paTimeStart2));
        memset(paTimeEnd2,0x0,sizeof(paTimeEnd2));
                       
    }
    else
    {
        snprintf(newRule,sizeof(newRule) - 1,"%s-%d-%s-%s-%d,",rulesBody.devMac,rulesBody.accessControl, \
                       tmpTimeBody->startTime,tmpTimeBody->endTime,rulesBody.netLimit);  
    }
    
     
    newRuleSize = strlen(newRule);

    HERT_LOGINFO("############newRule:%s##############\n", newRule);	
    
    /*remove the old record in nvram*/
    pStart = AccessRules;
    if((ruleNum > 0) && ((pTmp = strstr(pStart,rulesBody.devMac)) != NULL))
    {
        if(ruleNum == 1)
        {
            memset(AccessRules,0x0,MAX_ACCESS_RULES_SIZE);
            ruleNum = 0;
            currRuleSize = 0;	
        }
        else 
        {
           pEnd = strstr(pTmp,INTERNET_RULES_SEPERATOR);
           if(pEnd != NULL)
           {
        	     pTmp2 = RulesTmp;
               strncpy(pTmp2,pStart,(pTmp - pStart));
               pEnd++;
               if(*(pEnd) != '\0')
               {
                   pTmp2 += strlen(RulesTmp);
                   strcpy(pTmp2,pEnd);
               }
               memset(AccessRules,0x0,MAX_ACCESS_RULES_SIZE);
               strcpy(AccessRules,RulesTmp);
               memset(RulesTmp,0x0,MAX_ACCESS_RULES_SIZE);
       
               ruleNum --;
               currRuleSize = strlen(AccessRules);
            }
        }
    }
    
    /*if the rules number big than MAX RULES NUM or buffer size is not enough,remove the old one*/
    if((ruleNum >= MAX_ACCESS_RULES_NUMBER) || ((newRuleSize + currRuleSize)> MAX_ACCESS_RULES_SIZE))
    {
        pStart = AccessRules;
        pEnd = AccessRules + (currRuleSize - 1);
        while(((pEnd - pStart + newRuleSize) > MAX_ACCESS_RULES_SIZE) || (ruleNum >= MAX_ACCESS_RULES_NUMBER))
        {
            pTmp = pEnd--;
            for(;*pEnd != ',';pEnd--);
            memset(RulesTmp,0x0,MAX_ACCESS_RULES_SIZE);
            strncpy(RulesTmp,pStart,(pEnd - pStart + 1));
            ruleNum = countSubString(RulesTmp,INTERNET_RULES_SEPERATOR); 

#if 0
            /*delete iptables rules for the old client*/
            pTmp2 = pEnd + 1; //point to old record start
            strncpy(oldRule,pTmp2,(pTmp - pTmp2 + 1) );
            HERT_LOGINFO("############ oldRule:%s##############\n", oldRule);	
            if(hertParse_internetRules(oldRule,paMac,&oldAcc,&oldLim,paTimeStart,paTimeEnd) == 0)
            {
            	
            	  if(oldAcc == 1)
            	  {
                    sprintf(szCmd, "set_iptables.sh deleteDisable %s",paMac);
                    system(szCmd); 
                    memset(szCmd, 0x0, sizeof(szCmd));
                    memset(paTimeStart,0x0,sizeof(paTimeStart));
                    memset(paTimeEnd,0x0,sizeof(paTimeEnd));
                    
		                if(strstr(paTimeStart,",") && strstr(paTimeEnd,","))
		                {
		                /***************delete the first time period rule*****************/
		                    pstard1 = paTimeStart;
		                    pend1 = strstr(pstard1,",");
		                    *pend1 = '\0';
		                    strcpy(paTimeStart2,pstard1);	
		        
		                    pstard2 = paTimeEnd;
		                    pend2 = strstr(pstard2,",");
		                    *pend2 = '\0';
		                    strcpy(paTimeEnd2,pstard2);	

		                    sprintf(szCmd, "set_iptables.sh delete %s %s %s",paMac,paTimeStart2,paTimeEnd2);
		                    system(szCmd); 
		                    memset(szCmd, 0x0, sizeof(szCmd));
		                    memset(paTimeStart2,0x0,sizeof(paTimeStart2));
		                    memset(paTimeEnd2,0x0,sizeof(paTimeEnd2));
            
		                    /***************delete the second time period rule*****************/
                
		                    pstard1 = pend1 + 1;
		                    if((pstard1 != NULL) && (*pstard1 != '\0'))
		                    {
		                        strcpy(paTimeStart2,pstard1);	
		                    }
		        
		                    pstard2 = pend2 + 1;
		                    if((pstard2 != NULL) && (*pstard2 != '\0'))
		                    {
		                        strcpy(paTimeEnd2,pstard2);	
		                    }
		        
		        
		                    if((strlen(paTimeStart2) > 0) && (strlen(paTimeEnd2) > 0))
		                    {
		                        sprintf(szCmd, "set_iptables.sh delete %s %s %s",paMac,paTimeStart2,paTimeEnd2);
		                        system(szCmd); 
		                        memset(szCmd, 0x0, sizeof(szCmd));
		                        memset(paTimeStart2,0x0,sizeof(paTimeStart2));
		                        memset(paTimeEnd2,0x0,sizeof(paTimeEnd2));
		                    }
            
		                }
		                else
		                {
                        sprintf(szCmd, "set_iptables.sh delete %s %s %s",paMac,paTimeStart,paTimeEnd);
                        system(szCmd); 
                        memset(szCmd, 0x0, sizeof(szCmd));
                        memset(paTimeStart,0x0,sizeof(paTimeStart));
                        memset(paTimeEnd,0x0,sizeof(paTimeEnd));
		                }
		            }
		            else
		            {
                    sprintf(szCmd, "set_iptables.sh deleteDisable %s",paMac);
                    system(szCmd); 
                    memset(szCmd, 0x0, sizeof(szCmd));
                    memset(paTimeStart,0x0,sizeof(paTimeStart));
                    memset(paTimeEnd,0x0,sizeof(paTimeEnd));
		            }
                memset(paMac,0x0,sizeof(paMac));
                oldAcc = 0;
                oldLim = 1;
            }
            memset(oldRule,0x0,sizeof(oldRule));
#else
            /*delete iptables rules for the old client*/
            pTmp2 = pEnd + 1; //point to old record start
            strncpy(oldRule,pTmp2,(pTmp - pTmp2 + 1) );
            HERT_LOGINFO("############ oldRule:%s##############\n", oldRule);	
            if(hertParse_internetRules(oldRule,paMac,&oldAcc,&oldLim,paTimeStart,paTimeEnd) == 0)
            {
                //sprintf(szCmd, "set_iptables.sh deleteDropRule %s 1>/dev/null 2>&1",paMac);
                sprintf(szCmd, "set_iptables.sh deleteDropRule %s",paMac);
                system(szCmd); 
            }
            memset(oldRule,0x0,sizeof(oldRule));
            memset(szCmd, 0x0, sizeof(szCmd));
            memset(paTimeStart,0x0,sizeof(paTimeStart));
            memset(paTimeEnd,0x0,sizeof(paTimeEnd));
            memset(paMac,0x0,sizeof(paMac));
#endif
        }
        if(strlen(RulesTmp) > 0)
        {
            memset(AccessRules,0x0,MAX_ACCESS_RULES_SIZE);
            strcpy(AccessRules,RulesTmp);
            memset(RulesTmp,0x0,MAX_ACCESS_RULES_SIZE);  
        }
        
        currRuleSize = strlen(AccessRules);
        
    }
    pTmp = RulesTmp;
    strncpy(pTmp,newRule,newRuleSize);  
    if(currRuleSize > 0)
    {
        pTmp = pTmp + newRuleSize;
        strcat(pTmp,AccessRules);
    }
    nvram_bufset(RT2860_NVRAM, "InternetRules", RulesTmp);
    nvram_commit(RT2860_NVRAM);
    updateArryRules();
    return ret;
}



int hertUtil_updateQOSNvram(char *nvramName,char *currQOSRules,const char *newQOSRule)
{
    int  currRuleSize = 0,newRuleSize = 0,RuleNum = 0;
    char *pStart = NULL,*pEnd = NULL,*pTmp = NULL,*pTmp2 = NULL;
    char currQOSRulesTmp[MAX_QOS_RULES_SIZE],currQOSRulesStr[MAX_QOS_RULES_SIZE];
    char newQOSName[10],IPTVQOSRule[64];
    
    
    memset(currQOSRulesTmp,0x0,MAX_QOS_RULES_SIZE);
    memset(currQOSRulesStr,0x0,MAX_QOS_RULES_SIZE);
    memset(newQOSName,0x0,sizeof(newQOSName));
    memset(IPTVQOSRule,0x0,sizeof(IPTVQOSRule));
    
    currRuleSize = hertUtil_GetNvram_Values(nvramName, currQOSRulesTmp, MAX_QOS_RULES_SIZE);
    newRuleSize = strlen(newQOSRule);

    RuleNum = countSubString(currQOSRulesTmp,INTERNET_QOS_SEPERATOR);
    
    HERT_LOGINFO("############ newQOSRule:%s##############\n",newQOSRule);	
    
    if(strlen(newQOSRule) < 20)
    {
        HERT_LOGERR("############bad newQOSRule:%s##############\n",newQOSRule);
        return 1;
    }
    /*remove the old record in nvram*/
    //build QOS Name
    strncpy(newQOSName,newQOSRule,8);
    pStart = currQOSRulesTmp;
    if((RuleNum > 0) && ((pTmp = strstr(pStart,newQOSName)) != NULL))
    {
        if(RuleNum == 1)
        {
            memset(currQOSRulesTmp,0x0,MAX_QOS_RULES_NUMBER);
            RuleNum = 0;
            currRuleSize = 0;	
        }
        else 
        {
            pTmp2 = currQOSRulesStr;
            strncpy(pTmp2,pStart,(pTmp - pStart));
            pEnd = strstr(pTmp,INTERNET_QOS_SEPERATOR); 
            /*is the last one,no ";" left*/
            if((pEnd != NULL) && (*(++pEnd) != '\0'))
            {   
                    pTmp2 += strlen(pTmp2);
                    strcpy(pTmp2,pEnd);
            }
            memset(currQOSRulesTmp,0x0,MAX_QOS_RULES_NUMBER);
            strcpy(currQOSRulesTmp,currQOSRulesStr);
            memset(currQOSRulesStr,0x0,MAX_QOS_RULES_NUMBER);    
            RuleNum--;
            currRuleSize = strlen(currQOSRulesTmp);
        }
    }

    /*if the rules number big than MAX RULES NUM or buffer size is not enough,remove the old one*/
    if((RuleNum >= MAX_QOS_RULES_NUMBER) || ((newRuleSize + currRuleSize)> MAX_QOS_RULES_SIZE))
    {
    	  //back up IPTV name ,next process maybe remove this rule
        if((pStart = strstr(currQOSRulesTmp,IPTV_QOS_RULENAME_FLAG)) != NULL)
        {
        	  //IPTV QOS rule is not at the last
            if((pEnd = strstr(pStart,INTERNET_QOS_SEPERATOR)) != NULL)
            {
                strncpy(IPTVQOSRule,pStart,(pEnd - pStart));
            }
            else
            {
                strcpy(IPTVQOSRule,pStart);	
            }
        }
        pStart = currQOSRulesTmp;
        pEnd = currQOSRulesTmp + (currRuleSize - 1);
        while(((pEnd - pStart + newRuleSize) > MAX_QOS_RULES_SIZE) || (RuleNum >= MAX_QOS_RULES_NUMBER))
        { 
            for(;*pEnd != ';';pEnd--);
            pEnd--;
            memset(currQOSRulesStr,0x0,MAX_QOS_RULES_SIZE);
            strncpy(currQOSRulesStr,pStart,(pEnd - pStart + 1));
            RuleNum = countSubString(currQOSRulesStr,INTERNET_QOS_SEPERATOR);  
        } 
        if(strlen(currQOSRulesStr) > 0)
        {
            currRuleSize = strlen(currQOSRulesStr);
            pStart = currQOSRulesStr;
            pEnd = currQOSRulesStr + (currRuleSize - 1);
        }
        /*if IPTV QOS rule is removed ,add it again*/
        if((strlen(IPTVQOSRule) > 0) && (strstr(currQOSRulesStr,IPTV_QOS_RULENAME_FLAG) == NULL))
        {
        	  /*remove one old rule first and add IPTV one*/
            for(;*pEnd != ';';pEnd--);
            pEnd--;
            
            memset(currQOSRulesTmp,0x0,MAX_QOS_RULES_SIZE);
            strcpy(currQOSRulesTmp,IPTVQOSRule);
            strncat(currQOSRulesTmp,pStart,(pEnd - pStart + 1));
            
            memset(currQOSRulesStr,0x0,MAX_QOS_RULES_SIZE);
            strcpy(currQOSRulesStr,currQOSRulesTmp);
        }
    }		 
    else
    {
        strcpy(currQOSRulesStr,currQOSRulesTmp);
    }
    currRuleSize = strlen(currQOSRulesStr);
    pTmp = currQOSRules;
    strncpy(pTmp,newQOSRule,newRuleSize);  
    pTmp = pTmp + newRuleSize;
    if(currRuleSize > 0)
    {
        strcat(pTmp,";");
        pTmp++;
    }
    strcat(pTmp,currQOSRulesStr);
    return 0;
}


int hertUtil_updateQOSSetting(const char *tarMac,const int group)
{
    char currQOSUpRules[MAX_QOS_RULES_SIZE],currQOSDwRules[MAX_QOS_RULES_SIZE];
    char IPTVQOSRule[64],newQOSRule[64];
    int  newGroup,ret = 0;
    

    newGroup = 6; /*default*/
    memset(currQOSUpRules,0x0,MAX_QOS_RULES_SIZE);
    memset(currQOSDwRules,0x0,MAX_QOS_RULES_SIZE);
    memset(newQOSRule,0x0,sizeof(newQOSRule));
    memset(IPTVQOSRule,0x0,sizeof(IPTVQOSRule));
    
    if((strlen(tarMac) != MAC_LEN) || ((group < 0) || (group > 2)))
    {
        ret = -1;
        return ret;
    }
    
    
    /*QOS group 2:high-6:default-1:low */
    if(group == 0)
    {
        newGroup = 2;
    }
    else if(group == 1)
    {
    	  newGroup = 6;
    }
    else if(group == 2)
    {
    	  newGroup = 1;
    }
    
    snprintf(newQOSRule,sizeof(newQOSRule) - 1,"APP_%c%c%c%c,%d,1,%s,,,,,,,,,,,,,N/A", 
			 *(tarMac+12), *(tarMac+13), *(tarMac+15), *(tarMac+16), newGroup, tarMac);

	  hertUtil_updateQOSNvram("QoSULRules",currQOSUpRules,newQOSRule);
	  hertUtil_updateQOSNvram("QoSDLRules",currQOSDwRules,newQOSRule);
	  
	  if((strlen(currQOSUpRules) > 0) && (strlen(currQOSDwRules) > 0))
	  {
	      nvram_bufset(RT2860_NVRAM, "QoSULRules", currQOSUpRules);
	      nvram_bufset(RT2860_NVRAM, "QoSDLRules", currQOSDwRules);
	  }
    nvram_commit(RT2860_NVRAM);
    return 0;
}
