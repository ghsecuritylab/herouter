#ifndef __HEROUTER_HEADER_WEBS__
#define __HEROUTER_HEADER_WEBS__



#define JSON_BUFFER_SIZE 1024*10    /*keep the same with system*/
#define MAX_IMG_SIZE 1024*1024*6  /*max size is 6M*/

#define UPLOAD_IMG_TAG   "/herouter/AppRequest/UploadIMG/"
#define DOWNLOAD_IMG_TAG "/herouter/AppRequest/DownloadIMG/"

#define AES_APP_KEY    "app.heluyou@2014\0"
#define AES_ONENET_KEY "daya_!234qawashkdf\0"

#define MAX_PICTURE_NUMBER 3
#define PICTURE_URL_SIZE 130
#define NUMBER_TWO 2

#define MAX_ENCRYPT_LEN 1024

/*Terminal access rulse part*/
#define MAX_ACCESS_RULES_SIZE 512*3  /*one rule about 34bytes,max 32 rules*/
#define MAX_QOS_RULES_SIZE 512*5  /*one rule about less 70 bytes,max 32 rules*/
#define MAX_ACCESS_RULES_NUMBER 32
#define MAX_QOS_RULES_NUMBER 32
#define MAX_TIME_PERIOD_NUM 2
#define INTERNET_RULES_SEPERATOR ","
#define INTERNET_TIME_SEPERATOR "&"
#define INTERNET_QOS_SEPERATOR ";"
#define IPTV_QOS_RULENAME_FLAG "IPTV_"

/* BODY_REQUSET_CONST_PART */
typedef struct tagWebsMsgReqBody
{
    DWORD version;          /* 协议版本 */
    CHAR  msgSeq[20];       /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */
}HERT_WEBS_MSG_REQ_BODY;

typedef struct tagWebsMsgReqDevPassBody
{
    CHAR  random[64];           /* 随机数 */
    CHAR  adminPassword[130];   /* 密码进行摘要 */
}HERT_WEBS_MSG_REQ_VAR_DEVPASS_BODY;

typedef struct tagWebsMsgReqUploadPicBody
{
    CHAR  fileName[130];    /* 文件名称 */
    DWORD fileSize;         /* 文件大小, 单位Kbyte,不足1Kbyte的算作1Kbyte */
}HERT_WEBS_MSG_REQ_VAR_UPLOAD_PIC_BODY;

typedef struct tagWebsMsgReqUpgradeBody
{
    CHAR  routerVersion[64];    /* 更新的版本信息 */
}HERT_WEBS_MSG_REQ_VAR_UPGRADE_BODY;

typedef struct tagWebsMsgReqSetNetWorkBody
{
    CHAR  random[64];          /* 随机数 */
    CHAR  adminPassword[130];  /* 密码进行摘要 */
    CHAR  netType;             /* 宽带连接方式 */
    CHAR  netAccount[32];      /* 宽带账号 */
    CHAR  netPassword[32];     /* 宽带账号密码 */
    CHAR  wifiName[32];        /* Wifi名称 */
    CHAR  wifiPassword[32];    /* Wifi密码 */
    /*for static ip setting*/
    CHAR  ipv4Address[32];    /* IP地址 */
    CHAR  subnetMask[32];     /* 子网掩码 */
    CHAR  gateway[32];        /* 网关 */
    CHAR  dnsServer[32];      /* 首选DNS*/
    CHAR  dnsServerBackup[32];    /* 备用DNS */
 }HERT_WEBS_MSG_REQ_VAR_SETNET_BODY;

typedef struct tagWebsMsgReqFaultDectBody
{
    DWORD faultType;           /* 故障类型 */
}HERT_WEBS_MSG_REQ_VAR_FAULTDECT_BODY;

typedef struct tagWebsMsgReqWPSWBody
{
    CHAR  wpsSwitch;           /* WPS开关,1:打开, 0:关闭 */
}HERT_WEBS_MSG_REQ_VAR_WPSW_BODY;


typedef struct tagWebsMsgReqChannelBody
{
    CHAR  workingChannel[32];           /* WPS开关,1:打开, 0:关闭 */
}HERT_WEBS_MSG_REQ_VAR_CHANNEL_BODY;

typedef struct tagWebsMsgReqDevAllowBody
{
    CHAR  mac[32];            /* mac */
    CHAR  AllowAdd;           /* 允许/拒绝加入标识,1:允许加入, 0:拒绝加入 */
}HERT_WEBS_MSG_REQ_VAR_DEVALLOW_BODY;

typedef struct tagWebsMsgReqAddDevBody
{
    DWORD dwDevNum;            /* 设备数目 */
    HERT_WEBS_MSG_REQ_VAR_DEVALLOW_BODY *pDevAddList;
}HERT_WEBS_MSG_REQ_VAR_ADDDEV_BODY;

/* HERT_WEBS_MSG_RSP_BODY */
typedef struct tagWebsMsgRspBody
{
    DWORD version;          /* 协议版本 */
    CHAR  msgSeq[20];       /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */
    DWORD errorCode;        /* 错误码 */
}HERT_WEBS_MSG_RSP_BODY;

/* HERT_WEBS_MSG_RSP_SUBDEV_BODY */
#if 0
typedef struct tagWebsMsgRspsubDevBody
{
    CHAR  devName[128];        /* 设备名称， */
    CHAR  devType[32];         /* 设备类型 */
    CHAR  devID[64];           /* 标识号 */
    CHAR  mac[32];             /* Mac */
    CHAR  connectTime[16];     /* 连接时长, 单位分钟 */
}HERT_WEBS_MSG_RSP_SUBDEV_BODY;
#else
typedef HERT_COM_SUBDEV_BODY HERT_WEBS_MSG_RSP_SUBDEV_BODY;
#endif

/* HERT_WEBS_MSG_RSP_STATUS_BODY */
typedef struct tagWebsMsgRspStatusBody
{
    CHAR  devStatus;           /* 工作状态, 0:正常, 1:非正常 */
    CHAR  broadbandRate[10];   /* 带宽占用率,百分数 */
    DWORD wifiStatus;          /* wifi无线网开关状态, 0 开, 1 关 */
    DWORD downbandwidth;           /* 实时带宽, 单位K bit */
    DWORD runStatus;          /* 路由器运行状态, 0：空闲, 1：良好, 2：忙碌 */
    DWORD dwDevNumber;         /* 外设数目 */
    HERT_WEBS_MSG_RSP_SUBDEV_BODY *pDev;   /* 外设信息 */
}HERT_WEBS_MSG_RSP_STATUS_BODY;

/* HERT_WEBS_MSG_RSP_SPACE_BODY */
typedef struct tagWebsMsgRspSpaceBody
{
    CHAR    diskStatus;           /* U盘状态, 0：正常, 1:不正常 */
    DDWORD  totalSpace;           /* 总存储空间, 单位MB */
    DDWORD  usedSpace;            /* 已用空间, 单位MB */
    DDWORD  leftSpace;            /* 剩余容量, 单位MB */
}HERT_WEBS_MSG_RSP_SPACE_BODY;

/* HERT_WEBS_MSG_RSP_ABILITY_BODY */
typedef struct tagWebsMsgRspAbilityBody
{
    CHAR    devName[130];          /* 设备名称 */
    CHAR    fac[32];               /* 设备厂家 */
    CHAR    type[32];              /* 设备型号 */
    CHAR    softVersion[256];      /* 固件版本号 */
    CHAR    softUpdate;            /* 是否有固件版本更新, 0：无固件版本更新, 1：有固件版本更新 */
    CHAR    softUpdateDis[256];    /* 固件版本更新描述 */
}HERT_WEBS_MSG_RSP_ABILITY_BODY;

/* HERT_WEBS_MSG_RSP_PICINFO_BODY */
typedef struct tagWebsMsgRspPicInfoBody
{
    CHAR    picUrl[130];                   /* 照片下载地址 */
    CHAR    smallPicUrl[130];              /* 缩略图下载地址 */
}HERT_WEBS_MSG_RSP_PICINFO_BODY;

/* HERT_WEBS_MSG_RSP_PICLIST_BODY */
typedef struct tagWebsMsgRspPicListBody
{
    DWORD    imgCount;                           /* 照片数量 */
    HERT_WEBS_MSG_RSP_PICINFO_BODY *pPicList;    /* 照片下载地址列表 */
}HERT_WEBS_MSG_RSP_PICLIST_BODY;


/* HERT_WEBS_MSG_RSP_UPLOAD_IMG_BODY */
typedef struct tagWebsMsgRspUploadImgBody
{
    CHAR    picUrl[128];                   /* 照片下载地址 */
}HERT_WEBS_MSG_RSP_UPLOAD_IMG_BODY;


/* HERT_WEBS_MSG_RSP_FAULTITEM_BODY */
typedef struct tagWebsMsgRspFaultItemBody
{
    CHAR    faultType[32];                   /* 故障类型 */
    CHAR    faultReason[128];                /* 故障原因 */
    CHAR    faultProcess[32];                /* 处理方法 */
    CHAR    faultAllSteps[512];              /* 故障处理的所有步骤 */
}HERT_WEBS_MSG_RSP_FAULTITEM_BODY;

/* HERT_WEBS_MSG_RSP_FAULTDECT_BODY */
typedef struct tagWebsMsgRspFaultDectBody
{
    DWORD dwFaultNumber;                           /* 故障数目 */
    HERT_WEBS_MSG_RSP_FAULTITEM_BODY *pFaultItem;  /* 故障信息 */
}HERT_WEBS_MSG_RSP_FAULTDECT_BODY;

/* HERT_WEBS_MSG_RSP_DEVITEM_BODY */
typedef struct tagWebsMsgRspDevItemBody
{
    CHAR    devName[128];               /* 设备名称 */
    CHAR    devType[32];                /* 设备类型 */
    CHAR    devID[64];                  /* 标识号 */
    CHAR    mac[32];                    /* Mac */
}HERT_WEBS_MSG_RSP_DEVITEM_BODY;

/* HERT_WEBS_MSG_RSP_DEVTOADD_BODY */
typedef struct tagWebsMsgRspDevToAddBody
{
    DWORD dwDevNumber;                           /* 设备数目 */
    HERT_WEBS_MSG_RSP_DEVITEM_BODY *pDevItem;  /* 设备信息 */
}HERT_WEBS_MSG_RSP_DEVTOADD_BODY;

typedef struct tagWebsMsgReqGuestManageBody
{
    CHAR wifiSwitch;       /* 无线开关 0:关闭 1:打开 */	
    CHAR passwordSwitch;        /* 密码开关 0:关闭 1:打开*/
    CHAR password[32];     /* 密码依赖于密码开关，为1时，需要输入密码 */
    short openTime;  /* 开启时长 单位:小时 -1:永久 */
}HERT_WEBS_MSG_REQ_VAR_GUEST_MANAGE_BODY;

typedef struct tagWebsMsgRspGuestManageBody
{
    CHAR  wifiSwitch;      /* 无线开关 0:关闭 1:打开 */	
    CHAR  passwordSwitch;  /* 密码开关 0:关闭 1:打开 */
    CHAR  password[32];    /* 密码依赖于密码开关，为1时，需要输入密码 */
    int   openTime;        /* 开启时长 单位:小时 -1:永久 */
    int   remainTime;      /* 剩余时长 单位：分钟 永久为：-1 */
}HERT_WEBS_MSG_RSP_VAR_GUEST_MANAGE_BODY;

CHAR *ParseJsonString(CHAR *pData, CHAR *pszItemName, CHAR *pszItemValue);

char hertUtil_isWifiOn();
char *hertUtil_getSN();
char *hertUtil_getPassword();
int hertUtil_isStorageExist(void);
int hertUtil_getStorageSize(char *path, long long *freeSize, long long *totalSize);

int base64encoder(unsigned char *pData, unsigned int nlength, char * outstr, 
			 const unsigned int max_str_out);

int base64decoder (char * eapstr, unsigned char *pData, unsigned int *nlength);

long hertUtil_getFileSize(const char *filename);

int hertUtil_setNetConfig4App(HERT_WEBS_MSG_REQ_VAR_SETNET_BODY * NetConfig);

int hertUtil_aes_ecb_128_Encrypt(const unsigned char *sMsg, int cbMsg, unsigned char *sEncryptMsg, int *cbEncryptMsg);
int hertUtil_aes_ecb_128_Decrypt(unsigned char* inBuf,int len,unsigned char* outBuf);

void hertUtil_BuildErrMsgRsp(DWORD errorCode,const char* msgType,char *pszOutputData, int nBufferLen,HERT_WEBS_MSG_REQ_BODY *pReqBody);

int hertUtil_readMac(char *buf);

int hertUtil_getIsNeedUpgrade();

typedef enum
{
   HERT_ERR_SUCCESS         = 0,
   HERT_ERR_INVALID_JSON    = 0xa001,   /* Json消息格式错误 */
   HERT_ERR_INTERNEL_FALSE  = 0xa002,   /* 服务器内部错误 */
   HERT_ERR_DISK_NOENOUGH   = 0xa003,   /* 磁盘空间不足 */
   HERT_ERR_AUTH_FAILED     = 0xa004,   /* 身份验证失败 */
   HERT_ERR_DISK_NOEXIST    = 0xa005,   /* 磁盘不存在 */
   HERT_ERR_PICTURE_NOEXIST = 0xa006,   /* 照片(列表)不存在 */
   HERT_ERR_UPGRADE_DOWN    = 0xa007,   /* 下载固件升级文件失败 */
   HERT_ERR_UPGRADE_PROCESS = 0xa008,   /* 固件升级中 */
   HERT_ERR_UPGRADE_NONEED  = 0xa009,   /* 无需固件升级 */
   HERT_ERR_UNKNOW_MSGTYPE  = 0xa00a,   /* 未知消息类型 */
}HERT_ERR_NO;


/***********************************
** Error logging                   *
***********************************/
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

void vpcom_log(const char*szFile, int nLine, const char *format,...);
void vpcom_logInfo(const char*szFile, int nLine,  const char *format,...);
void vpcom_logErr(const char*szFile, int nLine, const char *format,...);             
VPCOM_LOGLEVEL vpcom_GetLogLevel();
void vpcom_SetLogLevel(VPCOM_LOGLEVEL level);

char* hertUtil_getDeviceType();
char* hertUtil_getFirmwareVersion();
int hertUtil_getHerouteUpImg();
void hertUtil_DelNetFiltMac(char *filterMac);
void hertUtil_AddNetFiltMac(char *filterMac);
int hertUtil_IsInFile(const char *szFile, const char *szfindstr);
VPCOM_LOGLEVEL hertUtil_getLoglevel();



typedef struct tagIpReachItem 
{
    unsigned long ip;
    unsigned int  nReachable;
    unsigned long expires;
    char   mac[32];          /* mac */
    unsigned char hostname[16];
} IPREACHITEM;


/* time Period */
typedef struct tagWebsMsgTimePeriodInfo
{
    CHAR    startTime[32];            /* 开始时间 hh:mm */
    CHAR    endTime[32];              /* 结束时间 hh:mm */
}HERT_WEBS_MSG_TIME_PERIOD_INFO;

/*MSG_GET_TERMINAL_STATUS_REQ*/
typedef struct tagWebsMsgReqTerminalStatusBody
{
    CHAR  devMac[32];    /* Mac地址 */
}HERT_WEBS_MSG_REQ_TERMINAL_STATUS_BODY;

/* HERT_WEBS_MSG_RSP_TERMINAL_STATUS_BODY */
typedef struct tagWebsMsgRspTerminalStatusBody
{
    DWORD  accessControl;           /* 0:禁止 1:允许 */
    DWORD  netLimit;                /* 0：高 1：中 2：低*/
    DWORD  periodNum;              /*时间段数量*/
    HERT_WEBS_MSG_TIME_PERIOD_INFO *netTime; /*上网时段*/
}HERT_WEBS_MSG_RSP_TERMINAL_STATUS_BODY;

/*MSG_SET_TERMINAL_ACCESS_REQ*/
typedef struct tagWebsMsgReqSetTerminalBody
{
    CHAR   devMac[32];               /* Mac地址 */
    DWORD  accessControl;           /* 0:禁止 1:允许 */
    DWORD  netLimit;                /* 0：高 1：中 2：低*/
    DWORD  periodNum;              /*时间段数量*/
    HERT_WEBS_MSG_TIME_PERIOD_INFO *netTime; /*上网时段*/
}HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY;


int hertUtil_IsAssicString(char *pszString);

int hertParse_internetRules(char *interRules,char *mac,int *accCon,int *netLimit,char *timeStart,char *timeEnd);
int hertUtil_getAccessRulesInNvram(const char *tarMAC, HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY *rulesBody);
int hertUtil_updateAccessRulesInNvram(HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY rulesBody);
int hertUtil_GetNvram_Values(char *pszTitle, char *szOutput, int nBuffSize);
int hertUtil_updateQOSSetting(const char *tarMac,const int group);
int herUtil_updateInterRules();
void updateArryRules();

extern int getIfIp(char *ifname, char *if_addr);
extern char* getLanIfName(void);
extern int initInternet(void);

#define HERT_LOGINFO(format, ...)   if ( VPCOM_LOG_NOTICE <= vpcom_GetLogLevel() ) \
                                         { LOGINFO((__FUNCTION__, __LINE__,  format, ##__VA_ARGS__)); }

#define HERT_LOGERR(format, ...)    if ( VPCOM_LOG_ERROR <= vpcom_GetLogLevel() ) \
                                         { LOGERROR((__FUNCTION__, __LINE__, format, ##__VA_ARGS__)); }

#define HERT_LOGDEBUG(format, ...)  if ( VPCOM_LOG_DEBUG <= vpcom_GetLogLevel() ) \
                                         { LOGDEBUG(( __FUNCTION__, __LINE__, format, ##__VA_ARGS__)); }

#define HERT_AES_DEBUG(funcprintlog) \
    if (hertUtil_IsInFile("/var/aesdebug","DEBUG")) \
    { \
        funcprintlog;\
    }
	
void hertUtil_setLoglevel();

DWORD hertUtil_getWorkStatus();

#endif /* __HEROUTER_HEADER_WEBS__ */
