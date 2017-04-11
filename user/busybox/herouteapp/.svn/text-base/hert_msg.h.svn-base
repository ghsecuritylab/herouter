
#ifndef __HE_ROUTE_MSG_HEADER__
#define __HE_ROUTE_MSG_HEADER__

/*
** C: route, S: platform
**
**
*/

#define CONN_REQ      1            /* 连接建立请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY*/
#define CONN_RESP     2            /* 连接建立响应	S->C, DATA_FORMAT: MSGHEADER + OPTION */
#define PUSH_DATA     3            /* 发送数据	双向, DATA_FORMAT: MSGHEADER + OPTION + BODY */
#define PING_REQ     12	           /* 心跳请求	C->S, DATA_FORMAT: MSGHEADER */
#define PING_RESP    13	           /* 心跳响应	S->C, DATA_FORMAT: MSGHEADER */


//---------------------------------------------------------------------------
//
// HEADER DEFINE
//
//---------------------------------------------------------------------------

#define MAX_LENGTH_SIZE 4

typedef struct tagMsgHeader
{
    CHAR MsgType;
    CHAR MsgLength[MAX_LENGTH_SIZE];
}HERT_MSGHEADER;

#define GET_MSG_TYPE(a)  (0x0f & (a >> 4))

//---------------------------------------------------------------------------
//
// OPTION DEFINE
//
//---------------------------------------------------------------------------
/* 连接建立请求-OPTION */
typedef struct tagConnReqOption
{
    CHAR nHighLength;  /* 选项1：协议描述（字符串格式） */
    CHAR nLowLength;
    CHAR *pProtocol;
    CHAR nVer;         /* 选项2：协议版本 */
    CHAR nConnFlag;    /* 选项3：连接标志, Bit(7)=user flag, Bit(6)=pass flag, other is reserve */
    CHAR nHighTime;    /* 选项4：保持连接时间（256秒=0x0100） */
    CHAR nLowTime;
}HERT_MSG_OPTION_CONN_REQ;

#define SET_USER_TRUE(a)  (0xff & (a | 0x80))
#define SET_USER_FALSE(a) (0xff & (a & 0xEF))

#define SET_PASS_TRUE(a)  (0xff & (a | 0x40))
#define SET_PASS_FALSE(a) (0xff & (a & 0xBF))

#define CONN_REQ_OPT1_PRT_DES  { 0x0, 0x03,'E', 'D', 'P'}

#define CONN_REQ_OPT2_PRT_VER  { 0x01 }

typedef enum ConnRspErrCode
{
    CNRSPERR_SUCCESS = 0x0,     /* 0：连接成功 */
    CNRSPERR_FAILED_PROTOCOL,   /* 1：验证失败-协议不匹配 */
    CNRSPERR_FAILED_DEVID,      /* 2：验证失败-设备ID鉴权失败 */
    CNRSPERR_FAILED_SERVER,     /* 3：验证失败-服务器失败 */
    CNRSPERR_FAILED_AUTH,       /* 4：验证失败-用户名密码鉴权失败 */
    CNRSPERR_FAILED_NOAUTH,     /* 5：验证失败-未授权 */
}CNRSPERR;

/* 连接建立响应-OPTION */
typedef struct tagConnRspOption
{
    CHAR nRever;           /* 选项1：保留选项 */
    CHAR nErrCode;         /* 选项2：返回码   */
}HERT_MSG_OPTION_CONN_RSP;

/* 发送数据-OPTION */
typedef struct tagPushDataOption
{
    CHAR nHighLength;     /* 选项1：目的地址（字符串格式） */
    CHAR nLowLength;
    CHAR *pData;
}HERT_MSG_OPTION_PUSH_DATA;

//---------------------------------------------------------------------------
//
// BODY DEFINE
//
//---------------------------------------------------------------------------
/* 连接建立请求-BODY */
typedef struct tagConnReqBody
{
    CHAR nDevHighLength;  /* 消息体-设备ID（字符串格式） */
    CHAR nDevLowLength;
    CHAR nUserHighLength; /* 消息体-用户ID（字符串格式） */
    CHAR nUserLowLength;
    CHAR *pUserData;
    CHAR nPassHighLength; /* 消息体-鉴权信息（字符串格式） */
    CHAR nPassLowLength;
    CHAR *pPassData;
}HERT_MSG_BODY_CONN_REQ;



/* 发送数据-BODY_REQUSET_CONST_PART */
typedef struct tagReqPushDataConstPart
{
    DWORD version;          /* 协议版本 */
    CHAR  msgType[64];      /* 消息类型 */
    CHAR  msgSeq[20];       /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */
}HERT_MSG_REQ_PUSH_DATA_CONSTPART;

/* 发送数据-BODY_RESPONSE_CONST_PART */
typedef struct tagRspPushDataConstPart
{
    DWORD version;          /* 协议版本 */
    CHAR  msgType[64];      /* 消息类型 */
    CHAR  msgSeq[20];       /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */
    DWORD errorCode;        /* 错误码 */
    CHAR  description[256]; /* 描述	*/
}HERT_MSG_RSP_PUSH_DATA_CONSTPART;

/* 发送数据-BODY_REQUSET_VAR_PART_INIT */
typedef struct tagReqPushDataVarPart_Init
{
    CHAR  devID[64];      /* 设备编号 */
    DWORD deviceType;     /* 设备类型 */
    CHAR  IP[32];         /* 设备IP */
}HERT_MSG_REQ_PUSH_DATA_VARPART_INIT;

/* 发送数据-BODY_REPONSE_VAR_PART_INIT */
typedef struct tagRspPushDataVarPart_Init
{
    DWORD cycle;          /* 心跳保活周期，单位秒，默认120（暂不使用） */
    CHAR  time[32];       /* 时间 */
    CHAR  timeZone[16];   /* 时区, 范围-12~+12 */
    CHAR  platformVersion[32];    /* 平台版本 */
    CHAR  password[128];  /* 平台分配的设备密码,AES加密 */
}HERT_MSG_RSP_PUSH_DATA_VARPART_INIT;


/* 发送数据-BODY_REQUSET_VAR_PART_ABILITY_NOTIFY */
typedef struct tagReqPushDataVarPart_Ability_Notify
{
    CHAR  devID[64];        /* 设备编号 */
    CHAR  devName[128];     /* 设备名称 */
    CHAR  fac[32];          /* 设备厂家 */
    CHAR  type[32];         /* 设备型号 */
    CHAR  softVersion[256]; /* 固件版本号 */
}HERT_MSG_REQ_PUSH_DATA_VARPART_ABILITY_NOTIFY;


/* 发送数据-BODY_REPONSE_VAR_PART_SPACE */
typedef struct tagRspPushDataVarPart_Space
{
    CHAR   diskStatus;       /* 0：正常 1:不正常 */
    DDWORD totalSpace;       /* 总存储空间, 单位MB */
    DDWORD usedSpace;        /* 已用空间, 单位MB */
    DDWORD leftSpace;        /* 剩余容量, 单位MB */
}HERT_MSG_RSP_PUSH_DATA_VARPART_SPACE;

/* 发送数据-BODY_REPONSE_VAR_PART_STATUS */
#if 0
typedef struct tagRspPushDataVarPart_DevInfo
{
    CHAR   devName[128];     /* 设备名称 */
    CHAR   devType[32];      /* 设备类型 */
    CHAR   devID[64];        /* 标识号 */
    CHAR   mac[32];          /* mac */
    CHAR   ConnectTime[16];  /* 连接时长, 单位分钟 */
}HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO;
#else
typedef HERT_COM_SUBDEV_BODY HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO;
#endif

typedef struct tagRspPushDataVarPart_Status
{
    DWORD  devStatus;             /* 工作状态,0：正常,1：不正常 */
    DWORD  runStatus;             /* 运行质量,0：优,1：良,2：中 */
    DWORD  downbandwidth;         /* 下行带宽,单位KB/S */
    CHAR   broadbandRate[16];     /* 带宽占用率, 百分数 */
    DWORD  wifiStatus;        /* wifi无线网开关状态, 0 开, 1 关 */
    CHAR   devNum;            /* this item is No need send */
    HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO *pDevList; /* 外设信息 */
}HERT_MSG_RSP_PUSH_DATA_VARPART_STATUS;


/* 发送数据-BODY_REQUEST_VAR_PART_DOWNLOAD */
typedef struct tagReqPushDataVarPart_Download
{
    CHAR   contentID[128];     /* 图片编号 */
    CHAR   URL[256];           /* 下载URL */
    CHAR   contentName[128];   /*图片名称*/
}HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD;


/* 发送数据-BODY_REQUEST_VAR_PART_DOWNLOAD_COMP */
typedef struct tagReqPushDataVarPart_Download_Comp
{
    CHAR   contentID[128];     /* 图片编号 */
}HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD_COMP;

/* 发送数据-BODY_REQUSET_VAR_GET_UPDATE */
typedef struct tagReqPushDataVarPart_GetUpdate
{
    CHAR  devID[64];          /* 标识号 */
    DWORD type;               /* 升级客户端类型, 2：路由器 */
    CHAR  currentVersion[32]; /* 当前版本 */
}HERT_MSG_REQ_PUSH_DATA_VARPART_GET_UPDATE;

/* 发送数据-BODY_REPONSE_VAR_GET_UPDATE */
typedef struct tagRspPushDataVarPart_GetUpdate
{
    CHAR  needUpdate;                /* 是否需要升级, 0：无需升级, 1：强制升级, 2：非强制升级 */
    CHAR  updateUrl[256];            /* 升级文件URL, 无需升级时，无此字段 */
    CHAR  updateDescription[256];    /* 升级原因说明, 各项说明通过竖线（|）分割 */
}HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE;

typedef struct tagFileInfo
{
   CHAR   fileID[32];        /* 文件编号 0表示任务尚未创建 */
   CHAR   fileName[256];     /* 下载文件名称 */
   CHAR   fileType[32];      /* 下载文件类型 RMVB、AVI等 */
   DWORD  fileStatus;        /* 任务状态 0：进行中 1：暂停 2：已完成 3: 未启动 */
   DWORD  fileSize;          /* 下载文件大小 单位MB */
   CHAR   fileAddress[256];  /* 下载文件地址 对于BT下载，此字段表示文件的路径 */
   DWORD  fileCompleteSize;  /* 已下载文件大小 单位MB */
}FILE_INFO;

typedef struct tagDownloadMission
{
    CHAR       missionID[32];        /* 任务编号 0表示任务尚未创建 */
    CHAR       missionName[256];     /* 任务名称 单文件时，即文件名 */
    CHAR       missionType[32];      /* 任务类型 RMVB、AVI等 */
    DWORD      missionStatus;        /* 任务状态 0：进行中 1：暂停 2：已完成 3: 未启动 */
    DWORD      missionSize;          /* 任务大小 单位MB */
    DWORD      missionCurSpeed;      /* 当前下载速度 单位KB/S */
    DWORD      missionCompleteSize;  /* 已下载大小 单位MB */
    CHAR       missionEndTime[32];   /* 暂不使用，格式yyyy-mm-ddHH:MM:SS */
    CHAR       missionDuration[16];  /* 单位 S */
    DWORD      dlFileNum;            /* 任务包含文件数量,如果是单文件下载，值为0 */
    FILE_INFO  *pFileList;           /* 文件列表 */
}DOWNLOAD_MISSION;

typedef struct tagReqPushDataVarPart_AddDownloadMission
{
    CHAR  dlAddress[256];  /* 下载链接 */
}HERT_MSG_REQ_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION;

typedef struct tagRspPushDataVarPart_AddDownloadMission
{
    CHAR   missionNum;               /* this item is No need send */
    DOWNLOAD_MISSION *pMissionInfo;  /* 任务信息 */
}HERT_MSG_RSP_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION;

typedef struct tagReqPushDataVarPart_DeviceOperate
{
    DWORD  operType;       /* 1:路由器WIFI控制 */
    CHAR   operPara[128];  /* operType为1时, 0:打开WIFI开关, 1:关闭WIFI开关 */
}HERT_MSG_REQ_PUSH_DATA_VARPART_DEVICEOPERATE;

struct ether_addr{
	unsigned char mac[6];
};
#define DOMAIN_SIZE 250
struct domain_info{
	char  domain[DOMAIN_SIZE];
	struct ether_addr  mac;
	struct in_addr  ip;
	 time_t time;
} domain_tmp;
struct domain_queue_type{
struct domain_info *data;
struct domain_info *next;
};

struct mymesg{
	long mtype;
 	struct domain_info domain_tmp;
};

struct dev_index_type{
		struct ether_addr mac;
		int num;
	    struct in_addr  ip;
        HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO  *devinfo;
        struct domain_queue_type *domain_queue;
	  };

struct dev_infos_type{
 struct ether_addr mac;
 HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO devinfo;
};
struct mac_list_type{
 int  num;
 struct dev_infos_type *dev_infos;
 };
struct flow_length_req{
long mtype;
struct dev_infos_type dev_info;
};
struct flow_type_rsp{
long mtype;
unsigned long  long  length;
struct dev_infos_type dev_info;
};
struct flow_type_queue{
struct  flow_type_rsp *next;
unsigned long  long  length;
struct dev_infos_type dev_info;
};


/* parse: 发送数据-路由器属性上报响应 */
RETCODE Parse_Msg_BodyConstPartRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody);

/* parse: 发送数据-路由器属性上报请求 */
RETCODE Parse_Msg_BodyConstPartReq(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody);

/* build: 连接建立请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_ConnReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_CONN_REQ *ptMsgOption, IN HERT_MSG_BODY_CONN_REQ *ptMsgBody);

/* parse: 连接建立响应	S->C, DATA_FORMAT: MSGHEADER + OPTION */
RETCODE Parse_Msg_ConnRsp(CHAR *ptMsg, IN DWORD nDataLength, HERT_MSG_OPTION_CONN_RSP *ptMsgOption);

/* build: 心跳请求	C->S, DATA_FORMAT: MSGHEADER */
RETCODE Build_Msg_PingReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen);

/* build: 发送数据-获取初始化信息请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_InitInfoReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_INIT *ptMsgVarBody);

/* parse: 发送数据-获取初始化信息响应	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_InitInfoRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_RSP_PUSH_DATA_VARPART_INIT *ptMsgVarBody);

/* build: 发送数据-路由器属性上报请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_AbilityNotifyReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_ABILITY_NOTIFY *ptMsgVarBody);


/* build: 发送数据-查询路由器存储信息响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_RouteSpaceRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_SPACE *ptMsgVarBody);

/* build: 发送数据-查询路由器工作状态响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
/* build: 发送数据-主动上报路由器状态请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_RouteStatusRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_STATUS *ptMsgVarBody,int isSendRequest);

/* parse: 发送数据-图片下载通知请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_DownloadReq(IN CHAR *ptMsg, IN DWORD datalength, 
                          OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *ptMsgVarBody);

/* build: 发送数据-图片下载通知响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_DownloadRsq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody);

/* parse: 发送数据-路由器属性上报请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_DownloadCompReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen, IN HERT_MSGHEADER *ptMsgHeader, 
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD_COMP *ptMsgVarBody);


/* build: 发送数据-路由器属性上报请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_GetUpgradeInfoReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_GET_UPDATE *ptMsgVarBody);

/* parse: 发送数据-路由器属性上报请求    S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_GetUpdateRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE *ptMsgVarBody);
                          
/* parse: 创建下载任务请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_AddDownloadMissionReq(IN CHAR *ptMsg, IN DWORD datalength, 
                          OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION *ptMsgVarBody);
                          
/* build: 创建下载任务响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_AddDownloadMissionRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION *ptMsgVarBody);

RETCODE Parse_Msg_DeviceOperateReq(IN CHAR *ptMsg, IN DWORD datalength, OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DEVICEOPERATE *ptMsgVarBody);
                                                  
RETCODE Build_Msg_DeviceOperateRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody);

RETCODE Build_Msg_DownloadCompReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD_COMP *ptMsgVarBody);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STR(a) #a

#define GET_STR_LENGTH(a) ((a) ? strlen(a) : 0) 

#define GET_LENGTH_PUSH_OPTION() (2 * sizeof(CHAR))

int GetLengthFromTwoBytes(CHAR nHighLength, CHAR nLowLength);

int GetHighLowFromString(CHAR *pdata, CHAR *nHighLength, CHAR *nLowLength);

CHAR *SetMsgLengthItem(CHAR *ptr, DWORD nMsgLen, int *nDataLen);

RETCODE Parse_Msg_Type(IN CHAR *ptMsg, OUT CHAR *msgType);
RETCODE Parse_Msg_Data(IN CHAR *ptMsg, OUT CHAR **pptMsgData, OUT DWORD *nMsgDataLen);
RETCODE Parse_Msg_ConnRsp(IN CHAR *ptMsg, IN DWORD nDataLength, OUT HERT_MSG_OPTION_CONN_RSP *ptMsgOption);

RETCODE Parse_Msg_PushData_Type(IN CHAR *ptMsg, IN int nDataLen, OUT CHAR *msgType, IN int nMsgTypeBuf);

CHAR *ParseJsonString(CHAR *pData, CHAR *szItemName, CHAR *szItemValue);

RETCODE Build_Msg_DomainStauteRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
		IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
		IN struct dev_index_type  *ptMsgVarBody);
RETCODE Build_Msg_ConNotifyRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
		IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
		IN struct flow_type_queue    *ptMsgVarBody);

#endif /* __HE_ROUTE_MSG_HEADER__ */
