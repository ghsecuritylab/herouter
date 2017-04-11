
#ifndef __HE_ROUTE_UTIL_HEADER__
#define __HE_ROUTE_UTIL_HEADER__

#include <stdarg.h>

#ifndef LOGDEBUG
#define LOGDEBUG(fmt)         vpcom_log fmt ;
#endif
#ifndef LOGINFO
#define LOGINFO(fmt)          vpcom_logInfo fmt ;
#endif
#ifndef LOGERROR
#define LOGERROR(fmt)         vpcom_logErr fmt ;
#endif
#ifndef LOG
#define LOG(fmt)              vpcom_log fmt ;
#endif

typedef enum
{
   VPCOM_LOG_OFF     = 0,
   VPCOM_LOG_EMERG   = 1,   
   VPCOM_LOG_CRIT    = 2,
   VPCOM_LOG_ALERT   = 3,
   VPCOM_LOG_ERROR   = 4,
   VPCOM_LOG_WARNING = 5,
   VPCOM_LOG_NOTICE  = 6,
   VPCOM_LOG_INFO    = 7,
   VPCOM_LOG_DEBUG   = 8
}VPCOM_LOGLEVEL;

char *hertUtil_getLanIP(void);
int hertUtil_isStorageExist(void);
int hertUtil_getStorageSize(char *path, long long *freeSize, long long *totalSize);
int hertUtil_IsInFile(const char *szFile, const char *szfindstr);
void hertUtil_setLoglevel();
char *hertUtil_getWanIP();
long hertUtil_getFileSize(const char *filename);

char hertUtil_isWifiOn();

char* hertUtil_getPlatformDstAddr();

char* hertUtil_getPlatformDstAddr();

char* hertUtil_getCnnPrtlData();

char* hertUtil_getFirmwareVersion();

char* hertUtil_getDeviceType();

char* hertUtil_getPlatformSVR();

unsigned short hertUtil_getPlatformSVRPort();

char* hertUtil_getPingSeverAddr();

unsigned short hertUtil_getPingSeverPort();

int hertUtil_readMac(char *buf);

char* hertUtil_getYearmonth();

DWORD hertUtil_getWorkStatus();

char *hertUtil_getBroadbandRate();

char* hertUtil_getWanIfNamePPP();
int heroute_ping_apiWan(char *pszHostName, char *pszSrcIP, int nPingCount, int nTimeOut);
int hertUtil_getWanDnsServerIP(char *pszPrimDnsServer, int nBufSize1, char *pszSecondaryDns, int nBuffSize2);
int hertUtil_getWanDectTime();
int hertUtil_getGoaheadIsRuning();
void hertUtil_setMonitorLoglevel();
extern int flash_read_mac(char *buf);
char* hertUtil_getUserData();

int hertUtil_getGoaheadIsBlock();
int hertUtil_getProftpdIsRuning();
char *hertUtil_getWanConnectionMode();
int hertUtil_getPppdRuning();
int hertUtil_getHerouteAppIsRuning();

int hertUtil_InitSem();
int hertUtil_UnInitSem();
int hertUtil_PostSem();
int hertUtil_WaitSem(int nSecond);
int hertUtil_getDnsdIsRuning();
int hertUtil_createDirIfNoExist(const char *pszDir);

int hertUtil_AddDownReqItemList(IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem);
int hertUtil_DelDownReqItemList(IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem);
int hertUtil_GetDownReqItemList(OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pItem);

/***********************************
** Error logging                   *
***********************************/
int vpcom_Printf(VPCOM_LOGLEVEL level, const char*szFile, int nLine, char *fmt, va_list vList);
void vpcom_log(const char*szFile, int nLine, const char *format,...);
void vpcom_logInfo(const char*szFile, int nLine,  const char *format,...);
void vpcom_logErr(const char*szFile, int nLine, const char *format,...);             
VPCOM_LOGLEVEL vpcom_GetLogLevel();
void vpcom_SetLogLevel(VPCOM_LOGLEVEL level);


#define HERT_LOGINFO(format, ...)   if ( VPCOM_LOG_NOTICE <= vpcom_GetLogLevel() ) \
                                         { LOGINFO((__FUNCTION__, __LINE__,  format, ##__VA_ARGS__)); }

#define HERT_LOGERR(format, ...)    if ( VPCOM_LOG_ERROR <= vpcom_GetLogLevel() ) \
                                         { LOGERROR((__FUNCTION__, __LINE__, format, ##__VA_ARGS__)); }

#define HERT_LOGDEBUG(format, ...)  if ( VPCOM_LOG_DEBUG <= vpcom_GetLogLevel() ) \
                                         { LOGDEBUG(( __FUNCTION__, __LINE__, format, ##__VA_ARGS__)); }
 
 
char* vpComGetMsgTypeText(char type);

void Print_Msg_Log(CHAR *ptMsg, DWORD dwLen);

int hertUtil_getDnsmasqIsRuning();

#endif /* __HE_ROUTE_UTIL_HEADER__ */

