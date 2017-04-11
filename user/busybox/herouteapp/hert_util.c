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
#include <sys/statvfs.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/vfs.h> 
#include <sys/statfs.h>
#include <sys/time.h>
#include "linux/autoconf.h"
#include <linux/wireless.h>
#include "hert_com.h"
#include "hert_app.h"
#include "hert_msg.h"
#include "hert_util.h"
#include "nvram.h"

#ifdef SUPPORT_SEM
#include <semaphore.h>

static char SEM_TASK_HEROUTE_PLATFORM[]= "heroute_platform_task";
static char SEM_TASK_HEROUTE_APP[]= "heroute_app_task";
static sem_t *g_sem_want_read = NULL;
static sem_t *g_sem_finish_read = NULL;
#endif

#ifdef CONFIG_DUAL_IMAGE

#define UBOOT_NVRAM	0
#define RT2860_NVRAM    1
#define RTDEV_NVRAM    	2
#define CERT_NVRAM    	3
#define WAPI_NVRAM    	4
#else
#define RT2860_NVRAM    0
#define RTDEV_NVRAM    	1
#define CERT_NVRAM    	2
#define WAPI_NVRAM    	3
#endif

#define MACRO_TXT(id) #id
#define MAP_MACRO_TXT(id) {id, MACRO_TXT(id)}

typedef struct
{
    char  mapId;
    char* mapTxt;
}COM_MAP_TXT;
#if 0
COM_MAP_TXT g_MsgTypeTxtMaps[] = 
{
    MAP_MACRO_TXT(CONN_REQ),
    MAP_MACRO_TXT(CONN_RESP),
    MAP_MACRO_TXT(PUSH_DATA),
    MAP_MACRO_TXT(PING_REQ),
    MAP_MACRO_TXT(PING_RESP),
};
#else
COM_MAP_TXT g_MsgTypeTxtMaps[] = 
{
    {CONN_REQ,  MACRO_TXT(CONN_REQ)},
    {CONN_RESP, MACRO_TXT(CONN_RESP)},
    {PUSH_DATA, MACRO_TXT(PUSH_DATA)},
    {PING_REQ,  MACRO_TXT(PING_REQ)},
    {PING_RESP, MACRO_TXT(PING_RESP)},
};
#endif


/*
*****************************************************************************
** FUNCTION:   vpComGetMsgTypeText
**
** PURPOSE:    This function is the entry of cms process.
**
** PARAMETERS: msgHdr - message header pointer
**
** RETURNS:    None
**
** NOTE:
*****************************************************************************
*/
char* vpComGetMsgTypeText(char type)
{
    unsigned int   nMax = (char)sizeof(g_MsgTypeTxtMaps)/sizeof(COM_MAP_TXT);
    unsigned int   i = 0;

    for(i = 0; i < nMax; i++)
    {
        if (g_MsgTypeTxtMaps[i].mapId == type)
        {
            return g_MsgTypeTxtMaps[i].mapTxt;
        }
    }

    return "Unknow message type";
}


#define MAX_LINE_SIZE 128

#define VPMSG_LOGOUT(szBuf, nPos ) \
                HERT_LOGINFO(szBuf);                  \
                nPos = 0;                             \
                memset(szBuf, 0x0, MAX_BUFF_SIZE );   \
                nPos += sprintf(szBuf + nPos, "\n");

#define VPMSG_PRINTF(szBuf, nPos, pbFormat, ... ) \
                if ( nPos + MAX_LINE_SIZE >= MAX_BUFF_SIZE ){                           \
                    HERT_LOGINFO(szBuf);                                                \
                    nPos = 0;                                                           \
                    memset(szBuf, 0x0, MAX_BUFF_SIZE );                                 \
                    nPos += sprintf(szBuf + nPos, "\n");                                \
                }                                                                       \
                nPos += snprintf(szBuf + nPos, MAX_BUFF_SIZE - nPos, pbFormat, ##__VA_ARGS__);  \

char g_logTitle[255];
static int g_logDestMode = -1; /* 0: console, 1: telnet */
static int g_LogTelnetFd = -1;

VPCOM_LOGLEVEL g_logLevel = VPCOM_LOG_ERROR;

char *g_moduleTitle[] = { "vpcom", "vpcfg", "vpsip", "vpdrc" };
char *g_loglevelTxt[] = { "LOGOFF", "EMERG", "CRIT", "ALERT", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

int vpcom_LogInit(int nDestLog)
{
   g_logDestMode = nDestLog;
   return 0;
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

   sprintf(strModule, "%s", "HERT_LOG");
   
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

VPCOM_LOGLEVEL hertUtil_getLoglevel()
{
    FILE *file;
    char loglevel[64];
    char cmd[128];
    const char *tempFileName = "/var/tempPlatloglevel.txt";

    memset(loglevel, 0x0, sizeof(loglevel));
    sprintf(cmd,"nvram_get HE_ROUTE_PLAT_LOGLEVEL > %s", tempFileName);
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
        return VPCOM_LOG_INFO;
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

VPCOM_LOGLEVEL hertUtil_getMonitorLoglevel()
{
    FILE *file;
    char loglevel[64];
    char cmd[128];
    const char *tempFileName = "/var/tempMonloglevel.txt";

    memset(loglevel, 0x0, sizeof(loglevel));
    sprintf(cmd,"nvram_get HE_ROUTE_MONITOR_LOGLEVEL > %s", tempFileName);
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
        return VPCOM_LOG_INFO;
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

/*
#define IFNAMSIZ     16
struct ifreq 
{
    char    ifr_name[IFNAMSIZ];  // interface name, e.g., "eth0"
    union 
    {
        struct  sockaddr ifru_addr;
        struct  sockaddr ifru_dstaddr;
        struct  sockaddr ifru_broadaddr;
        short   ifru_flags;
        int     ifru_metric;
        caddr_t ifru_data;
    } ifr_ifru;
};
*/

char *hertUtil_getWanIP()
{
    static char buf[64];
    struct ifreq ifr;
    int fd;
    unsigned long ip;
    struct in_addr tmp_addr;

    strcpy(ifr.ifr_name, hertUtil_getWanInterface());
    memset(buf, 0x0, sizeof(buf));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl(fd, SIOCGIFADDR, &ifr))
    {
        HERT_LOGDEBUG("ioctl error ifr.ifr_name(%s)", ifr.ifr_name);
        close(fd);
        return buf;
    }
    close(fd);
    memcpy(&ip,ifr.ifr_addr.sa_data + 2,4);
    tmp_addr.s_addr=ip;
    HERT_LOGDEBUG("__ %s : %s\n", ifr.ifr_name, inet_ntoa(tmp_addr));
    sprintf(buf, "%s",inet_ntoa(tmp_addr));
    return buf;
}


void hertUtil_setLoglevel()
{
    VPCOM_LOGLEVEL nloglevel = hertUtil_getLoglevel();
    vpcom_SetLogLevel(nloglevel);
}

void hertUtil_setMonitorLoglevel()
{
    VPCOM_LOGLEVEL nloglevel = hertUtil_getMonitorLoglevel();
    vpcom_SetLogLevel(nloglevel);
}

char* hertUtil_getUserData()
{
    FILE *file;
    static char UserData[64];
    char cmd[128];
    const char *tempFileName = "/var/tempuserdata.txt";

    memset(UserData, 0x0, sizeof(UserData));
    sprintf(cmd,"nvram_get HE_ROUTE_UserData > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return UserData;
    }   
    if (fgets(UserData, 64, file) && (strlen(UserData) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(UserData);
    }    
    fclose(file);
    unlink(tempFileName);

    return UserData;
}

char* hertUtil_getPlatformDstAddr()
{
    FILE *file;
    static char PlatformDstAddr[64];
    char cmd[128];
    const char *tempFileName = "/var/tempplatformdstAddr.txt";

    memset(PlatformDstAddr, 0x0, sizeof(PlatformDstAddr));
    sprintf(cmd,"nvram_get HE_ROUTE_PlatformDstAddr > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return PlatformDstAddr;
    }   
    if (fgets(PlatformDstAddr, 64, file) && (strlen(PlatformDstAddr) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(PlatformDstAddr);
    }    
    fclose(file);
    unlink(tempFileName);

    return PlatformDstAddr;
}

char* hertUtil_getCnnPrtlData()
{
    FILE *file;
    static char CnnPrtlData[64];
    char cmd[128];
    const char *tempFileName = "/var/tempcnnprtldata.txt";

    memset(CnnPrtlData, 0x0, sizeof(CnnPrtlData));
    sprintf(cmd,"nvram_get HE_ROUTE_CnnPrtlData > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return CnnPrtlData;
    }   
    if (fgets(CnnPrtlData, 64, file) && (strlen(CnnPrtlData) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(CnnPrtlData);
    }    
    fclose(file);
    unlink(tempFileName);

    return CnnPrtlData;
}

char* hertUtil_getPlatformSVR()
{
    FILE *file;
    static char platformSVR[64];
    char cmd[128];
    const char *tempFileName = "/var/tempplatformSVR.txt";

    memset(platformSVR, 0x0, sizeof(platformSVR));
    sprintf(cmd,"nvram_get HE_ROUTE_SVRIPADDR > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return platformSVR;
    }   
    if (fgets(platformSVR, 64, file) && (strlen(platformSVR) >= 2) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(platformSVR);
    }    
    fclose(file);
    unlink(tempFileName);
    return platformSVR;
}

unsigned short hertUtil_getPlatformSVRPort()
{
    FILE *file;
    static char platformSVRPort[64];
    char cmd[128];
    const char *tempFileName = "/var/tempplatformSVRPort.txt";

    memset(platformSVRPort, 0x0, sizeof(platformSVRPort));
    sprintf(cmd,"nvram_get HE_ROUTE_SVRPORT > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return atoi(platformSVRPort);
    }   
    if (fgets(platformSVRPort, 64, file) && (strlen(platformSVRPort) >= 2) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(platformSVRPort);
    }    
    fclose(file);
    unlink(tempFileName);
    return atoi(platformSVRPort);
}

char* hertUtil_getPingSeverAddr()
{
    FILE *file;
    static char pingSVR[64];
    char cmd[128];
    const char *tempFileName = "/var/tempPingSVRAddr.txt";

    memset(pingSVR, 0x0, sizeof(pingSVR));
    sprintf(cmd,"nvram_get HE_ROUTE_PINGSERVERADDR > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        strcpy(pingSVR, "www.baidu.com");
        return pingSVR;
    }   
    if (fgets(pingSVR, 64, file) && (strlen(pingSVR) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(pingSVR);
    }
    if (IS_EMPTY_STR(pingSVR)) 
    {
        strcpy(pingSVR, "www.baidu.com");
    }
    fclose(file);
    unlink(tempFileName);
    return pingSVR;
}

unsigned short hertUtil_getPingSeverPort()
{
    FILE *file;
    static char pingSVRPort[64];
    char cmd[128];
    const char *tempFileName = "/var/tempPingSVRPort.txt";

    memset(pingSVRPort, 0x0, sizeof(pingSVRPort));
    sprintf(cmd,"nvram_get HE_ROUTE_PINGSERVERPORT > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 80; /* default is http port */
    }   
    if (fgets(pingSVRPort, 64, file) && (strlen(pingSVRPort) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(pingSVRPort);
    }    
    if (IS_EMPTY_STR(pingSVRPort)) 
    {
        strcpy(pingSVRPort, "80");
    }
    fclose(file);
    unlink(tempFileName);
    return atoi(pingSVRPort);
}

int hertUtil_getGoaheadIsRuning()
{
    FILE *file;
    char buffer[128];
    char cmd[128];
    const char *tempFileName = "/var/tempgoaheadruning.txt";
    int bFind = 0;
    int nLineCount = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep goahead > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }   

    while (fgets(buffer, 128, file) && (nLineCount <= 10) ) 
    {
        //find the chain
        if(strstr(buffer,"goahead") && (!strstr(buffer,"grep goahead")))
        {
            bFind = 1;
            break;
        }
        nLineCount++;
    }
    if(nLineCount > 10)
    {
        HERT_LOGERR("Much lines for file: nLineCount(%d)\n",nLineCount);
    }
    fclose(file);
    HERT_LOGDEBUG(" Goahead Run(%d)\n", bFind);

    return bFind;
}

void hertUtil_Dump(HERT_MSG_RSP_PUSH_DATA_VARPART_STATUS *pstatusData)
{
    HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO *pDev = NULL;
    int i = 0;

    pDev = pstatusData->pDevList;

    for(i = 0; i < pstatusData->devNum; i++)
    {
        HERT_LOGINFO("%s %s %s %s %s", 
            pDev->devName, pDev->devType, pDev->devID, pDev->mac, pDev->connectTime);
        pDev++;
    }
}
int hertUtil_getProftpdIsRuning()
{
    FILE *file;
    char buffer[256];
    char cmd[128];
    const char *tempFileName = "/var/tmpproftpdruning.txt";
    int bFind = 0;
    int nLineCount = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep proftpd> %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }

    while (fgets(buffer, 256, file) && (nLineCount<= 10)) 
    {
        //find the chain
       // if(strstr(buffer,"S    proftpd")&&strstr(buffer,"accepting connections"))
        if(strstr(buffer,"S    proftpd"))
        {
            bFind = 1;
            break;
        }         
        nLineCount++;
    }  
    if(nLineCount > 10)
    {
        HERT_LOGERR("Much lines for file: nLineCount(%d)\n",nLineCount);
    }
    fclose(file);

    HERT_LOGDEBUG(" proftpd  Run(%d)\n", bFind);
    return bFind;
}
int hertUtil_getPppdRuning()
{
    FILE *file;
    char buffer[256];
    char cmd[128];
    const char *tempFileName = "/var/tmpPppdruning.txt";
    int bFind = 0;
    int nLineCount = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep pppd> %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }

    while (fgets(buffer, 256, file) && (nLineCount<= 10)) 
    {
        //find the chain
       // if(strstr(buffer,"S    proftpd")&&strstr(buffer,"accepting connections"))
        if(strstr(buffer,"S    pppd"))
        {
            bFind = 1;
            break;
        }         
        nLineCount++;
    }  
    if(nLineCount > 10)
    {
        HERT_LOGERR("Much lines for file: nLineCount(%d)\n",nLineCount);
    }
    fclose(file);

    HERT_LOGDEBUG(" pppd  Run(%d)\n", bFind);
    return bFind;
}
int hertUtil_getDnsdIsRuning()
{
    FILE *file;
    char buffer[128];
    char cmd[128];
    const char *tempFileName = "/var/tempdnsdruning.txt";
    int bFind = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep dnsd > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }

    while (fgets(buffer, 128, file)) 
    {
        //find the chain
        if(strstr(buffer,"dnsd -i"))
        {
            bFind = 1;
        }         
    }  
    fclose(file);
    unlink(tempFileName);

    return bFind;
}

int hertUtil_getDnsmasqIsRuning()
{
    FILE *file;
    char buffer[256];
    char cmd[128];
    const char *tempFileName = "/var/tempdnsmasqruning.txt";
    int bFind = 0;
    int nLineCount = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep dnsmasq > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }

    while (fgets(buffer, 256, file) && (nLineCount<= 10)) 
    {
        //find the chain
        if(strstr(buffer,"dnsmasq") && (!strstr(buffer,"grep dnsmasq")))
        {
            bFind = 1;
            break;
        }
        nLineCount++;
    }  
    if(nLineCount > 10)
    {
        HERT_LOGERR("Much lines for file: nLineCount(%d)\n",nLineCount);
    }
    fclose(file);

    HERT_LOGDEBUG(" Dnsmasq Run(%d)\n", bFind);
    return bFind;
}

int hertUtil_getHerouteAppIsRuning()
{
    FILE *file;
    char buffer[256];
    char cmd[128];
    const char *tempFileName = "/var/tempherouteappruning.txt";
    int bFind = 0;
    int nLineCount = 0;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"ps | grep herouteapp > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 1; /* default is running  */
    }

    while (fgets(buffer, 256, file) && (nLineCount<= 10)) 
    {
        //find the chain
        if(strstr(buffer,"S    herouteapp"))
        {
            bFind = 1;
            break;
        }         
        nLineCount++;
    }  
    if(nLineCount > 10)
    {
        HERT_LOGERR("Much lines for file: nLineCount(%d)\n",nLineCount);
    }
    fclose(file);

    HERT_LOGDEBUG(" HerouteApp Run(%d)\n", bFind);
    return bFind;
}

char *hertUtil_getWanConnectionMode()
{
    static char szWanConnectionMode[64];

    const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");

    memset(szWanConnectionMode, 0x0, sizeof(szWanConnectionMode));
    strcpy(szWanConnectionMode, cm);
    return szWanConnectionMode;
}
/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 * */
int hertUtil_getDnsServerViaConf(char *pszPrimDnsServer, int nBufSize1, char *pszSecondaryDns, int nBuffSize2)
{
    FILE *fp;
    char line[200] , *p;
    char szResolvFile[128] = { 0x0 };

    sprintf(szResolvFile, "/etc/resolv.conf");
    
    if((fp = fopen(szResolvFile , "r")) == NULL)
    {
        HERT_LOGERR("Failed get_dns_servers \n");
        return -1;
    }

    memset(pszPrimDnsServer, 0x0, nBufSize1);   
    memset(pszSecondaryDns, 0x0, nBuffSize2);   
    while(fgets(line , 200 , fp))
    {
        if(line[0] == '#')
        {
            continue;
        }
        if(strncmp(line, "nameserver" , 10) == 0)
        {
            p = strtok(line , " ");
            p = strtok(NULL , " ");
            if (*pszPrimDnsServer == 0x0 )
            {
                strcpy(pszPrimDnsServer, p);
            }
            else if (*pszSecondaryDns == 0x0 )
            {
                strcpy(pszSecondaryDns, p);
            }
        }
    }
    fclose(fp);
    REMOVE_CRLN(pszPrimDnsServer);
    REMOVE_CRLN(pszSecondaryDns);
	
    HERT_LOGDEBUG("pszPrimDnsServer=%s, pszSecondaryDns=%s \n", pszPrimDnsServer, pszSecondaryDns);
    return 0;
}
 
int hertUtil_getWanDnsServerIP(char *pszPrimDnsServer, int nBufSize1, char *pszSecondaryDns, int nBuffSize2)
{
    char *pszConnMode = NULL;
    char *pszVal = NULL;
    if((!pszPrimDnsServer) || (!pszSecondaryDns))
    {
        return -1;
    }
    memset(pszPrimDnsServer, 0x0, nBufSize1);
    memset(pszSecondaryDns, 0x0, nBuffSize2);
    pszConnMode = hertUtil_getWanConnectionMode();
    HERT_LOGDEBUG("pszConnMode(%s)\n", pszConnMode);
    if (!strcmp(pszConnMode, "STATIC"))
    {
        pszVal = (char *)nvram_bufget(RT2860_NVRAM, "wan_primary_dns");
        if(pszVal)
        {
            sprintf(pszPrimDnsServer, "%s", pszVal);
        }

        pszVal = (char *)nvram_bufget(RT2860_NVRAM, "wan_secondary_dns");
        if(pszVal)
        {
            sprintf(pszSecondaryDns, "%s", pszVal);
        }
        REMOVE_CRLN(pszPrimDnsServer);
        REMOVE_CRLN(pszSecondaryDns);
    }
    else
    {
        return hertUtil_getDnsServerViaConf(pszPrimDnsServer, nBufSize1, pszSecondaryDns, nBuffSize2);
    }
	return 0;
}

int hertUtil_createDirIfNoExist(const char *pszDir)
{
    char szCmd[128];
    char szFileName[128];
    FILE *fp;

    /* make dir anyway */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "mkdir %s", pszDir);
    system(szCmd);

    memset(szFileName, 0x0, sizeof(szFileName));
    sprintf(szFileName, "%s/test.txt", pszDir);

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "echo test > %s", szFileName);
    system(szCmd);

	
    if((fp = fopen(szFileName , "r")))
    {
        fclose(fp);
    }
    else
    {

        HERT_LOGERR("Failed to create dir(%s) \n", pszDir);
        return -1;
    }
    unlink(szFileName);

    return 0;
}

#define FILE_DOWNREQ_LIST "/var/platDownReqList.txt"
#define IS_SAME_STR(a,b) (a && b && (0 == strcmp(a,b)))


static int hertUtil_EditDownReqItemList(IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem, int bAddorDel)
{
    HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD tTemp;
    FILE *fd = NULL;
    char line[512];
    char szCmd[1024];
    char szTempFile[128];
    static int  nRet = 1;

    if (!pItem)
    {
        HERT_LOGINFO("Invalid parameters(%p)", pItem);
        return 3;
    }

    if (2 == nRet)
    {
        HERT_LOGINFO("Still process, the caller must wait some seconds....");
        return nRet;
    }
    nRet = 2;

    /* set temp file name */
    memset(szTempFile, 0x0, sizeof(szTempFile));
    sprintf(szTempFile, "%s_temp", FILE_DOWNREQ_LIST);

    /* clear temp file */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm -rf %s", szTempFile);
    system(szCmd);

    /* copy list file to temp file */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "cp %s %s", FILE_DOWNREQ_LIST, szTempFile);
    system(szCmd);

    /* clear list file */
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm -rf %s", FILE_DOWNREQ_LIST);
    system(szCmd);
	
    HERT_LOGINFO("%s\n%s\n%s\n%d", pItem->contentID, pItem->contentName, pItem->URL, bAddorDel);


    /* backup saved list items */
    fd = fopen(szTempFile, "r");
    if (fd != NULL)
    {
        while (fgets(line, sizeof(line), fd))
        {
            memset(&tTemp, 0x0, sizeof(HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD));
            sscanf(line, "%s %s %s", tTemp.contentID, tTemp.contentName, tTemp.URL);
            REMOVE_CRLN(tTemp.contentID);
            REMOVE_CRLN(tTemp.contentName);
            REMOVE_CRLN(tTemp.URL);
            HERT_LOGINFO("%s\n%s\n%s", tTemp.contentID, tTemp.contentName, tTemp.URL);
            if (IS_SAME_STR(pItem->contentID, tTemp.contentID) && 
                IS_SAME_STR(pItem->contentName, tTemp.contentName) &&
                IS_SAME_STR(pItem->URL, tTemp.URL))
            {
                HERT_LOGINFO("It is same item(%s\n%s\n%s)", tTemp.contentID, tTemp.contentName, tTemp.URL);
                continue;
            }
            memset(szCmd, 0x0, sizeof(szCmd));
            sprintf(szCmd, "echo \"%s %s %s\" >> %s", tTemp.contentID, tTemp.contentName, tTemp.URL, FILE_DOWNREQ_LIST);
            HERT_LOGINFO("Save item cmd: %s", szCmd);
            system(szCmd);
        }

        fclose(fd);
    }
	
    /* add new item */
    if (bAddorDel)
    {
        memset(szCmd, 0x0, sizeof(szCmd));
        sprintf(szCmd, "echo \"%s %s %s\" >> %s", pItem->contentID, pItem->contentName, pItem->URL, FILE_DOWNREQ_LIST);
        HERT_LOGINFO("Save item cmd: %s", szCmd);
        system(szCmd);
    }
    nRet = 0;
    return nRet;
}

int hertUtil_AddDownReqItemList(IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem)
{
    int ret = 0;

    while(2 == (ret=hertUtil_EditDownReqItemList(pItem, 1)))
    {
        sleep(1);
    }
    return ret;
}

int hertUtil_DelDownReqItemList(IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem)
{
    int ret = 0;

    while(2 == (ret=hertUtil_EditDownReqItemList(pItem, 0)))
    {
        sleep(1);
    }
    return ret;
}


int hertUtil_GetDownReqItemList(OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem)
{
    FILE *fd = NULL;
    char line[512];
    int  nRet = 1;

    fd = fopen("/var/platDownReqList.txt", "r");

    if (fd != NULL)
    {
        while (fgets(line, sizeof(line), fd))
        {
            memset(pItem, 0x0, sizeof(HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD));
            sscanf(line, "%s %s %s", pItem->contentID, pItem->contentName, pItem->URL);
            REMOVE_CRLN(pItem->contentID);
            REMOVE_CRLN(pItem->contentName);
            REMOVE_CRLN(pItem->URL);
            HERT_LOGINFO("%s\n%s\n%s\n", pItem->contentID, pItem->contentName, pItem->URL);
            if (IS_EMPTY_STR(pItem->contentID) || IS_EMPTY_STR(pItem->contentName) || IS_EMPTY_STR(pItem->URL))
            {
                continue;
            }
            nRet = 0;
            break;
        }

        fclose(fd);
    }

    return nRet;
}


int hertUtil_getGoaheadIsBlock()
{
    FILE *file;
    char buffer[128];
    char cmd[128];
    char sharebuf[4] = {0};
    const char *tempFileName = "/var/tempgoaheadblocking.txt";
    int isBlock = 0;
    int nLineCount = 0;
    int tensShareNum = 0;
    char *tmpptr = NULL;

    memset(buffer, 0x0, sizeof(buffer));
    sprintf(cmd,"top -n 1 |grep goahead > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 0; /* default is no blocking  */
    }   

    while(fgets(buffer, 128, file) && (nLineCount <= 2) ) 
    {
        //find the chain
        if((tmpptr = strstr(buffer,"% goahead")) != NULL)
        {
            memset(sharebuf,0x0,sizeof(sharebuf));   
            tmpptr = tmpptr - 2;  
            //care the tens number only
            strncpy(sharebuf,tmpptr,1);  
            if(strcmp(sharebuf," ") == 0)
            {
                continue;	
            }
            else
            {
                tensShareNum = atoi(sharebuf);
                //we think goahead is block when the cpu share more than 40% 
                if((tensShareNum == 0) || (tensShareNum >= 4))	
                {   
                    isBlock = 1;
                    break;	
                }	
            }        
        }
        nLineCount++;
    }
    
    fclose(file);

    HERT_LOGDEBUG(" Goahead isBlocking(%d)\n", isBlock);

    return isBlock;
}

