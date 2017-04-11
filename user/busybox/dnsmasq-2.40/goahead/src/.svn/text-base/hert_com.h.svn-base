
#ifndef __HE_ROUTE_COM_HEADER__
#define __HE_ROUTE_COM_HEADER__

#ifndef STR
#define STR(a) #a
#endif

#ifndef CHAR
#define CHAR char
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef DDWORD
#define DDWORD long long
#endif

#ifndef SINT32
#define SINT32 signed long
#endif

#define IS_EMPTY_STR(a) ((!a) || ((a) && (*(a) == 0x0) ))

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define REMOVE_CRLN_LAST(data) \
        if( (strlen(data) >= 1) && ((*(data + strlen(data) - 1) == 0x0a) || \
            (*(data + strlen(data) - 1) == 0x0d)) ) \
        { \
            *(data + strlen(data) - 1) = 0x0; \
        } \


#define REMOVE_CRLN(data) \
        REMOVE_CRLN_LAST(data) \
        REMOVE_CRLN_LAST(data)

//===========================================================
//
// define wireless part start
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

#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1F)


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


typedef struct tagDevInfo
{
    unsigned long curtime;
    char szProduct[64];
    char szProdType[64];
    char szProdManu[64];
    char szProdSerial[64];
    char szMac[64];
}DEVINFO;

/* HERT_COM_SUBDEV_BODY */
typedef struct tagHertComsubDevBody
{
    CHAR    devName[128];               /* 设备名称 */
    CHAR    devType[32];                /* 设备类型 */
    CHAR    devID[64];                  /* 标识号 */
    CHAR    mac[32];                    /* Mac */
    CHAR    connectTime[16];            /* 连接时长, 单位分钟 */
}HERT_COM_SUBDEV_BODY;



////////////////////////////////////////////////////////////////
//
// dhcp client parts start
//
////////////////////////////////////////////////////////////////
#define CHECK_EMPTY_MAC(macarray) \
	if ((macarray[0]==0) && (macarray[1]==0) && (macarray[2]==0) && \
		(macarray[3]==0) && (macarray[4]==0) && (macarray[5]==0)) \
	{ \
		continue; \
	}
	
#define DUMP_STR(str) \
    if (hertUtil_IsInFile("/var/hertdumpstr","DEBUG")) \
    { \
        herUtil_dumpHexString(__FUNCTION__, __LINE__, str); \
    }


typedef struct tagFiledhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip;
        unsigned long expires;
} FILEDHCPITEM;

typedef struct tagdhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip;
        unsigned long expires;
        unsigned int  nReachable;
} DHCPITEM;

#define MAX_ITEMS_NUMBER 16

////////////////////////////////////////////////////////////////
//
// dhcp client parts end
//
////////////////////////////////////////////////////////////////

void herUtil_dumpHexString(const char *pszFunction, int nLine, char *pszData);
char hertUtil_isWifiOn();
char *hertUtil_getSN();
int base64encoder(unsigned char *pData, unsigned int nlength, char * outstr, 
			 const unsigned int max_str_out);
int base64decoder (char * eapstr, unsigned char *pData, unsigned int *nlength);
int hertUtil_isStorageExist(void);
int hertUtil_getStorageSize(char *path, long long *freeSize, long long *totalSize) ;
int hertUtil_IsInFile(const char *szFile, const char *szfindstr);
long hertUtil_getFileSize(const char *filename);
void herUtil_dumpAesHexString(char *pszFunction, int nLine, char *pszData, int nDataLen);
char* hertUtil_getFirmwareVersion();
int hertUtil_IsEnableWritePath(char *pszPath);
int hertUtil_GetUSBPath(char *outPathName);
int hretUtil_GetUSBSize(long long *freeSize, long long *totalSize);
int hertUtil_GetUSBPartitionMaxFreeSize(long long *freeSize, char *pszOutPathName, int nPathNameSize);
char *hertUtil_getLanIP(void);
char* hertUtil_getDeviceType();
char* hertUtil_getYearmonth();
int hertUtil_readMac(char *buf);
char *hertUtil_getWanInterface();
DWORD hertUtil_getSeconds();
void hertUtil_toUpper(char *pszStr);
void hertUtil_tolower(char * pszStr);
char *hertUtil_getBroadbandRate();
DWORD hertUtil_getBroadband();
DWORD hertUtil_getWorkStatus();
int hertUtil_getHerouteUpImg();
int hertUtil_getIsNeedUpgrade();
int hertUtil_getWanDectTime();
int hertUtil_getLanDectTime();
int hertUtil_UpdateWirelessClientInfo();
int hertUtil_UpdateWirelessClientItem(int nIndex, DEVINFO *pDevInfo);
int hertUtil_IsWirelessClientMac(unsigned char *pszMac);
int hertUtil_SaveWirelessClientMac(unsigned char *pszMac);
int hertUtil_IsInSaveWirelessClientMac(unsigned char *pszMac);
int  hertUtil_UpdateDevInfo();
int  hertUtil_GetDevInfo(char *pszMac, DEVINFO *pDevInfo);
int hertUtil_GetDhcpOption60ClientItem(int nIndex, DEVINFO *pDevInfo);
int  hertUtil_UpdateIpReachInfo(int bDoPing);
int  hertUtil_GetIpReachInfo(char *pszMac, DEVINFO *pDevInfo);
int hertUtil_GetDhcpClientItem(int nIndex, DEVINFO *pDevInfo);
int hertUtil_GetDhcpClientLeaseItem(int nIndex, void *pDhcpItem);
char*  hertUtil_GetHostName(char *pszMac);
int  hertUtil_GetIsInDevList(char *pszMac, HERT_COM_SUBDEV_BODY *pDevList, int nNum);
int hertUtil_IsAssicString(char *pszString);
char* hertUtil_GetDateTime();
int hertUtil_SetDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin);
int hertUtil_GetDeviceList(HERT_COM_SUBDEV_BODY **ppDev, DWORD *pdwDevNumber);
int hertUtil_base64_encode(const unsigned char *in,  unsigned long inlen, 
                        unsigned char *out, unsigned long *outlen);
time_t getUptime();
int hertUtil_InitSem();
int hertUtil_UnInitSem();
int hertUtil_PostSem();
int hertUtil_WaitSem(int nSecond);

void hertUtil_SetAESPwdFromOnenet(const char *szAes);
char* hertUtil_GetAESPwdFromOnenet();

int hertUtil_IsExsitPath(char *pszPath);

#endif /* __HE_ROUTE_COM_HEADER__ */

