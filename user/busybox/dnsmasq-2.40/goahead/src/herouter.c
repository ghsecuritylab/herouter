#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <linux/wireless.h>
#include "wsIntrn.h"
#include "md5.h"
#include "nvram.h"
#include "hert_com.h"
#include "herouter.h"
#include "utils.h"

typedef struct tagToAddDev
{
    unsigned long curtime;
    char szProduct[64];
    char szProdType[64];
    char szProdManu[64];
    char szProdSerial[64];
    char szMac[64];
}TOADDDEV;

#define TOADDDEVFILE "/var/toaddDevList"
#define TEMP_TOADDDEVFILE "/var/toaddDevList_temp"
#define ALLOWTOADDLIST "/var/allowAddlist"
#define TEMP_ALLOWTOADDLIST "/var/allowAddlist_temp"
//wifi control interface
int websHertAppPro_WifiStrengthREQ(const char *pszSrcBody, char *pszOutputData, int nBufferLen);
int websHertAppPro_SetWifiStrengthREQ(const char *pszSrcBody, char *pszOutputData, int nBufferLen);
int websHertAppPro_SetWifiClosedREQ(webs_t wp, const char *pszSrcBody, char *pszOutputData, int nBufferLen);

extern int HertApp_ResetTimerAll();
extern int HertApp_SetWPSPBC();

char currMsgType[128];
int  isAlreadyResponse;
char MD5Password[130];/* can be removed */
int toAddDevNum;
int firstTime;
char *repeatStart;

int g_nWpsState = 0;
int g_nRemainTime = 0; /* minutes */

extern NETWORK_STATUS herouterStatus;

int g_upgradeState = 0; /* 0: no process, 1: upgrading. */

int GetPictureListStatus = 0;/* 0:net send yet; 1:process sending; */
int infoNumbers = 0;

/*stror format linke:"ID-MAC-AccessFlag-[startTime1-endTime1&startTime2-endTime2]"*/
char InternetAccessRules[MAX_ACCESS_RULES_NUMBER][64];

#define FORMAT_JSON_ITEM_INT(buf, ptv, a) \
    memset(buf, 0x0, sizeof(buf)); \
    snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":%d", #a, ptv); \
    HERT_LOGINFO("%s=%s", #ptv, buf);

#define FORMAT_JSON_ITEM_LNG(buf, ptv, a) \
    memset(buf, 0x0, sizeof(buf)); \
    snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":%ld", #a, ptv); \
    HERT_LOGINFO("%s=%s", #ptv, buf);

#define FORMAT_JSON_ITEM_LNGLNG(buf, ptv, a) \
    memset(buf, 0x0, sizeof(buf)); \
    snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":%lld", #a, ptv); \
    HERT_LOGINFO("%s=%s", #ptv, buf);

#define FORMAT_JSON_ITEM_STR(buf, ptv, a) \
    memset(buf, 0x0, sizeof(buf)); \
    if(strlen(ptv) == 0) \
        snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":\"\"", #a); \
    else \
        snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":\"%s\"", #a, ptv); \
    HERT_LOGINFO("%s=%s", #ptv, buf);

#define ADD_ITEM_TO_JSON(Json, Item, JsonSize)\
    if (JsonSize > strlen(Json) + strlen(Item)) \
    { \
        if (strlen(Json) > 0 ) \
        { \
            snprintf(Json + strlen(Json), JsonSize - strlen(Json), ",%s", Item); \
        } \
        else \
        { \
            snprintf(Json + strlen(Json), JsonSize - strlen(Json), "{%s", szItem); \
        } \
    }\
    else \
    { \
        HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", JsonSize, Json, strlen(Json), strlen(Item),Item); \
        return 1; \
    }

#define ADD_STR_TO_JSON(Json, str,JsonSize)\
    if (JsonSize > strlen(Json) + strlen(str)) \
    { \
        snprintf(Json + strlen(Json), JsonSize - strlen(Json), str); \
    }\
    else \
    { \
        HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=(%d), str(%s)", JsonSize, Json, strlen(Json), str); \
        return 1; \
    }


/*
*
*故障码 异常原因 用户提示信息 备注 
*C1 WAN口网线未插入                                   请将网线插入“宽带口”，或查看网线是否松动 　 
*C2 USB口 未接入磁盘                                  若要使用家庭空间功能，请插入U盘或移动硬盘。 　 
*C3 DHCP方式上网 ，没有获得上网参数                   您采用了自动获取（DHCP）方式上网，获取上网参数失败，请咨询宽带运营商 　 
*C4 DHCP方式上网，获取上网参数成功，但外网连接异常    您采用了自动获取（DHCP）方式上网，获取上网参数成功，但网络连接不通畅，请咨询宽带运营商 　 
*C5 PPPoE拨号账号或密码错误                           宽带账号或密码错误，请检查输入是否正确 　 
*C6 PPPoE拨号无法获取上网参数                         宽带拨号（PPPoE）失败，请检查网线是否连接正确，如还有问题请咨询宽带运营商　 
*C7 PPPoE拨号成功，获取上网参数成功，但外网连接异常   宽带拨号（PPPoE）成功，但网络连接不通畅，请咨询宽带运营商 
*
*/

struct tagWebsMsgRspFaultItemBody Fault_1 =
{
    .faultType = "C1",
    .faultReason = "\x57\x41\x4E\xE5\x8F\xA3\xE7\xBD\x91\xE7\xBA\xBF\xE6\x9C\xAA\xE6\x8F\x92\xE5\x85\xA5",
    .faultProcess = "empty",
    .faultAllSteps = "\xE8\xAF\xB7\xE5\xB0\x86\xE7\xBD\x91\xE7\xBA\xBF\xE6\x8F\x92\xE5\x85\xA5\xE2\x80\x9C\xE5\xAE\xBD\xE5\xB8\xA6\xE5\x8F\xA3\xE2\x80\x9D\xEF\xBC\x8C\xE6\x88\x96\xE6\x9F\xA5\xE7\x9C\x8B\xE7\xBD\x91\xE7\xBA\xBF\xE6\x98\xAF\xE5\x90\xA6\xE6\x9D\xBE\xE5\x8A\xA8",
};

struct tagWebsMsgRspFaultItemBody Fault_2 =
{
    .faultType = "C2",
    .faultReason = "\x55\x53\x42\xE5\x8F\xA3\xE6\x9C\xAA\xE6\x8F\x92\xE5\x85\xA5\xE7\xA3\x81\xE7\x9B\x98",
    .faultProcess = "empty",
    .faultAllSteps = "\xE8\x8B\xA5\xE8\xA6\x81\xE4\xBD\xBF\xE7\x94\xA8\xE5\xAE\xB6\xE5\xBA\xAD\xE7\xA9\xBA\xE9\x97\xB4\xE5\x8A\x9F\xE8\x83\xBD\xEF\xBC\x8C\xE8\xAF\xB7\xE6\x8F\x92\xE5\x85\xA5\x55\xE7\x9B\x98\xE6\x88\x96\xE7\xA7\xBB\xE5\x8A\xA8\xE7\xA1\xAC\xE7\x9B\x98",
};

struct tagWebsMsgRspFaultItemBody Fault_3 =
{
    .faultType = "C3",
    .faultReason = "\x44\x48\x43\x50\xE6\x96\xB9\xE5\xBC\x8F\xE4\xB8\x8A\xE7\xBD\x91\x20\xEF\xBC\x8C\xE6\xB2\xA1\xE6\x9C\x89\xE8\x8E\xB7\xE5\xBE\x97\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0",
    .faultProcess = "empty",
    .faultAllSteps = "\xE6\x82\xA8\xE9\x87\x87\xE7\x94\xA8\xE4\xBA\x86\xE8\x87\xAA\xE5\x8A\xA8\xE8\x8E\xB7\xE5\x8F\x96\xEF\xBC\x88\x44\x48\x43\x50\xEF\xBC\x89\xE6\x96\xB9\xE5\xBC\x8F\xE4\xB8\x8A\xE7\xBD\x91\xEF\xBC\x8C\xE8\x8E\xB7\xE5\x8F\x96\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C\xE8\xAF\xB7\xE5\x92\xA8\xE8\xAF\xA2\xE5\xAE\xBD\xE5\xB8\xA6\xE8\xBF\x90\xE8\x90\xA5\xE5\x95\x86",
};

struct tagWebsMsgRspFaultItemBody Fault_4 =
{
    .faultType = "C4",
    .faultReason = "\x44\x48\x43\x50\xE6\x96\xB9\xE5\xBC\x8F\xE4\xB8\x8A\xE7\xBD\x91\xEF\xBC\x8C\xE8\x8E\xB7\xE5\x8F\x96\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x8C\xE4\xBD\x86\xE5\xA4\x96\xE7\xBD\x91\xE8\xBF\x9E\xE6\x8E\xA5\xE5\xBC\x82\xE5\xB8\xB8",
    .faultProcess = "empty",
    .faultAllSteps = "\xE6\x82\xA8\xE9\x87\x87\xE7\x94\xA8\xE4\xBA\x86\xE8\x87\xAA\xE5\x8A\xA8\xE8\x8E\xB7\xE5\x8F\x96\xEF\xBC\x88\x44\x48\x43\x50\xEF\xBC\x89\xE6\x96\xB9\xE5\xBC\x8F\xE4\xB8\x8A\xE7\xBD\x91\xEF\xBC\x8C\xE8\x8E\xB7\xE5\x8F\x96\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x8C\xE4\xBD\x86\xE7\xBD\x91\xE7\xBB\x9C\xE8\xBF\x9E\xE6\x8E\xA5\xE4\xB8\x8D\xE9\x80\x9A\xE7\x95\x85\xEF\xBC\x8C\xE8\xAF\xB7\xE5\x92\xA8\xE8\xAF\xA2\xE5\xAE\xBD\xE5\xB8\xA6\xE8\xBF\x90\xE8\x90\xA5\xE5\x95\x86",
};

struct tagWebsMsgRspFaultItemBody Fault_5 =
{
		.faultType = "C5",
    .faultReason = "\x50\x50\x50\x6F\x45\xE6\x8B\xA8\xE5\x8F\xB7\xE8\xB4\xA6\xE5\x8F\xB7\xE6\x88\x96\xE5\xAF\x86\xE7\xA0\x81\xE9\x94\x99\xE8\xAF\xAF",
    .faultProcess = "empty",
    .faultAllSteps = "\xE5\xAE\xBD\xE5\xB8\xA6\xE8\xB4\xA6\xE5\x8F\xB7\xE6\x88\x96\xE5\xAF\x86\xE7\xA0\x81\xE9\x94\x99\xE8\xAF\xAF\xEF\xBC\x8C\xE8\xAF\xB7\xE6\xA3\x80\xE6\x9F\xA5\xE8\xBE\x93\xE5\x85\xA5\xE6\x98\xAF\xE5\x90\xA6\xE6\xAD\xA3\xE7\xA1\xAE",
};

struct tagWebsMsgRspFaultItemBody Fault_6 =
{
		.faultType = "C6",
    .faultReason = "\x50\x50\x50\x6F\x45\xE6\x8B\xA8\xE5\x8F\xB7\xE6\x97\xA0\xE6\xB3\x95\xE8\x8E\xB7\xE5\x8F\x96\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0",
    .faultProcess = "empty",
    .faultAllSteps = "\xE5\xAE\xBD\xE5\xB8\xA6\xE6\x8B\xA8\xE5\x8F\xB7\xEF\xBC\x88\x50\x50\x50\x6F\x45\xEF\xBC\x89\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C\xE8\xAF\xB7\xE6\xA3\x80\xE6\x9F\xA5\xE7\xBD\x91\xE7\xBA\xBF\xE6\x98\xAF\xE5\x90\xA6\xE8\xBF\x9E\xE6\x8E\xA5\xE6\xAD\xA3\xE7\xA1\xAE\xEF\xBC\x8C\xE5\xA6\x82\xE8\xBF\x98\xE6\x9C\x89\xE9\x97\xAE\xE9\xA2\x98\xE8\xAF\xB7\xE5\x92\xA8\xE8\xAF\xA2\xE5\xAE\xBD\xE5\xB8\xA6\xE8\xBF\x90\xE8\x90\xA5\xE5\x95\x86",
};

struct tagWebsMsgRspFaultItemBody Fault_7 =
{
		.faultType = "C7",
    .faultReason = "\x50\x50\x50\x6F\x45\xE6\x8B\xA8\xE5\x8F\xB7\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x8C\xE8\x8E\xB7\xE5\x8F\x96\xE4\xB8\x8A\xE7\xBD\x91\xE5\x8F\x82\xE6\x95\xB0\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x8C\xE4\xBD\x86\xE5\xA4\x96\xE7\xBD\x91\xE8\xBF\x9E\xE6\x8E\xA5\xE5\xBC\x82\xE5\xB8\xB8",
    .faultProcess = "empty",
    .faultAllSteps = "\xE5\xAE\xBD\xE5\xB8\xA6\xE6\x8B\xA8\xE5\x8F\xB7\xEF\xBC\x88\x50\x50\x50\x6F\x45\xEF\xBC\x89\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x8C\xE4\xBD\x86\xE7\xBD\x91\xE7\xBB\x9C\xE8\xBF\x9E\xE6\x8E\xA5\xE4\xB8\x8D\xE9\x80\x9A\xE7\x95\x85\xEF\xBC\x8C\xE8\xAF\xB7\xE5\x92\xA8\xE8\xAF\xA2\xE5\xAE\xBD\xE5\xB8\xA6\xE8\xBF\x90\xE8\x90\xA5\xE5\x95\x86",
};

int websHertAppPro_ParseMsgReq(const char *pszSrcBody, HERT_WEBS_MSG_REQ_BODY *ptReqBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0,i = 0,timeBodySize = 0,timeNum = 0;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[256];
    CHAR    szMsgType[64];
    HERT_WEBS_MSG_REQ_VAR_DEVPASS_BODY    *pDevBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_UPLOAD_PIC_BODY *pUpdBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_UPGRADE_BODY    *pUprBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_SETNET_BODY     *pSetNetBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_FAULTDECT_BODY  *pFalutDectBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_WPSW_BODY       *pWpsSwitchBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_ADDDEV_BODY     *pAddDevBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_DEVALLOW_BODY   *pAddAllowLstBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_GUEST_MANAGE_BODY *pGuestManageBody = NULL;
    HERT_WEBS_MSG_REQ_TERMINAL_STATUS_BODY *pTerminalStatusBody = NULL;
    HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY    *pTerminalReqBody = NULL;
    HERT_WEBS_MSG_TIME_PERIOD_INFO         *pTerminalTimeBody =NULL;
    HERT_WEBS_MSG_REQ_VAR_CHANNEL_BODY     *pChannelBody =NULL; 
    int *wifiStrength=NULL;//wifi 信号强度
    if ( (!pszSrcBody) || (!pszOutputData)  || (!ptReqBody))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p), ptReqBody(%p)", 
                    pszSrcBody, pszOutputData, ptReqBody);
        return 1;
    }

    /* ====== parse msg data(body) */
    pData = (CHAR*)pszSrcBody;
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    memset(szMsgType, 0x0, sizeof(szMsgType));
    memset(pszOutputData, 0x0, nBufferLen);
    
    strncpy(szMsgType,currMsgType,sizeof(szMsgType));
    if(strlen(szMsgType) < 1)
    {
        HERT_LOGERR("get msgType error.");
        return 1;
    }

    if(strncmp(currMsgType,"MSG_SET_ROUTER_ADD_DEV_REQ",sizeof("MSG_SET_ROUTER_ADD_DEV_REQ")) == 0)
    {
        toAddDevNum = 0;
        firstTime = 1;
    }
    
    repeatStart = NULL;
    //find ios repeat flag
    repeatStart = strstr(pData,"}={");
    
    while(NULL != (pData = ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptReqBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptReqBody->msgSeq, szItemValue);
        }
  
        if (strstr(szMsgType, STR(MSG_GET_ROUTER_PASSWORD_REQ)))
        {
            if (!pDevBody)
            {
                pDevBody = (HERT_WEBS_MSG_REQ_VAR_DEVPASS_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(random)) == 0 )
            {
                strncpy(pDevBody->random, szItemValue, sizeof(pDevBody->random));
                HERT_LOGINFO("<<<<<<<<<<<<<<<<<pDevBody->random:%s<<<<<<<<<<<<<<<<<\n", pDevBody->random);
            }
            else if (strcmp(szItemName, STR(adminPassword)) == 0 )
            {
                memset(MD5Password,0x0,sizeof(MD5Password));
                strncpy(MD5Password,szItemValue,sizeof(szItemValue));
                strncpy(pDevBody->adminPassword, szItemValue, sizeof(pDevBody->adminPassword));
                HERT_LOGINFO("<<<<<<<<<<<<<<<<<pDevBody->adminPassword:%s<<<<<<<<<<<<<<<<<\n", pDevBody->adminPassword);
            }
        }
        else if (strstr(szMsgType, STR(MSG_GET_UPLOAD_IMG_REQ)))
        {
            if (!pUpdBody)
            {
                pUpdBody = (HERT_WEBS_MSG_REQ_VAR_UPLOAD_PIC_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(fileName)) == 0 )
            {
                strncpy(pUpdBody->fileName, szItemValue, sizeof(pUpdBody->fileName));
            }
            else if (strcmp(szItemName, STR(fileSize)) == 0 )
            {
                pUpdBody->fileSize = atoi(szItemValue);
            }
        }
        else if (strstr(szMsgType, STR(MSG_ROUTER_UPDATE_REQ)))
        {
             if (!pUprBody)
            {
                pUprBody = (HERT_WEBS_MSG_REQ_VAR_UPGRADE_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(routerVersion)) == 0 )
            {
                strncpy(pUprBody->routerVersion, szItemValue, sizeof(pUprBody->routerVersion));
            }
        }
        else if (strstr(szMsgType, STR(MSG_SET_NETWORK_REQ)))
        {
            if (!pSetNetBody)
            {
                pSetNetBody = (HERT_WEBS_MSG_REQ_VAR_SETNET_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(random)) == 0 )
            {
                strncpy(pSetNetBody->random, szItemValue, sizeof(pSetNetBody->random));
            }
            else if (strcmp(szItemName, STR(adminPassword)) == 0 )
            {
                strncpy(pSetNetBody->adminPassword, szItemValue, sizeof(pSetNetBody->adminPassword));
            }
            else if (strcmp(szItemName, STR(netType)) == 0 )
            {
                pSetNetBody->netType = atoi(szItemValue);
            }
            else if (strcmp(szItemName, STR(netAccount)) == 0 )
            {
                strncpy(pSetNetBody->netAccount, szItemValue, sizeof(pSetNetBody->netAccount));
            }
            else if (strcmp(szItemName, STR(netPassword)) == 0 )
            {
                strncpy(pSetNetBody->netPassword, szItemValue, sizeof(pSetNetBody->netPassword));
            }
            else if (strcmp(szItemName, STR(wifiName)) == 0 )
            {
                strncpy(pSetNetBody->wifiName, szItemValue, sizeof(pSetNetBody->wifiName));
            }
            else if (strcmp(szItemName, STR(wifiPassword)) == 0 )
            {
                strncpy(pSetNetBody->wifiPassword, szItemValue, sizeof(pSetNetBody->wifiPassword));
            }
            else if (strcmp(szItemName, STR(ipv4Address)) == 0 )
            {
                strncpy(pSetNetBody->ipv4Address, szItemValue, sizeof(pSetNetBody->ipv4Address));
            }
            else if (strcmp(szItemName, STR(subnetMask)) == 0 )
            {
                strncpy(pSetNetBody->subnetMask, szItemValue, sizeof(pSetNetBody->subnetMask));
            }
            else if (strcmp(szItemName, STR(gateway)) == 0 )
            {
                strncpy(pSetNetBody->gateway, szItemValue, sizeof(pSetNetBody->gateway));
            }
            else if (strcmp(szItemName, STR(dnsServer)) == 0 )
            {
                strncpy(pSetNetBody->dnsServer, szItemValue, sizeof(pSetNetBody->dnsServer));
            }
            else if (strcmp(szItemName, STR(dnsServerBackup)) == 0 )
            {
                strncpy(pSetNetBody->dnsServerBackup, szItemValue, sizeof(pSetNetBody->dnsServerBackup));
            }                    
        }
        else if (strstr(szMsgType, STR(MSG_GET_ROUTER_FAULT_DETEC_REQ)))
        {
            if (!pFalutDectBody)
            {
                pFalutDectBody = (HERT_WEBS_MSG_REQ_VAR_FAULTDECT_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(faultType)) == 0 )
            {
                pFalutDectBody->faultType = atoi(szItemValue);
            }
        }
        else if (strstr(szMsgType, STR(MSG_SET_ROUTER_WPS_SWITCH_REQ)))
        {
            if (!pWpsSwitchBody)
            {
                pWpsSwitchBody = (HERT_WEBS_MSG_REQ_VAR_WPSW_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(wpsSwitch)) == 0 )
            {
                pWpsSwitchBody->wpsSwitch = atoi(szItemValue);
            }
        }
        else if (strstr(szMsgType, STR(MSG_SET_ROUTER_ADD_DEV_REQ)))
        {
            if (!pAddDevBody)
            {
                pAddDevBody = (HERT_WEBS_MSG_REQ_VAR_ADDDEV_BODY*)pszOutputData;
                if(toAddDevNum > 0)
                {
                    if((pAddDevBody->pDevAddList = (HERT_WEBS_MSG_REQ_VAR_DEVALLOW_BODY*)calloc(toAddDevNum ,sizeof(struct tagWebsMsgReqDevAllowBody))) == NULL)	
                    {
                        HERT_LOGERR("faild to calloc memory\n");
                        ret = -1;
                        break;
                    }
                    pAddAllowLstBody = pAddDevBody->pDevAddList;
                    memset(pAddAllowLstBody->mac,0x0,sizeof(pAddAllowLstBody->mac));	
                    pAddAllowLstBody->AllowAdd = '0';
                    HERT_LOGINFO("--- toAddDevNum:%d ---\n",toAddDevNum);
                }
                else
                {
                    ret = -1;
                    break;
                }
                //pAddDevBody->dwDevNum = 0;
                pAddDevBody->dwDevNum = toAddDevNum;
                i = 1;
            }
            if( strcmp(szItemName, STR(mac)) == 0)
            {
                strncpy(pAddAllowLstBody->mac, szItemValue, sizeof(pAddAllowLstBody->mac));
            }
            if( strcmp(szItemName, STR(allowAdd)) == 0)
            {
                /* allowadd start from 0, so add 1 for next step check */
                pAddAllowLstBody->AllowAdd = atoi(szItemValue) + 0x01;
                HERT_LOGINFO("--- pAddAllowLstBody->AllowAdd: %d ---\n",pAddAllowLstBody->AllowAdd);
            }
            if( (pAddAllowLstBody->AllowAdd != 0x0) && (*(pAddAllowLstBody->mac) != 0x0))
            {
                if( (strcmp(szItemName, STR(allowAdd)) == 0) || 
                    (strcmp(szItemName, STR(mac)) == 0) )
                {            
                    pAddAllowLstBody->AllowAdd = pAddAllowLstBody->AllowAdd - 0x01;/* here, set data to back by reduce 1*/
                }
                if(i < pAddDevBody->dwDevNum)
                {
                    pAddAllowLstBody++;
                    memset(pAddAllowLstBody->mac,0x0,sizeof(pAddAllowLstBody->mac));	
                    pAddAllowLstBody->AllowAdd = 0x0;
                    i++;
                }
            }
        }
        else if (strstr(szMsgType, STR(MSG_SET_VISITOR_MANAGEMENT_REQ)))
        {
            if (!pGuestManageBody)
            {
                pGuestManageBody = (HERT_WEBS_MSG_REQ_VAR_GUEST_MANAGE_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(wifiSwitch)) == 0)
            {
                pGuestManageBody->wifiSwitch = atoi(szItemValue);
            }
            else if (strcmp(szItemName, STR(passwordSwitch)) == 0)
            {
                pGuestManageBody->passwordSwitch = atoi(szItemValue);
            } 
            else if (strcmp(szItemName, STR(password)) == 0)
            {
                strncpy(pGuestManageBody->password, szItemValue, sizeof(pGuestManageBody->password));
            }
            else if (strcmp(szItemName, STR(openTime)) == 0)
            {
                pGuestManageBody->openTime = atoi(szItemValue);
            }                                   	
        }   
        else if (strstr(szMsgType, STR(MSG_GET_TERMINAL_STATUS_REQ)))
        {
            if (!pTerminalStatusBody)
            {
                pTerminalStatusBody = (HERT_WEBS_MSG_REQ_TERMINAL_STATUS_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(devMac)) == 0 )
            {
                strncpy(pTerminalStatusBody->devMac, szItemValue, sizeof(pTerminalStatusBody->devMac));
            }
        }    
        else if (strstr(szMsgType, STR(MSG_SET_TERMINAL_ACCESS_REQ)))
        {  	
            if (!pTerminalReqBody)
            {
                pTerminalReqBody = (HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY*)pszOutputData;
                timeBodySize = sizeof(struct tagWebsMsgTimePeriodInfo);
                if((pTerminalReqBody->netTime = (HERT_WEBS_MSG_TIME_PERIOD_INFO*)calloc(1 ,(timeBodySize * MAX_TIME_PERIOD_NUM ) + 1)) == NULL)	
                {
                    HERT_LOGERR("faild to calloc memory\n");
                    ret = -1;
                    break;
                }
                pTerminalTimeBody = pTerminalReqBody->netTime;
                memset(pTerminalTimeBody,0x0,(timeBodySize * MAX_TIME_PERIOD_NUM) + 1);	
                pTerminalReqBody->periodNum = 1;
                timeNum = 0;
            }
            
            if (strcmp(szItemName, STR(devMac)) == 0)
            {
                strncpy(pTerminalReqBody->devMac, szItemValue, sizeof(pTerminalReqBody->devMac));
            }
            else if (strcmp(szItemName, STR(accessControl)) == 0)
            {
                pTerminalReqBody->accessControl = atoi(szItemValue);
            } 
            else if (strcmp(szItemName, STR(netLimit)) == 0)
            {
                pTerminalReqBody->netLimit = atoi(szItemValue);	
            }
            else if (strcmp(szItemName, STR(startTime)) == 0)
            {
                if(timeNum == 2)
                {
                    pTerminalTimeBody++;
                    pTerminalReqBody->periodNum++;
                }
                strncpy(pTerminalTimeBody->startTime, szItemValue, sizeof(pTerminalTimeBody->startTime));
                timeNum++;
            }  
            else if (strcmp(szItemName, STR(endTime)) == 0)
            {
                if(timeNum == 2)
                {
                    pTerminalTimeBody++;
                    pTerminalReqBody->periodNum++;
                }
                strncpy(pTerminalTimeBody->endTime, szItemValue, sizeof(pTerminalTimeBody->endTime));
                timeNum++;
            }  
            
        } //设置wifi 信号强度   
        else if (strstr(szMsgType, STR(MSG_SET_WIFI_STRENGTH_REQ)))
        {
            if (!wifiStrength)
            {
                wifiStrength = (int *)pszOutputData;
            }
            if (strcmp(szItemName, STR(wifiStrength)) == 0 )
            {
			          *wifiStrength=(atoi(szItemValue)&0xff);
            }
        }
        else if (strstr(szMsgType, STR(MSG_SET_ROUTER_CHANNEL_REQ)))
        {
            if (!pChannelBody)
            {
                pChannelBody = (HERT_WEBS_MSG_REQ_VAR_CHANNEL_BODY*)pszOutputData;
            }
            if (strcmp(szItemName, STR(workingChannel)) == 0 )
            {
                strncpy(pChannelBody->workingChannel, szItemValue, sizeof(pChannelBody->workingChannel));
            }
        }
        else if (strstr(szMsgType, STR(UNKNOW_MESSAGE_TYPE)))
        {
            /*do nothing*/
        }
        
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

int websHertAppPro_BuildMsgRsp(HERT_WEBS_MSG_REQ_BODY *pReqBody, const char *szMsgType, char *pVarBody, DWORD nErrorCode, 
                               char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_RSP_SUBDEV_BODY         *pSubDev     = NULL;
    HERT_WEBS_MSG_RSP_STATUS_BODY         *pStatusBody = NULL;
    HERT_WEBS_MSG_RSP_SPACE_BODY          *pSpaceBody  = NULL;
    HERT_WEBS_MSG_RSP_ABILITY_BODY        *pAbilityBody= NULL;
    HERT_WEBS_MSG_RSP_PICINFO_BODY        *pPicInfo    = NULL;
    HERT_WEBS_MSG_RSP_PICLIST_BODY        *pPickListBody = NULL;
    HERT_WEBS_MSG_RSP_FAULTDECT_BODY      *pFaultDectBody = NULL;
    HERT_WEBS_MSG_RSP_FAULTITEM_BODY      *pFaultItem  = NULL;
    HERT_WEBS_MSG_RSP_DEVTOADD_BODY       *pDevToAddBody = NULL;
    HERT_WEBS_MSG_RSP_DEVITEM_BODY        *pDevItem    = NULL;
    HERT_WEBS_MSG_RSP_TERMINAL_STATUS_BODY *pInterRules = NULL;
    HERT_WEBS_MSG_TIME_PERIOD_INFO        *pInterRulesTime = NULL;
    HERT_WEBS_MSG_RSP_VAR_GUEST_MANAGE_BODY *pGuestManege = NULL;
    CHAR    *pData    = NULL;
    CHAR     szItem[512];
    int      i = 0;

    if ( (!pReqBody) || (!pVarBody)  || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pReqBody(%p),pVarBody(%p), pszOutputData(%p)", 
                    pReqBody, pVarBody, pszOutputData);
        return 1;
    }
    memset(pszOutputData, 0x0, nBufferLen);
    pData = pszOutputData;
    HERT_LOGINFO(">>>>>>>>>>>>>>>>>pData:%s>>>>>>>>>>>>>>>>>\n", pData);
    
    /* ===== Format Json message data */
    if(strstr(szMsgType, STR(MSG_GET_IMG_LIST_RSP)) && GetPictureListStatus != 0)
    {
        /*had added before,so just skip there*/	 
    }
    else
	{
        FORMAT_JSON_ITEM_LNG(szItem, pReqBody->version, version);  /* version */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_STR(szItem, pReqBody->msgSeq, msgSeq);    /* msgSeq */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_LNG(szItem, nErrorCode, errorCode);       /* errorCode */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_STR(szItem, szMsgType, msgType);    /* msgType */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
     }

    /* check msg body */
    if (strstr(szMsgType, STR(MSG_GET_ROUTER_STATUS_RSP)))
    {
        pStatusBody = (HERT_WEBS_MSG_RSP_STATUS_BODY*)pVarBody;
        FORMAT_JSON_ITEM_INT(szItem, pStatusBody->devStatus, devStatus);    /* devStatus */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_STR(szItem, pStatusBody->broadbandRate, broadbandRate);    /* broadbandRate */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_LNG(szItem, pStatusBody->runStatus, runStatus);    /* workStatus */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_LNG(szItem, pStatusBody->downbandwidth, downbandwidth);      /* broadband */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        FORMAT_JSON_ITEM_LNG(szItem, pStatusBody->wifiStatus, wifiStatus);    /* wifiStatus */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        ADD_STR_TO_JSON(pData, ",\"",nBufferLen);
        ADD_STR_TO_JSON(pData, STR(subDevList),nBufferLen);
        ADD_STR_TO_JSON(pData, "\":",nBufferLen);
        ADD_STR_TO_JSON(pData, "[",nBufferLen);

        pSubDev = pStatusBody->pDev;
        for(i = 0; i < pStatusBody->dwDevNumber; i++)
        {
            if ( i == 0 )
            {
                ADD_STR_TO_JSON(pData, "{",nBufferLen);
            }
            else
            {
                ADD_STR_TO_JSON(pData, ",{",nBufferLen);
            }
            //pSubDev = pSubDev + i;

            FORMAT_JSON_ITEM_STR(szItem, pSubDev->devName, devName);         /* devName */
            if (nBufferLen > strlen(pData) + strlen(szItem))
            {
                snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
            }
            else
            {
                HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem),szItem);
                return 1;
            }
            if(strstr(pSubDev->devName,"android"))
                sprintf(pSubDev->devType,"%d",1);
            else if(strstr(pSubDev->devName,"iPhone"))
                sprintf(pSubDev->devType,"%d",2);
            else if(strstr(pSubDev->devName,"Mac"))
                sprintf(pSubDev->devType,"%d",3);
            else if(strstr(pSubDev->devName,"PC")||strstr(pSubDev->devName,"WINDOWS"))
                sprintf(pSubDev->devType,"%d",4);
			else if(strstr(pSubDev->devName,"iPad"))
                sprintf(pSubDev->devType,"%d",5);

            else
                sprintf(pSubDev->devType,"%d",0);
     
            FORMAT_JSON_ITEM_STR(szItem, pSubDev->devType, devType);          /* devType */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pSubDev->devID, devID);              /* devID */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        
            FORMAT_JSON_ITEM_STR(szItem, pSubDev->mac, mac);                  /* mac */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        
            FORMAT_JSON_ITEM_STR(szItem, pSubDev->connectTime, connectTime);  /* connectTime */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            ADD_STR_TO_JSON(pData, "}",nBufferLen);
            pSubDev++;
        }
        ADD_STR_TO_JSON(pData, "]",nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_SN_RSP)))
    {
        FORMAT_JSON_ITEM_STR(szItem, pVarBody, devSN);    /* devSN */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_PASSWORD_RSP)))
    {
        FORMAT_JSON_ITEM_STR(szItem, pVarBody, password);    /* password */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_SPACE_RSP)))
    {
        pSpaceBody = (HERT_WEBS_MSG_RSP_SPACE_BODY*)pVarBody;
        FORMAT_JSON_ITEM_INT(szItem, pSpaceBody->diskStatus, diskStatus);    /* diskStatus */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_LNGLNG(szItem, pSpaceBody->totalSpace, totalSpace);    /* totalSpace */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_LNGLNG(szItem, pSpaceBody->usedSpace,  usedSpace);     /* usedSpace */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_LNGLNG(szItem, pSpaceBody->leftSpace,  leftSpace);     /* leftSpace */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_ABILITY_RSP)))
    {
        pAbilityBody = (HERT_WEBS_MSG_RSP_ABILITY_BODY*)pVarBody;
        FORMAT_JSON_ITEM_STR(szItem, pAbilityBody->devName, devName);          /* devName */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_STR(szItem, pAbilityBody->fac, fac);                  /* fac */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_STR(szItem, pAbilityBody->type,  type);               /* type */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_STR(szItem, pAbilityBody->softVersion,  softVersion); /* softVersion */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_INT(szItem, pAbilityBody->softUpdate,  softUpdate);   /* softUpdate */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_STR(szItem, pAbilityBody->softUpdateDis,  softUpdateDis);   /* softUpdateDis */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_UPLOAD_IMG_RSP)))
    {
        FORMAT_JSON_ITEM_STR(szItem, pVarBody, url);    /* url */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_ROUTER_UPDATE_RSP)) ||
             strstr(szMsgType, STR(MSG_SET_ROUTER_WPS_SWITCH_RSP)) ||
             strstr(szMsgType, STR(MSG_SET_NETWORK_RSP)) ||
             strstr(szMsgType, STR(MSG_SET_ROUTER_ADD_DEV_RSP)) ||
             strstr(szMsgType, STR(MSG_ROUTER_UPDATE_RSP)) ||
             strstr(szMsgType, STR(MSG_SET_ROUTER_CHANNEL_RSP)) )
    {
        /* NO VAR BODY */
    }
    else if (strstr(szMsgType, STR(MSG_GET_IMG_LIST_RSP)))
    {
        pPickListBody = (HERT_WEBS_MSG_RSP_PICLIST_BODY*)pVarBody;
        
        //FORMAT_JSON_ITEM_LNG(szItem, pPickListBody->imgCount, imgCount);    /* imgCount */
        //ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        if(GetPictureListStatus == 0)
        {
            ADD_STR_TO_JSON(pData, ",\"",nBufferLen);
            ADD_STR_TO_JSON(pData, STR(imgList),nBufferLen);
            ADD_STR_TO_JSON(pData, "\":",nBufferLen);
            ADD_STR_TO_JSON(pData, "[",nBufferLen);
        }

        pPicInfo = pPickListBody->pPicList;
        for(i = 0; i < infoNumbers; i++)
        {
            if ( i == 0 && GetPictureListStatus == 0)
            {
                ADD_STR_TO_JSON(pData, "{",nBufferLen);
            }
            else
            {
                ADD_STR_TO_JSON(pData, ",{",nBufferLen);
            }
            //pPicInfo = pPicInfo + i;

            FORMAT_JSON_ITEM_STR(szItem, pPicInfo->picUrl, picUrl);             /* picUrl */
            if (nBufferLen > strlen(pData) + strlen(szItem))
            {
                snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
            }
            else
            {
                HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem), szItem);
                return 1;
            }
        
            FORMAT_JSON_ITEM_STR(szItem, pPicInfo->smallPicUrl, smallPicUrl);    /* smallPicUrl */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            ADD_STR_TO_JSON(pData, "}",nBufferLen);
            pPicInfo ++;
        }
        
        if(nErrorCode != 0)
        {
            ADD_STR_TO_JSON(pszOutputData, "]",nBufferLen);
        }
        
        if(GetPictureListStatus == 0)
            GetPictureListStatus = 1;   
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_FAULT_DETEC_RSP)))
    {
        pFaultDectBody = (HERT_WEBS_MSG_RSP_FAULTDECT_BODY*)pVarBody;

        ADD_STR_TO_JSON(pData, ",\"",nBufferLen);
        ADD_STR_TO_JSON(pData, STR(faultList),nBufferLen);
        ADD_STR_TO_JSON(pData, "\":",nBufferLen);
        ADD_STR_TO_JSON(pData, "[",nBufferLen);

        pFaultItem = pFaultDectBody->pFaultItem;
        for(i = 0; i < pFaultDectBody->dwFaultNumber; i++)
        {
            if ( i == 0 )
            {
                ADD_STR_TO_JSON(pData, "{",nBufferLen);
            }
            else
            {
                ADD_STR_TO_JSON(pData, ",{",nBufferLen);
            }
            //pFaultItem = pFaultItem + i;

            FORMAT_JSON_ITEM_STR(szItem, pFaultItem->faultType, faultType);             /* faultType */
            if (nBufferLen > strlen(pData) + strlen(szItem))
            {
                snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
            }
            else
            {
                HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem), szItem);
                return 1;
            }
        
            FORMAT_JSON_ITEM_STR(szItem, pFaultItem->faultReason, faultReason);        /* faultReason */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pFaultItem->faultProcess, faultProcess);      /* faultProcess */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pFaultItem->faultAllSteps, faultAllSteps);    /* faultAllSteps */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            ADD_STR_TO_JSON(pData, "}",nBufferLen);
            pFaultItem++;
        }
        ADD_STR_TO_JSON(pData, "]",nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_TOADD_DEV_RSP)))
    {
        pDevToAddBody = (HERT_WEBS_MSG_RSP_DEVTOADD_BODY*)pVarBody;

        ADD_STR_TO_JSON(pData, ",\"",nBufferLen);
        ADD_STR_TO_JSON(pData, STR(subDevToAddList),nBufferLen);
        ADD_STR_TO_JSON(pData, "\":",nBufferLen);
        ADD_STR_TO_JSON(pData, "[",nBufferLen);

        pDevItem = pDevToAddBody->pDevItem;
        for(i = 0; i < pDevToAddBody->dwDevNumber; i++)
        {
            if ( i == 0 )
            {
                ADD_STR_TO_JSON(pData, "{",nBufferLen);
            }
            else
            {
                ADD_STR_TO_JSON(pData, ",{",nBufferLen);
            }
            //pDevItem = pDevItem + i;

            FORMAT_JSON_ITEM_STR(szItem, pDevItem->devName, devName);             /* devName */
            if (nBufferLen > strlen(pData) + strlen(szItem))
            {
                snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
            }
            else
            {
                HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem), szItem);
                return 2;
            }
        
            FORMAT_JSON_ITEM_STR(szItem, pDevItem->devType, devType);            /* devType */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pDevItem->devID, devID);                /* devID */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pDevItem->mac, mac);                    /* Mac */
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            ADD_STR_TO_JSON(pData, "}",nBufferLen);
            pDevItem++;
        }
        ADD_STR_TO_JSON(pData, "]",nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_TERMINAL_STATUS_RSP)))
    {
        pInterRules = (HERT_WEBS_MSG_RSP_TERMINAL_STATUS_BODY*)pVarBody;

        FORMAT_JSON_ITEM_INT(szItem, pInterRules->accessControl, accessControl);    /* accessControl */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        
        FORMAT_JSON_ITEM_INT(szItem, pInterRules->netLimit, netLimit);    /* netLimit */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        pInterRulesTime = pInterRules->netTime;

        ADD_STR_TO_JSON(pData, ",\"",nBufferLen);
        ADD_STR_TO_JSON(pData, STR(netTime),nBufferLen);
        ADD_STR_TO_JSON(pData, "\":",nBufferLen);
        ADD_STR_TO_JSON(pData, "[",nBufferLen);

        ADD_STR_TO_JSON(pData, "{",nBufferLen);

        FORMAT_JSON_ITEM_STR(szItem, pInterRulesTime->startTime, startTime);  
        if (nBufferLen > strlen(pData) + strlen(szItem))
        {
            snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
        }
        else
        {
            HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem), szItem);
            return 1;
        }
        
        FORMAT_JSON_ITEM_STR(szItem, pInterRulesTime->endTime, endTime); 
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

        ADD_STR_TO_JSON(pData, "}",nBufferLen);
        
        if(pInterRules->periodNum == 2)
        {
        	  pInterRulesTime++;
        	  
            ADD_STR_TO_JSON(pData, ",",nBufferLen);
            ADD_STR_TO_JSON(pData, "{",nBufferLen);

            FORMAT_JSON_ITEM_STR(szItem, pInterRulesTime->startTime, startTime);  
            if (nBufferLen > strlen(pData) + strlen(szItem))
            {
                snprintf(pData + strlen(pData), nBufferLen - strlen(pData), "%s", szItem);
            }
            else
            {
                HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nBufferLen, pData, strlen(pData), strlen(szItem), szItem);
                return 1;
            }
        
            FORMAT_JSON_ITEM_STR(szItem, pInterRulesTime->endTime, endTime); 
            ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);

            ADD_STR_TO_JSON(pData, "}",nBufferLen);
        }
        
        
        ADD_STR_TO_JSON(pData, "]",nBufferLen);
    }//wifi 信号强度
    else if (strstr(szMsgType, STR(MSG_GET_WIFI_STRENGTH_RSP)))
    {
	      FORMAT_JSON_ITEM_INT(szItem, *pVarBody, wifiStrength);    /* wifi 信号强度 */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
	
	  }
    else if (strstr(szMsgType, STR(MSG_GET_VISITOR_MANAGEMENT_RSP)))
    {
        pGuestManege = (HERT_WEBS_MSG_RSP_VAR_GUEST_MANAGE_BODY*)pVarBody;
        FORMAT_JSON_ITEM_INT(szItem, pGuestManege->wifiSwitch, wifiSwitch);    /* wifiSwitch */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);    
        FORMAT_JSON_ITEM_INT(szItem, pGuestManege->passwordSwitch, passwordSwitch);    /* passwordSwitch */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen); 
        FORMAT_JSON_ITEM_STR(szItem, pGuestManege->password, password);    /* password */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_INT(szItem, pGuestManege->openTime, openTime);    /* openTime */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
        FORMAT_JSON_ITEM_INT(szItem, pGuestManege->remainTime, remainTime);    /* remainTime */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);        
    }
    else if (strstr(szMsgType, STR(MSG_GET_CURRENT_ROUTER_CHANNEL_RSP)))
    {
        FORMAT_JSON_ITEM_STR(szItem, pVarBody, channel);    /* channel */
        ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
    }
	
    if(strstr(szMsgType, STR(MSG_GET_IMG_LIST_RSP)))
    {
        /*don't add "}" here*/
        if(nErrorCode != 0)
        {
            ADD_STR_TO_JSON(pData, "}",nBufferLen);
        }
    }
    else
    {
        ADD_STR_TO_JSON(pData, "}",nBufferLen);
    }
 
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    return ret;
}

/* Json字符串解析，该函数的使用方法请参考Parse_Msg_BodyConstPartRsp函数。
CHAR *ParseJsonString(CHAR *pData, CHAR *pszItemName, CHAR *pszItemValue);

*/
int websHertAppPro_GetStatus(HERT_WEBS_MSG_RSP_STATUS_BODY *pRspBody)
{
    int ret = 0;

    ret = hertUtil_GetDeviceList(&pRspBody->pDev, &pRspBody->dwDevNumber);
    if(ret)
    {
        HERT_LOGINFO("hertUtil_GetDeviceList was failed!");
    }

    if(strncmp(herouterStatus.pingStatus,STRING_PASS,strlen(STRING_PASS)) == 0 )
    {
        pRspBody->devStatus = 0;
    }
    else
    {
        pRspBody->devStatus = 1;
    }
    pRspBody->runStatus = hertUtil_getWorkStatus();
    pRspBody->downbandwidth  = hertUtil_getBroadband();
    pRspBody->wifiStatus = (hertUtil_isWifiOn() == 1) ? 0 : 1;

    return ret;
}

/*
*   功能：
*       3.1	获取路由器工作状态
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetRouteStatus(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    char    szBody[64];
    HERT_WEBS_MSG_RSP_STATUS_BODY tRspBody;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(szBody, 0x0, sizeof(szBody));
        goto GetRouterStatus_Rsp;
    }
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(szBody, 0x0, sizeof(szBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetRouterStatus_Rsp;
    }
    
    memset(tRspBody.broadbandRate,0x0,sizeof(tRspBody.broadbandRate)); 
    
    /* process message data　*/
    ret = websHertAppPro_GetStatus(&tRspBody);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_GetStatus(%d)", ret);
        /* no return, for we need response the error code to app */
        ret = HERT_ERR_INTERNEL_FALSE;
    }

GetRouterStatus_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_STATUS_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
          
        ret = HERT_ERR_INTERNEL_FALSE; 
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_STATUS_RSP),pszOutputData, nBufferLen,&tReqBody);       
        if (tRspBody.pDev)
        {
            free(tRspBody.pDev);
        }  
        return ret;        
    }

    /* free memory source */
    if (tRspBody.pDev)
    {
        free(tRspBody.pDev);
    }
    return ret;
}


/*
*   功能：
*       3.2	获取路由器SN号码
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetRouteSN(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    char    szBody[64];
    char    szDevSN[64];
    char    if_mac[32];

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(szDevSN, 0x0, sizeof(szDevSN));
        goto GetRouteSN_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(szDevSN, 0x0, sizeof(szDevSN));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetRouteSN_Rsp;
    }
    
    if ( hertUtil_readMac(if_mac) <= 0 )
    {
        ret = HERT_ERR_INTERNEL_FALSE;
        goto GetRouteSN_Rsp;
    }

    /* process message data　*/
    sprintf(szDevSN, "%s%02X%02X%02X%02X%02X%02X", hertUtil_getSN(),0xff & if_mac[0], 0xff & if_mac[1],0xff & if_mac[2],\
                  0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5]);
    //sprintf(szDevSN, "%s","ABCDA12B123010F2");

GetRouteSN_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_SN_RSP), szDevSN, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_SN_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*       3.3	获取路由器的设备密码
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetRoutePassword(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    #define HASH_SIZE   16
    #define MD5PWD_SIZE   130

    int     ret = 0,enptPwdLength = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_DEVPASS_BODY tDevPassBody;
    char    szPass[130];
    char    szPassword[130];
    char    encryptPassword[256] = {0};
    char    BaseRspPass[MD5PWD_SIZE] = {0};
    char    onenetPassword[130] = {0};	
    char    base64Password[130] = {0};	
    
    /*MD5 define*/
    unsigned char *buf;
    MD5_CONTEXT     md5ctx;
    unsigned char   hash[HASH_SIZE] = {0};
    char      *r, *strReturn;
    char      result[(HASH_SIZE * 2) + 1];    
    char      MD5Temp[MD5PWD_SIZE] = {0};
    char      MD5AdminPwd[MD5PWD_SIZE] = {0};
    char      Base64AdminPwd[MD5PWD_SIZE] = {0};
    char      *ptr = NULL;
    int       i;
    int       cbEncryptMsg = 0;
 
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tDevPassBody, 0x0, sizeof(tDevPassBody));
        goto GetRoutePassword_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(szPass, 0x0, sizeof(szPass));
    memset(&tDevPassBody, 0x0, sizeof(tDevPassBody));
    HERT_AES_DEBUG(HERT_LOGERR("<<<<<<<<<<<<<<<<<pszSrcBody:%s<<<<<<<<<<<<<<<<<\n", pszSrcBody));

    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tDevPassBody, sizeof(tDevPassBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetRoutePassword_Rsp;
    }

    if(strlen(tDevPassBody.adminPassword) < 1){
        HERT_LOGINFO("######## the length of adminPassword is 0 ########\n"); 	
        memset(tDevPassBody.adminPassword,0x0,sizeof(tDevPassBody.adminPassword));
        strcpy(tDevPassBody.adminPassword,MD5Password);
        HERT_LOGINFO("######## new tDevPassBody.adminPassword:%s ########\n",tDevPassBody.adminPassword); 
    }
    
    /* process message data　*/
    memset(szPassword, 0x0, sizeof(szPassword));
    sprintf(szPassword, "%s", hertUtil_getPassword());
    HERT_AES_DEBUG(HERT_LOGERR("szPassword(%s)", szPassword));

    //tDevPassBody->szPass,TBD.


    /* *	Take the MD5 hash of the string argument. */    
    MD5Init(&md5ctx);     
    MD5Update(&md5ctx, szPassword, (unsigned int)strlen(szPassword));    
    MD5Final(hash, &md5ctx);

    ptr = MD5Temp;
    if(strlen(tDevPassBody.random)>0 && strlen(tDevPassBody.adminPassword)>0)
    {
        strcpy(ptr,tDevPassBody.random);				
        ptr += strlen(tDevPassBody.random);
    
        snprintf(ptr, MD5PWD_SIZE - strlen(tDevPassBody.random), "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", \
                 hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
                 hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);

        memset(hash,0x0,HASH_SIZE);
        memset(md5ctx.buffer,0x0,64);

        HERT_AES_DEBUG(HERT_LOGERR("######## MD5Temp:%s ########\n",MD5Temp));

        MD5Init(&md5ctx);  
        MD5Update(&md5ctx, MD5Temp, (unsigned int)strlen(MD5Temp));
        MD5Final(hash, &md5ctx);

        snprintf(MD5AdminPwd, MD5PWD_SIZE, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", \
                 hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
                 hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);

        HERT_AES_DEBUG(HERT_LOGERR("######## MD5AdminPwd:%s ########\n",MD5AdminPwd));

        websEncode64(Base64AdminPwd,MD5AdminPwd,MD5PWD_SIZE);

        HERT_AES_DEBUG(HERT_LOGERR("######## Base64AdminPwd:%s ########\n",Base64AdminPwd));


        if(strncmp(tDevPassBody.adminPassword,Base64AdminPwd,strlen(Base64AdminPwd)) == 0)
        {
            
            //const char *nvram_tmp = nvram_get(RT2860_NVRAM, "AES_PWD_FROM_ONENET");	
            const char *nvram_tmp = hertUtil_GetAESPwdFromOnenet();	
            
            HERT_AES_DEBUG(HERT_LOGERR("######## password from AES_PWD_FROM_ONENET:%s########\n",nvram_tmp));
            
            memset(szPassword,0x0,sizeof(szPassword));      
            if(nvram_tmp != NULL)
            {
                //decode base 64
                strcpy(base64Password,nvram_tmp);
                websDecode64(onenetPassword, base64Password, sizeof(onenetPassword) - 1);
                HERT_AES_DEBUG(HERT_LOGERR("######## onenetPassword:%s########\n",onenetPassword));

                /*Decrypt aes 128,comment for cann't Decrypt AES 128 from onenet success.
                hertUtil_aes_ecb_128_Decrypt(aesPassword,strlen(aesPassword),szPassword);
                HERT_LOGINFO("######## Decrypt_AES(%d):%s########\n",strlen(szPassword),szPassword);
                */
            }
            
            if(strlen(onenetPassword) > 0)
            {
                memset(szPass,0x0,sizeof(szPass));  
                hertUtil_aes_ecb_128_Encrypt(onenetPassword, strlen(onenetPassword), szPass, &cbEncryptMsg);
                HERT_LOGERR("######## encryptPassword(%d)/(%d):%s########\n",strlen(szPass), cbEncryptMsg, szPass);
                herUtil_dumpAesHexString(__FUNCTION__, __LINE__, szPass, cbEncryptMsg); 
                if (1)
                {
                    unsigned long outlen = MD5PWD_SIZE;
                    cbEncryptMsg = (cbEncryptMsg == 32) ? cbEncryptMsg/2 : cbEncryptMsg;
                    hertUtil_base64_encode(szPass,cbEncryptMsg, BaseRspPass, &outlen);
                    herUtil_dumpAesHexString(__FUNCTION__, __LINE__, BaseRspPass, outlen); 
                }
                else
                {
                    websEncode64(BaseRspPass,szPass,MD5PWD_SIZE);
                    herUtil_dumpHexString1(__FUNCTION__, __LINE__, BaseRspPass, strlen(BaseRspPass));
                }
                HERT_AES_DEBUG(HERT_LOGERR("######## BaseRspPass:%s########\n",BaseRspPass));
                ret = 0;
             }
             else
             {
                ret = HERT_ERR_INTERNEL_FALSE; 
             }	
        }
        else
        {
            ret = HERT_ERR_AUTH_FAILED;
        }
    }
    else
    {
        ret = HERT_ERR_INVALID_JSON;
    }

GetRoutePassword_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_PASSWORD_RSP), BaseRspPass, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_PASSWORD_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret; 
}

/*
*   功能：
*       3.4	获取路由器剩余磁盘空间
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetRouteSpace(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_RSP_SPACE_BODY tRspBody;
    char    szBody[64] = {0};

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto GetRouteSpace_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetRouteSpace_Rsp;
    }

    /* process message data　*/
    if (!hertUtil_isStorageExist())
    {
        HERT_LOGINFO("The storage is not exist!");
        tRspBody.diskStatus = 1;
    }
    else
    {
        HERT_LOGINFO("The storage is exist!");
        tRspBody.diskStatus = 0;

        hretUtil_GetUSBSize(&tRspBody.leftSpace,
                                &tRspBody.totalSpace);

        tRspBody.usedSpace = tRspBody.totalSpace - tRspBody.leftSpace;        /* 已用空间, 单位MB */
        HERT_LOGINFO("tRspBody.usedSpace(%lld)", tRspBody.usedSpace);
    }
    
GetRouteSpace_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_SPACE_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_SPACE_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*       3.5	获取路由器属性
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetRouteAbility(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_RSP_ABILITY_BODY tRspBody;
    char    szBody[64];
    FILE* fr = NULL;
    char heDevName[] = { 0x54, 0x8C, 0x8D, 0xEF, 0x75, 0x31, 0x0};
    /* 发送数据-BODY_REPONSE_VAR_GET_UPDATE */
    typedef struct tagRspPushDataVarPart_GetUpdate
    {
        CHAR  needUpdate;                /* 是否需要升级, 0：无需升级, 1：强制升级, 2：非强制升级 */
        CHAR  updateUrl[256];            /* 升级文件URL, 无需升级时，无此字段 */
        CHAR  updateDescription[256];    /* 升级原因说明, 各项说明通过竖线（|）分割 */
    }HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE;
    HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE tmpData;
#define TEMP_NEEDUPGRADEINFO "/var/upgradeinfo"

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto GetRouteAbility_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetRouteAbility_Rsp;
    }

    /* process message data　*/
    strcpy(tRspBody.devName, heDevName);     /* 设备名称 */
    strcpy(tRspBody.fac, "DARE");              /* 设备厂家 */
    strcpy(tRspBody.type, hertUtil_getDeviceType());            /* 设备型号 */
    sprintf(tRspBody.softVersion, "%s", hertUtil_getFirmwareVersion());   /* 固件版本号 */
    tRspBody.softUpdate = hertUtil_getIsNeedUpgrade();
    // tRspBody.softUpdateDis
    fr  = fopen(TEMP_NEEDUPGRADEINFO, "rb");
    if ( fr)
    {
        HERT_LOGINFO("Failed to open TOADDDEVFILE(%s)\n", TEMP_NEEDUPGRADEINFO);
        if ( fread(&tmpData, 1, sizeof(tmpData), fr) == sizeof(tmpData))
        {
            tRspBody.softUpdate = tmpData.needUpdate;
        }
        else
        {
            HERT_LOGINFO("TOADDDEVFILE(%s)\n", TEMP_NEEDUPGRADEINFO);
        }
        fclose(fr);
    }

GetRouteAbility_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_ABILITY_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_ABILITY_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*       3.6	获取图片上传地址
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GeUploadPicture(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_UPLOAD_PIC_BODY tUploadPicture;
    HERT_WEBS_MSG_RSP_UPLOAD_IMG_BODY tRspBody;
    DDWORD  freeSpace = 0; 
    char lan_if_addr[16] = {0};
    char    pathName[256] = {0};

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tUploadPicture, 0x0, sizeof(tUploadPicture));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto UploadPicture_response;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tUploadPicture, 0x0, sizeof(tUploadPicture));
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tUploadPicture, sizeof(tUploadPicture));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto UploadPicture_response;
    }

    /* process message data　*/
    if (!hertUtil_isStorageExist())
    {
        HERT_LOGINFO("The storage is not exist!");
        ret = HERT_ERR_DISK_NOEXIST;
        goto UploadPicture_response;
    }
    else
    {
        HERT_LOGINFO("The storage is exist!");
        hertUtil_GetUSBPartitionMaxFreeSize(&freeSpace, pathName, sizeof(pathName));

    }

    if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
        HERT_LOGINFO("can't get lan ip.\n");
        ret = HERT_ERR_INTERNEL_FALSE;
        goto UploadPicture_response;
    }

    if(freeSpace > (((long long)tUploadPicture.fileSize)/(long long)1024))
    {
        snprintf(tRspBody.picUrl, sizeof(tRspBody.picUrl), "http://%s:80/herouter/AppRequest/UploadIMG/%s",lan_if_addr, tUploadPicture.fileName);
        ret = 0;
    }
    else
    {
        HERT_LOGINFO("free size is(%lld) too small for upload pic",freeSpace);
        ret = HERT_ERR_DISK_NOENOUGH;
    }
    
    //tUploadPicture->tRspBody,TBD.
UploadPicture_response:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_UPLOAD_IMG_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_UPLOAD_IMG_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*       3.8	获取路由器照片下载地址
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/

int websHertAppPro_GeUpDownloadList(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
  int     ret = 0;
	HERT_WEBS_MSG_REQ_BODY tReqBody;
	HERT_WEBS_MSG_RSP_PICLIST_BODY tRspBody;
	HERT_WEBS_MSG_RSP_PICINFO_BODY *tempRspBody = NULL;
	char    szBody[64];
	char    picPath[32] = {0};
	char    cmdBuf[64] = {0};
	char    getsLine[64] = {0};
	FILE    *fp = NULL;
	int     Picture_Num = 0;
	char lan_if_addr[16] = {0},pathName[128] = {0};;
	long long freeSize = 0;

	if ( (!pszSrcBody) || (!pszOutputData))
	{
	    HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
	                pszSrcBody, pszOutputData);
	    ret = HERT_ERR_INVALID_JSON;
	    memset(&tReqBody, 0x0, sizeof(tReqBody));
	    memset(&tRspBody, 0x0, sizeof(tRspBody));
	    goto DownloadList_response;
	}
	memset(&tReqBody, 0x0, sizeof(tReqBody));
	memset(&tRspBody, 0x0, sizeof(tRspBody));
	
	GetPictureListStatus = 0;
	infoNumbers = 0;
	/* parse message data　*/
	ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
	if (ret)
	{
	    HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
	    ret  = HERT_ERR_INVALID_JSON;
	    goto DownloadList_response;
	}
	
	/* process message data　*/
	if (!hertUtil_isStorageExist())
	{
	    HERT_LOGINFO("The storage is not exist!");
	    ret = HERT_ERR_DISK_NOEXIST;
	    tRspBody.imgCount  = 0;
	    goto DownloadList_response;
	}
	else
	{
	    HERT_LOGINFO("The storage is exist!");
		  hretUtil_GetUSBPartitionName(pathName, sizeof(pathName));
	    snprintf(picPath,sizeof(picPath),"%s/picture/",pathName);

	}
	
	snprintf(cmdBuf,sizeof(cmdBuf),"ls %s |cat >> /var/count_UsbImg",picPath);
    
		
	system(cmdBuf);
	
	
	if (NULL == (fp = fopen("/var/count_UsbImg", "r"))) {
	    HERT_LOGINFO("isWanPortConnect: open /var/count_UsbImg error\n");
	    ret  = HERT_ERR_INTERNEL_FALSE; // 1;
	    tRspBody.imgCount  = 0;
	    goto DownloadList_response;
	}
	else
	{
	    int getLen = 0;
			  
	    if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
	    	  HERT_LOGINFO("WPSRestart error, can't get lan ip.\n");
	    	  fclose(fp);
	    	  unlink("/var/count_UsbImg");
	    	  ret = HERT_ERR_INTERNEL_FALSE;
	    	  goto DownloadList_response;
	    }

		  //fseek(fp, 0L, SEEK_SET);
		  tRspBody.imgCount = 0;
		  if((tRspBody.pPicList = (HERT_WEBS_MSG_RSP_PICLIST_BODY*)calloc(MAX_PICTURE_NUMBER ,sizeof(struct tagWebsMsgRspPicInfoBody))) == NULL)
		  {
		      HERT_LOGERR("faild to calloc memory!!");
		      ret  = HERT_ERR_INTERNEL_FALSE;
		      fclose(fp);
		      unlink("/var/count_UsbImg");
		      goto DownloadList_response;
		  }

		  //memset(tRspBody.pPicList,0x0,sizeof(struct tagWebsMsgRspPicInfoBody)*Picture_Num + 1);
		
	    tempRspBody = tRspBody.pPicList; 
	    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	    while (fgets(getsLine, PICTURE_URL_SIZE, fp))
	    {
	        /*getsLine - 1 means remove the char of '\n'*/
	        getLen = strlen(getsLine);
	        getsLine[getLen-1] = '\0';
	        
	        memset(tempRspBody->picUrl,0x0,PICTURE_URL_SIZE);	
	        snprintf(tempRspBody->picUrl,PICTURE_URL_SIZE,"http://%s:80/herouter/AppRequest/DownloadIMG/%s",lan_if_addr,getsLine);
	        memset(tempRspBody->smallPicUrl,0x0,PICTURE_URL_SIZE);	
	        snprintf(tempRspBody->smallPicUrl,PICTURE_URL_SIZE,"empty");
	        
	        Picture_Num ++;
	        infoNumbers ++;
	        
	        memset(getsLine, 0x0, sizeof(getsLine));
	        //every 3 pictures info ,send in one reponse 
	        if(infoNumbers == 3){
	        	  ret = 0;
	        	  
	            websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_IMG_LIST_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
	            websWrite(wp, T("%s"),pszOutputData);
     
	            infoNumbers = 0;
	            tempRspBody = tRspBody.pPicList;
	        }
	        else{
	            tempRspBody ++;	
	        }
	     }
	     
	      //FORMAT_JSON_ITEM_LNG(szItem, pPickListBody->imgCount, imgCount);    /* imgCount */
        //ADD_ITEM_TO_JSON(pData, szItem,nBufferLen);
       fclose(fp);
       unlink("/var/count_UsbImg");
       
       if(Picture_Num > 0)
       {
           isAlreadyResponse = 1;	
           if(infoNumbers > 0)
           {
               ret = 0;
               websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_IMG_LIST_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
               websWrite(wp, T("%s"),pszOutputData);
           }
           else
           {
               /*do nothing*/	
           }
	   	     
           /*add imgCount at last*/
           memset(cmdBuf,0x0,sizeof(cmdBuf));
           snprintf(cmdBuf,sizeof(cmdBuf),",\"imgCount\":%d",Picture_Num);
	   	     
           memset(pszOutputData,0x0,nBufferLen);
           ADD_STR_TO_JSON(pszOutputData, "]",nBufferLen);
           ADD_STR_TO_JSON(pszOutputData, cmdBuf,nBufferLen);
           ADD_STR_TO_JSON(pszOutputData, "}",nBufferLen);
           
           websWrite(wp, T("%s"),pszOutputData);
           websDone(wp, 200);
           
           if(tRspBody.pPicList)
               free(tRspBody.pPicList);
           return 0;
       }
       else
       {  
	   	     ret = HERT_ERR_PICTURE_NOEXIST;
	   	     HERT_LOGERR("No Picture Find!!!(%d)", ret);
	   	     goto DownloadList_response;
       }   
	}
	
DownloadList_response:
	/* build message data　*/
	ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_IMG_LIST_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
	if (ret)
	{
      HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
      free(tRspBody.pPicList);
      ret = HERT_ERR_INTERNEL_FALSE;
      hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_IMG_LIST_RSP),pszOutputData, nBufferLen,&tReqBody);         
      if(tRspBody.pPicList)
	        free(tRspBody.pPicList);
      return ret;  
	}
	if(tRspBody.pPicList)
	    free(tRspBody.pPicList);
	return ret;
}


/*
*   功能：
*       3.10	路由器固件升级
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_RouteUpgrade(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0x6005;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_UPGRADE_BODY tUpgradeBody;
    char    szBody[64];
    char    szCmd[128];
    int     bDoNext = 1;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tUpgradeBody, 0x0, sizeof(tUpgradeBody));
        goto RouteUpgrade_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tUpgradeBody, 0x0, sizeof(tUpgradeBody));
    memset(szCmd, 0x0, sizeof(szCmd));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tUpgradeBody, sizeof(tUpgradeBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto RouteUpgrade_Rsp;
    }
    HERT_LOGINFO("let herouteapp to download the firmware from upgarde file server\n");

    /* check it is already upgrading...? */
    if (g_upgradeState == 1)
    {
        ret = HERT_ERR_UPGRADE_PROCESS;
        goto RouteUpgrade_Rsp;
    }

    /* check it is just now upgraded ? */
    if (hertUtil_getHerouteUpImg() == 1)
    {
        ret = 0;
        system("nvram_set HE_ROUTE_UPIMAG 0");
        goto RouteUpgrade_Rsp;
    }
	
    /* let herouteapp to download the firmware from upgarde file server　*/
    system("rm -rf /var/upgradenow");
    system("rm -rf /var/fmdownresult");

    /* check it is need do upgrade ? */
    if (hertUtil_getIsNeedUpgrade())
    {
        sprintf(szCmd, "echo downloadnow > /var/downloadnow");
        system(szCmd);
    }
    else
    {
        bDoNext = 0;
        ret = HERT_ERR_UPGRADE_NONEED;
    }

    HERT_LOGINFO("bDoNext(%d), wait for herouteapp to download the firmware from upgarde file server\n", bDoNext);
    /* wait for herouteapp to download the firmware from upgarde file server　*/
    while(bDoNext)
    {
        if (hertUtil_IsInFile("/var/fmdownresult", "faileddownfm"))
        {
            ret = HERT_ERR_UPGRADE_DOWN;
            break;
        }
        else if (hertUtil_IsInFile("/var/fmdownresult", "successdownfm"))
        {
            ret = HERT_ERR_UPGRADE_PROCESS;
            g_upgradeState = 1; /* it is waiting for upgrade for the image file was downloaded */
            break;
        }
        sleep(1);
    }

RouteUpgrade_Rsp:
    HERT_LOGINFO("websHertAppPro_BuildMsgRsp\n");
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_ROUTER_UPDATE_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_ROUTER_UPDATE_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    HERT_LOGINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    return ret;
}

/*
*   功能：
*       3.11	路由器网络设置
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetNetWorkConfig(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_SETNET_BODY tNetSetBody;
    char    szBody[130] = {0};
    char    szPassword[130] = {0};
    
    /*define MD5*/
    #define HASH_SIZE   16
    #define MD5PWD_SIZE   130
    int length = 0;
    MD5_CONTEXT		md5ctx;    
    unsigned char	hash[HASH_SIZE] = {0};    
    char      MD5Temp[MD5PWD_SIZE] = {0};
    char      MD5AdminPwd[MD5PWD_SIZE] = {0};
    char      Base64AdminPwd[MD5PWD_SIZE] = {0};
    char      *ptr = NULL;
    
    
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tNetSetBody, 0x0, sizeof(tNetSetBody));
        goto SetNetWork_Rsp;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tNetSetBody, 0x0, sizeof(tNetSetBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tNetSetBody, sizeof(tNetSetBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetNetWork_Rsp;
    }

    sprintf(szPassword, "%s", hertUtil_getPassword());
    HERT_LOGINFO("szPassword(%s)", szPassword);

    /* process message data　*/

    /*MD5 START*/
    length = strlen(szPassword);

    /* * Take the MD5 hash of the string argument. */    
    MD5Init(&md5ctx);    
    MD5Update(&md5ctx, szPassword, (unsigned int)length);    
    MD5Final(hash, &md5ctx);

    memset(MD5Temp,0x0,sizeof(MD5Temp));
    ptr = MD5Temp;
    if(strlen(tNetSetBody.random)>0 &&  strlen(tNetSetBody.adminPassword)>0)
    {
        strcpy(ptr,tNetSetBody.random);
        ptr += strlen(tNetSetBody.random);

        HERT_LOGINFO("######## MD5Temp-11:%s  ########\n",MD5Temp);

        snprintf(ptr, MD5PWD_SIZE - strlen(tNetSetBody.random), "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", \
                 hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
                 hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);

        memset(hash,0x0,HASH_SIZE);
        memset(md5ctx.buffer,0x0,64);

        HERT_LOGINFO("######## MD5Temp:%s  ########\n",MD5Temp);

        MD5Init(&md5ctx);  
        MD5Update(&md5ctx, MD5Temp, (unsigned int)strlen(MD5Temp));  
        MD5Final(hash, &md5ctx);

        snprintf(MD5AdminPwd, MD5PWD_SIZE, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", \
                 hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],\
                 hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15]);

        HERT_LOGINFO("######## MD5AdminPwd:%s  ########\n",MD5AdminPwd);

        websEncode64(Base64AdminPwd,MD5AdminPwd,MD5PWD_SIZE);

        HERT_LOGINFO("######## Base64AdminPwd:%s  ########\n",Base64AdminPwd);

        if(strncmp(Base64AdminPwd,tNetSetBody.adminPassword,strlen(Base64AdminPwd)) == 0)
        {
            /*Need handle password with aes and base64*/
            ret = hertUtil_setNetConfig4App(&tNetSetBody);
            if(ret == 0){
                ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_NETWORK_RSP), szBody, ret, pszOutputData, nBufferLen);

                websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
                websWrite(wp, T("%s\n"),pszOutputData);
                websDone(wp, 200);
				//wizard complete

				nvram_bufset(RT2860_NVRAM, "needWizard", "2");
                nvram_commit(RT2860_NVRAM);
                initInternet();	
                isAlreadyResponse = 1;
                return 0;
            }
        }
        else
        {
            //ret = HERT_ERR_AUTH_FAILED;
        }
    }
    /*MD5 END*/
    else{
        ret = HERT_ERR_INVALID_JSON;
    }

    //tNetSetBody,TBD.
SetNetWork_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_NETWORK_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_NETWORK_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}

/*
*   功能：
*       3.12	路由器故障检测
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GeFaultDetect(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int   ret = 0,faultdext_Bbody_size = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_FAULTDECT_BODY tFaultDectBody;
    HERT_WEBS_MSG_RSP_FAULTDECT_BODY tRspBody;
    HERT_WEBS_MSG_RSP_FAULTITEM_BODY tempBody,*itemBody;
    int   usb_status = 0;
    char wanPortStatus[8] = {0},siteStatus[8] = {0},WanIpStatus[8] = {0},PPPStatus[8] = {0};
    const char *contype;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tFaultDectBody, 0x0, sizeof(tFaultDectBody));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto GeFaultDetect_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tFaultDectBody, 0x0, sizeof(tFaultDectBody));
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tFaultDectBody, sizeof(tFaultDectBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GeFaultDetect_Rsp;
    }

    /*max fault item is 2*/
    tRspBody.dwFaultNumber = 0;
    /*faultType[32]+faultReason[128]+faultProcess[32]+faultAllSteps[512]*/
    faultdext_Bbody_size = sizeof(tempBody.faultType) + sizeof(tempBody.faultReason) + \
                                     sizeof(tempBody.faultProcess) + sizeof(tempBody.faultAllSteps);
    tRspBody.pFaultItem = (HERT_WEBS_MSG_RSP_FAULTDECT_BODY *)malloc(faultdext_Bbody_size*2 + 1);
    if (!tRspBody.pFaultItem)
    {
        HERT_LOGERR("No memory!!!!\n");
        ret = HERT_ERR_INTERNEL_FALSE;
        goto GeFaultDetect_Rsp;
    }
    memset(tRspBody.pFaultItem,0x0,faultdext_Bbody_size*2 + 1);
    itemBody = tRspBody.pFaultItem;
    /*support network detecte only at nowtime*/
    if ( (tFaultDectBody.faultType == 1) || (tFaultDectBody.faultType == 0) )
    {
        
        contype = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
        
        strcpy(PPPStatus,herouterStatus.PPPOEStatus);	
        strcpy(wanPortStatus,herouterStatus.wanPortStatus);
        strcpy(WanIpStatus,herouterStatus.WanIpStatus);
        if(strncmp(herouterStatus.pingStatus,STRING_PASS,strlen(STRING_PASS)) == 0 )
        {
                strcpy(siteStatus,STRING_PASS);
        }
        else
        {
                strcpy(siteStatus,STRING_FAIL);
        }
        
        usb_status = hertUtil_isStorageExist();
        
        if(strncmp(wanPortStatus,STRING_FAIL,strlen(STRING_FAIL)) == 0){
            memcpy(itemBody->faultType,Fault_1.faultType,sizeof(itemBody->faultType));    	
            memcpy(itemBody->faultReason,Fault_1.faultReason,sizeof(itemBody->faultReason)); 
            memcpy(itemBody->faultProcess,Fault_1.faultProcess,sizeof(itemBody->faultProcess)); 
            memcpy(itemBody->faultAllSteps,Fault_1.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
            tRspBody.dwFaultNumber++;
            itemBody ++;
        }
        else if(strncmp(WanIpStatus,STRING_FAIL,strlen(STRING_FAIL)) == 0)
        {
            //for DHCP	
            if(!strncmp(contype, "DHCP", 5))
            {
		            memcpy(itemBody->faultType,Fault_3.faultType,sizeof(itemBody->faultType));    	
		            memcpy(itemBody->faultReason,Fault_3.faultReason,sizeof(itemBody->faultReason)); 
		            memcpy(itemBody->faultProcess,Fault_3.faultProcess,sizeof(itemBody->faultProcess)); 
		            memcpy(itemBody->faultAllSteps,Fault_3.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
            }
            else
            {
            	  //check the username and the password if correct
                if(strncmp(PPPStatus,STRING_FAIL,strlen(STRING_FAIL)) == 0)
                {
		                memcpy(itemBody->faultType,Fault_5.faultType,sizeof(itemBody->faultType));    	
		                memcpy(itemBody->faultReason,Fault_5.faultReason,sizeof(itemBody->faultReason)); 
		                memcpy(itemBody->faultProcess,Fault_5.faultProcess,sizeof(itemBody->faultProcess)); 
		                memcpy(itemBody->faultAllSteps,Fault_5.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
		            }
		            else
		            {
		                memcpy(itemBody->faultType,Fault_6.faultType,sizeof(itemBody->faultType));    	
		                memcpy(itemBody->faultReason,Fault_6.faultReason,sizeof(itemBody->faultReason)); 
		                memcpy(itemBody->faultProcess,Fault_6.faultProcess,sizeof(itemBody->faultProcess)); 
		                memcpy(itemBody->faultAllSteps,Fault_6.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
		            }
            }    
            tRspBody.dwFaultNumber++;   
            itemBody ++;           	
        }
        else if(strncmp(siteStatus,STRING_FAIL,strlen(STRING_FAIL)) == 0)
        {
            if(!strncmp(contype, "DHCP", 5))
            {
		            memcpy(itemBody->faultType,Fault_4.faultType,sizeof(itemBody->faultType));    	
		            memcpy(itemBody->faultReason,Fault_4.faultReason,sizeof(itemBody->faultReason)); 
		            memcpy(itemBody->faultProcess,Fault_4.faultProcess,sizeof(itemBody->faultProcess)); 
		            memcpy(itemBody->faultAllSteps,Fault_4.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
		        }
		        else
		        {
		            memcpy(itemBody->faultType,Fault_7.faultType,sizeof(itemBody->faultType));    	
		            memcpy(itemBody->faultReason,Fault_7.faultReason,sizeof(itemBody->faultReason)); 
		            memcpy(itemBody->faultProcess,Fault_7.faultProcess,sizeof(itemBody->faultProcess)); 
		            memcpy(itemBody->faultAllSteps,Fault_7.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
		        }
		        tRspBody.dwFaultNumber++;
		        itemBody ++;
        }
        
        if(usb_status == 0){
		        memcpy(itemBody->faultType,Fault_2.faultType,sizeof(itemBody->faultType));    	
		        memcpy(itemBody->faultReason,Fault_2.faultReason,sizeof(itemBody->faultReason)); 
		        memcpy(itemBody->faultProcess,Fault_2.faultProcess,sizeof(itemBody->faultProcess)); 
		        memcpy(itemBody->faultAllSteps,Fault_2.faultAllSteps,sizeof(itemBody->faultAllSteps)); 
		        tRspBody.dwFaultNumber++;
        }
        
    }

	if ( (tFaultDectBody.faultType == 2) || (tFaultDectBody.faultType == 0) )
    {
        /*not support now*/
    }
	
    if ( (tFaultDectBody.faultType == 3) || (tFaultDectBody.faultType == 0) )
    {
        /*not support now*/
    }
    
    /* process message data　*/
    //tFaultDectBody->tRspBody,TBD.
GeFaultDetect_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_FAULT_DETEC_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_FAULT_DETEC_RSP),pszOutputData, nBufferLen,&tReqBody);    
        /* free memory source */
        if (tRspBody.pFaultItem)
        {
            free(tRspBody.pFaultItem);
        }
        return ret;  
    }
    
    /* free memory source */
    if (tRspBody.pFaultItem)
    {
        free(tRspBody.pFaultItem);
    }

    return ret;
}


/*
*   功能：
*       二期 4.2.1 终端访问状态查询
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetTerminalAccessStatus(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0,rst = 0,timeBodySize = 0;;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_TERMINAL_STATUS_BODY tTerBody;
    HERT_WEBS_MSG_RSP_TERMINAL_STATUS_BODY tRspBody;
    HERT_WEBS_MSG_TIME_PERIOD_INFO *timeBody,tempBody;
    int  accessControl = 0, netLimit = 1;
    char access_Tstart[32] = {0},access_Tend[32] = {0},paMac[32] = {0},*pRules = NULL;
    char *pstard1 = NULL,*pend1 = NULL,*pstard2 = NULL,*pend2 = NULL;
    const char *internetRules = nvram_get(RT2860_NVRAM, "InternetRules");	
    
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tTerBody, 0x0, sizeof(tTerBody));
        goto GetTerminalAccess_Rsp;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tTerBody, 0x0, sizeof(tTerBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tTerBody, sizeof(tTerBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetTerminalAccess_Rsp;
    }
    /* process message data　*/

    timeBodySize = sizeof(tempBody.startTime) + sizeof(tempBody.endTime);
    tRspBody.netTime = (HERT_WEBS_MSG_TIME_PERIOD_INFO *)malloc((timeBodySize * MAX_TIME_PERIOD_NUM) + 1);
    if (!tRspBody.netTime)
    {
        HERT_LOGERR("No memory!!!!\n");
        ret = HERT_ERR_INTERNEL_FALSE;
        goto GetTerminalAccess_Rsp;
    }
    
    memset(tRspBody.netTime,0x0,timeBodySize+1);
    memset(access_Tstart,0x0,sizeof(access_Tstart));
    memset(access_Tend,0x0,sizeof(access_Tend));
    
    if((strlen(tTerBody.devMac) > 0) && ((pRules = strstr(internetRules,tTerBody.devMac)) != NULL))
    {
        rst=hertParse_internetRules(pRules,paMac,&accessControl,&netLimit,access_Tstart,access_Tend);
        if(rst == 0)
        {
    		    HERT_LOGINFO("###get rules(accessControl:%d,netLimit:%d,tstart:%s,tend:%s)#######", accessControl,netLimit,access_Tstart,access_Tend);
		        tRspBody.accessControl =accessControl;
		        tRspBody.netLimit =netLimit;

		        timeBody = tRspBody.netTime;
		    
		        if(strstr(access_Tstart,",") && strstr(access_Tend,","))
		        {
		        	  /***************get the first start&end time*****************/
                pstard1 = access_Tstart;
                pend1 = strstr(pstard1,",");
                *pend1 = '\0';
                strcpy(timeBody->startTime,pstard1);	
		        
                pstard2 = access_Tend;
                pend2 = strstr(pstard2,",");
                *pend2 = '\0';
                strcpy(timeBody->endTime,pstard2);	
            
                timeBody++;
            
                pstard1 = pend1 + 1;
                if((pstard1 != NULL) && (*pstard1 != '\0'))
                {
                    strcpy(timeBody->startTime,pstard1);	
                }
		        
                pstard2 = pend2 + 1;
                if((pstard2 != NULL) && (*pstard2 != '\0'))
                {
                    strcpy(timeBody->endTime,pstard2);	
                }
                tRspBody.periodNum = 2;
		        }
		        else
		        {
		            strcpy(timeBody->startTime,access_Tstart);	
		            strcpy(timeBody->endTime,access_Tend);	
		            tRspBody.periodNum = 1;
		        }
        }
    }
    else if(strstr(internetRules,tTerBody.devMac) == NULL)
    {
    	  /*if no record fine in nvram set default value*/
    	  
        timeBody = tRspBody.netTime;
        
        tRspBody.accessControl = 1;
        tRspBody.netLimit = 1;
        tRspBody.periodNum = 1;
        strcpy(timeBody->startTime,"00:00");	
		    strcpy(timeBody->endTime,"24:00");	
    }
	
GetTerminalAccess_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_TERMINAL_STATUS_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_TERMINAL_STATUS_RSP),pszOutputData, nBufferLen,&tReqBody);     
        if (tRspBody.netTime)
        {
            free(tRspBody.netTime);
            tRspBody.netTime = NULL;
        }  
        return ret;  
    }
    
    if (tRspBody.netTime)
    {
        free(tRspBody.netTime);
    }  
    
    return ret;
}


/*
*   功能：
*       二期 4.2.2 终端访问控制设置
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetTerminalAccessStatus(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0x6005;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_SET_TERMINAL_BODY newTerminalBody,currTerminalBody;
    HERT_WEBS_MSG_TIME_PERIOD_INFO *newTimeBody = NULL,*currTimeBody = NULL;
    char    szBody[64] = {0},szCmd[128] = {0},action[8] = {0};
    int     bDoNext = 1;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&newTerminalBody, 0x0, sizeof(newTerminalBody));
        goto SetTerminal_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&newTimeBody, 0x0, sizeof(newTimeBody));
    memset(&currTerminalBody, 0x0, sizeof(currTerminalBody));
    memset(szCmd, 0x0, sizeof(szCmd));
    memset(action, 0x0, sizeof(action));
    
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&newTerminalBody, sizeof(newTerminalBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetTerminal_Rsp;
    }
    
    
    newTimeBody = newTerminalBody.netTime;
    if(strlen(newTimeBody->startTime) < 1 || strlen(newTimeBody->endTime) < 1)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetTerminal_Rsp;
    }

#if 0
    /*update iptables setting,start*/
    memset(szCmd, 0x0, sizeof(szCmd));

    /*if find record in nvram ,delete the rules in iptables first*/
    if(0 == hertUtil_getAccessRulesInNvram(newTerminalBody.devMac,&currTerminalBody))
    {
        currTimeBody = currTerminalBody.netTime;
        /*current access internet rules is disable in iptables*/
        if(currTerminalBody.accessControl == 1)
        {
        	
            strcpy(action,"deleteDisable");
            sprintf(szCmd, "set_iptables.sh %s %s ",action,currTerminalBody.devMac);
            system(szCmd); 
            memset(szCmd, 0x0, sizeof(szCmd));
            memset(action, 0x0, sizeof(action));
            
            strcpy(action,"delete");
            sprintf(szCmd, "set_iptables.sh %s %s %s %s",action,currTerminalBody.devMac,currTimeBody->startTime,currTimeBody->endTime);
            system(szCmd); 
            if(currTerminalBody.periodNum == 2)
            {
                currTimeBody++;
                memset(szCmd, 0x0, sizeof(szCmd));
                sprintf(szCmd, "set_iptables.sh %s %s %s %s",action,currTerminalBody.devMac,currTimeBody->startTime,currTimeBody->endTime);
                system(szCmd); 
            }
            memset(szCmd, 0x0, sizeof(szCmd));
            memset(action, 0x0, sizeof(action));
        }
        else
        {
            strcpy(action,"deleteDisable");
            sprintf(szCmd, "set_iptables.sh %s %s ",action,currTerminalBody.devMac);
            system(szCmd); 
            memset(szCmd, 0x0, sizeof(szCmd));
            memset(action, 0x0, sizeof(action));
        }
    }

    hertUtil_updateAccessRulesInNvram(newTerminalBody);

    if(newTerminalBody.accessControl == 1)
    {
        strcpy(action,"addDisable");
        sprintf(szCmd, "set_iptables.sh %s %s",action,newTerminalBody.devMac);
        system(szCmd); 
        memset(szCmd, 0x0, sizeof(szCmd));
        
        strcpy(action,"add");
        sprintf(szCmd, "set_iptables.sh %s %s %s %s",action,newTerminalBody.devMac,newTimeBody->startTime,newTimeBody->endTime);
        system(szCmd); 
        if(newTerminalBody.periodNum == 2)
        {
            newTimeBody++;
            memset(szCmd, 0x0, sizeof(szCmd));
            sprintf(szCmd, "set_iptables.sh %s %s %s %s",action,newTerminalBody.devMac,newTimeBody->startTime,newTimeBody->endTime);
            system(szCmd); 
        }
        memset(action, 0x0, sizeof(action));
    }
    else
    {
     
        strcpy(action,"addDisable");
        sprintf(szCmd, "set_iptables.sh %s %s ",action,newTerminalBody.devMac);
        system(szCmd); 
        memset(szCmd, 0x0, sizeof(szCmd));
        memset(action, 0x0, sizeof(action));
    }
    /*update iptables setting,end*/
    
#else

    hertUtil_updateAccessRulesInNvram(newTerminalBody);
    
#endif    
    
    /*update QOS setting,start*/
    hertUtil_updateQOSSetting(newTerminalBody.devMac,newTerminalBody.netLimit);

    //response before restart goahead
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_TERMINAL_ACCESS_RSP), szBody, ret, pszOutputData, nBufferLen);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
    websWrite(wp, T("%s\n"),pszOutputData);
    websDone(wp, 200);
    isAlreadyResponse = 1;
    
    if (newTerminalBody.netTime)
    {
        free(newTerminalBody.netTime);
    }  
    
    if(currTerminalBody.netTime)
    {
        free(currTerminalBody.netTime);
    }
    
    /* restart Qos and kill goahead */
    QoSRestart(); 
	  system("echo restartgoahead > /var/rstgoahead");
    /*update QOS setting,end*/  
    return 0;
    


SetTerminal_Rsp:
    HERT_LOGINFO("websHertAppPro_BuildMsgRsp\n");
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_TERMINAL_ACCESS_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_TERMINAL_ACCESS_RSP),pszOutputData, nBufferLen,&tReqBody);    
        if (newTerminalBody.netTime)
        {
            free(newTerminalBody.netTime);
        }  
        return ret;  
    }

    if (newTerminalBody.netTime)
    {
        free(newTerminalBody.netTime);
    }  
    return ret;
}



/*
*   功能：
*       3.13.1.	设置路由器WPS开关
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetWPSSwitch(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_WPSW_BODY tWpsBody;
    char    szBody[64];
    int     wps_enable = 0;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tWpsBody, 0x0, sizeof(tWpsBody));
        goto SetWPSSwitch_Rsp;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tWpsBody, 0x0, sizeof(tWpsBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tWpsBody, sizeof(tWpsBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetWPSSwitch_Rsp;
    }
    /* process message data　*/

	/*avoid when app wait our meesage may be timeout,response message to app quickly if when json format is correct */
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_ROUTER_WPS_SWITCH_RSP), szBody, ret, pszOutputData, nBufferLen);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
	websWrite(wp, T("%s\n"),pszOutputData);
	websDone(wp, 200);
	isAlreadyResponse = 1;

	   
    if(tWpsBody.wpsSwitch != ' ')
    {
        wps_enable = (int)(tWpsBody.wpsSwitch);
        HertApp_ResetTimerAll();
        if (wps_enable == 0){
            nvram_bufset(RT2860_NVRAM, "WscModeOption", "0");
        }else{
            nvram_bufset(RT2860_NVRAM, "WscModeOption", "7");
        }
        nvram_commit(RT2860_NVRAM);
#ifndef MINIUPNPD_SUPPORT
        doSystem("kill -9 `cat /var/run/wscd.pid.ra0`");
#endif
        if (wps_enable == 0) {
#if 0
            doSystem("iwpriv ra0 set WscConfMode=0 1>/dev/null 2>&1");
#if defined (RTDEV_SUPPORT)
        const char *raix_wsc_enable = nvram_bufget(RTDEV_NVRAM, "WscModeOption");
        if (strcmp(raix_wsc_enable, "0") == 0)
#endif
            doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
#endif
            HERT_LOGINFO("######## SKIP to close WPS  ########\n");
            g_nWpsState = 2;
        } else {
            char lan_if_addr[16];
            if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
                printf("WPSRestart error, can't get lan ip.\n");
                goto SetWPSSwitch_Rsp;
            }
            doSystem("iwpriv ra0 set WscConfMode=%d", 7);
            doSystem("route add -host 239.255.255.250 dev br0");
#ifndef MINIUPNPD_SUPPORT
            doSystem("wscd -m 1 -a %s -i ra0 &", lan_if_addr);
#endif
            g_nWpsState = 1;
        }
#ifdef MINIUPNPD_SUPPORT 
				doSystem("miniupnpd.sh init");
#endif
    }
    
    sleep(3);
    
    /*setup PBC*/
    const char *wps_status = nvram_bufget(RT2860_NVRAM, "WscModeOption");
    if (strcmp(wps_status, "7") == 0)
    {
        HERT_LOGINFO("######## start PBC ########\n");
        doSystem("iwpriv ra0 set WscConfStatus=2");
        HertApp_SetWPSPBC();
    }

	if(isAlreadyResponse == 1)
        return ret;		
	
    //tWpsBody,TBD.
SetWPSSwitch_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_ROUTER_WPS_SWITCH_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_ROUTER_WPS_SWITCH_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
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

void ConfWPAGeneral(int nvram, int mbssid)
{
  char *key_renewal_interval = "0";	
	char *wepdefaultkey = "1";

	STFs(nvram, mbssid, "EncrypType", "AES");

	STFs(nvram, mbssid, "DefaultKeyID", wepdefaultkey);	// DefaultKeyID is 1
	STFs(nvram, mbssid, "RekeyInterval", key_renewal_interval);
	STFs(nvram, mbssid, "RekeyMethod", "TIME");		
	STFs(nvram, mbssid, "IEEE8021X", "0");
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

/*
*   功能：
*       4.1.1.	设置路由器访客管理
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetGuestManage(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_GUEST_MANAGE_BODY tGuestBody;
    char    szBody[64];
    
    int     wifi_enable = -1;
    int     pwd_enable  = -1;
    int     open_time   = -1;
    char_t  *mssid_1 = NULL;
    int bssid_num = 1;
    int mbssid = 1;
    char_t *security_mode = "WPAPSKWPA2PSK";
    int new_bssid_num, old_bssid_num = 1;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tGuestBody, 0x0, sizeof(tGuestBody));
        goto GuestManage_Rsp;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tGuestBody, 0x0, sizeof(tGuestBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tGuestBody, sizeof(tGuestBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GuestManage_Rsp;
    }
    
    /* process message data　*/
    if (tGuestBody.wifiSwitch != ' ')
    {
        wifi_enable = (int)(tGuestBody.wifiSwitch);
        printf("############guest wifi_enable: %d\n", wifi_enable);
        bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
        old_bssid_num = bssid_num;
        mssid_1 = strcat(nvram_bufget(RT2860_NVRAM, "SSID1"), "_GUEST");
        if (wifi_enable == 1)
        {
            nvram_bufset(RT2860_NVRAM, racat("SSID", 2), mssid_1);
            if (bssid_num < 2)
                nvram_bufset(RT2860_NVRAM, "BssidNum", "2");
                
            open_time = (int)tGuestBody.openTime;
            char hour[2];
            memset(hour, 0x0, sizeof(hour));
            sprintf(hour, "%d", open_time);
            nvram_bufset(RT2860_NVRAM, "GuestOpenTime", hour);
            
            HERT_LOGINFO("######## Starting the guest clock ########\n");
            char sec[128];
            memset(sec, 0x0, sizeof(sec));
            sprintf(sec, "%ld", getUptime());
            nvram_bufset(RT2860_NVRAM, "GuestStartTime", sec);                          
        }
        else if (wifi_enable == 0)
        {
            nvram_bufset(RT2860_NVRAM, racat("SSID", 2), "");
            if (bssid_num >= 2)
                nvram_bufset(RT2860_NVRAM, "BssidNum", "1");
                
            nvram_bufset(RT2860_NVRAM, "GuestOpenTime", "0");
            nvram_bufset(RT2860_NVRAM, "GuestStartTime", "0");
        }
        
        new_bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
        printf("new_bssid_num:%d old_bssid_num:%d\n", new_bssid_num, old_bssid_num);
        revise_mbss_value(old_bssid_num, new_bssid_num);
    }
                
    if (tGuestBody.passwordSwitch != ' ')
    {
        pwd_enable = (int)(tGuestBody.passwordSwitch);
        printf("############guest pwd_enable: %d\n", pwd_enable);
            
        if (pwd_enable == 0)
        {
            STFs(RT2860_NVRAM, mbssid, "AuthMode", "OPEN");
            STFs(RT2860_NVRAM, mbssid, "EncrypType", "NONE");
        }
        else if (pwd_enable == 1)
        {      
            ConfWPAGeneral(RT2860_NVRAM, mbssid);            	                            
            STFs(RT2860_NVRAM, mbssid, "AuthMode", security_mode);
            nvram_bufset(RT2860_NVRAM, racat("WPAPSK", mbssid+1), tGuestBody.password);         
        }       
    }  
    nvram_commit(RT2860_NVRAM);
    
    //response before restart goahead
		ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_VISITOR_MANAGEMENT_RSP), szBody, 0, pszOutputData, nBufferLen);
		websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
		websWrite(wp, T("%s\n"),pszOutputData);
		websDone(wp, 200);
		isAlreadyResponse = 1;
    if (new_bssid_num > old_bssid_num)
    {
        doSystem("reboot");
    }
    initInternet();
    if (isAlreadyResponse == 1)
        return ret;
    
GuestManage_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_VISITOR_MANAGEMENT_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_VISITOR_MANAGEMENT_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }  

    return ret;
}

/*
*   功能：
*       4.1.1访客管理状态查询
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetGuestManage(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_RSP_VAR_GUEST_MANAGE_BODY tRspBody;
    char    szBody[64];
    char    *mssid_1 = NULL;
    char    *auth_mode = NULL;
    

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto GuestManageStatus_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tRspBody, 0x0, sizeof(tRspBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GuestManageStatus_Rsp;
    }

    /* process message data　*/
    mssid_1 = nvram_bufget(RT2860_NVRAM, "SSID2");
    if (strstr(mssid_1, "_GUEST") != NULL)
        tRspBody.wifiSwitch = 1;
    else
        tRspBody.wifiSwitch = 0;
    
    auth_mode = getNthValue(1, nvram_bufget(RT2860_NVRAM, "AuthMode"));
    if (strcmp(auth_mode, "WPAPSKWPA2PSK") == 0)
    {
        tRspBody.passwordSwitch = 1;
        strcpy(tRspBody.password, nvram_get(RT2860_NVRAM, "WPAPSK2"));
    }
    else
    {
        tRspBody.passwordSwitch = 0;
    }   
    tRspBody.openTime = atoi(nvram_get(RT2860_NVRAM, "GuestOpenTime"));
    tRspBody.remainTime = g_nRemainTime;
    
GuestManageStatus_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_VISITOR_MANAGEMENT_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_VISITOR_MANAGEMENT_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}

/*
*   功能：
*       3.13.2.	获取路由器中待接入设备信息
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GeToAddDevList(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_RSP_DEVTOADD_BODY tRspBody;
    HERT_WEBS_MSG_RSP_DEVITEM_BODY *pDevItem;  /* 设备信息 */
    char    szBody[64];
    FILE* fr = NULL;
    TOADDDEV toAdd;
    int  nBufSize = 0;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tRspBody, 0x0, sizeof(tRspBody));
        goto GeToAddDevList_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(szBody, 0x0, sizeof(szBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GeToAddDevList_Rsp;
    }

    /* process message data　*/
    tRspBody.dwDevNumber = 0;
    nBufSize = hertUtil_getFileSize(TOADDDEVFILE) + 1;
    tRspBody.pDevItem = malloc(nBufSize);/* max 32 client */
    if (!tRspBody.pDevItem)
    {
        HERT_LOGERR("No memory!!!!\n");
        ret = HERT_ERR_INTERNEL_FALSE;
        goto GeToAddDevList_Rsp;
    }
    memset(tRspBody.pDevItem, 0x0, nBufSize);
    pDevItem = tRspBody.pDevItem;

    fr  = fopen(TOADDDEVFILE, "rb");
    if (fr)
    {
        /* File was opened successfully. */
        memset(&toAdd, 0x0, sizeof(toAdd));
        /* Attempt to read => write */
        while (fread(&toAdd, 1, sizeof(toAdd), fr) == sizeof(toAdd))
        {
            strcpy(pDevItem->devID,toAdd.szProdSerial);
            strcpy(pDevItem->devName, toAdd.szProduct);
            strcpy(pDevItem->devType, toAdd.szProdType);
            strcpy(pDevItem->mac, toAdd.szMac);
            pDevItem++;
            tRspBody.dwDevNumber++;
            memset(&toAdd, 0x0, sizeof(toAdd));
        }
        fclose(fr);
    }
    else
    {
        HERT_LOGERR("fopen error\n");
        ret = 0;
    }

GeToAddDevList_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_ROUTER_TOADD_DEV_RSP), (char*)&tRspBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        if (tRspBody.pDevItem)
        {
            free(tRspBody.pDevItem);
        }
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_TOADD_DEV_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    /* free memory source */
    if (tRspBody.pDevItem)
    {
        free(tRspBody.pDevItem);
    }

    return ret;
}

/*
*   功能：
*      二期 	wifi信号强度查询
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_WifiStrengthREQ(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    char    szBody[64];
    unsigned  char szWifiStrength;
	int tx_power=atoi((char *) nvram_bufget(RT2860_NVRAM, "TxPower"));

    HERT_LOGINFO("<<<<<<<<<<<<<<<<<tx_power:%d<<<<<<<<<<<<<<<<<\n",tx_power);
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        goto GetWifiStrengthREQ_ERR;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetWifiStrengthREQ_ERR;
    }
    /* process message data　*/
	if(tx_power==100)
	szWifiStrength=0;
	else if(tx_power>=60&&tx_power<100)
	szWifiStrength=1;
	else  if(tx_power<60&&tx_power>=40)
	szWifiStrength=2;
	else 
	szWifiStrength=-1;
	
    HERT_LOGINFO("<<<<<<<<<<<<<<<<szWifiStrength:%d<<<<<<<<<<<<<<<<<\n",szWifiStrength);

GetWifiStrengthREQ_ERR:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_WIFI_STRENGTH_RSP), &szWifiStrength, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_WIFI_STRENGTH_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}
/*
*   功能：
*       二期 wifi信号强度设置
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetWifiStrengthREQ(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
   
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    int     szBody;
    int  szWifiStrength;
    char newTxPower[8] = {0x0};
    //getCfgZero(1,"TxPower");
    char * tx_power=(char *) nvram_bufget(RT2860_NVRAM, "TxPower");

    HERT_LOGINFO("<<<<<<<<<<<<<<<<<old tx_power:%s<<<<<<<<<<<<<<<<<\n",tx_power);
    if ((!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        goto SetWifiStrengthREQ_ERR;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, &szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetWifiStrengthREQ_ERR;
    }
    szWifiStrength=szBody;
    
    memset(newTxPower,0x0,sizeof(newTxPower));
    
    /* process message data　*/
    if(szWifiStrength==0)
    {
        //tx_power="100";
        strcpy(newTxPower,"100");
    }
    else if(szWifiStrength==1)
    {
        //tx_power="10";
        strcpy(newTxPower,"60");
    }
    else if(szWifiStrength==2)
    {
        //tx_power="40";
        strcpy(newTxPower,"40");
    }
    else
        goto SetWifiStrengthREQ_ERR;
        
    nvram_bufset(RT2860_NVRAM,"TxPower",newTxPower);
    HERT_LOGINFO("<<<<<<<<<<<<<<<<szWifiStrength:%d <<cur tx_power:%s <<szBody:%d<<<<<<<<<<<<<<<\n",szWifiStrength,newTxPower,szBody);
    nvram_commit(RT2860_NVRAM);
SetWifiStrengthREQ_ERR:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_WIFI_STRENGTH_RSP), &szWifiStrength, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_WIFI_STRENGTH_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*      江西需求 wifi信道设置
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetWifiChannelREQ(webs_t wp, const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_CHANNEL_BODY tChnlBody;
    char    szBody[64],ChannelSelectedBuf[64] = {0};
    char_t	*sz11aChannel, *sz11bChannel, *sz11gChannel, *ChannelSelected;
    int     wps_enable = 0,num_channel = 1,num_Mhz = 0;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tChnlBody, 0x0, sizeof(tChnlBody));
        goto SetChannel_Rsp;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(&tChnlBody, 0x0, sizeof(tChnlBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tChnlBody, sizeof(tChnlBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetChannel_Rsp;
    }
    
    
    /* process message data　*/
		
		if(strlen(tChnlBody.workingChannel) == 0)
	  {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(&tChnlBody, 0x0, sizeof(tChnlBody));
        goto SetChannel_Rsp;
	  }
	  
	  
	  num_channel = atoi(tChnlBody.workingChannel);
	  num_Mhz = 2412 + 5*(num_channel - 1);
		snprintf(ChannelSelectedBuf,sizeof(ChannelSelectedBuf),"%dMHz (Channel %d)",num_Mhz,num_channel);
		
		nvram_bufset(RT2860_NVRAM, "ChannelSelected", ChannelSelectedBuf);
		nvram_bufset(RT2860_NVRAM, "Channel", tChnlBody.workingChannel);
		doSystem("iwpriv ra0 set Channel=%s", sz11gChannel);

	  /*avoid when app wait our meesage may be timeout,response message to app quickly if when json format is correct */
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_ROUTER_CHANNEL_RSP), szBody, ret, pszOutputData, nBufferLen);
	  websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
	  websWrite(wp, T("%s\n"),pszOutputData);
	  websDone(wp, 200);
	  isAlreadyResponse = 1;

		nvram_commit(RT2860_NVRAM);
		initInternet();
		
	  if(isAlreadyResponse == 1)
        return ret;		
	
    //tWpsBody,TBD.
SetChannel_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_ROUTER_CHANNEL_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_ROUTER_CHANNEL_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}


/*
*   功能：
*        江西需求 wifi信道获取
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_GetWifiChannelREQ(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    char    szBody[64];
    char    szWifiChnl[32];
    char    if_mac[32];

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        memset(szWifiChnl, 0x0, sizeof(szWifiChnl));
        goto GetChannel_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    memset(szWifiChnl, 0x0, sizeof(szWifiChnl));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody, sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto GetChannel_Rsp;
    }


    char *value = nvram_bufget(RT2860_NVRAM, "ChannelSelected");
    char *value2 = nvram_bufget(RT2860_NVRAM, "Channel");
    //autochannel
		if((0 == strcmp(value, "")) || (0 == strcmp(value, "AutoSelect"))|| (0 == strcmp(value, "0")))
	  {
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
	      snprintf(szWifiChnl, sizeof(szWifiChnl), "%d",channel); 
	  }//manu
	  else
	  {
					strncpy(szWifiChnl,value2,sizeof(szWifiChnl));
	  }

GetChannel_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_GET_CURRENT_ROUTER_CHANNEL_RSP), szWifiChnl, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_CURRENT_ROUTER_CHANNEL_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}



/*
*   功能：
*      二期.	关闭Wifi
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int  websHertAppPro_SetWifiClosedREQ(webs_t wp, const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{

    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
  char    szBody[64];
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        goto SetWifiClosed_ERR;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody,sizeof(szBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
        goto SetWifiClosed_ERR;
    }
    
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_WIFI_CLOSED_RSP), szBody, 0, pszOutputData, nBufferLen);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),strlen(pszOutputData));
    websWrite(wp, T("%s\n"),pszOutputData);
    websDone(wp, 200);
    isAlreadyResponse = 1;
		
//关闭WIFI
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

    if (isAlreadyResponse == 1)
        return ret;
        
SetWifiClosed_ERR:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_WIFI_CLOSED_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_SET_WIFI_CLOSED_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}





/*
*   功能：
*      二期.	未知消息类型处理
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0xa00a}
*   返回：
*   0:正确返回，其他值-错误码
*/
int  websHertAppPro_UnknowMSGTypeREQ(webs_t wp, const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{

    int     ret = 0;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    char    szBody[64];
    
    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        goto UnknowMSGType_ERR;
    }
    
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, szBody,sizeof(szBody));
    
    
    if(strlen(tReqBody.msgSeq) < 1 || tReqBody.version == 0)
    {
        strcpy(tReqBody.msgSeq,"1");
        tReqBody.version = 1;
    }
    
    ret = HERT_ERR_UNKNOW_MSGTYPE;
    hertUtil_BuildErrMsgRsp(ret,STR(UNKNOW_MESSAGE_TYPE_RSP),pszOutputData, nBufferLen,&tReqBody);   
    return ret;
              
UnknowMSGType_ERR:
    /* build message data　*/
    hertUtil_BuildErrMsgRsp(ret,STR(UNKNOW_MESSAGE_TYPE_RSP),pszOutputData, nBufferLen,&tReqBody); 

    return ret;
}





/*
*   功能：
*       3.13.3.	设置路由器中待接入设备
*   输入：
*   pszSrcBody:
*       比如：{“version”:1,”msgSeq”:1,可变属性部分根据具体请求填充}
*   nBufferLen:
*       pszOutputData内存缓存长度
*   输出：
*   pszOutputData:
*      比如：{“version”:1,”msgSeq”:1,”errorCode”:0,可变属性部分根据具体响应填充}
*   返回：
*   0:正确返回，其他值-错误码
*/
int websHertAppPro_SetAllowAddDev(const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    int     ret = 0,i,rdLen;
    HERT_WEBS_MSG_REQ_BODY tReqBody;
    HERT_WEBS_MSG_REQ_VAR_DEVALLOW_BODY *tAllowAddBody = NULL;
    HERT_WEBS_MSG_REQ_VAR_ADDDEV_BODY tDEVBody;
    char    szBody[64];
    char    szCmd[256];
    FILE*   fr = NULL;
    FILE*   fw = NULL;
    char line[128] = {0};
    TOADDDEV tempData;

    if ( (!pszSrcBody) || (!pszOutputData))
    {
        HERT_LOGERR("Invalid parameter: pszSrcBody(%p),pszOutputData(%p)", 
                    pszSrcBody, pszOutputData);
        ret = HERT_ERR_INVALID_JSON;
        memset(&tReqBody, 0x0, sizeof(tReqBody));
        //memset(&tAllowAddBody, 0x0, sizeof(tAllowAddBody));
        memset(&tDEVBody, 0x0, sizeof(tDEVBody));
        memset(szBody, 0x0, sizeof(szBody));
        goto SetAllowAddDev_Rsp;
    }
    memset(&tReqBody, 0x0, sizeof(tReqBody));
    //memset(&tAllowAddBody, 0x0, sizeof(tAllowAddBody));
    memset(&tDEVBody, 0x0, sizeof(tDEVBody));
    memset(szBody, 0x0, sizeof(szBody));

    tDEVBody.pDevAddList = NULL;
    /* parse message data　*/
    ret = websHertAppPro_ParseMsgReq(pszSrcBody, &tReqBody, (char*)&tDEVBody, sizeof(tDEVBody));
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_ParseMsgReq(%d)", ret);
        ret = HERT_ERR_INVALID_JSON;
		if(tDEVBody.pDevAddList != NULL)
		    free(tDEVBody.pDevAddList);
        goto SetAllowAddDev_Rsp;
    }
	
    tAllowAddBody = tDEVBody.pDevAddList;
		
    HERT_LOGINFO("tDEVBody.dwDevNum:%d\n", tDEVBody.dwDevNum); 

    /* process message data??*/
    for(i = 0;i < tDEVBody.dwDevNum; i++,tAllowAddBody++)
    {	
	    HERT_LOGINFO("tAllowAddBody->AllowAdd:%d\n", tAllowAddBody->AllowAdd); 
	    if (tAllowAddBody->AllowAdd != 0x0)
	    {
	    	HERT_LOGINFO("####add allowDev:%s####\n",tAllowAddBody->mac);
            if(!hertUtil_IsInFile(ALLOWTOADDLIST, tAllowAddBody->mac))
            {
	            /* add to allow list */
	            memset(szCmd, 0x0, sizeof(szCmd));
	            sprintf(szCmd, "echo %s >> %s", tAllowAddBody->mac, ALLOWTOADDLIST);
	            system(szCmd);
                //hertUtil_DelNetFiltMac(tAllowAddBody->mac);
            }
			    
	        //remove the devInfo from TOADDDEVFILE start
	        memset(line, 0x0, sizeof(line));
	        memset(&tempData, 0x0, sizeof(tempData));
			    
	        fr = fopen(TOADDDEVFILE, "rb");
	        fw = fopen(TEMP_TOADDDEVFILE, "wb");
            rdLen = sizeof(tempData);
            if(fr != NULL && fw != NULL)
            {
                while ((i=fread(&tempData, 1, rdLen, fr)) == rdLen)
                {
	                HERT_LOGINFO("1-read buf %d, tempData.szMac(%s)\n", i, tempData.szMac);
	                if (!strstr(tempData.szMac, tAllowAddBody->mac))
	                {
	                  /* add others to DEVFILE list */
                      if ( 1 != fwrite(&tempData, sizeof(tempData), 1, fw))
                      {
                          HERT_LOGERR("[%s,%d]: fwrite error\n", __FUNCTION__, __LINE__);
                      }
	                }
	                memset(&tempData, 0x0, sizeof(tempData));
	            }
	        }
            else
            {
                HERT_LOGERR("Failed at open %s or %s", TOADDDEVFILE, TEMP_TOADDDEVFILE);
            }
          
            if(fr != NULL)
                fclose(fr);
              
            if(fw != NULL)         
                fclose(fw);

	        /* copy file */
	        memset(szCmd, 0x0, sizeof(szCmd));
	        sprintf(szCmd, "rm %s", TOADDDEVFILE);
	        system(szCmd);
          
          rdLen = hertUtil_getFileSize(TEMP_TOADDDEVFILE);
          if(rdLen > 0)
          {		
	            memset(szCmd, 0x0, sizeof(szCmd));
	            sprintf(szCmd, "cp %s %s", TEMP_TOADDDEVFILE, TOADDDEVFILE);
	            system(szCmd);
	        }
	
	        memset(szCmd, 0x0, sizeof(szCmd));
	        sprintf(szCmd, "rm %s", TEMP_TOADDDEVFILE);
	        system(szCmd);
	        //remove the devInfo from TOADDDEVFILE end
	    }
	    else if ((tAllowAddBody->AllowAdd == 0x0) && hertUtil_IsInFile(ALLOWTOADDLIST, tAllowAddBody->mac))
	    {
	    	
	        HERT_LOGINFO("####remove allowDev:%s####\n",tAllowAddBody->mac);  	    	
	        /* remove from allow list */
	        fr = fopen(ALLOWTOADDLIST, "r");
	        if (fr != NULL)
	        {
	            // read pass header line
	            while (fgets(line, 128, fr))
	            {
	                HERT_LOGINFO("fgets line(%s)\n", line);
	                if (!strstr(line, tAllowAddBody->mac))
	                {
	                    /* add to allow list */
	                    memset(szCmd, 0x0, sizeof(szCmd));
	                    rdLen = strlen(line);
	                    line[rdLen-1] = '\0';
	                    sprintf(szCmd, "echo %s >> %s", line, TEMP_ALLOWTOADDLIST);
	                    system(szCmd);
	                }
	                memset(line, 0x0, sizeof(line));
	            }
	            fclose(fr);
	        }
			
	        //hertUtil_DelNetFiltMac(tAllowAddBody->mac);
	        //hertUtil_AddNetFiltMac(tAllowAddBody->mac);

	        /* copy file */
	        memset(szCmd, 0x0, sizeof(szCmd));
	        sprintf(szCmd, "rm %s", ALLOWTOADDLIST);
	        system(szCmd);
	
	        memset(szCmd, 0x0, sizeof(szCmd));
	        sprintf(szCmd, "cp %s %s", TEMP_ALLOWTOADDLIST, ALLOWTOADDLIST);
	        system(szCmd);
	
	        memset(szCmd, 0x0, sizeof(szCmd));
	        sprintf(szCmd, "rm %s", TEMP_ALLOWTOADDLIST);
	        system(szCmd);
			
	    }
    }
	
    if(tDEVBody.pDevAddList)
        free(tDEVBody.pDevAddList); 

SetAllowAddDev_Rsp:
    /* build message data　*/
    ret = websHertAppPro_BuildMsgRsp(&tReqBody, STR(MSG_SET_ROUTER_ADD_DEV_RSP), szBody, ret, pszOutputData, nBufferLen);
    if (ret)
    {
        HERT_LOGERR("Failed at websHertAppPro_BuildMsgRsp(%d)", ret);
        ret = HERT_ERR_INTERNEL_FALSE;
        hertUtil_BuildErrMsgRsp(ret,STR(MSG_GET_ROUTER_TOADD_DEV_RSP),pszOutputData, nBufferLen,&tReqBody);         
        return ret;  
    }

    return ret;
}

/* parse: push data msgtype */
int websHertAppPro_Parse_Msg_Type(const char *ptMsg, int nDataLen, CHAR *pszMsgType, int nMsgTypeBuf)
{
    int ret = 0;
    CHAR szMsgType[64] = {0x0};
    int  nMsgTypeLen = 0;
    int  i = 0;
    CHAR *pStart = NULL;
    CHAR *pEnd   = NULL;

    if ( (!ptMsg) || (!pszMsgType) )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), pszMsgType(%p)", ptMsg, pszMsgType);
        return 1;
    }

    memset(szMsgType, 0x0, sizeof(szMsgType));
    sprintf(szMsgType, "%s", STR(msgType));
    nMsgTypeLen = strlen(szMsgType);
    for(i = 0; i + nMsgTypeLen < nDataLen; i++)
    {
        if (0 == memcmp(ptMsg+i, szMsgType, nMsgTypeLen))
        {
            break;
        }
    }
    if ((i + nMsgTypeLen) == nDataLen)
    {
        HERT_LOGERR("Failed to find msgtype(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return 2;
    }
    i = i + nMsgTypeLen;
    pStart = strstr(ptMsg + i, "\"");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return 2;
    }
    pStart = strstr(pStart+1, ":");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return 2;
    }
    pStart = strstr(pStart+1, "\"");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return 2;
    }
    pStart += 1;

    pEnd = strstr(pStart, "\"");
    if (!pEnd)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return 2;
    }
    memset(pszMsgType, 0x0, nMsgTypeBuf);
    strncpy(pszMsgType, pStart, pEnd - pStart);

    return ret;
}


void websHertAppPro_UploadImg(webs_t wp){
    char *ptr = NULL,pathName[32] = {0},pathName2[32] = {0};
    char proUpFileName[PICTURE_URL_SIZE] = {0};
    char saveFileName[PICTURE_URL_SIZE] = {0};
    char cmdLine[128] = {0};
    FILE *fp;
    long long writeNum = 0;
    static int nFileNum = 0;
    char szOnlyFileName[PICTURE_URL_SIZE] = { 0x0 };
    long long freeSize = 0;

    if(NULL == wp->query || NULL == wp->url)
        return;

    herUtil_dumpHexString(__FUNCTION__, __LINE__, wp->url);

    if((ptr = strstr(wp->url,UPLOAD_IMG_TAG)) != NULL)
    {
        ptr += strlen(UPLOAD_IMG_TAG);	
    }
    else
    {
        return;
    }
    
    hertUtil_GetUSBPartitionMaxFreeSize(&freeSize, pathName, sizeof(pathName));
    sprintf(proUpFileName, "%s/picture/uploading_file",pathName);	
    sprintf(pathName2, "%s/picture",pathName);	

    
    isAlreadyResponse = 1;
    
    //for first upload create the dir
    if(access(pathName2, 0) != 0)
    {
        sprintf(cmdLine, "mkdir %s",pathName2);
        system(cmdLine);
        HERT_LOGINFO("######## mkdir:%s ########\n",pathName2);
        memset(cmdLine,0x0,sizeof(cmdLine));
    }	
            	
    
    HERT_LOGINFO("######## write proUpFileName:%s ########\n",proUpFileName);
    fp = fopen(proUpFileName,"wb");	  
    if (fp)
    {
        writeNum = fwrite(wp->query,wp->writeBodyLength,1,fp);
        fclose(fp);
        HERT_LOGINFO("######## write Picture complete:%ld-%ld ########\n",wp->writeBodyLength,writeNum);

        if(0)
        {
            strncpy(szOnlyFileName, ptr, sizeof(szOnlyFileName));
        }
        else
        {
            websDecode64(szOnlyFileName, ptr, sizeof(szOnlyFileName));
        }
        HERT_LOGINFO("######## szOnlyFileName(%s) ########\n", szOnlyFileName);
        herUtil_dumpHexString(__FUNCTION__, __LINE__, szOnlyFileName);
		
        /*rename to the realname*/
        if (hertUtil_IsInFile("/var/hertchstest","DEBUG"))
        {
            sprintf(saveFileName, "%s/picture/\"%s\"",pathName,szOnlyFileName);
        }
        else
        {
#if 0
            if (!hertUtil_IsAssicString(ptr))
            {
                nFileNum++;
                sprintf(saveFileName, "%s/picture/picture_%04d",pathName,nFileNum);
            }
            else
            {
                sprintf(saveFileName, "%s/picture/%s",pathName,ptr);
            }
#endif
            if (strstr(szOnlyFileName, "\`"))
            {
                char szTemp[PICTURE_URL_SIZE] = { 0x0 };
                int i = 0;
                int j = 0;

                memset(szTemp, 0x0, sizeof(szTemp));
                for(i = 0; i < strlen(szOnlyFileName); i++)
                {
                    if (szOnlyFileName[i] == '`')
                    {
                        szTemp[j] = '\\';
                        j++;
                    }
                    szTemp[j] = szOnlyFileName[i];
                    j++;
                }
                memset(szOnlyFileName, 0x0, sizeof(szOnlyFileName));
                strcpy(szOnlyFileName, szTemp);
                HERT_LOGINFO("######## szOnlyFileName(%s) ########\n", szOnlyFileName);
            }

            sprintf(saveFileName, "%s/picture/\"%s\"",pathName,szOnlyFileName);
        }
        sprintf(cmdLine, "cp %s %s",proUpFileName,saveFileName);
        system(cmdLine);
        memset(cmdLine,0x0,sizeof(cmdLine));
        sprintf(cmdLine, "rm %s -rf",proUpFileName);
        system(cmdLine);
        
        
        websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),40);
        websWrite(wp, T("%s\n"),"{\"version\":1,\"msgSeq\":\"9\",\"errorCode\":0}");
        websDone(wp, 200);
    }
    else{
        websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),40);
        websWrite(wp, T("%s\n"),"{\"version\":1,\"msgSeq\":\"2\",\"errorCode\":1}");
        websDone(wp, 200);
    }
}



void websHertAppPro_DownloadImg(webs_t wp){
    char *ptr = NULL;
    char readFileName[PICTURE_URL_SIZE] = {0};
    char ImgStream[WEBS_BUFSIZE] = {0},pathName[32] = {0};;
    FILE *fp;
    long long readNum = 0,img_size = 0,count_size = 0;
    char szOnlyFileName[PICTURE_URL_SIZE] = { 0x0 };
    long long freeSize = 0;

    if(NULL == wp->url)
        return;
    
    if((ptr = strstr(wp->url,DOWNLOAD_IMG_TAG)) != NULL)
    {
        ptr += strlen(DOWNLOAD_IMG_TAG);	
    }
    else
    {
        return;
    }
    
    HERT_LOGINFO("######## Enter in DownloadImg ########\n");
    
    if(hretUtil_GetUSBPartitionName( pathName, sizeof(pathName)) != 0)
    {
        HERT_LOGERR("no USB storage find!");    
        return;
    }
    HERT_LOGINFO("######## ptr(%s) ########\n", ptr);
    if(0)
    {
        strncpy(szOnlyFileName, ptr, sizeof(szOnlyFileName));
    }
    else
    {
        websDecode64(szOnlyFileName, ptr, sizeof(szOnlyFileName));
    }
    HERT_LOGINFO("######## szOnlyFileName(%s) ########\n", szOnlyFileName);
    herUtil_dumpHexString(__FUNCTION__, __LINE__, szOnlyFileName);
    
    sprintf(readFileName, "%s/picture/%s",pathName, szOnlyFileName);	

    fp = fopen(readFileName,"rb");	  
    if (fp)
    {
        fseek(fp, 0L, SEEK_END);  
        img_size = ftell(fp);
        
        if(img_size > MAX_IMG_SIZE)
        {
            HERT_LOGERR("the image %s is too big,cancle handle",readFileName);
            fclose(fp);
            return;
        }
        
        fseek(fp, 0L, SEEK_SET);  

        memset(ImgStream,0,WEBS_BUFSIZE);
        websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),img_size);
        while( (readNum = fread( ImgStream, 1, WEBS_BUFSIZE, fp )) > 0 )
        {
            wp->readBodyLength = readNum;
            WriteImg(wp,ImgStream);
            count_size += readNum;
            memset(ImgStream,0,WEBS_BUFSIZE);
        }
        websDone(wp, 200);
        
        fclose(fp);
        HERT_LOGINFO("######## readNum Picture complete:%lld ########\n",count_size);
        isAlreadyResponse = 1;
    }
    return;
}


int websHertAppProcess(webs_t wp,const char *pszSrcBody, char *pszOutputData, int nBufferLen)
{
    char szMsgType[128] = { 0x0 };
    int  nBuffSize  = 0;
    int  ret = 0;

    nBuffSize = sizeof(szMsgType);
    memset(szMsgType, 0x0, nBuffSize);

    hertUtil_setLoglevel();

    ret = websHertAppPro_Parse_Msg_Type(pszSrcBody, strlen(pszSrcBody), szMsgType, nBuffSize - 1);
    if ( ret )
    {
        HERT_LOGERR("Failed at websHertAppPro_Parse_Msg_Type(%d)", ret);
        return ret;
    }
    
    memset(currMsgType,0x0,sizeof(currMsgType));
    strncpy(currMsgType, szMsgType,sizeof(currMsgType));

    HERT_LOGINFO(">>>>>>>>>>>>>>>>>MSG TYPE:%s>>>>>>>>>>>>>>>>>\n", szMsgType);

    if (strstr(szMsgType, STR(MSG_GET_ROUTER_STATUS_REQ)))
    {
        ret = websHertAppPro_GetRouteStatus(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_SN_REQ)))
    {
        ret = websHertAppPro_GetRouteSN(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_PASSWORD_REQ)))
    {
        ret = websHertAppPro_GetRoutePassword(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_SPACE_REQ)))
    {
        ret = websHertAppPro_GetRouteSpace(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_ABILITY_REQ)))
    {
        ret = websHertAppPro_GetRouteAbility(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_UPLOAD_IMG_REQ)))
    {
        ret = websHertAppPro_GeUploadPicture(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_IMG_LIST_REQ)))
    {
        ret = websHertAppPro_GeUpDownloadList(wp,pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_ROUTER_UPDATE_REQ)))
    {
        ret = websHertAppPro_RouteUpgrade(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_SET_NETWORK_REQ)))
    {
        ret = websHertAppPro_SetNetWorkConfig(wp,pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_FAULT_DETEC_REQ)))
    {
        ret = websHertAppPro_GeFaultDetect(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_SET_ROUTER_WPS_SWITCH_REQ)))
    {
        ret = websHertAppPro_SetWPSSwitch(wp,pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_ROUTER_TOADD_DEV_REQ)))
    {
        ret = websHertAppPro_GeToAddDevList(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_SET_ROUTER_ADD_DEV_REQ)))
    {
        ret = websHertAppPro_SetAllowAddDev(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_VISITOR_MANAGEMENT_REQ)))
    {
        ret = websHertAppPro_GetGuestManage(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_SET_VISITOR_MANAGEMENT_REQ)))
    {
        ret = websHertAppPro_SetGuestManage(wp,pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_GET_TERMINAL_STATUS_REQ)))
    {
        ret = websHertAppPro_GetTerminalAccessStatus(pszSrcBody, pszOutputData, nBufferLen);
    }
    else if (strstr(szMsgType, STR(MSG_SET_TERMINAL_ACCESS_REQ)))
    {
        ret = websHertAppPro_SetTerminalAccessStatus(wp,pszSrcBody, pszOutputData, nBufferLen);
    }//wifi 信号强度查询
    else if (strstr(szMsgType, STR(MSG_GET_WIFI_STRENGTH_REQ)))
    {
          ret = websHertAppPro_WifiStrengthREQ(pszSrcBody, pszOutputData, nBufferLen);
    }//wifi 信号强度设置
	  else if (strstr(szMsgType, STR(MSG_SET_WIFI_STRENGTH_REQ)))
    {
          ret = websHertAppPro_SetWifiStrengthREQ(pszSrcBody, pszOutputData, nBufferLen);
    }
   //本地关闭WIFI
   else if (strstr(szMsgType, STR(MSG_SET_WIFI_CLOSED_REQ)))
   {
          ret = websHertAppPro_SetWifiClosedREQ(wp, pszSrcBody, pszOutputData, nBufferLen);
   }
   //获取无线信道
	 else if (strstr(szMsgType, STR(MSG_GET_CURRENT_ROUTER_CHANNEL_REQ)))
   {
          ret = websHertAppPro_GetWifiChannelREQ(pszSrcBody, pszOutputData, nBufferLen);
   }
   //设置无线信道
   else if (strstr(szMsgType, STR(MSG_SET_ROUTER_CHANNEL_REQ)))
   {
          ret = websHertAppPro_SetWifiChannelREQ(wp, pszSrcBody, pszOutputData, nBufferLen);
   }/*Unknow message type*/
   else 
   {
          memset(currMsgType,0x0,sizeof(currMsgType));
          strncpy(currMsgType, STR(UNKNOW_MESSAGE_TYPE),sizeof(currMsgType));
          ret = websHertAppPro_UnknowMSGTypeREQ(wp, pszSrcBody, pszOutputData, nBufferLen);     
   }
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<MSG PUSH:%s<<<<<<<<<<<<<<<<<\n", szMsgType);

    return ret;
}




/********************************** Description *******************************/

/*
 *	This module implements the /heroute/AppRequest handler. It process the request 
 * from HeRouter APP,parse the body content and response the correct result to APP.
 */

/*********************************** Includes *********************************/



int websHertAppHandler(webs_t wp)
{
    char jsonBuf[JSON_BUFFER_SIZE] = {0};
    int rspDateLen = 0;
    if (hertUtil_IsInFile("/var/goaheadnoapphandle","DEBUG"))
    {
        return 1;
    }
    
    isAlreadyResponse = 0;
    
    HERT_LOGINFO("#########@@ come apprequest:%s @@########\n",wp->url);

    if(strstr(wp->url,UPLOAD_IMG_TAG) != NULL)
        websHertAppPro_UploadImg(wp);
    else if(strstr(wp->url,DOWNLOAD_IMG_TAG) != NULL)
        websHertAppPro_DownloadImg(wp);		
    else
        websHertAppProcess(wp,wp->query,jsonBuf, JSON_BUFFER_SIZE);

    rspDateLen = strlen(jsonBuf);
    
    
    HERT_LOGINFO("\n################ReponseJsonBody:%s\n",jsonBuf) 
    
    if(isAlreadyResponse == 0)
    {
        websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nContent-Length: %d\nCache-Control: no-cache\n\n"),rspDateLen);
        websWrite(wp, T("%s\n"),jsonBuf);
        websDone(wp, 200);
    }

    return 1;
}
