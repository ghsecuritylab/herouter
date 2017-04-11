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
#include <pthread.h>
#include "linux/autoconf.h"
#include <time.h>
#include "hert_com.h"
#include "hert_app.h"
#include "hert_msg.h"
#include "hert_util.h"
#include "hert_tcp.h"
#include "hert_api.h"

#include <curl.h>
#include <easy.h>
#include <linux/wireless.h>
#include "nvram.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include<signal.h>

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

const CHAR *NO_MSG_VAR_BODY = "NO_MSG_VAR_BODY";

typedef struct tagHertWorkData
{
    HERT_STATUS status;
    int bStopRun;
    DWORD msgSeq;
    pthread_mutex_t mutex_cc;
    CHAR szDstAddress[64];
    CHAR szCnnPrtlData[64];
    CHAR szCnnUserData[64];
    CHAR szCnnPassData[128];
    HERT_MSG_RSP_PUSH_DATA_VARPART_INIT initData;
    HERT_MSG_REQ_PUSH_DATA_VARPART_ABILITY_NOTIFY abilityData;
    HERT_MSG_RSP_PUSH_DATA_VARPART_SPACE spaceData;
    HERT_MSG_RSP_PUSH_DATA_VARPART_STATUS statusData;
    HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD downData;
    HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE upgradeData;
    int informPlatHour;
    int informPlatMin;
    int timingComplete;
}HERT_WORKDATA;

static HERT_SESSION  g_session;
static HERT_WORKDATA g_workData;

extern void HERT_dump(char *pszData, const char *pszFunction, int nLine);

#define HEX_DUMP(a) HERT_dump(a, __FUNCTION__, __LINE__)

void HERT_APP_OnSignal(int i)
{
    HERT_LOGINFO("Signal caugh(%s)\n",strerror(errno));
    closelog();
    exit(1);
}

RETCODE HERT_APP_Init()
{
    RETCODE ret = RETCODE_SUCCESS;
    pthread_t th_control;
    char if_mac[32];     
    //char szDevName[64] = "\xE5\x92\x8C\xE8\xB7\xAF\xE7\x94\xB1";
    char szDevName[64] = {0x0};
    const char *SSID = nvram_bufget(RT2860_NVRAM, "SSID1");
    /*****************check it is after default load ****************/	
    char *isTheFirstStart = nvram_bufget(RT2860_NVRAM, "IS_THE_FIRSTTIME_START");
	
    int i = 0,time_hour = 18,time_min = 0,time_period = 0;
    int randSourcw = 0;
		
    if(!strcmp(isTheFirstStart,"1"))
    {
        char if_mac[32], Strssid[128];
        memset(if_mac, 0x0, sizeof(if_mac));
        memset(Strssid, 0x0, sizeof(Strssid));
        flash_read_mac(if_mac);

        sprintf(Strssid,"CMCC_%02X%02X%02X",0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5]);
        memcpy(szDevName, Strssid, sizeof(szDevName));
        HERT_LOGINFO("szDevName = %s\n", szDevName);
    }
    /*****************check it is after default load end****************/
    else
    {
        memcpy(szDevName,SSID,sizeof(szDevName));
    }
    
    /*for rand source start*/
    i  = strlen("CMCC_");
    for(;i < 11;i++)
    {
        randSourcw += (int)(szDevName[i]);
    }
    /*for rand source end*/
    
    
    memset(&g_session, 0x0, sizeof(g_session));
    memset(&g_workData, 0x0, sizeof(g_workData));
    memset(if_mac, 0x0, sizeof(if_mac));

    sprintf(g_workData.szDstAddress, "%s", hertUtil_getPlatformDstAddr());
    sprintf(g_workData.szCnnUserData, "%s", hertUtil_getUserData());
//strcpy(g_workData.szCnnPassData, "cnpass");
//sprintf(g_workData.szCnnPassData,"{\"SN\":\"%s\",\"MAC\":\"%s\"}","ABCDA12B123010F2","00:0C:43:76:20:66");
    sprintf(g_workData.szCnnPrtlData,"%s", hertUtil_getCnnPrtlData());

    if ( hertUtil_readMac(if_mac) <= 0 )
    {
        return ret;
    }
    HERT_LOGINFO("if_mac = %s\n", if_mac);
    sprintf(g_workData.szCnnPassData,"{\"SN\":\"%s%02X%02X%02X%02X%02X%02X\",\"MAC\":\"%02X:%02X:%02X:%02X:%02X:%02X\"}",
            hertUtil_getSN(),0xff & if_mac[0], 0xff & if_mac[1],0xff & if_mac[2],0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5],
            0xff & if_mac[0], 0xff & if_mac[1], 0xff & if_mac[2], 0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5]);
    HERT_LOGINFO("g_workData.szCnnPassData = %s\n", g_workData.szCnnPassData);

    sprintf(g_workData.abilityData.devID, "%s%02X%02X%02X%02X%02X%02X", 
            hertUtil_getSN(),0xff & if_mac[0], 0xff & if_mac[1],0xff & if_mac[2],0xff & if_mac[3], 0xff & if_mac[4], 0xff & if_mac[5]);            /* 设备编号 */

    HEX_DUMP(g_workData.abilityData.devID);

    strcpy(g_workData.abilityData.devName, szDevName);     /* 设备名称 */
    strcpy(g_workData.abilityData.fac, "DARE");              /* 设备厂家 */
    strcpy(g_workData.abilityData.type, hertUtil_getDeviceType());            /* 设备型号 */
    sprintf(g_workData.abilityData.softVersion, "%s", hertUtil_getFirmwareVersion());   /* 固件版本号 */

		
    signal(SIGINT,HERT_APP_OnSignal);
    signal(SIGHUP,HERT_APP_OnSignal);
    signal(SIGTERM,HERT_APP_OnSignal);

    pthread_mutex_init(&g_workData.mutex_cc,NULL);

    /* lock mutex for thread blocked */
    pthread_mutex_lock(&g_workData.mutex_cc);

    if(hertUtil_InitSem() < 0 )
    {
        HERT_LOGERR("Failed to hertUtil_InitSem app!");
        return RETCODE_INTERNEAL_ERROR;
    }
	
    /*inform time set start,used  setting the time of inform DevstatusInfo to platform,we inform between 18:00-22:00,every 10mins as point*/
    time_period = (randSourcw)%(24);
    HERT_LOGINFO("##############randSourcw:%d,time_period:%d################\n",randSourcw,time_period);
    /*calc the hour*/
    if((i = (time_period/6)) < 4)
    {
        time_hour = time_hour + i;
    }
    /*calc the mins*/
    if((i = (time_period%6)) < 6)
    {
        time_min = time_min + (i*10);
    }

    g_workData.informPlatHour = time_hour;
    g_workData.informPlatMin = time_min;
    g_workData.timingComplete = 0;
    /*inform time end*/
	
    pthread_create(&th_control,NULL,(void*)Msg_Process_PushData_DownLoad_Thread, NULL);

    return ret;
}

RETCODE HERT_APP_UnInit()
{
    RETCODE ret = RETCODE_SUCCESS;

    hertUtil_UnInitSem();

    g_workData.bStopRun = 1;

    pthread_mutex_unlock(&g_workData.mutex_cc);

    pthread_mutex_destroy(&g_workData.mutex_cc);
    
    while(g_workData.bStopRun != 2)
    {
        sleep(2);
    }

    if (g_session.listensock)
    {
        close(g_session.listensock);
    }

    HERT_LOGINFO("g_workData.bStopRun(%d)", g_workData.bStopRun);
    return ret;
}

int Msg_Process_Internel_download(char *pszUrl, char *pszOutFile);

int main(int argc, char **argv)
{
    SINT32  maxFd;
    fd_set  readFds;
    struct timeval tv;
    int n;
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    char *pWanIp = NULL;
    int nRecvZeroCount = 0;
	
    if (hertUtil_IsInFile("/var/hertdowntool","DEBUG") && (argc == 3 ) )
    {
        Msg_Process_Internel_download(argv[1], argv[2]);
        return 0;
    }
    if (hertUtil_IsInFile("/var/hertdevlistshow","DEBUG") )
    {
        HERT_APP_Init();
        vpcom_SetLogLevel(VPCOM_LOG_DEBUG);
        //herUtil_GetDeviceList(&g_workData.statusData);
        hertUtil_Dump(&g_workData.statusData);
        return 0;
    }

hert_restart:

    hertUtil_setLoglevel();

    HERT_LOGINFO("Start he-route app...");

    ret = HERT_APP_Init();
    if ( RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Failed to HERT_APP_Init app!");
        return -1;
    }
    

    //strcpy(g_session.authserver,"10.189.24.66");
    strcpy(g_session.authserver,hertUtil_getPlatformSVR());


    strcpy(g_session.authdomain,DEFAULT_AUTHDOMAIN);
    //g_session.authport = DEFAULT_AUTHPORT;
    g_session.authport = hertUtil_getPlatformSVRPort();
    if (hertUtil_IsInFile("/var/hertdebug","DEBUG"))
    {
        strcpy(g_session.localaddress,hertUtil_getLanIP());
    }
    else
    {
        strcpy(g_session.localaddress,hertUtil_getWanIP());
    }
    g_session.localport = 0;

    /* it will wait for 1.wan is up, 2.server is up */
    while(RETCODE_SUCCESS != (ret = HERT_TCP_Init(&g_session)))
    {
        sleep(5);
    }
        
    if ( RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Failed to HERT_TCP_Init app!");
        return -2;
    }

    maxFd = g_session.listensock + 1;

    while(!g_workData.bStopRun)
    {
        tv.tv_sec  = 4;
        tv.tv_usec = 0;
        FD_ZERO(&readFds);
        FD_SET(g_session.listensock, &readFds);           
        n = select(maxFd, &readFds, NULL, NULL, &tv);

        if (n < 0)
        {
            /* interrupted by signal or something, continue */
            continue;
        }

        hertUtil_setLoglevel();

        if ( 0 == n )
        {
            /* timeout: do other thing */
            ret = Msg_Process_Entry(NULL, 0);
            if (RETCODE_TCPSOCKET_ERROR == ret)
            {
                pWanIp = hertUtil_getWanIP();
                HERT_LOGERR("######## now start restart app -1-! ########");
                if ((*pWanIp != 0x0))
                {
                    HERT_APP_UnInit(); /* restart app */
                    goto hert_restart;
                }
            }
            
            continue; 
        }
        if (FD_ISSET(g_session.listensock, &readFds))
        {
            memset(&t, 0x0, sizeof(t));
            if (RETCODE_SUCCESS != HERT_TCP_RecvTransaction(&g_session, &t))
            {
                sleep(5);
                nRecvZeroCount++;
                pWanIp = hertUtil_getWanIP();
                HERT_LOGDEBUG("######## now start restart app -2-! ########");
                if ((*pWanIp != 0x0) && (nRecvZeroCount>=3)) /* the connect may cause error if coninues recev zero packet */
                {
                    nRecvZeroCount = 0;
                    HERT_APP_UnInit(); /* restart app */
                    goto hert_restart;
                }
                continue;
            }
            nRecvZeroCount = 0;

            /* process message entry */
            ret = Msg_Process_Entry(t.data, t.length);
        }
       
        /* we check the wan ip if tcp socket failed */
        if (RETCODE_TCPSOCKET_ERROR == ret)
        {
            pWanIp = hertUtil_getWanIP();
            //if ((*pWanIp != 0x0) && strcmp(g_session.localaddress, pWanIp))
            HERT_LOGERR("######## now start restart app! -3- ########");
            if ((*pWanIp != 0x0))
            {
                HERT_APP_UnInit(); /* restart app */
                goto hert_restart;
            }
        }
#if 0
        if (hertUtil_IsInFile("/var/hertrstdebug","DEBUG"))
        {
            HERT_LOGERR("######## now start restart app! debug start ########");
            HERT_APP_UnInit(); /* restart app */
            HERT_LOGERR("######## now start restart app! debug end ########");
            goto hert_restart;
        }
#endif
    }

    return 0;
}

RETCODE Msg_Process_Entry(IN CHAR *ptMsg, IN DWORD nMsgDataLen)
{
    HERT_MSGHEADER *pMsgHdr = NULL;
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    msgType = 0;
 
    if (!ptMsg)
    {
        HERT_LOGINFO("===============Msg_Process_Internel===============\n");
        return Msg_Process_Internel();
    }
    pMsgHdr = (HERT_MSGHEADER*)ptMsg;
    msgType = GET_MSG_TYPE(pMsgHdr->MsgType);
    HERT_LOGINFO(">>>>>>>>>>>>>>>>>MSG(%d):%s>>>>>>>>>>>>>>>>>\n",
                 msgType, vpComGetMsgTypeText(msgType));
    switch(msgType)
    {
        case PING_RESP:
        {
            HERT_LOGINFO("___PING_RESP__\n");
            break;
        }
        case CONN_RESP:
        {
            HERT_MSG_OPTION_CONN_RSP tMsgOption;
            memset(&tMsgOption, 0x0, sizeof(tMsgOption));
            ret = Parse_Msg_ConnRsp((CHAR*)ptMsg, nMsgDataLen, &tMsgOption);
            if ( RETCODE_SUCCESS != ret)
            {
                HERT_LOGERR("Failed at Parse_Msg_ConnRsp:%d!!\n", ret);
                break;
            }
            ret = Msg_Process_ConnResp(&tMsgOption);
            if ( RETCODE_SUCCESS != ret)
            {
                HERT_LOGERR("Failed at Msg_Process_ConnResp:%d!!\n", ret);
                break;
            }
            break;
        }
        case PUSH_DATA:
        {
            if ( g_workData.status != HERT_REGISTERED )
            {
                HERT_LOGERR("No need process push data for g_workData.status(%d)!!\n", g_workData.status);
                break;
            }

            HERT_LOGINFO("Msg_Process_PushData: dwMsgDataLen(%d)\n", nMsgDataLen);		
            ret = Msg_Process_PushData(ptMsg, nMsgDataLen);
            if ( RETCODE_SUCCESS != ret)
            {
                HERT_LOGERR("Failed at Parse_Msg_Data:%d!!\n", ret);
                break;
            }
            break;
        }
        case CONN_REQ:
        case PING_REQ:
        default:
        {
            HERT_LOGERR("Invalid MSG(%d):%s!!\n", msgType, vpComGetMsgTypeText(msgType));
            break;
        }

    }
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<MSG(%d):%s<<<<<<<<<<<<<<<<<\n",
                 msgType, vpComGetMsgTypeText(msgType));
    return ret;
}

RETCODE Msg_Process_ConnResp(HERT_MSG_OPTION_CONN_RSP *ptMsgOption)
{
    RETCODE ret = RETCODE_SUCCESS;
    
    if(!ptMsgOption)
    {
        HERT_LOGERR("Invalid parameter ptMsgOption(%p)!!\n", ptMsgOption);
    }
    if ( CNRSPERR_SUCCESS == ptMsgOption->nErrCode )
    {
        g_workData.status = HERT_REGISTERED;
    }
    else
    {
        HERT_LOGERR("ptMsgOption->nErrCode(%d)!!\n", ptMsgOption->nErrCode);
    }
    return ret;
}


RETCODE Msg_Process_PushData(IN CHAR *ptMsg, IN DWORD nMsgDataLen)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_OPTION_PUSH_DATA    *pTempOption = NULL;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_OPTION_PUSH_DATA        tMsgOption;
    CHAR   *ptMsgCnstBody = NULL;
    CHAR   *pMsgData = NULL;
    DWORD   dwOptionLen = 0;
    DWORD   dwDataLen = 0;
    int i = 0;
    /*next_ptMsg: seconder request start point,next_nMsgDataLen:seconder request length*/
    CHAR *next_ptMsg = NULL;
    DWORD next_nMsgDataLen = nMsgDataLen;
	
#define FREE_DATA() \
    if (tMsgOption.pData) \
    { \
        free(tMsgOption.pData); \
    }

	
Process_PushData_Start:	

    if(next_ptMsg != NULL && next_nMsgDataLen != nMsgDataLen)
    {
        
        ptMsg = next_ptMsg;
        nMsgDataLen = next_nMsgDataLen;	
		HERT_LOGINFO("start to precess multi request in left bytes(%d)....",nMsgDataLen);
	}

    if(!ptMsg || !nMsgDataLen)
    {
        HERT_LOGERR("Invalid parameter, nMsgDataLen(%d), ptMsg(%p)!!\n", nMsgDataLen, ptMsg);
        return RETCODE_INVALID_PARAM;
    }

    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));

    /* ====== parse msg data(option + body) */    
    ret = Parse_Msg_Data(ptMsg, (CHAR**)&pMsgData, &dwDataLen);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p)", ptMsg);
        return RETCODE_NO_MEMORY;
    }
    if ( pMsgData - ptMsg + dwDataLen > nMsgDataLen)
    {
        HERT_LOGERR("Maybe data was missed:t->length(%d), pMsgData - ptMsg + dwMsgDataLen= %d!!\n", 
                            nMsgDataLen, pMsgData - ptMsg + dwDataLen);
        return RETCODE_INVALID_MSGDATA;
    }
	/*maybe one select of read contains two request packs*/
	else if(pMsgData - ptMsg + dwDataLen + 4 < nMsgDataLen)
	{
        next_ptMsg = ptMsg + (pMsgData - ptMsg + dwDataLen);
        next_nMsgDataLen = next_nMsgDataLen - (pMsgData - ptMsg + dwDataLen);
	}
	else  /*this is usual request or no requests left in next bytes */
	{
        next_ptMsg = NULL;
		next_nMsgDataLen = nMsgDataLen;
	}
	
    pTempOption = (HERT_MSG_OPTION_PUSH_DATA*)pMsgData;

    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    tMsgOption.nHighLength = pTempOption->nHighLength;
    tMsgOption.nLowLength  = pTempOption->nLowLength;

    dwOptionLen = GetLengthFromTwoBytes(tMsgOption.nHighLength, tMsgOption.nLowLength);

    HERT_LOGINFO("tMsgOption.nHighLength(%d), tMsgOption.nLowLength(%d), dwOptionLen(%d)", 
                 tMsgOption.nHighLength, tMsgOption.nLowLength, dwOptionLen);

    if (!tMsgOption.pData)
    {
        tMsgOption.pData = malloc(dwOptionLen + 1);
        if (tMsgOption.pData)
        {
            memset(tMsgOption.pData, 0x0, dwOptionLen + 1);
        }
    }
    if (tMsgOption.pData)
    {
        memcpy(tMsgOption.pData, (CHAR*)pTempOption + GET_LENGTH_PUSH_OPTION(), dwOptionLen);
    }
    for(i = 0; i < dwOptionLen; i++)
    {
        HERT_LOGINFO("-----%x--------\n", 0xff & (*(tMsgOption.pData + i)));

    }

    ptMsgCnstBody = ptMsg + dwOptionLen + GET_LENGTH_PUSH_OPTION();

    /* cacl the length of body data */
    dwDataLen = nMsgDataLen - dwOptionLen - GET_LENGTH_PUSH_OPTION();

    /* ====== parse msg push data type */    
    ret = Parse_Msg_PushData_Type(ptMsgCnstBody, dwDataLen, tMsgConstBody.msgType, sizeof(tMsgConstBody.msgType) - 1);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_PushData_Type(%d)", ret);
        FREE_DATA();
        return ret;
    }

    HERT_LOGINFO(">>>>>>>>>>>>>>>>>MSG PUSH:%s>>>>>>>>>>>>>>>>>\n", tMsgConstBody.msgType);

    if (strstr(tMsgConstBody.msgType, STR(MSG_GET_INITINFO_RSP)))
    {
        ret = Msg_Process_PushData_InitInfoRsp(ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_ROUTER_ABILITY_NOTIFY_RSP)))
    {
        ret = Msg_Process_PushData_AbilityNotifyRsp(ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_GET_ROUTER_SPACE_REQ)))
    {
        ret = Msg_Process_PushData_RouterSpaceReq(&tMsgOption, ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_GET_ROUTER_STATUS_REQ)))
    {
        ret = Msg_Process_PushData_RouterStatusReq(&tMsgOption, ptMsgCnstBody,0);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_DOWNLOAD_IMG_REQ)))
    {
        ret = Msg_Process_PushData_DownLoadImgReq(&tMsgOption, ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_DOWNLOAD_COMP_NOTIFY_RSP)))
    {
        ret = Msg_Process_PushData_DownloadCompNotifyRsp(ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_GET_DEVUPDATE_INFO_RSP)))
    {
        ret = Msg_Process_PushData_GetUpdateInfoRsp(ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_ROUTER_STATUS_NOTIFY_RSP)))
    {
        ret = Msg_Process_PushData_RouterStatusNotifyRsp(ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_ADD_DOWNLOAD_MISSION_REQ)))
    {
        ret = Msg_Process_PushData_AddDownloadMissionReq(&tMsgOption, ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_DEVICE_OPERATION_REQ)))
    {
        ret = Msg_Process_PushData_DeviceOperateReq(&tMsgOption, ptMsgCnstBody);
    }
    else if (strstr(tMsgConstBody.msgType, STR(MSG_SUBDEV_STATUS_DOMAINNAME_RSP)) ||
        strstr(tMsgConstBody.msgType, STR(MSG_SUBDEV_CONNECTION_NOTIFY_RSP)) ||
        strstr(tMsgConstBody.msgType, STR(MSG_ROUTER_STATUS_NOTIFY_RSP)) ||
        strstr(tMsgConstBody.msgType, STR(MSG_DOWNLOAD_COMP_NOTIFY_RSP)) ||
        strstr(tMsgConstBody.msgType, STR(MSG_ROUTER_ABILITY_NOTIFY_RSP)) )
    {
        /* DO NOTHING */
    }
    else
    {
        ret = Msg_Process_PushData_UnknowDataReq(&tMsgOption, ptMsgCnstBody);
    }

    HERT_LOGINFO("<<<<<<<<<<<<<<<<<MSG PUSH:%s<<<<<<<<<<<<<<<<<\n", tMsgConstBody.msgType);

    FREE_DATA();

    if(next_ptMsg != NULL && next_nMsgDataLen != nMsgDataLen)
    {
        goto Process_PushData_Start;	
	}

    return ret;
}



RETCODE Msg_Process_PushData_InitInfoRsp(IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_RSP_PUSH_DATA_VARPART_INIT tMsgVarBody;
    DWORD dwDataLen = 0;

    HERT_LOGINFO("Do Msg_Process_PushData_InitInfoRsp");

    if(!ptMsgCnstBody)
    {
        HERT_LOGERR("Invalid parameter, ptMsgCnstBody(%p)!!\n", ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));
    memset(&tMsgVarBody, 0x0, sizeof(tMsgVarBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_InitInfoRsp(ptMsgCnstBody, dwDataLen, &tMsgConstBody, &tMsgVarBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_InitInfoRsp(%d)", ret);
        return ret;
    }

    memcpy(&g_workData.initData, &tMsgVarBody, sizeof(HERT_MSG_RSP_PUSH_DATA_VARPART_INIT));


    /* ====== send ability notify */    
    ret = Msg_Process_Internel_SendPushAbiNtfReq();
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Msg_Process_Internel_SendPushAbiNtfReq(%d)", ret);
        return ret;
    }

    return ret;

}

RETCODE Msg_Process_PushData_AbilityNotifyRsp(IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    DWORD dwDataLen = 0;

    if(!ptMsgCnstBody)
    {
        HERT_LOGERR("Invalid parameter, ptMsgCnstBody(%p)!!\n", ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_BodyConstPartRsp(ptMsgCnstBody, dwDataLen, &tMsgConstBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_BodyConstPartRsp(%d)", ret);
        return ret;
    }

    HERT_LOGINFO("Ability Notify Response was processed, here, do nothing\n");

    return ret;

}


RETCODE Msg_Process_PushData_RouterStatusNotifyRsp(IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    DWORD dwDataLen = 0;

    if(!ptMsgCnstBody)
    {
        HERT_LOGERR("Invalid parameter, ptMsgCnstBody(%p)!!\n", ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_BodyConstPartRsp(ptMsgCnstBody, dwDataLen, &tMsgConstBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_BodyConstPartRsp(%d)", ret);
        return ret;
    }

    HERT_LOGINFO("Router Status Response was processed, here, do nothing\n");

    return ret;

}
//new
RETCODE  Msg_Process_PushData_UpdateDevConNotifyREQ(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption,IN CHAR *ptMsgCnstBody, struct flow_type_queue *presponse)
{
	RETCODE ret = RETCODE_SUCCESS;
	HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
	DWORD dwDataLen = 0;
	CHAR *pData;

	HERT_LOGINFO("Do  Msg_Process_PushData_UpdateDevConNotifyREQ");

	tRspMsgConstBody.version = 0x0010;
	strcpy(tRspMsgConstBody.msgType, STR(MSG_SUBDEV_CONNECTION_NOTIFY_REQ));
	sprintf(tRspMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);
	/* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
	ret = Build_Msg_ConNotifyRsp(&pData, &dwDataLen, ptMsgOption, &tRspMsgConstBody,presponse);
	if (RETCODE_SUCCESS != ret)
	{
	//	HERT_LOGERR(" Build_Msg_DomainStauteRsp Failed at ret(%d)!!\n", ret);
		return ret;
	}

	ret = HERT_TCP_SendTransactionExt(&g_session, pData, dwDataLen);
	if (RETCODE_SUCCESS != ret)
	{
		HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
		//FREE_PPDATA(pData);
		return ret;
	}
	return ret;
}

RETCODE Msg_Process_PushData_UpdateDomainNameREQ(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption,IN CHAR *ptMsgCnstBody,struct dev_index_type * pdevindex)
{
	RETCODE ret = RETCODE_SUCCESS;
	HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
	DWORD dwDataLen = 0;
	CHAR *pData;

	HERT_LOGINFO("Do  Msg_Process_PushData_UpdateDomainNameREQ");

	tRspMsgConstBody.version = 0x0010;
	strcpy(tRspMsgConstBody.msgType, STR(MSG_SUBDEV_STATUS_DOMAINNAME_REQ));
	sprintf(tRspMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);

	HERT_LOGINFO("devName=%s\n",pdevindex->devinfo->devName);
	/* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
	ret = Build_Msg_DomainStauteRsp(&pData, &dwDataLen, ptMsgOption, &tRspMsgConstBody,pdevindex);
	if (RETCODE_SUCCESS != ret)
	{
	//	HERT_LOGERR(" Build_Msg_DomainStauteRsp Failed at ret(%d)!!\n", ret);
		return ret;
	}

	ret = HERT_TCP_SendTransactionExt(&g_session, pData, dwDataLen);
	if (RETCODE_SUCCESS != ret)
	{
		HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
		//FREE_PPDATA(pData);
		return ret;
	}
	return ret;
}



/*this function also use for timing inform to plat ,the flag is "isSendRequest" */
RETCODE Msg_Process_PushData_RouterStatusReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption,IN CHAR *ptMsgCnstBody,int isSendRequest) 
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    DWORD dwDataLen = 0;
    char *pData = NULL;
    DWORD dwDevNumber = 0;

#define FREE_PPDATA(a) \
	if(a) \
	{ \
		free(a); \
		a = NULL; \
	} \
    g_workData.statusData.devNum = 0; \
    if (g_workData.statusData.pDevList) \
    { \
        free(g_workData.statusData.pDevList); \
        g_workData.statusData.pDevList = NULL; \
    }

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody));
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));

    //for status request process,"else" for timing inform
	if(isSendRequest == 0)
	{
		/* ====== parse msg body */
		ret = Parse_Msg_BodyConstPartReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody);
		if ( ret != RETCODE_SUCCESS )
		{
			HERT_LOGERR("Failed at Parse_Msg_BodyConstPartReq(%d)", ret);
			return ret;
		}

    	tRspMsgConstBody.version = tReqMsgConstBody.version;
    	strcpy(tRspMsgConstBody.msgType, STR(MSG_GET_ROUTER_STATUS_RSP));
    	strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);
	}
	else
	{
    	tRspMsgConstBody.version = 0x0010;
    	strcpy(tRspMsgConstBody.msgType, STR(MSG_ROUTER_STATUS_NOTIFY_REQ));
		sprintf(tRspMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);
	}

    memset(t.data, 0x0, sizeof(t.data));

    FREE_PPDATA(pData);

    hertUtil_GetDeviceList(&g_workData.statusData.pDevList, &dwDevNumber);
    g_workData.statusData.devNum = dwDevNumber;
	
    g_workData.statusData.downbandwidth = hertUtil_getBroadband();
 
    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_RouteStatusRsp(&pData, &dwDataLen, ptMsgOption, &tRspMsgConstBody, &g_workData.statusData,isSendRequest);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteStatusRsp Failed at ret(%d)!!\n", ret);
        FREE_PPDATA(pData);
        return ret;
    }

    ret = HERT_TCP_SendTransactionExt(&g_session, pData, dwDataLen);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        FREE_PPDATA(pData);
        return ret;
    }
    FREE_PPDATA(pData);
    return ret;
}

RETCODE Msg_Process_PushData_RouterSpaceReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    DWORD dwDataLen = 0;

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody));
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_BodyConstPartReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_BodyConstPartReq(%d)", ret);
        return ret;
    }
    tRspMsgConstBody.version = tReqMsgConstBody.version;
    strcpy(tRspMsgConstBody.msgType, STR(MSG_GET_ROUTER_SPACE_RSP));
    strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);

    memset(t.data, 0x0, sizeof(t.data));

    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_RouteSpaceRsp(t.data, sizeof(t.data), &t.length, ptMsgOption, &tRspMsgConstBody, &g_workData.spaceData);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteSpaceRsp Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }
    return ret;
}

RETCODE Msg_Process_PushData_AddDownloadMissionReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION tReqMsgVarBody;
    HERT_MSG_RSP_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION tRspMsgVarBody;
    DWORD dwDataLen = 0;

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody)); 
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));
    memset(&tReqMsgVarBody, 0x0, sizeof(tReqMsgVarBody));
    memset(&tRspMsgVarBody, 0x0, sizeof(tRspMsgVarBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_AddDownloadMissionReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody, &tReqMsgVarBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_AddDownloadMissionReq(%d)", ret);
        return ret;
    }
    tRspMsgConstBody.version = tReqMsgConstBody.version;
    strcpy(tRspMsgConstBody.msgType, STR(MSG_ADD_DOWNLOAD_MISSION_RSP));
    strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);

    memset(t.data, 0x0, sizeof(t.data));

    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_AddDownloadMissionRsp(t.data, sizeof(t.data), &t.length, ptMsgOption, &tRspMsgConstBody, &tRspMsgVarBody/*&g_workData.missionInfo*/);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteSpaceRsp Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }
    return ret;
}



RETCODE Msg_Process_PushData_DeviceOperateReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_DEVICEOPERATE tReqMsgVarBody;
    DWORD dwDataLen = 0;
    char sysCmd[32];

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody)); 
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));
    memset(&tReqMsgVarBody, 0x0, sizeof(tReqMsgVarBody));
    memset(sysCmd,0x0,sizeof(sysCmd));

    /* ====== parse msg body */    
    ret = Parse_Msg_DeviceOperateReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody, &tReqMsgVarBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_AddDownloadMissionReq(%d)", ret);
        return ret;
    }
    
    /*1:device wifi control*/
    if(tReqMsgVarBody.operType == 1)
    {   /*close wifi*/
        if(strcmp(tReqMsgVarBody.operPara,"1") == 0)  
        {
#if defined (RT2860_MBSS_SUPPORT)
            int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
            do {
                bssid_num--;
                system("ifconfig ra%d down", bssid_num);
            } while (bssid_num > 0);
#else
            system("ifconfig ra0 down");
#endif
            system("rmmod rt2860v2_ap");
            system("reg s b0180000");
            FILE *pp = popen("reg p 400", "r");
            char reg[11];
            fscanf(pp, "%s\n", reg);
            pclose(pp);
            snprintf(sysCmd,sizeof(sysCmd) - 1,"reg w 400 %x",strtoul(reg, NULL, 16)&(~(7<<9)));
            //system("reg w 400 %x", strtoul(reg, NULL, 16)&(~(7<<9)));
            system(sysCmd);
            system("reg w 1204 8");
            system("reg w 1004 3");
            nvram_set(RT2860_NVRAM, "WiFiOff", "1");
            
        } /*open wifi*/
        else if(strcmp(tReqMsgVarBody.operPara,"0") == 0)  
        {
        	
            system("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT)
            int idx = 0;
            int bssid_num = atoi(nvram_bufget(RT2860_NVRAM, "BssidNum"));
            do {
                system("ifconfig ra%d up", idx);
                idx++;
            } while (idx < bssid_num);
#else
            system("ifconfig ra0 up");
#endif
            nvram_set(RT2860_NVRAM, "WiFiOff", "0");
        }
        nvram_commit(RT2860_NVRAM);
    }
    
    
    tRspMsgConstBody.version = tReqMsgConstBody.version;
    strcpy(tRspMsgConstBody.msgType, STR(MSG_DEVICE_OPERATION_RSP));
    strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);

    memset(t.data, 0x0, sizeof(t.data));

    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_DeviceOperateRsp(t.data, sizeof(t.data), &t.length, ptMsgOption, &tRspMsgConstBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteSpaceRsp Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }
    if(atoi(tReqMsgVarBody.operPara) == 0)
    {
        sleep(1);
        system("internet.sh");
    }
    return ret;
}

RETCODE Msg_Process_PushData_UnknowDataReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    DWORD dwDataLen = 0;
    char  *pchr = NULL;
    char  szErrVersion[] = { 0xE7,0x89,0x88,0xE6,0x9C,0xAC,0xE5,0xA4,0xAA,0xE4,0xBD,0x8E,0x0}; // 版本太低

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody));
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));

    /* ====== parse msg body */
    ret = Parse_Msg_UnknowDataReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_AddDownloadMissionReq(%d)", ret);
        return ret;
    }
    
    tRspMsgConstBody.version = tReqMsgConstBody.version;
    strcpy(tRspMsgConstBody.msgType, tReqMsgConstBody.msgType);
    strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);

    pchr = strstr(tRspMsgConstBody.msgType, "REQ");
    if(pchr)
    {
        strncpy(pchr, "RSP", strlen("RSP"));
    }

    memset(t.data, 0x0, sizeof(t.data));

    tRspMsgConstBody.errorCode = 0x6003;
    //strcpy(tRspMsgConstBody.description, "版本太低");
    memcpy(tRspMsgConstBody.description, szErrVersion, sizeof(szErrVersion));

    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_UnknowDataRsp(t.data, sizeof(t.data), &t.length, ptMsgOption, &tRspMsgConstBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteSpaceRsp Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_PushData_DoDownloadCompReq(HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *pDownItem)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_OPTION_PUSH_DATA        tMsgOption;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD_COMP tMsgVarBody;                          

    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));
    memset(&tMsgVarBody, 0x0, sizeof(tMsgVarBody));

    tMsgOption.pData   = g_workData.szDstAddress;
    GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);

    tMsgConstBody.version = 0x0010;                             /* 协议版本 */
    sprintf(tMsgConstBody.msgType, STR(MSG_DOWNLOAD_COMP_NOTIFY_REQ));  /* 消息类型 */
    sprintf(tMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);  /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */

    strcpy(tMsgVarBody.contentID, pDownItem->contentID);

    memset(t.data, 0x0, sizeof(t.data));

    /* build: C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
    ret = Build_Msg_DownloadCompReq(t.data, sizeof(t.data), &t.length, &tMsgOption, &tMsgConstBody, &tMsgVarBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_RouteSpaceRsp Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }
    return ret;
}
//#define DEBUG_DOWNLOAD 1

size_t Msg_Process_DownLoad_Write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}


struct hert_curl_data 
{
  char trace_ascii; /* 1 or 0 */
};

static void hert_curl_dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size,
          char nohex)
{
    size_t i;
    size_t c;

    unsigned int width=0x10;

    if(nohex)
    {
        /* without the hex output, we can fit more on screen */
        width = 0x40;
    }

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);

    for(i=0; i<size; i+= width)
    {
        fprintf(stream, "%4.4lx: ", (long)i);

        if(!nohex) 
        {
            /* hex not disabled, show it */
            for(c = 0; c < width; c++)
                if(i+c < size)
                    fprintf(stream, "%02x ", ptr[i+c]);
                else
                    fputs("   ", stream);
        }

        for(c = 0; (c < width) && (i+c < size); c++) 
        {
            /* check for 0D0A; if found, skip past and start a new line of output */
            if (nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) 
            {
                i+=(c+2-width);
                break;
            }
            fprintf(stream, "%c", (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
            /* check again for 0D0A, to avoid an extra \n if it's at width */
            if (nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A)
            {
                i+=(c+3-width);
                break;
            }
        }
        fputc('\n', stream); /* newline */
    }
    fflush(stream);
}

static int hert_curl_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
    struct hert_curl_data *config = (struct hert_curl_data *)userp;
    const char *text;
    (void)handle; /* prevent compiler warning */
  
    if (!hertUtil_IsInFile("/var/curldebug","DEBUG"))
    {
        HERT_LOGINFO("SKIP TO PRINT CUR LIBS DATA");
        return 0;
    }

    switch (type) 
    {
    case CURLINFO_TEXT:
        fprintf(stderr, "== Info: %s", data);
    default: /* in case a new one is introduced to shock us */
        return 0;

    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
    break;
    }

    hert_curl_dump(text, stderr, (unsigned char *)data, size, config->trace_ascii);
    return 0;
}

void Msg_Process_PushData_DownLoad_Thread(void * arg)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    CHAR szOutfilename[128];
    CHAR pathName[64] = {0};
    CHAR szPicture_pathName[64] = {0};
    struct hert_curl_data config;
    int  nRetry = 0;
    long long freeSize = 0;
    HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD tDownItem;

#ifdef DEBUG_DOWNLOAD /* debug */
    CHAR *url = "http://10.189.24.66 :4030/intro.jpg";
#endif

    printf("=============start Msg_Process_PushData_DownLoad_Thread(pid %u)============\n", getpid());
    while(!g_workData.bStopRun)
    {
        /* if there is no down item, we need wait for mutex */
        memset(&tDownItem, 0x0, sizeof(tDownItem));
        if ( 0 != hertUtil_GetDownReqItemList(&tDownItem))
        {
            pthread_mutex_lock(&g_workData.mutex_cc);

            memset(&tDownItem, 0x0, sizeof(tDownItem));
            if ( 0 != hertUtil_GetDownReqItemList(&tDownItem))
            {
                sleep(1);
                continue;
            }
        }

        if(g_workData.bStopRun)
        {
            break;
        }
        sleep(1); /* we sleep 1 second for wait the configurations update */

        nRetry = 0;

        memset(szOutfilename, 0x0, sizeof(szOutfilename));
#ifdef DEBUG_DOWNLOAD /* debug */
        sprintf(szOutfilename, "/media/sda1/intro.jpg");
#else
        hertUtil_GetUSBPartitionMaxFreeSize(&freeSize, pathName, sizeof(pathName));

        /* ------- add for check dir exist, else it will crashed, start -------*/
        memset(szPicture_pathName, 0x0, sizeof(szPicture_pathName));
        sprintf(szPicture_pathName, "%s/picture", pathName);
        if ( hertUtil_createDirIfNoExist(szPicture_pathName) < 0 )
        {
            sleep(1);
            continue;
        }
        /* ------- add for check dir exist, else it will crashed, end -------*/

        sprintf(szOutfilename, "%s/picture/%s",pathName, tDownItem.contentName);	

#endif
        HERT_LOGINFO("szOutfilename(%s), url(%s)", szOutfilename, tDownItem.URL);

/* download action */
DoDownLoad:
        curl = curl_easy_init();
        if (curl) 
        {
            fp = fopen(szOutfilename,"wb");
            if (fp)
            {
#ifdef DEBUG_DOWNLOAD /* debug */
                res = curl_easy_setopt(curl, CURLOPT_URL, url);
#else
                res = curl_easy_setopt(curl, CURLOPT_URL, tDownItem.URL);
#endif
                res = curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, hert_curl_trace);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGFUNCTION) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGDATA) failed: %s\n", curl_easy_strerror(res));
                }

                /* the DEBUGFUNCTION has no effect until we enable VERBOSE */
                res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_VERBOSE) failed: %s\n", curl_easy_strerror(res));
                }

                res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Msg_Process_DownLoad_Write_data);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEFUNCTION) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEDATA) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_perform(curl);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }

            }
            curl_easy_cleanup(curl);

            fflush(fp);
            fsync(fp);
            fclose(fp);
        }

        sleep(1);

        if (hertUtil_getFileSize(szOutfilename) <= 0 )
        {
            nRetry++;
            HERT_LOGERR("hertUtil_getFileSize(%s) is empty, retry(%d) do it again\n", szOutfilename, nRetry);
            if ( nRetry <= 3 )
            {
                goto DoDownLoad;
            }
        }
		hertUtil_DelDownReqItemList(&tDownItem);

        /* send download complete request if it is success to download */
        Msg_Process_PushData_DoDownloadCompReq(&tDownItem);
    }
    g_workData.bStopRun = 2;

    return;
}

RETCODE Msg_Process_PushData_DownLoadImgReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tRspMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tReqMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD tMsgVarBody;
    DWORD dwDataLen = 0;

    if( (!ptMsgOption) || (!ptMsgCnstBody) )
    {
        HERT_LOGERR("Invalid parameter, ptMsgOption(%p), ptMsgCnstBody(%p)!!\n", ptMsgOption, ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);
                          
    memset(&tRspMsgConstBody, 0x0, sizeof(tRspMsgConstBody));
    memset(&tReqMsgConstBody, 0x0, sizeof(tReqMsgConstBody));

    /* ====== parse msg body */
    ret = Parse_Msg_DownloadReq(ptMsgCnstBody, dwDataLen, &tReqMsgConstBody, &tMsgVarBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_DownloadReq(%d)", ret);
        return ret;
    }
    tRspMsgConstBody.version = tReqMsgConstBody.version;
    strcpy(tRspMsgConstBody.msgType, STR(MSG_DOWNLOAD_IMG_RSP));
    strcpy(tRspMsgConstBody.msgSeq, tReqMsgConstBody.msgSeq);
    if (!hertUtil_isStorageExist())
    {
        HERT_LOGERR("The storage is not exist!");
        tRspMsgConstBody.errorCode = 0x0001;
        strcpy(tRspMsgConstBody.description, "The storage is not exist!");
    }
    else
    {
        memcpy(&g_workData.downData, &tMsgVarBody, sizeof(HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD));
        hertUtil_AddDownReqItemList(&g_workData.downData);
        pthread_mutex_unlock(&g_workData.mutex_cc);
    }

    ret = Build_Msg_DownloadRsq(t.data, sizeof(t.data), &t.length, ptMsgOption, &tRspMsgConstBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_DownloadRsq Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_PushData_DownloadCompNotifyRsp(IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    DWORD dwDataLen = 0;

    if(!ptMsgCnstBody)
    {
        HERT_LOGERR("Invalid parameter, ptMsgCnstBody(%p)!!\n", ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_BodyConstPartRsp(ptMsgCnstBody, dwDataLen, &tMsgConstBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_BodyConstPartRsp(%d)", ret);
        return ret;
    }

    HERT_LOGINFO("Dowload complete notify Response was processed, here, do nothing\n");

    return ret;

}

RETCODE Msg_Process_PushData_GetUpdateInfoRsp(IN CHAR *ptMsgCnstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_RSP_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE tMsgVarBody;
    DWORD dwDataLen = 0;
    FILE* fw = NULL;

#define TEMP_NEEDUPGRADEINFO "/var/upgradeinfo"

    HERT_LOGINFO("Do Msg_Process_PushData_InitInfoRsp");

    if(!ptMsgCnstBody)
    {
        HERT_LOGERR("Invalid parameter, ptMsgCnstBody(%p)!!\n", ptMsgCnstBody);
        return RETCODE_INVALID_PARAM;
    }
    dwDataLen = strlen(ptMsgCnstBody);

    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));
    memset(&tMsgVarBody, 0x0, sizeof(tMsgVarBody));

    /* ====== parse msg body */    
    ret = Parse_Msg_GetUpdateRsp(ptMsgCnstBody, dwDataLen, &tMsgConstBody, &tMsgVarBody);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Failed at Parse_Msg_InitInfoRsp(%d)", ret);
        return ret;
    }

    fw = fopen(TEMP_NEEDUPGRADEINFO, "w");
    if ( !fw)
    {
        HERT_LOGERR("TEMP_NEEDUPGRADEINFO(%s)\n", TEMP_NEEDUPGRADEINFO);
        return -2;
    }

    if ( 1 != fwrite(&tMsgVarBody, sizeof(tMsgVarBody), 1, fw))
    {
        HERT_LOGERR("fwrite error\n");
    }

    fclose(fw);

    memcpy(&g_workData.upgradeData, &tMsgVarBody, sizeof(HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE));

    return ret;
}

RETCODE Msg_Process_Internel_SendConnReq()
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_OPTION_CONN_REQ tMsgOption;
    HERT_MSG_BODY_CONN_REQ tMsgBody;
    HERT_TRANSACTION t;

    memset(t.data, 0x0, sizeof(t.data));
    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgBody, 0x0, sizeof(tMsgBody));

    tMsgOption.pProtocol   = g_workData.szCnnPrtlData;
    GetHighLowFromString(tMsgOption.pProtocol, &tMsgOption.nHighLength, &tMsgOption.nLowLength);
    tMsgOption.nVer        = 1;
    tMsgOption.nConnFlag   = SET_USER_TRUE(tMsgOption.nConnFlag);
    tMsgOption.nConnFlag   = SET_PASS_TRUE(tMsgOption.nConnFlag);
    tMsgOption.nHighTime   = 0x01;
    tMsgOption.nLowTime    = 0x00;

    tMsgBody.nDevHighLength= 0x00;
    tMsgBody.nDevLowLength = 0x00;
    tMsgBody.pUserData     = g_workData.szCnnUserData;
    GetHighLowFromString(tMsgBody.pUserData, &tMsgBody.nUserHighLength, &tMsgBody.nUserLowLength);

    tMsgBody.pPassData     = g_workData.szCnnPassData;
    GetHighLowFromString(tMsgBody.pPassData, &tMsgBody.nPassHighLength, &tMsgBody.nPassLowLength);
    ret = Build_Msg_ConnReq(t.data, sizeof(t.data), &t.length, &tMsgOption, &tMsgBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_ConnReq Failed at ret(%d)!!\n", ret);
        return ret;
    }

    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}


RETCODE Msg_Process_Internel_SendPushInitReq()
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_OPTION_PUSH_DATA tMsgOption;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_INIT tMsgVarBody;
    HERT_TRANSACTION t;

    memset(t.data, 0x0, sizeof(t.data));
    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));
    memset(&tMsgVarBody, 0x0, sizeof(tMsgVarBody));

    tMsgOption.pData   = g_workData.szDstAddress;
    GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);

    tMsgConstBody.version = 0x0010;                             /* 协议版本 */
    sprintf(tMsgConstBody.msgType, STR(MSG_GET_INITINFO_REQ));  /* 消息类型 */
    sprintf(tMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);  /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */

    strcpy(tMsgVarBody.devID, g_workData.abilityData.devID);      /* 设备编号 */
    tMsgVarBody.deviceType = 1;                  /* 设备类型 */
    if (hertUtil_IsInFile("/var/hertdebug","DEBUG"))
    {
        strcpy(tMsgVarBody.IP, hertUtil_getLanIP()); /* 设备IP   */
    }
    else
    {
        strcpy(tMsgVarBody.IP, hertUtil_getWanIP()); /* 设备IP   */
    }

    ret = Build_Msg_InitInfoReq(t.data, sizeof(t.data), &t.length, &tMsgOption, &tMsgConstBody, &tMsgVarBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_ConnReq Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_Internel_SendPushAbiNtfReq()
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_OPTION_PUSH_DATA tMsgOption;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_ABILITY_NOTIFY *ptMsgVarBody;
    HERT_TRANSACTION t;

    memset(t.data, 0x0, sizeof(t.data));
    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));

    tMsgOption.pData   = g_workData.szDstAddress;
    GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);

    tMsgConstBody.version = 0x0010;                             /* 协议版本 */
    sprintf(tMsgConstBody.msgType, STR(MSG_ROUTER_ABILITY_NOTIFY_REQ));  /* 消息类型 */
    sprintf(tMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);  /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */

    ptMsgVarBody = &g_workData.abilityData;

    ret = Build_Msg_AbilityNotifyReq(t.data, sizeof(t.data), &t.length, &tMsgOption, &tMsgConstBody, ptMsgVarBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_ConnReq Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_Internel_SendHeartbeatReq()
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_TRANSACTION t;

    memset(t.data, 0x0, sizeof(t.data));

    /* build: ping req C->S, DATA_FORMAT: MSGHEADER */
    ret = Build_Msg_PingReq(t.data, sizeof(t.data), &t.length);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_PingReq Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_Internel_SendGetUpgradeInfoReq()
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSG_OPTION_PUSH_DATA tMsgOption;
    HERT_MSG_REQ_PUSH_DATA_CONSTPART tMsgConstBody;
    HERT_MSG_REQ_PUSH_DATA_VARPART_GET_UPDATE tMsgVarBody;
    HERT_TRANSACTION t;

    memset(t.data, 0x0, sizeof(t.data));
    memset(&tMsgOption, 0x0, sizeof(tMsgOption));
    memset(&tMsgConstBody, 0x0, sizeof(tMsgConstBody));
    memset(&tMsgVarBody, 0x0, sizeof(tMsgVarBody));

    tMsgOption.pData   = g_workData.szDstAddress;
    GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);

    tMsgConstBody.version = 0x0010;                                   /* 协议版本 */
    sprintf(tMsgConstBody.msgType, STR(MSG_GET_DEVUPDATE_INFO_REQ));  /* 消息类型 */
    sprintf(tMsgConstBody.msgSeq, "%ld", ++g_workData.msgSeq);        /* 消息序号: 从1开始递增，响应消息与对应的请求消息序号相同 */

    tMsgVarBody.type = 2;                                                     /* 升级客户端类型 */
    strcpy(tMsgVarBody.currentVersion, g_workData.abilityData.softVersion);   /* 当前版本 */
    strcpy(tMsgVarBody.devID, g_workData.abilityData.devID);                  /* 设备标识号 */

    ret = Build_Msg_GetUpgradeInfoReq(t.data, sizeof(t.data), &t.length, &tMsgOption, &tMsgConstBody, &tMsgVarBody);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("Build_Msg_ConnReq Failed at ret(%d)!!\n", ret);
        return ret;
    }
    
    ret = HERT_TCP_SendTransaction(&g_session, &t);
    if (RETCODE_SUCCESS != ret)
    {
        HERT_LOGERR("HERT_TCP_SendTransaction Failed at ret(%d)!!\n", ret);
        return ret;
    }

    return ret;
}

RETCODE Msg_Process_Internel_DownloadForUpgrade()
{
    RETCODE ret = RETCODE_SUCCESS;
    CURL *curl;
    FILE *fp;
    CURLcode res;
    CHAR szOutfilename[128];
    struct hert_curl_data config;
    int  nRetry = 0;
    static int nIsDownLoading = 0;

    if (nIsDownLoading)
    {
        return ret;
    }
    nIsDownLoading = 1;

    printf("=============start Msg_Process_Internel_Upgrade(pid %u)============\n", getpid());
    system("echo rm -rf /var/fmdownresult");
    while(hertUtil_IsInFile("/var/downloadnow", "downloadnow"))
    {
        sleep(1); /* we sleep 1 second for wait the configurations update */

        HERT_LOGINFO("szOutfilename(%s)", szOutfilename);

        nRetry = 0;

        memset(szOutfilename, 0x0, sizeof(szOutfilename));
        sprintf(szOutfilename, "/var/%s", "imgfile");
        HERT_LOGINFO("szOutfilename(%s), url(%s)", szOutfilename, g_workData.upgradeData.updateUrl);

/* download action */
DoDownLoad:
        curl = curl_easy_init();
        if (curl) 
        {
            fp = fopen(szOutfilename,"wb");
            if (fp)
            {
                res = curl_easy_setopt(curl, CURLOPT_URL, g_workData.upgradeData.updateUrl);

                res = curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, hert_curl_trace);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGFUNCTION) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGDATA) failed: %s\n", curl_easy_strerror(res));
                }

                /* the DEBUGFUNCTION has no effect until we enable VERBOSE */
                res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_VERBOSE) failed: %s\n", curl_easy_strerror(res));
                }

                res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Msg_Process_DownLoad_Write_data);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEFUNCTION) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEDATA) failed: %s\n", curl_easy_strerror(res));
                }
                res = curl_easy_perform(curl);
                /* Check for errors */
                if(res != CURLE_OK)
                {
                    HERT_LOGERR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }

            }
            curl_easy_cleanup(curl);
            fclose(fp);
        }

        sleep(1);

        if (hertUtil_getFileSize(szOutfilename) <= 0 )
        {
            nRetry++;
            HERT_LOGERR("hertUtil_getFileSize(%s) is empty, retry(%d) do it again\n", szOutfilename, nRetry);
            if ( nRetry <= 3 )
            {
                goto DoDownLoad;
            }
            system("rm -rf /var/downloadnow 2>/dev/null");
            system("echo faileddownfm > /var/fmdownresult");
            break;
        }
        else
        {
            system("rm -rf /var/downloadnow 2>/dev/null");
            system("echo successdownfm > /var/fmdownresult");
            break;
        }
    }
    nIsDownLoading = 0;

    return ret;
}

RETCODE Msg_Process_Internel_UpdateSpaceData()
{
    RETCODE ret = RETCODE_SUCCESS;

    if (!hertUtil_isStorageExist())
    {
        HERT_LOGINFO("The storage is not exist!");
        memset(&g_workData.spaceData, 0x0, sizeof(g_workData.spaceData));
        g_workData.spaceData.diskStatus = 1;
    }
    else
    {
        HERT_LOGINFO("The storage is exist!");
        g_workData.spaceData.diskStatus = 0;
        hretUtil_GetUSBSize(&g_workData.spaceData.leftSpace,
                                &g_workData.spaceData.totalSpace);

        g_workData.spaceData.usedSpace  = g_workData.spaceData.totalSpace - g_workData.spaceData.leftSpace;        /* 已用空间, 单位MB */
        HERT_LOGINFO("g_workData.spaceData.usedSpace(%ld)", g_workData.spaceData.usedSpace);
    }

    return ret;
}

/*
  0: do nothing
  1: major parameter is changed, need reboot app 
  2: ability parmeter is changed, need update ability

*/
int Msg_Process_Internel_UpdateParameter()
{
    int ret = 0;
    char szDevName[64] = {0x0};
    const char *SSID = nvram_bufget(RT2860_NVRAM, "SSID1");

    if(SSID)
    {
        memcpy(szDevName,SSID,sizeof(szDevName));
    }
    if(strcmp(g_workData.abilityData.devName,szDevName) ) /* devname was not same */
    {
        strcpy(g_workData.abilityData.devName, szDevName);     /* 设备名称 */
        ret = 2;
    }
    return ret;
}
	
RETCODE Msg_Process_Internel_UpdateStatusData()
{
    RETCODE ret = RETCODE_SUCCESS;

    g_workData.statusData.wifiStatus = (hertUtil_isWifiOn() == 1) ? 0 : 1;
    g_workData.statusData.devStatus  = 0; /* it must be work if it can reachable to internet */
    g_workData.statusData.runStatus     = hertUtil_getWorkStatus();
    g_workData.statusData.downbandwidth = hertUtil_getBroadband();
    strcpy(g_workData.statusData.broadbandRate, hertUtil_getBroadbandRate());

    return ret;
}
int parse_strToMac(char *str,struct ether_addr *pmac)
{
	return sscanf(str,"%2hx:%2hx:%2hx:%2hx:%2hx:%2hx",&pmac->mac[0],&pmac->mac[1],&pmac->mac[2],&pmac->mac[3],&pmac->mac[4],&pmac->mac[5]);
}
int hertUtil_maccmp(struct ether_addr a,struct ether_addr b){
	return (a.mac[0]==b.mac[0]&&a.mac[1]==b.mac[1]&&  \
			a.mac[2]==b.mac[2]&&a.mac[3]==b.mac[3]&&   \
			a.mac[4]==b.mac[4]&&a.mac[5]==b.mac[5]);
}
int hertUtil_add_domain_queue(struct dev_index_type * dev_indexs,struct mymesg *mesg,int devNum,int *tolal)
{
	int i=0;
		
	for(i=0;i<devNum;i++)
	{
   HERT_LOGINFO("dev_indexs[%d].mac=%x,mesg->domain_tmp.mac=%x\n",i,dev_indexs[i].mac,mesg->domain_tmp.mac);
		if(hertUtil_maccmp(dev_indexs[i].mac,mesg->domain_tmp.mac))
		{
			//printf(" mac is eque\n");
			struct domain_queue_type *new_queue=(struct domain_queue_type *)malloc(sizeof(struct domain_queue_type));
			struct domain_queue_type *tmp=NULL;    
			new_queue->data=(struct domain_info*)malloc(sizeof(struct domain_info));
			(*(new_queue->data))=mesg->domain_tmp;
			if(dev_indexs[i].domain_queue==NULL){
				dev_indexs[i].domain_queue=new_queue;
				new_queue->next=NULL;	
				dev_indexs[i].ip= new_queue->data->ip;
			}else
			{
				tmp=dev_indexs[i].domain_queue->next;
				dev_indexs[i].domain_queue->next=new_queue;
				new_queue->next=tmp;
			}

			dev_indexs[i].num++;

		(*tolal) +=sizeof(struct domain_info);
		//printf("total=%d\n",tolal);

		if((*tolal)>5000)
			return 0;
		}
	}
	return 0;
}
int hertUtil_isClientOnline(struct ether_addr  mac,struct dev_index_type * dev_indexs,int devNum){
	int i=0; 
	int ret=0;
	for(i=0;i<devNum;i++)
	{
		if(hertUtil_maccmp(mac,dev_indexs[i].mac)){
			ret=1;
			break;
		}
	}
	return ret;
}
int hertUtil_isTcpdumpOn(int *tcpdump_pid){
    char filepath[50]={0};
	if(*tcpdump_pid==0)
	{
		return 0;
	}
	sprintf(filepath, "/proc/%u", *tcpdump_pid);//生成要读取的文件的路径
	if(!access(filepath,F_OK)){	
		return 1;
	}
	*tcpdump_pid=0;
	return 0;
}
static time_t getUptime()
{
	char sCmd[128]={0};
	FILE *fp = NULL;
	int fret;
	char s[1024];
	char *end = NULL;
	time_t seconds = -1;

	sprintf(sCmd,"cat /proc/uptime > /tmp/SysUptime");
	system(sCmd);
	fret = access("/tmp/SysUptime",R_OK);
	if( fret==0 )
	{
		fp = fopen("/tmp/SysUptime","r");
		if( fp != NULL)
		{
			if( fgets(s,sizeof(s),fp))
			{
				end = strstr(s,".");
				if(end !=NULL )
				{
					*end = '\0';
					seconds = atoi(s);
				}
			}
			fclose(fp);
		}	 
	}
	return seconds;
}
int hertUtil_TcpdumpFlag()
{
	char tmp[64]={0};
	int ret=0;
    FILE *fp;
	if(! (fp = popen("nvram_get 2860 TcpdumpSwitch", "r")) ){
		return -1;
	}
	
	if(!fgets(tmp, sizeof(tmp), fp)){
		ret=-1;
		goto ERROR;
	}
	if(!strstr(tmp,"OPEN"))	{
	    ret=-1;
		goto ERROR;
	}
ERROR:
    pclose(fp);
	return ret;

}

RETCODE Msg_Process_Internel()
{
	RETCODE ret = RETCODE_SUCCESS;
	HERT_MSG_RSP_PUSH_DATA_VARPART_INIT initData;
	static DWORD currTimeSecond = 0;
	static DWORD currTimeSecondUpgrade = 0;
	DWORD  cycle = 0,now_hours = 0,now_mins = 0; 
	static int nFirstGetUpgradeInfo = 1;
	time_t sztm;
	struct tm *sz_systime;
	HERT_MSG_OPTION_PUSH_DATA tMsgOption;
	CHAR   *ptMsgCnstBody = "timing inform to plat";

	memset(&initData, 0x0, sizeof(initData));

	hertUtil_setLoglevel();

	//----------------debug only start ----------------
#if 0
	if ( hertUtil_IsInFile("/var/debugupgrade","DEBUG"))
	{
		memset(g_workData.upgradeData.url, 0x0, sizeof(g_workData.upgradeData.url));
		strcpy(g_workData.upgradeData.url, "http://10.10.10.3:4030/zhuzhh_uImageR187");
		/* do download for upgrade */
		if ( hertUtil_IsInFile("/var/downloadnow","downloadnow"))
		{
			Msg_Process_Internel_DownloadForUpgrade();
		}
	}
#endif
	//----------------debug only end ----------------

	if ( g_workData.status != HERT_REGISTERED )
	{
		HERT_LOGINFO("internel do connect request");
		ret = Msg_Process_Internel_SendConnReq();
		if (RETCODE_SUCCESS != ret)
		{
			HERT_LOGERR("Msg_Process_Internel_SendConnReq Failed at ret(%d)!!\n", ret);
			return ret;
		}
		return ret; /* no need do nothing if no registered */
	}
	else if ( 0 == memcmp(&initData, &g_workData.initData, sizeof(HERT_MSG_RSP_PUSH_DATA_VARPART_INIT)))
	{
		HERT_LOGINFO("internel do push data init request");
		ret = Msg_Process_Internel_SendPushInitReq();
		if (RETCODE_SUCCESS != ret)
		{
			HERT_LOGERR("Msg_Process_Internel_SendConnReq Failed at ret(%d)!!\n", ret);
			return ret;
		}
	}
	else
	{
		cycle = g_workData.initData.cycle;
		cycle = (cycle == 0) ? 120 : cycle; /* set default time if it is zero */
		HERT_LOGDEBUG("currTimeSecond=%d, cycle=%d, hertUtil_getSeconds()=%d",
				currTimeSecond, cycle, hertUtil_getSeconds());

		if ( currTimeSecond + cycle < hertUtil_getSeconds() )
		{
			HERT_LOGINFO("internel do heartbeat");
			/* do heartbeat */
			ret = Msg_Process_Internel_SendHeartbeatReq();
			if (RETCODE_SUCCESS != ret)
			{
				HERT_LOGERR("Msg_Process_Internel_SendConnReq Failed at ret(%d)!!\n", ret);
				return ret;
			}
			currTimeSecond = hertUtil_getSeconds();
		}

		if ( nFirstGetUpgradeInfo || (currTimeSecondUpgrade + 120 < hertUtil_getSeconds()) )
		{
			HERT_LOGINFO("internel do GetUpgrade");
			nFirstGetUpgradeInfo = 0;
			/* do heartbeat */
			ret = Msg_Process_Internel_SendGetUpgradeInfoReq();
			if (RETCODE_SUCCESS != ret)
			{
				HERT_LOGERR("Msg_Process_Internel_SendGetUpgradeInfoReq Failed at ret(%d)!!\n", ret);
				return ret;
			}
			currTimeSecondUpgrade = hertUtil_getSeconds();
		}

		/* do download for upgrade */
		if ( hertUtil_IsInFile("/var/downloadnow","downloadnow"))
		{
			Msg_Process_Internel_DownloadForUpgrade();
		}
	}

	/* do update space data */
	ret = Msg_Process_Internel_UpdateSpaceData();
	if (RETCODE_SUCCESS != ret)
	{
		HERT_LOGERR("Msg_Process_Internel_UpdateSpaceData Failed at ret(%d)!!\n", ret);
	}

	/* do update status data */
	ret = Msg_Process_Internel_UpdateStatusData();
	if (RETCODE_SUCCESS != ret)
	{
		HERT_LOGERR("Msg_Process_Internel_UpdateStatusData Failed at ret(%d)!!\n", ret);
	}

	/* do parameter changed check */
	if (Msg_Process_Internel_UpdateParameter() == 2)
	{
		HERT_LOGINFO("Configuration is changed, resend Init Request");
		ret = Msg_Process_Internel_SendPushAbiNtfReq();
		if (RETCODE_SUCCESS != ret)
		{
			HERT_LOGERR("Msg_Process_Internel_SendConnReq Failed at ret(%d)!!\n", ret);
			return ret;
		}
	}


	/*check is need to info status to plat? start*/
	time(&sztm);
	sz_systime=localtime(&sztm);/*get local time include hour,min,sec*/
	//we Time-Zone are GMT-8
	now_hours = sz_systime->tm_hour;
	now_mins = sz_systime->tm_min;

#if 0
	if(now_hours >= 24)
	{
		now_hours = now_hours - 24;
	}
#endif
/////////////////////////////////////////////////////
	if(!hertUtil_TcpdumpFlag())
	{
		//new 
		struct msg_pid_type{
			long mtype;
			pid_t pid;
		};

		struct msg_pid_type  msg_pid;
		int i;
		DWORD dwDevNumber = 0;
		ssize_t  size=0;
		struct mymesg mesg;
		int total;
		static struct mac_list_type  macList;

		static pid_t tcpdump_pid=0;
		static  time_t now_sec=0;
		HERT_LOGINFO("start............msg_pid...%d.....SIGUSR2 = %d ....\n",tcpdump_pid,SIGUSR2);
		g_workData.statusData.devNum = 0;

		if (g_workData.statusData.pDevList)  
		{ 
			free(g_workData.statusData.pDevList); 
			g_workData.statusData.pDevList = NULL; 
		}
		key_t key=ftok("/sbin/tcpdump",23);
		if(-1==key)
		{
			perror("ftok");
			// return -1;
		}
		int shfd=msgget(key,IPC_CREAT);

		if(shfd==-1)
		{
			perror("mesgget");
			// return -1;
		}
		//get tcpdump pid
		bzero(&msg_pid,sizeof(msg_pid));
		msg_pid.mtype='P';

		while ((size= msgrcv(shfd,&msg_pid,sizeof(msg_pid),msg_pid.mtype,IPC_NOWAIT))>0){
			tcpdump_pid=msg_pid.pid;	
			bzero(&msg_pid,sizeof(msg_pid));
			msg_pid.mtype='P';
		}

		if(!hertUtil_isTcpdumpOn(&tcpdump_pid)){
			HERT_LOGINFO("the tcpdump is't exist %u\n",now_sec);
			if(getUptime()-now_sec>30||now_sec==0){
				HERT_LOGINFO("now start tcpdump now %u \n", getUptime());
				system("killall tcpdump");
				system("tcpdump -XvvennSs 0 -i br0 tcp >/dev/zero &");
				now_sec=getUptime();
			}
		}
		else{
			kill(tcpdump_pid,SIGUSR2);
		}	
		hertUtil_GetDeviceList(&g_workData.statusData.pDevList, &dwDevNumber);
		g_workData.statusData.devNum = dwDevNumber;
		HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO    *devinfos;
		devinfos=g_workData.statusData.pDevList;
		total=dwDevNumber*sizeof(struct domain_queue_type);
		struct dev_index_type *pdevindex=NULL;

		if(dwDevNumber!=0)
		{
			//create  pdevindex  and  init it
			pdevindex=(struct dev_index_type *)malloc(dwDevNumber*sizeof(struct dev_index_type));
			for(i=0;i<dwDevNumber;i++){
				parse_strToMac(devinfos[i].mac,&pdevindex[i].mac);
				HERT_LOGINFO("str mac=%s  ether_addr=%x\n",devinfos[i].mac,pdevindex[i].mac);
				pdevindex[i].devinfo=devinfos+i;
				pdevindex[i].domain_queue=NULL;
				pdevindex[i].num=0;
			}

			//recv  domain name from tcpdump and  add into pdevindex	
			bzero(&mesg,sizeof(mesg));
			mesg.mtype='F';
			while ((size= msgrcv(shfd,&mesg,sizeof(mesg),mesg.mtype,IPC_NOWAIT))>0){
				HERT_LOGINFO("\nmesg size =%d\n",size);
				hertUtil_add_domain_queue(pdevindex,&mesg,dwDevNumber,&total);
				bzero(&mesg,sizeof(mesg));
				mesg.mtype='F';
			}
			//send domain to server
			for(i=0;i<dwDevNumber;i++)
			{

				memset(&tMsgOption, 0x0, sizeof(tMsgOption));
				tMsgOption.pData  = g_workData.szDstAddress;
				GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);

				ret=Msg_Process_PushData_UpdateDomainNameREQ(&tMsgOption,ptMsgCnstBody,pdevindex+i); 
				if ( RETCODE_SUCCESS != ret)
				{
					//HERT_LOGERR("failed Msg_Process_PushData_UpdateDomainNameREQ:%d!!\n", ret);
				}

			}

		}
		//monitor this  have  devices live ? if have  send signal to tcpdump 
		if(macList.dev_infos){
			for(i=0;i<macList.num;i++)
			{
				if(!hertUtil_isClientOnline(macList.dev_infos[i].mac,pdevindex,dwDevNumber)){
					struct flow_length_req request;
					bzero(&request,sizeof(request));
					request.mtype='R';
					request.dev_info=macList.dev_infos[i];
					//	request.dev_info.devinfo=macList.dev_infos[i].devinfo.devinfo,
					HERT_LOGINFO("this mac %x has leave.........................\n",request.dev_info.mac);
					if(hertUtil_isTcpdumpOn(&tcpdump_pid)){
						msgsnd(shfd, &request,sizeof(request),IPC_NOWAIT);
						kill(tcpdump_pid,SIGUSR2);
						HERT_LOGINFO("SIGUSR2 has send\n ");
					}
				}
			}
			macList.num=0;
			free(macList.dev_infos);
			macList.dev_infos=NULL;
		}   
		HERT_LOGINFO("2 dwDevNumber=%d\n",dwDevNumber);

		if(dwDevNumber>0){
			macList.dev_infos=(struct dev_infos_type*)malloc(dwDevNumber*sizeof(struct dev_infos_type));	 
			macList.num=dwDevNumber;

			for(i=0;i<dwDevNumber;i++)
			{
				macList.dev_infos[i].mac=pdevindex[i].mac;

				macList.dev_infos[i].devinfo=*(pdevindex[i].devinfo);
			}
		}	

		// recv  devs flow 
		struct flow_type_queue flow_queue;
		struct flow_type_rsp response;
		bzero(&flow_queue,sizeof(flow_queue));
		bzero(&response,sizeof(response));
		response.mtype='A';
		while ((size= msgrcv(shfd,&response,sizeof(response),response.mtype,IPC_NOWAIT))>0){
			HERT_LOGINFO("\nrsp  size =%d\n",size);
			HERT_LOGINFO("**********************recv*******************************\n");
			HERT_LOGINFO("mac=%s connect sec=%s total flow=%llu devNam=%s\n ",response.dev_info.devinfo.mac, \
					response.dev_info.devinfo.connectTime,response.length, \
					response.dev_info.devinfo.devName);
			struct flow_type_queue *next;
			next=malloc(sizeof(struct flow_type_queue));
			*next=*((struct flow_type_queue * )&response);
			next->next=flow_queue.next;
			flow_queue.next=next;
			HERT_LOGINFO("**********************recv*******************************\n");
			bzero(&response,sizeof(response));
			response.mtype='A';
		}
		if(flow_queue.next){ 
			memset(&tMsgOption, 0x0, sizeof(tMsgOption));
			tMsgOption.pData  = g_workData.szDstAddress;
			GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);
			ret=Msg_Process_PushData_UpdateDevConNotifyREQ(&tMsgOption,ptMsgCnstBody,&flow_queue); 
			if ( RETCODE_SUCCESS != ret)
			{
				HERT_LOGERR("failed Msg_Process_PushData_UpdateDevConNotifyREQ:%d!!\n", ret);
			}
		}

		//for free;
		total=0;
		if(pdevindex){
			free(pdevindex);
			pdevindex=NULL;
		}

		g_workData.statusData.devNum = 0;

		if (g_workData.statusData.pDevList)  
		{ 
			free(g_workData.statusData.pDevList); 
			g_workData.statusData.pDevList = NULL; 
		}
	}
////////////////////////////////////////////////////////////////
	HERT_LOGINFO("timing inform time(%d:%d)\n", g_workData.informPlatHour,g_workData.informPlatMin);
	HERT_LOGINFO("now system time(%d:%d)\n", now_hours,now_mins);

	if((g_workData.timingComplete == 1)  && \
			(now_hours >= g_workData.informPlatHour)  && \
			(now_mins > g_workData.informPlatMin))
	{
		g_workData.timingComplete = 0;
	}

	if((g_workData.timingComplete == 0)  && \
			(now_hours == g_workData.informPlatHour)  && \
			(now_mins == g_workData.informPlatMin))
	{
		memset(&tMsgOption, 0x0, sizeof(tMsgOption));
		tMsgOption.pData   = g_workData.szDstAddress;
		GetHighLowFromString(tMsgOption.pData, &tMsgOption.nHighLength, &tMsgOption.nLowLength);
		/*inform devstatus info to plat*/ 

		ret = Msg_Process_PushData_RouterStatusReq(&tMsgOption,ptMsgCnstBody,1);  

		if ( RETCODE_SUCCESS != ret)
		{
			HERT_LOGERR("Failed at Parse_Msg_Data:%d!!\n", ret);
		}

		if (tMsgOption.pData) 
		{ 
			free(tMsgOption.pData); 
		}	
		g_workData.timingComplete = 1;
	}

	/*check is need to info status to plat? end*/

	return ret;
}

/*
 ** =============only for download util============== 
 ** usage:
 **  herouteapp http://10.10.10.3:4030/cjpeg /var/cjpeg
 */
int Msg_Process_Internel_download(char *pszUrl, char *pszOutFile)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	struct hert_curl_data config;
	int  nRetry = 0;

	printf("======== pszUrl(%s), pszOutFile(%s) ======\n", pszUrl, pszOutFile);
	while(nRetry <= 3)
	{
		nRetry = 0;

		HERT_LOGINFO("pszOutFile(%s), url(%s)", pszOutFile, pszUrl);

		curl = curl_easy_init();
		if (curl) 
		{
			fp = fopen(pszOutFile,"wb");
			if (fp)
			{
				res = curl_easy_setopt(curl, CURLOPT_URL, pszUrl);

				res = curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, hert_curl_trace);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGFUNCTION) failed: %s\n", curl_easy_strerror(res));
				}
				res = curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_setopt(CURLOPT_DEBUGDATA) failed: %s\n", curl_easy_strerror(res));
				}

				/* the DEBUGFUNCTION has no effect until we enable VERBOSE */
				res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_setopt(CURLOPT_VERBOSE) failed: %s\n", curl_easy_strerror(res));
				}

				res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Msg_Process_DownLoad_Write_data);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEFUNCTION) failed: %s\n", curl_easy_strerror(res));
				}
				res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_setopt(CURLOPT_WRITEDATA) failed: %s\n", curl_easy_strerror(res));
				}
				res = curl_easy_perform(curl);
				/* Check for errors */
				if(res != CURLE_OK)
				{
					HERT_LOGERR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				}

			}
			curl_easy_cleanup(curl);
			fclose(fp);
		}

		sleep(1);

		if (hertUtil_getFileSize(pszOutFile) <= 0 )
		{
			nRetry++;
			continue;
		}
		break;
	}

	return 0;
}
