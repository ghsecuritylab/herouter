#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include "linux/autoconf.h"
#include "hert_com.h"
#include "hert_app.h"
#include "hert_msg.h"
#include "hert_util.h"
#include "nvram.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

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


#define SET_MSG_HEADER_TYPE(pt, type) \
    HERT_MSGHEADER *pMsgHdr = (HERT_MSGHEADER*)pt; \
    pMsgHdr->MsgType = (type << 4); \
    pt++;

#define COPY_CHAR(dst, src) \
    *dst = src; \
    HERT_LOGINFO("%08s=%x, dst=%x", #src, (0xff & src), (0xff & (*dst)) ); \
    dst += sizeof(CHAR);

#define COPY_STR(dst, src, length) \
    if (src && (strlen(src)> 0 ) ) \
    { \
        HERT_LOGINFO("%08s=%s", #src, src); \
        memcpy(dst, src, length); dst += length; \
    }

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
    snprintf(buf, sizeof(buf) - strlen(buf), "\"%s\":\"%s\"", #a, ptv); \
    HERT_LOGINFO("%s=%s", #ptv, buf);

#define ADD_ITEM_TO_JSON(Json, Item)\
    if (nJsonSize > strlen(Json) + strlen(Item)) \
    { \
        if (strlen(Json) > 0 ) \
        { \
            snprintf(Json + strlen(Json), nJsonSize - strlen(Json), ",%s", Item); \
        } \
        else \
        { \
            snprintf(Json + strlen(Json), nJsonSize - strlen(Json), "{%s", szItem); \
        } \
    }\
    else \
    { \
        HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nJsonSize, Json, strlen(Json), strlen(Item),Item);\    
        free(Json); \
        return RETCODE_NO_MEMORY; \
    }

#define ADD_STR_TO_JSON(Json, str)\
    if (nJsonSize > strlen(Json) + strlen(str)) \
    { \
        snprintf(Json + strlen(Json), nJsonSize - strlen(Json), str); \
    }\
    else \
    { \
        HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=(%d)", nJsonSize, strlen(Json)); \
        free(Json); \
        return RETCODE_NO_MEMORY; \
    }

#define FREE_MEM(a) \
	if (a) free(a); \
	a = NULL;

extern void dump(char *pszData, char *pszFunction, int nLine);

#define HEX_DUMP(a) dump(a, __FUNCTION__, __LINE__)

int GetLengthFromTwoBytes(CHAR nHighLength, CHAR nLowLength)
{
    return (int)( ( nHighLength << 8 ) | nLowLength );
}

CHAR *SetMsgLengthItem(CHAR *ptr, DWORD nMsgLen, int *nDataLen)
{
    CHAR *pBytes = NULL;
    CHAR  cTemp  = 0x0;
    CHAR  szLog[128] = {0x0};

    pBytes = (CHAR*)&nMsgLen;
    HERT_LOGINFO("pBytes1=(%x), pBytes2=(%x), pBytes3=(%x), pBytes4=(%x)", 
                 0xff & (*pBytes), 0xff & (*(pBytes + 1)), 0xff & (*(pBytes + 2)), 0xff & (*(pBytes + 3)));

    if ( nMsgLen >= 0 && nMsgLen <= 127)
    {
        *ptr = nMsgLen;
        *nDataLen = 1;
    }
    else if (nMsgLen >= 128 && nMsgLen <= 16383)
    {
        *ptr = 0xff & ( (0x7f & (*pBytes)) | 0x80);
        *(ptr + 1) = 0xff & (( (0x80 & (*pBytes)) >> 7) | ((*(pBytes+1)) << 1));
        *nDataLen = 2;
    }
    else if (nMsgLen >= 16384 && nMsgLen <= 2097151)
    {
        *ptr = 0xff & ( (0x7f & (*pBytes)) | 0x80);

        cTemp = ( (0x80 & (*pBytes)) >> 7) | ((*(pBytes+1)) << 1);
        *(ptr + 1) = 0xff & ( (0x7f & cTemp) | 0x80);

        cTemp = ( (0x80 & (*(pBytes+1))) >> 7) | ((*(pBytes+2)) << 1);
        *(ptr + 2) = 0xff & cTemp;

        *nDataLen = 3;
    }
    else if (nMsgLen >= 2097152 && nMsgLen <= 268435455)
    {
        *ptr = 0xff & ( (0x7f & (*pBytes)) | 0x80);

        cTemp = ( (0x80 & (*pBytes)) >> 7) | ((*(pBytes+1)) << 1);		
        *(ptr + 1) = 0xff & ( (0x7f & cTemp) | 0x80);

        cTemp = ( (0x80 & (*(pBytes+1))) >> 7) | ((*(pBytes+2)) << 1);		
        *(ptr + 2) = 0xff & ( (0x7f & cTemp) | 0x80);

        cTemp = ( (0x80 & (*(pBytes+2))) >> 7) | ((*(pBytes+3)) << 1);
        *(ptr + 3) = 0xff & cTemp;

        *nDataLen = 4;
    }

    memset(szLog, 0x0, sizeof(szLog));
    sprintf(szLog, "HEADER BYTES: %x %x %x %x, length=%d", 
        (*nDataLen) >=1 ? 0xff & (*(ptr + 0)) : 0,
        (*nDataLen) >=2 ? 0xff & (*(ptr + 1)) : 0,
        (*nDataLen) >=3 ? 0xff & (*(ptr + 2)) : 0,
        (*nDataLen) >=4 ? 0xff & (*(ptr + 3)) : 0,
        (*nDataLen));
    HERT_LOGINFO(szLog);

    return ptr;
}

int GetHighLowFromString(CHAR *pdata, CHAR *nHighLength, CHAR *nLowLength)
{
    short nLength  = 0;
    CHAR *pByte    = (CHAR*)&nLength;

    nLength = strlen(pdata);
    *nHighLength = *(pByte+1);
    *nLowLength  = *pByte;

    HERT_LOGINFO("pdata(%s)=(%d), *nHighLength=(%x), *nLowLength=(%x)", 
                 pdata, strlen(pdata), 0xff & (*nHighLength), 0xff & (*nLowLength));
    return 0;
}

/* parse: msgtype */
RETCODE Parse_Msg_Type(IN CHAR *ptMsg, OUT CHAR *msgType)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSGHEADER *ptMsgHeader = (HERT_MSGHEADER *)ptMsg;
    if (!ptMsg)
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p)", ptMsg);
        return RETCODE_NO_MEMORY;
    }
    *msgType = ptMsgHeader->MsgType;

    return ret;
}

/* parse: push data msgtype */
RETCODE Parse_Msg_PushData_Type(IN CHAR *ptMsg, IN int nDataLen, OUT CHAR *pszMsgType, IN int nMsgTypeBuf)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR szMsgType[64] = {0x0};
    int  nMsgTypeLen = 0;
    int  i = 0;
    CHAR *pStart = NULL;
    CHAR *pEnd   = NULL;

    if ( (!ptMsg) || (!pszMsgType) )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), pszMsgType(%p)", ptMsg, pszMsgType);
        return RETCODE_NO_MEMORY;
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
        return RETCODE_INVALID_MSGDATA;
    }
    i = i + nMsgTypeLen;
    pStart = strstr(ptMsg + i, "\"");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return RETCODE_INVALID_MSGDATA;
    }
    pStart = strstr(pStart+1, ":");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return RETCODE_INVALID_MSGDATA;
    }
    pStart = strstr(pStart+1, "\"");
    if (!pStart)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return RETCODE_INVALID_MSGDATA;
    }
    pStart += 1;

    pEnd = strstr(pStart, "\"");
    if (!pEnd)
    {
        HERT_LOGERR("Invalid format data(RETCODE_INVALID_MSGDATA): ptMsg(%p)", ptMsg);
        return RETCODE_INVALID_MSGDATA;
    }
    memset(pszMsgType, 0x0, nMsgTypeBuf);
    strncpy(pszMsgType, pStart, pEnd - pStart);

    return ret;
}


/* parse: msgdata(OPTION + BODY) */
RETCODE Parse_Msg_Data(IN CHAR *ptMsg, OUT CHAR **pptMsgData, OUT DWORD *nMsgDataLen)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *ptData= NULL;
    int   i = 0;
    
    if (!ptMsg)
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p)", ptMsg);
        return RETCODE_NO_MEMORY;
    }
    /* msg length start from second char */
    ptData = ptMsg + sizeof(CHAR);
    *nMsgDataLen = (*ptData) & 0x7F;
    for(i = 1; i <= MAX_LENGTH_SIZE; i++)
    {
        if ( ((*ptData) && 0x80) == 0 ) /* the highest bit is 0, so next byte is not length */
        {
            break;
        }
        ptData++;

        *nMsgDataLen = (*nMsgDataLen) | (((*ptData) && 0x7F) << (7 * i));
    }
    
    *pptMsgData = ptData;

    return ret;
}

/* {“version”:0,” msgType”:”json”, “msgSeq”:0, “errorCode”,” description”:”errdesc”:”abcdefg, */
CHAR *ParseJsonString(CHAR *pData, CHAR *pszItemName, CHAR *pszItemValue)
{
    CHAR    szName[128];
    CHAR    szValue[256];
    CHAR *pchrR = NULL;
    CHAR *pchrL = NULL;
    CHAR *pchrEnd = NULL;
    CHAR *pchrTmp = NULL;
    int   nItemLength = 0;

#define STR_QUOTATION "\""
#define STR_SEPRATION ":"
#define STR_END1      ","
#define STR_END2      "}"

    memset(szName, 0x0, sizeof(szName));
    memset(szValue, 0x0, sizeof(szValue));

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


    /* 2.get end possition */
    pchrEnd = strstr(pchrL, STR_END1);
    if(pchrEnd)
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

    return pchrEnd;
}

/* build: 连接建立请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_ConnReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_CONN_REQ *ptMsgOption, IN HERT_MSG_BODY_CONN_REQ *ptMsgBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptProtLen   = 0;
    int nBodyLength   = 0;
    int nBodyUserLen  = 0;
    int nBodyPassLen  = 0;
    int nLengthSize   = 0;
    DWORD dwLength    = 0;
    CHAR msgType = CONN_REQ;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgBody) )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = 6 * sizeof(CHAR);
    HERT_LOGINFO("sizeof(HERT_MSG_OPTION_CONN_REQ)=%d, sizeof(pProtocol)=%d", 
                 sizeof(HERT_MSG_OPTION_CONN_REQ), sizeof(ptMsgOption->pProtocol));

    /* count option var item length */
    nOptProtLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptProtLen != GET_STR_LENGTH(ptMsgOption->pProtocol))
    {
        HERT_LOGERR("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pProtocol)=%d", 
                     nOptProtLen, GET_STR_LENGTH(ptMsgOption->pProtocol));
    }
    HERT_LOGINFO("nOptionLength=%d, nOptProtLen=%d", nOptionLength, nOptProtLen);
    nOptionLength   += nOptProtLen;

    /* ===== count body length */
    nBodyLength   = 6 * sizeof(CHAR);


    /*  count body var item length */
    nBodyUserLen = GetLengthFromTwoBytes(ptMsgBody->nUserHighLength, ptMsgBody->nUserLowLength);
    if ( nBodyUserLen != GET_STR_LENGTH(ptMsgBody->pUserData))
    {
        HERT_LOGINFO("Invalid user data length of body, nBodyUserLen=%d, strlen(ptMsgBody->pUserData)=%d", 
                     nBodyUserLen, GET_STR_LENGTH(ptMsgBody->pUserData));
    }

    /* count body var item length */
    nBodyPassLen = GetLengthFromTwoBytes(ptMsgBody->nPassHighLength, ptMsgBody->nPassLowLength);
    if ( nBodyPassLen != GET_STR_LENGTH(ptMsgBody->pPassData))
    {
        HERT_LOGERR("Invalid user data length of body, nBodyPassLen=%d, strlen(ptMsgBody->pPassData)=%d", 
                     nBodyPassLen, GET_STR_LENGTH(ptMsgBody->pPassData));
    }
    HERT_LOGINFO("nBodyLength=%d, nBodyUserLen=%d, nBodyPassLen=%d", nBodyLength, nBodyUserLen, nBodyPassLen);

    nBodyLength += nBodyUserLen;
    nBodyLength += nBodyPassLen;

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    dwLength = nOptionLength + nBodyLength;
    Ptr = SetMsgLengthItem(Ptr, dwLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;
    HERT_LOGINFO("dwLength=%d, nLengthSize=%d", dwLength, nLengthSize);

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    COPY_STR(Ptr, ptMsgOption->pProtocol, nOptProtLen);

    COPY_CHAR(Ptr, ptMsgOption->nVer);
    COPY_CHAR(Ptr, ptMsgOption->nConnFlag);
    COPY_CHAR(Ptr, ptMsgOption->nHighTime);
    COPY_CHAR(Ptr, ptMsgOption->nLowTime);

    /* set body */
    COPY_CHAR(Ptr, ptMsgBody->nDevHighLength); /* device */
    COPY_CHAR(Ptr, ptMsgBody->nDevLowLength);

    COPY_CHAR(Ptr, ptMsgBody->nUserHighLength); /* user */
    COPY_CHAR(Ptr, ptMsgBody->nUserLowLength);
    COPY_STR(Ptr, ptMsgBody->pUserData, nBodyUserLen);
    
    COPY_CHAR(Ptr, ptMsgBody->nPassHighLength); /* pass */
    COPY_CHAR(Ptr, ptMsgBody->nPassLowLength);
    COPY_STR(Ptr, ptMsgBody->pPassData, nBodyPassLen);

    *nDataLen = Ptr - ptMsg;

    return ret;
}

/* parse: 连接建立响应	S->C, DATA_FORMAT: MSGHEADER + OPTION */
RETCODE Parse_Msg_ConnRsp(IN CHAR *ptMsg, IN DWORD nDataLength, OUT HERT_MSG_OPTION_CONN_RSP *ptMsgOption)
{
    RETCODE ret = RETCODE_SUCCESS;
    DWORD   dwDataLen = 0;

    ret = Parse_Msg_Data((CHAR*)ptMsg, (CHAR**)&ptMsgOption, &dwDataLen);
    if ( ret != RETCODE_SUCCESS )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p)", ptMsg);
        return RETCODE_NO_MEMORY;
    }
    return ret;
}

/* build: 心跳请求	C->S, DATA_FORMAT: MSGHEADER */
RETCODE Build_Msg_PingReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen)
{
    RETCODE ret = RETCODE_SUCCESS;
    HERT_MSGHEADER tMsgHeader;
    CHAR *Ptr = NULL;

    if (!ptMsg || sizeof(HERT_MSGHEADER) > buffsize )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p)", ptMsg);
        return RETCODE_NO_MEMORY;
    }
    memset(ptMsg, 0x0, buffsize);

    Ptr = (CHAR*)&tMsgHeader;
    memset(&tMsgHeader, 0x0, sizeof(HERT_MSGHEADER));
    SET_MSG_HEADER_TYPE(Ptr, PING_REQ); /* set header: type */
    *nDataLen = 2;
    memcpy(ptMsg, &tMsgHeader, *nDataLen);

    return ret;
}

/* build: 发送数据-获取初始化信息请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_InitInfoReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_INIT *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;    

    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message data */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devID, devID);      /* devID */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->deviceType, deviceType); /* deviceType */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->IP, IP);         /* IP */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}


/* build: 发送数据-获取升级信息请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_GetUpgradeInfoReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_GET_UPDATE *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;    

    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message data */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devID, devID);       /* devID */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->type, type);         /* type */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->currentVersion, currentVersion); /* currentVersion */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}

/* parse: 发送数据-获取升级信息响应    S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_GetUpdateRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_RSP_PUSH_DATA_VARPART_GET_UPDATE *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[128];

    if ( (!ptMsg) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ====== parse msg data(body) */
    pData = ptMsg;
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData = ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(errorCode)) == 0 )
        {
            ptMsgConstBody->errorCode = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(needUpdate)) == 0 )
        {
            ptMsgVarBody->needUpdate = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(updateUrl)) == 0 )
        {
            strcpy(ptMsgVarBody->updateUrl, szItemValue);
        }
        else if (strcmp(szItemName, STR(updateDescription)) == 0 )
        {
            strcpy(ptMsgVarBody->updateDescription, szItemValue);
        }
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* parse: 发送数据-获取初始化信息响应    S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_InitInfoRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_RSP_PUSH_DATA_VARPART_INIT *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[128];

    if ( (!ptMsg) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ====== parse msg data(body) */
    pData = ptMsg;
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData = ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(errorCode)) == 0 )
        {
            ptMsgConstBody->errorCode = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(description)) == 0 )
        {
            strcpy(ptMsgConstBody->description, szItemValue);
        }
        else if (strcmp(szItemName, STR(cycle)) == 0 )
        {
            ptMsgVarBody->cycle = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(time)) == 0 )
        {
            strcpy(ptMsgVarBody->time, szItemValue);
        }
        else if (strcmp(szItemName, STR(timeZone)) == 0 )
        {
            strcpy(ptMsgVarBody->timeZone, szItemValue);
        }
        else if (strcmp(szItemName, STR(platformVersion)) == 0 )
        {
            strcpy(ptMsgVarBody->platformVersion, szItemValue);
        }
        else if (strcmp(szItemName, STR(password)) == 0 )
        {
            strcpy(ptMsgVarBody->password, szItemValue);
            //nvram_bufset(RT2860_NVRAM, "AES_PWD_FROM_ONENET", szItemValue); 
            hertUtil_SetAESPwdFromOnenet(szItemValue);
            HERT_LOGINFO("set AES_PWD_FROM_ONENET:%s\n",szItemValue);
        }
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}


/* build: 发送数据-路由器属性上报请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_AbilityNotifyReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen, 
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_ABILITY_NOTIFY *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;    

    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);

    /* ===== Format Json message var body */
    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devID, devID);      /* devID */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devName, devName);    /* devName */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->fac, fac);        /* fac */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->type, type);        /* type */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->softVersion, softVersion); /* softVersion */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;


    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}

/* parse: 发送数据-响应 */
RETCODE Parse_Msg_BodyConstPartRsp(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[128];

    if ( (!ptMsg) || (!ptMsgConstBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p),ptMsgConstBody(%p)", 
                    ptMsg, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }
    memset(ptMsgConstBody, 0x0, sizeof(HERT_MSG_RSP_PUSH_DATA_CONSTPART));

    /* ====== parse msg data(body) */
    pData = ptMsg;
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData= ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(errorCode)) == 0 )
        {
            ptMsgConstBody->errorCode = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(description)) == 0 )
        {
            strcpy(ptMsgConstBody->description, szItemValue);
        }
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}
/* parse: 发送数据-请求 */
RETCODE Parse_Msg_BodyConstPartReq(IN CHAR *ptMsg, IN DWORD buffsize,
                          OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[128];

    if ( (!ptMsg) || (!ptMsgConstBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p),ptMsgConstBody(%p)", 
                    ptMsg, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }
    memset(ptMsgConstBody, 0x0, sizeof(HERT_MSG_REQ_PUSH_DATA_CONSTPART));

    /* ====== parse msg data(body) */
    pData = ptMsg;
    HERT_LOGINFO("<<<<<<<<<<<<<<<<<pData:%s<<<<<<<<<<<<<<<<<\n", pData);

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData = ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* build: 发送数据-查询路由器存储信息响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_RouteSpaceRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_SPACE *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    CHAR *szJson;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;    
    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
    ADD_ITEM_TO_JSON(szJson, szItem);

    /* ===== Format Json message var body */
    FORMAT_JSON_ITEM_INT(szItem, ptMsgVarBody->diskStatus, diskStatus);      /* diskStatus */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNGLNG(szItem, ptMsgVarBody->totalSpace, totalSpace);      /* totalSpace */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNGLNG(szItem, ptMsgVarBody->usedSpace, usedSpace);       /* usedSpace */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNGLNG(szItem, ptMsgVarBody->leftSpace, leftSpace);       /* leftSpace */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}
//new
RETCODE Build_Msg_ConNotifyRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
		IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
		IN struct flow_type_queue   *ptMsgVarBody)
{
	RETCODE ret = RETCODE_SUCCESS;
	CHAR *szJson;
	CHAR szItem[128];
	CHAR *Ptr = NULL;
	int nOptionLength = 0;
	int nOptDataLen   = 0;
	int nBodyLength   = 0;
	int nLengthSize   = 0;
	DWORD i = 0;
	CHAR msgType = PUSH_DATA;
	int nJsonSize = 0;
	DWORD buffsize = 0;
	char  dataTraffic[20]={0};	
	struct flow_type_queue *tmp=NULL;
	if ( (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
	{
		HERT_LOGERR("Invalid parameter: ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", ptMsgOption, ptMsgConstBody, ptMsgVarBody);
		return RETCODE_INVALID_PARAM;
	}
	//printf("///////////////////////////////////////////\n");

	/* ===== count option length */
	nOptionLength = GET_LENGTH_PUSH_OPTION();

	/* count option var item length */
	nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
	if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
	{
		HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
				nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));

	}
	nOptionLength += nOptDataLen;
	//修改
	nJsonSize = 256 +1024;
	//printf("nJsonSize=%d\n",nJsonSize);
	szJson = malloc(nJsonSize);
	//printf("malloc=%p\n",szJson);
	if (!szJson)
	{
		HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
		return RETCODE_NO_MEMORY;
	}
	memset(szJson, 0x0, nJsonSize);
	/* ===== Format Json message const body */
	FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
	ADD_ITEM_TO_JSON(szJson, szItem);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
	ADD_ITEM_TO_JSON(szJson, szItem);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
	ADD_ITEM_TO_JSON(szJson, szItem);   

	struct flow_type_queue *next=ptMsgVarBody->next;
	ptMsgVarBody->next=NULL;
	ADD_STR_TO_JSON(szJson, ",\"");
	ADD_STR_TO_JSON(szJson, STR(subDevConnectionList));
	ADD_STR_TO_JSON(szJson, "\":");
	ADD_STR_TO_JSON(szJson, "[");
	i=0;

	while(next)
	{
		if ( i == 0 )
		{
			ADD_STR_TO_JSON(szJson, "{");
		}
		else
		{
			ADD_STR_TO_JSON(szJson, ",{");
		}

		 tmp=next;
		FORMAT_JSON_ITEM_STR(szItem,tmp->dev_info.devinfo.devName, devName);   /* devName */

		if (nJsonSize > strlen(szJson) + strlen(szItem))
		{
			snprintf(szJson + strlen(szJson), nJsonSize - strlen(szJson), "%s", szItem);
		}
		else
		{
			HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nJsonSize, szJson, strlen(szJson), strlen(szItem), szItem);
			ret=RETCODE_NO_MEMORY;
			goto ERROR;
		}

		if(strstr(tmp->dev_info.devinfo.devName,"android"))
			sprintf(tmp->dev_info.devinfo.devType,"%d",1);
		else if(strstr(tmp->dev_info.devinfo.devName,"iPhone"))
			sprintf(tmp->dev_info.devinfo.devType,"%d",2);
		else if(strstr(tmp->dev_info.devinfo.devName,"Mac"))
			sprintf(tmp->dev_info.devinfo.devType,"%d",3);
		else if(strstr(tmp->dev_info.devinfo.devName,"PC")||strstr(tmp->dev_info.devinfo.devName,"WINDOWS"))
			sprintf(tmp->dev_info.devinfo.devType,"%d",4);
		else if(strstr(tmp->dev_info.devinfo.devName,"iPad"))
			sprintf(tmp->dev_info.devinfo.devType,"%d",5);

		else
			sprintf(tmp->dev_info.devinfo.devType,"%d",0);

		FORMAT_JSON_ITEM_STR(szItem, tmp->dev_info.devinfo.devType, devType);      /* devType */
		ADD_ITEM_TO_JSON(szJson, szItem);

		//printf("devinfo devID=%s\n",tmp->dev_info.devinfo.devID);	
		FORMAT_JSON_ITEM_STR(szItem, tmp->dev_info.devinfo.devID,devID);        /* devID */
		ADD_ITEM_TO_JSON(szJson, szItem);

		FORMAT_JSON_ITEM_STR(szItem, tmp->dev_info.devinfo.mac, Mac);          /* mac */
		ADD_ITEM_TO_JSON(szJson, szItem);
		//printf("deivce mac=%s\n",tmp->dev_info.devinfo.mac);

		FORMAT_JSON_ITEM_STR(szItem, tmp->dev_info.devinfo.connectTime,connectTime);          /* mac */
		ADD_ITEM_TO_JSON(szJson, szItem);
		//printf("dateTraffic %lldB\n",tmp->length);
		sprintf(dataTraffic,"%0.3lf",tmp->length*1.0000/1024/1024);
		FORMAT_JSON_ITEM_STR(szItem,dataTraffic,dataTraffic);          /* mac */
		ADD_ITEM_TO_JSON(szJson, szItem);
		ADD_STR_TO_JSON(szJson, "}");
		next=next->next;
		free(tmp);
		i++;
	}
	ADD_STR_TO_JSON(szJson, "]");
	ADD_STR_TO_JSON(szJson, "}");



	/* ===== count body length */
	nBodyLength   = strlen(szJson);

	/* ====== set msg struct */
	buffsize = nOptionLength + nBodyLength + 256;
	Ptr = malloc(buffsize);
	if (!Ptr)
	{
		HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
				nOptionLength + nBodyLength, buffsize);
		ret=RETCODE_NO_MEMORY;
		goto ERROR;
	}
	memset(Ptr, 0x0, buffsize);

	*ptMsg = Ptr;

	/* set header: type */
	SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */

	Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
	Ptr += nLengthSize;

	/* set option */
	COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
	COPY_CHAR(Ptr, ptMsgOption->nLowLength);

	COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

	/* set body */
	COPY_STR(Ptr, szJson, nBodyLength);

	*nDataLen = Ptr - *ptMsg;

ERROR:
	while(next!=NULL)
	{
		tmp=next;
		next=next->next;
		tmp->next=NULL;
		free(tmp);
	}

	FREE_MEM(szJson);
	return ret;
}

int hertUtil_isEncryptoDomin()
{
	char tmp[64]={0};
	int ret=0;
    FILE *fp;
	if(! (fp = popen("nvram_get 2860 HE_ROUTE_ENCRYPTODOMAIN", "r")) ){
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

extern int hert_des_ecb_crypt(unsigned char *cbc_in,unsigned char *key_data,int size,int type);
RETCODE Build_Msg_DomainStauteRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
		IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
		IN struct dev_index_type    *ptMsgVarBody)
{
	RETCODE ret = RETCODE_SUCCESS;
	CHAR *szJson;
	CHAR szItem[128];
	CHAR *Ptr = NULL;
	int nOptionLength = 0;
	int nOptDataLen   = 0;
	int nBodyLength   = 0;
	int nLengthSize   = 0;
	DWORD i = 0;
	CHAR msgType = PUSH_DATA;
	struct tagDomainname * pDomain;
	int nJsonSize = 0;
	DWORD buffsize = 0;
#define ENCRYPTO_KEY "heluyou!"	
#ifndef DOMAIN_SIZE 
#define DOMAIN_SIZE 250
#endif
	char key[9]=ENCRYPTO_KEY;
	if ( (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
	{
		HERT_LOGERR("Invalid parameter: ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
				ptMsgOption, ptMsgConstBody, ptMsgVarBody);
		return RETCODE_INVALID_PARAM;
	}
	//printf("doman num=%d\n",ptMsgVarBody->num);
	if(ptMsgVarBody->num==0){
		return -1;

	}

	struct domain_queue_type *domain_queue=ptMsgVarBody->domain_queue;
	ptMsgVarBody->domain_queue=NULL;
	struct domain_queue_type *tmp;
	//////////////////////////////////////////////////
	/* ===== count option length */
	nOptionLength = GET_LENGTH_PUSH_OPTION();

	/* count option var item length */
	nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
	if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
	{
		HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
				nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));

	}
	nOptionLength += nOptDataLen;
	//修改
	nJsonSize = 256 + ptMsgVarBody->num*sizeof(struct domain_info)*2+sizeof(HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO);
	//printf("nJsonSize=%d\n",nJsonSize);
	szJson = malloc(nJsonSize);
	//printf("malloc=%p\n",szJson);
	if (!szJson)
	{
		HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
		return RETCODE_NO_MEMORY;
	}
	memset(szJson, 0x0, nJsonSize);
	/* ===== Format Json message const body */
	FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
	ADD_ITEM_TO_JSON(szJson, szItem);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
	ADD_ITEM_TO_JSON(szJson, szItem);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
	ADD_ITEM_TO_JSON(szJson, szItem);   

	ADD_STR_TO_JSON(szJson, ",\"");
	ADD_STR_TO_JSON(szJson, STR(subDevDomainnameList));
	ADD_STR_TO_JSON(szJson, "\":");
	ADD_STR_TO_JSON(szJson, "[");
	
	ADD_STR_TO_JSON(szJson, "{");

	//	printf("devinfo addr=%p\n",ptMsgVarBody->devinfo);	
	//	printf("devinfo name=%s\n",ptMsgVarBody->devinfo->devName);	
	FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devinfo->devName, devName);  	/* devName */
	ADD_STR_TO_JSON(szJson, szItem);   
	if(strstr(ptMsgVarBody->devinfo->devName,"android"))
		sprintf(ptMsgVarBody->devinfo->devType,"%d",1);
	else if(strstr(ptMsgVarBody->devinfo->devName,"iPhone"))
		sprintf(ptMsgVarBody->devinfo->devType,"%d",2);
	else if(strstr(ptMsgVarBody->devinfo->devName,"Mac"))
		sprintf(ptMsgVarBody->devinfo->devType,"%d",3);
	else if(strstr(ptMsgVarBody->devinfo->devName,"PC")||strstr(ptMsgVarBody->devinfo->devName,"WINDOWS"))
		sprintf(ptMsgVarBody->devinfo->devType,"%d",4);
	else if(strstr(ptMsgVarBody->devinfo->devName,"iPad"))
		sprintf(ptMsgVarBody->devinfo->devType,"%d",5);

	else
		sprintf(ptMsgVarBody->devinfo->devType,"%d",0);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devinfo->devType, devType);      /* devType */
	ADD_ITEM_TO_JSON(szJson, szItem);

	HERT_LOGINFO("devinfo devID=%s\n",ptMsgVarBody->devinfo->devID);	
	FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devinfo->devID, devID);        /* devID */
	ADD_ITEM_TO_JSON(szJson, szItem);

	FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->devinfo->mac, Mac);          /* mac */
	ADD_ITEM_TO_JSON(szJson, szItem);
	FORMAT_JSON_ITEM_STR(szItem, inet_ntoa(ptMsgVarBody->ip), devIP);          /*devIP */
	ADD_ITEM_TO_JSON(szJson, szItem);
	//printf("the device ip=%s\n",inet_ntoa(ptMsgVarBody->ip));
	//printf("deivce mac=%x\n",ptMsgVarBody->mac);
	ADD_STR_TO_JSON(szJson, ",\"");
	ADD_STR_TO_JSON(szJson, STR(domainnameList));
	ADD_STR_TO_JSON(szJson, "\":");
	ADD_STR_TO_JSON(szJson, "[");
	i=0;
	char *buf,*buf2;
	while(domain_queue!=NULL)
	{
		//printf("domain i =%d \n",i);
		if ( i == 0 )
		{
			ADD_STR_TO_JSON(szJson, "{");
		}
		else
		{
			ADD_STR_TO_JSON(szJson, ",{");
		}

    	if(hertUtil_isEncryptoDomin())
		{
		  int len=strlen(domain_queue->data->domain);
		   int rel_len=(len%8>0?(len/8+1)*8:len);
			if(len>DOMAIN_SIZE)
			{
			  ret=-1;
			  goto ERROR;
			}
			if(hert_des_ecb_crypt(domain_queue->data->domain,key,rel_len,1))
			{
				ret=-1;
				goto ERROR;
			}
		char buf[DOMAIN_SIZE]={0};
		int outlen=DOMAIN_SIZE;
		hertUtil_base64_encode(domain_queue->data->domain,rel_len,buf,&outlen);
		if(outlen<=0||outlen>=DOMAIN_SIZE)
		{
		  ret=-1;
		  HERT_LOGERR("domain base64 ERROR\n");
		  goto ERROR;
		}
        
		FORMAT_JSON_ITEM_STR(szItem,buf,domain);      /* domain, */         
	
		}
		else{

			FORMAT_JSON_ITEM_STR(szItem, domain_queue->data->domain,domain);      /* domain, */
		}


		if (nJsonSize > strlen(szJson) + strlen(szItem))
		{
			snprintf(szJson + strlen(szJson), nJsonSize - strlen(szJson), "%s", szItem);
		}
		else
		{
			HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nJsonSize, szJson, strlen(szJson), strlen(szItem), szItem);
			ret=RETCODE_NO_MEMORY;
			goto ERROR;
		}
		//
		char time[20]={0};
		struct tm *tm=localtime(&domain_queue->data->time);
		sprintf(time,"%04d%02d%02d%02d%02d%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
		// printf("time  %s \n",time);
		FORMAT_JSON_ITEM_STR(szItem,time,accessTime);      /* accessTime */
		ADD_ITEM_TO_JSON(szJson, szItem);
		ADD_STR_TO_JSON(szJson, "}");

		i++;
		tmp=domain_queue;
		domain_queue=domain_queue->next;
		tmp->next=NULL;
		free(tmp);
		ptMsgVarBody->num--;

		//printf("left num  =%d \n",ptMsgVarBody->num);
	}
	ADD_STR_TO_JSON(szJson, "]");
    ADD_STR_TO_JSON(szJson, "}");
	ADD_STR_TO_JSON(szJson, "]");
	ADD_STR_TO_JSON(szJson, "}");

	/* ===== count body length */
	nBodyLength   = strlen(szJson);

	/* ====== set msg struct */
	buffsize = nOptionLength + nBodyLength + 256;
	Ptr = malloc(buffsize);
	if (!Ptr)
	{
		HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
				nOptionLength + nBodyLength, buffsize);
		ret=RETCODE_NO_MEMORY;
		goto ERROR;
	}
	memset(Ptr, 0x0, buffsize);

	*ptMsg = Ptr;

	/* set header: type */
	SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */

	Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
	Ptr += nLengthSize;

	/* set option */
	COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
	COPY_CHAR(Ptr, ptMsgOption->nLowLength);

	COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

	/* set body */
	COPY_STR(Ptr, szJson, nBodyLength);

	*nDataLen = Ptr - *ptMsg;

ERROR:
	FREE_MEM(szJson);
	while(domain_queue!=NULL)
	{
		tmp=domain_queue;
		domain_queue=domain_queue->next;
		tmp->next=NULL;
		free(tmp);
		ptMsgVarBody->num--;
	}
	return ret;
}



/* build: 发送数据-查询路由器工作状态响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_RouteStatusRsp(IN OUT CHAR **ptMsg, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_STATUS *ptMsgVarBody,int isSendRequest)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    DWORD i = 0;
    CHAR msgType = PUSH_DATA;
    HERT_MSG_RSP_PUSH_DATA_VARPART_DEVINFO *pDevList = NULL;
    int nJsonSize = 0;
    DWORD buffsize = 0;

    if ( (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;
    nJsonSize = 256 + ptMsgVarBody->devNum * 160;
    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);

    //only status response need errorcode and descript
	  if(isSendRequest == 0)
	  {
	      FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
	      ADD_ITEM_TO_JSON(szJson, szItem);
	    
	      FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
	      ADD_ITEM_TO_JSON(szJson, szItem);
	  }

    /* ===== Format Json message var body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->devStatus, devStatus);           /* devStatus */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->runStatus, runStatus);           /* runStatus */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->downbandwidth, downbandwidth);   /* downbandwidth */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->broadbandRate, broadbandRate);   /* broadbandRate */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_LNG(szItem, ptMsgVarBody->wifiStatus, wifiStatus);      /* wifiStatus */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, ",\"");
    ADD_STR_TO_JSON(szJson, STR(subDevList));
    ADD_STR_TO_JSON(szJson, "\":");
    ADD_STR_TO_JSON(szJson, "[");

    for(i = 0; i < ptMsgVarBody->devNum; i++)
    {
        if ( i == 0 )
        {
            ADD_STR_TO_JSON(szJson, "{");
        }
        else
        {
            ADD_STR_TO_JSON(szJson, ",{");
        }

        pDevList = &ptMsgVarBody->pDevList[i];
        
        FORMAT_JSON_ITEM_STR(szItem, pDevList->devName, devName);      /* devName */
        if (nJsonSize > strlen(szJson) + strlen(szItem))
        {
            snprintf(szJson + strlen(szJson), nJsonSize - strlen(szJson), "%s", szItem);
        }
        else
        {
            HERT_LOGERR("No memory: Json(%d):%s, strlen(Json)=%d, Item(%d):%s", nJsonSize, szJson, strlen(szJson), strlen(szItem), szItem);
            FREE_MEM(szJson);
            return RETCODE_NO_MEMORY;
        }
        if(strstr(pDevList->devName,"android"))
            sprintf(pDevList->devType,"%d",1);
        else if(strstr(pDevList->devName,"iPhone"))
            sprintf(pDevList->devType,"%d",2);
        else if(strstr(pDevList->devName,"Mac"))
            sprintf(pDevList->devType,"%d",3);
        else if(strstr(pDevList->devName,"PC")||strstr(pDevList->devName,"WINDOWS"))
            sprintf(pDevList->devType,"%d",4);
        else if(strstr(pDevList->devName,"iPad"))
            sprintf(pDevList->devType,"%d",5);

        else
            sprintf(pDevList->devType,"%d",0);
        
        FORMAT_JSON_ITEM_STR(szItem, pDevList->devType, devType);      /* devType */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_STR(szItem, pDevList->devID, devID);        /* devID */
        ADD_ITEM_TO_JSON(szJson, szItem);
        if(isSendRequest == 0)
        {
            FORMAT_JSON_ITEM_STR(szItem, pDevList->mac, mac);          /* mac */
        }
        else
        {
            FORMAT_JSON_ITEM_STR(szItem, pDevList->mac, Mac);          /* mac */
        }
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_STR(szItem, pDevList->connectTime, ConnectTime);  /* ConnectTime */
        ADD_ITEM_TO_JSON(szJson, szItem);

        ADD_STR_TO_JSON(szJson, "}");

    }
    ADD_STR_TO_JSON(szJson, "]");

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    /* ====== set msg struct */
    buffsize = nOptionLength + nBodyLength + 256;
    Ptr = malloc(buffsize);
    if (!Ptr)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }
    memset(Ptr, 0x0, buffsize);

    *ptMsg = Ptr;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - *ptMsg;

    FREE_MEM(szJson);

    return ret;
}

/* build: 创建下载任务响应  C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_AddDownloadMissionRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_RSP_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    CHAR *szJson;
    int nJsonSize = 0;
    DWORD i = 0, j = 0;
    DOWNLOAD_MISSION *pdlMission = NULL;
    FILE_INFO *pfileInfo = NULL;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;
    nJsonSize = 512 + ptMsgVarBody->missionNum * sizeof(ptMsgVarBody);   
    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
    ADD_ITEM_TO_JSON(szJson, szItem);

    /* ===== Format Json message var body */
    ADD_STR_TO_JSON(szJson, ",\"");
    ADD_STR_TO_JSON(szJson, STR(missionInfo));
    ADD_STR_TO_JSON(szJson, "\":");
    for (i = 0; i < ptMsgVarBody->missionNum; i++)
    {
        if (i == 0)
        {
            ADD_STR_TO_JSON(szJson, "{");
        }
        pdlMission = &ptMsgVarBody->pMissionInfo[i];
        
        FORMAT_JSON_ITEM_STR(szItem, pdlMission->missionID, missionID);  /* missionID */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_STR(szItem, pdlMission->missionName, missionName);  /* missionName */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_STR(szItem, pdlMission->missionType, missionType);  /* missionType */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_LNG(szItem, pdlMission->missionStatus, missionStatus);  /* missionStatus */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_LNG(szItem, pdlMission->missionSize, missionSize);  /* missionSize */
        ADD_ITEM_TO_JSON(szJson, szItem); 
        
        ADD_STR_TO_JSON(szJson, "[");
        
        FORMAT_JSON_ITEM_LNG(szItem, pdlMission->missionCurSpeed, missionCurSpeed);  /* missionCurSpeed */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_LNG(szItem, pdlMission->missionCompleteSize, missionCompleteSize);  /* missionCompleteSize */
        ADD_ITEM_TO_JSON(szJson, szItem);

        FORMAT_JSON_ITEM_STR(szItem, pdlMission->missionEndTime, missionEndTime);  /* missionEndTime */
        ADD_ITEM_TO_JSON(szJson, szItem);
        
        FORMAT_JSON_ITEM_STR(szItem, pdlMission->missionDuration, missionDuration);  /* missionDuration */
        ADD_ITEM_TO_JSON(szJson, szItem);       
        
        FORMAT_JSON_ITEM_LNG(szItem, pdlMission->dlFileNum, dlFileNum);  /* dlFileNum */
        ADD_ITEM_TO_JSON(szJson, szItem); 
        
        for (j = 0; j < pdlMission->dlFileNum; j++)
        {
            pfileInfo = &pdlMission->pFileList[j];
            if (j == 0)
            {
                ADD_STR_TO_JSON(szJson, "{");
            }
            FORMAT_JSON_ITEM_STR(szItem, pfileInfo->fileID, fileID);  /* fileID */
            ADD_ITEM_TO_JSON(szJson, szItem);
                        
            FORMAT_JSON_ITEM_STR(szItem, pfileInfo->fileName, fileName);  /* fileName */
            ADD_ITEM_TO_JSON(szJson, szItem);
             
            FORMAT_JSON_ITEM_STR(szItem, pfileInfo->fileType, fileType);  /* fileType */
            ADD_ITEM_TO_JSON(szJson, szItem);
            
            FORMAT_JSON_ITEM_LNG(szItem, pfileInfo->fileStatus, fileStatus);  /* fileStatus */
            ADD_ITEM_TO_JSON(szJson, szItem);
            
            FORMAT_JSON_ITEM_LNG(szItem, pfileInfo->fileSize, fileSize);  /* fileSize */
            ADD_ITEM_TO_JSON(szJson, szItem);                     
                    
            FORMAT_JSON_ITEM_STR(szItem, pfileInfo->fileAddress, fileAddress);  /* fileAddress */
            ADD_ITEM_TO_JSON(szJson, szItem);
                          
            FORMAT_JSON_ITEM_LNG(szItem, pfileInfo->fileStatus, fileStatus);  /* fileStatus */
            ADD_ITEM_TO_JSON(szJson, szItem);
        }
        ADD_STR_TO_JSON(szJson, "]");
        ADD_STR_TO_JSON(szJson, "}");                                     
    }
    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}
/* parse: 设备操作请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_DeviceOperateReq(IN CHAR *ptMsg, IN DWORD datalength, OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DEVICEOPERATE *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[512];

    if ( (!ptMsg) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }
    /* ====== parse msg data(body) */
    pData = ptMsg;

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData= ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(operType)) == 0 )
        {
            ptMsgVarBody->operType = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(operPara)) == 0 )
        {
            strcpy(ptMsgVarBody->operPara, szItemValue);
        }

        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* parse: UnknowData请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_UnknowDataReq(IN CHAR *ptMsg, IN DWORD datalength, OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[512];

    if ( (!ptMsg) || (!ptMsgConstBody) )
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), ptMsgConstBody(%p)", ptMsg, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }
    /* ====== parse msg data(body) */
    pData = ptMsg;

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData= ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }

        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* build: 设备操作响应  C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_DeviceOperateRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    CHAR *szJson;
    int nJsonSize = 0;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;
    nJsonSize = 512;   
    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}

/* build: unknow message 响应  C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_UnknowDataRsp(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    CHAR *szJson;
    int nJsonSize = 0;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;
    nJsonSize = 512;   
    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}

/* parse: 创建下载任务请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_AddDownloadMissionReq(IN CHAR *ptMsg, IN DWORD datalength, OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_ADD_DOWNLOAD_MISSION *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[512];

    if ( (!ptMsg) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }
    /* ====== parse msg data(body) */
    pData = ptMsg;

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData= ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(dlAddress)) == 0 )
        {
            strcpy(ptMsgVarBody->dlAddress, szItemValue);
        }

        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* parse: 发送数据-图片下载通知请求	S->C, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Parse_Msg_DownloadReq(IN CHAR *ptMsg, IN DWORD datalength, OUT HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          OUT HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR    *pData    = NULL;
    CHAR    szItemName[128];
    CHAR    szItemValue[512];

    if ( (!ptMsg) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }
    /* ====== parse msg data(body) */
    pData = ptMsg;

    memset(szItemName, 0x0, sizeof(szItemName));
    memset(szItemValue, 0x0, sizeof(szItemValue));
    while(NULL != (pData= ParseJsonString(pData, szItemName, szItemValue)))
    {
        HERT_LOGINFO("szItemName(%s), szItemValue(%s)", szItemName, szItemValue);
        if (strcmp(szItemName, STR(version)) == 0 )
        {
            ptMsgConstBody->version = atoi(szItemValue);
        }
        else if (strcmp(szItemName, STR(msgType)) == 0 )
        {
            strcpy(ptMsgConstBody->msgType, szItemValue);
        }
        else if (strcmp(szItemName, STR(msgSeq)) == 0 )
        {
            strcpy(ptMsgConstBody->msgSeq, szItemValue);
        }
        else if (strcmp(szItemName, STR(contentID)) == 0 )
        {
            strcpy(ptMsgVarBody->contentID, szItemValue);
        }
        else if (strcmp(szItemName, STR(URL)) == 0 )
        {
            strcpy(ptMsgVarBody->URL, szItemValue);
        }
        else if (strcmp(szItemName, STR(contentName)) == 0 )
        {
            strcpy(ptMsgVarBody->contentName, szItemValue);
        }
        memset(szItemName, 0x0, sizeof(szItemName));
        memset(szItemValue, 0x0, sizeof(szItemValue));
    }
    return ret;
}

/* build: 发送数据-图片下载通知响应	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_DownloadRsq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_RSP_PUSH_DATA_CONSTPART *ptMsgConstBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;

    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->errorCode, errorCode);   /* error code */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->description, description);   /* descript */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}


/* build: 发送数据-路由器属性上报请求	C->S, DATA_FORMAT: MSGHEADER + OPTION + BODY */
RETCODE Build_Msg_DownloadCompReq(IN OUT CHAR *ptMsg, IN DWORD buffsize, OUT DWORD *nDataLen,
                          IN HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN HERT_MSG_REQ_PUSH_DATA_CONSTPART *ptMsgConstBody,
                          IN HERT_MSG_REQ_PUSH_DATA_VARPART_DOWNLOAD_COMP *ptMsgVarBody)
{
    RETCODE ret = RETCODE_SUCCESS;
    CHAR *szJson;
    CHAR szItem[128];
    CHAR *Ptr = NULL;
    int nOptionLength = 0;
    int nOptDataLen   = 0;
    int nBodyLength   = 0;
    int nLengthSize   = 0;
    CHAR msgType = PUSH_DATA;
    int nJsonSize = 512;

    if ( (!ptMsg) || (!nDataLen) || (!ptMsgOption) || (!ptMsgConstBody)  || (!ptMsgVarBody))
    {
        HERT_LOGERR("Invalid parameter: ptMsg(%p), nDataLen(%d),ptMsgOption(%p),ptMsgConstBody(%p),ptMsgVarBody(%p)", 
                    ptMsg, nDataLen, ptMsgOption, ptMsgConstBody, ptMsgVarBody);
        return RETCODE_INVALID_PARAM;
    }

    /* ===== count option length */
    nOptionLength = GET_LENGTH_PUSH_OPTION();
    
    /* count option var item length */
    nOptDataLen   = GetLengthFromTwoBytes(ptMsgOption->nHighLength, ptMsgOption->nLowLength);
    if ( nOptDataLen != GET_STR_LENGTH(ptMsgOption->pData))
    {
        HERT_LOGINFO("Invalid user data length of body, nOptProtLen=%d, strlen(ptMsgOption->pData)=%d", 
                     nOptDataLen, GET_STR_LENGTH(ptMsgOption->pData));
    }
    nOptionLength += nOptDataLen;    

    szJson = malloc(nJsonSize);
    if (!szJson)
    {
        HERT_LOGERR("Failed to malloc buff size(%d)", nJsonSize);
        return RETCODE_NO_MEMORY;
    }
    memset(szJson, 0x0, nJsonSize);
    
    /* ===== Format Json message const body */
    FORMAT_JSON_ITEM_LNG(szItem, ptMsgConstBody->version, version);  /* version */
    ADD_ITEM_TO_JSON(szJson, szItem);

    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgType, msgType);  /* msg type */
    ADD_ITEM_TO_JSON(szJson, szItem);
    
    FORMAT_JSON_ITEM_STR(szItem, ptMsgConstBody->msgSeq, msgSeq);   /* msg seq */
    ADD_ITEM_TO_JSON(szJson, szItem);

    /* ===== Format Json message var body */
    FORMAT_JSON_ITEM_STR(szItem, ptMsgVarBody->contentID, contentID);      /* contentID */
    ADD_ITEM_TO_JSON(szJson, szItem);

    ADD_STR_TO_JSON(szJson, "}");

    /* ===== count body length */
    nBodyLength   = strlen(szJson);

    if ( (nOptionLength + nBodyLength) > buffsize)
    {
        HERT_LOGERR("Invalid parameter: nOptionLength + nBodyLength(%d), buffsize(%d)", 
                    nOptionLength + nBodyLength, buffsize);
        FREE_MEM(szJson);
        return RETCODE_NO_MEMORY;
    }

    /* ====== set msg struct */
    memset(ptMsg, 0x0, buffsize);
    Ptr = ptMsg;

    /* set header: type */
    SET_MSG_HEADER_TYPE(Ptr, msgType); /* set header: type */
    
    Ptr = SetMsgLengthItem(Ptr, nOptionLength + nBodyLength, &nLengthSize); /* set header: length */
    Ptr += nLengthSize;

    /* set option */
    COPY_CHAR(Ptr, ptMsgOption->nHighLength); /* NOTE: high and low MAYBE need exchange, pls edit it when debug */
    COPY_CHAR(Ptr, ptMsgOption->nLowLength);
    
    COPY_STR(Ptr, ptMsgOption->pData, nOptDataLen);

    /* set body */
    COPY_STR(Ptr, szJson, nBodyLength);

    *nDataLen = Ptr - ptMsg;

    FREE_MEM(szJson);
    return ret;
}

