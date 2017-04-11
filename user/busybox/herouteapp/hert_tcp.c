#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>
#include <netdb.h>
#include <syslog.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "hert_com.h"
#include "hert_app.h"
#include "hert_msg.h"
#include "hert_util.h"
#include "hert_tcp.h"


int HERT_SOCKET_Close(int s)
{
    return close(s);
}

void HERT_Dump_Buffer(char *pData, int nLength)
{
    int i = 0;
    char * p = pData;
    CHAR szLogBuf[128] = {0x0};

    memset(szLogBuf, 0x0, sizeof(szLogBuf));
    for(i = 0; i < 16; i++)
    {
        sprintf(szLogBuf + strlen(szLogBuf), "%02x ",i);
    }
    HERT_LOGINFO(szLogBuf);

    i = 0;
    memset(szLogBuf, 0x0, sizeof(szLogBuf));
    while(i < nLength)
    {
        sprintf(szLogBuf + strlen(szLogBuf), "%02x ",(unsigned char)*(p));
        if ((i%16 == 0x0f) && (i != 0))
        {
            HERT_LOGINFO(szLogBuf);
            memset(szLogBuf, 0x0, sizeof(szLogBuf));
        }
        p++;
        i++;
    }
    if (*szLogBuf != 0x0)
    {
        HERT_LOGINFO(szLogBuf);
    }
    HERT_LOGINFO("\n");
}

void HERT_Dump_Transaction(HERT_TRANSACTION * t)
{
    HERT_Dump_Buffer(t->data, t->length);
}

void HERT_dump(char *pszData, const char *pszFunction, int nLine)
{
    HERT_TRANSACTION t;

    memset(&t, 0x0, sizeof(t));
    strcpy(t.data, pszData);
    t.length=strlen(pszData);
    HERT_LOGINFO("-------%s,%d----------\n", pszFunction, nLine);
    HERT_Dump_Transaction(&t);
    HERT_LOGINFO("=======%s,%d==========\n", pszFunction, nLine);
}

void HERT_Dump_SendTransaction(HERT_TRANSACTION * t)
{
    HERT_LOGINFO("Sent transaction:");
    HERT_Dump_Transaction(t);
}

void HERT_Dump_RecvTransaction(HERT_TRANSACTION * t)
{
    HERT_LOGINFO("Received transaction:");
    HERT_Dump_Transaction(t);
}

RETCODE HERT_TCP_SendTransaction(HERT_SESSION *s, HERT_TRANSACTION * t)
{
    int r;

    r = send(s->listensock,(void *)t,t->length,0);

    HERT_Dump_SendTransaction(t);

    return r > 0 ? RETCODE_SUCCESS : RETCODE_TCPSOCKET_ERROR;
}

RETCODE HERT_TCP_SendTransactionExt(HERT_SESSION *s, char *pData, int length)
{
    int r;

    r = send(s->listensock,(void *)pData,length,0);

    HERT_Dump_Buffer(pData, length);

    return r > 0 ? RETCODE_SUCCESS : RETCODE_TCPSOCKET_ERROR;
}
RETCODE HERT_TCP_RecvTransaction(HERT_SESSION *s, HERT_TRANSACTION * t)
{
    int r = recv(s->listensock,(char *)t,2400,0);

    t->length = r;

    HERT_Dump_RecvTransaction(t);

    return r > 0 ? RETCODE_SUCCESS : RETCODE_TCPSOCKET_ERROR;
}

/*
void genmd5(char *p,int len,char *digest)
{
    MD5_CTX context;
    MD5Init(&context);
    MD5Update(&context, p, len);
    MD5Final(digest, &context);
}
*/

/*
**  This functions makes the MD5 based data packet which is used to login,
**  logout and handle heartbeats
*/
#if 0
void makecredentials(char * credentials,HERT_SESSION *s,INT2 msg,INT4 extra)
{
	INT2 j = htons(msg);
	int i=0;
	char buffer[150];
	INT4 ts = htonl(extra);

	memcpy(buffer,s->nonce,16);
	i += 16;
	memcpy(buffer+i,s->password,strlen(s->password));
	i += strlen(s->password);
	memcpy(buffer+i,&(ts),sizeof(INT4));
	i += sizeof(INT4);
	memcpy(buffer+i,&j,sizeof(INT2));
	i += sizeof(INT2);

	genmd5(buffer,i,credentials);
}
#endif

RETCODE HERT_TCP_Init(HERT_SESSION * s)
{
    RETCODE ret = RETCODE_SUCCESS;
    struct hostent * he;
    int err;

    s->listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* set local socket address */
    s->localaddr.sin_family = AF_INET;
    s->localaddr.sin_port = htons(s->localport);
    if(strcmp(s->localaddress,""))
    {
        HERT_LOGINFO("Using local address %s\n",s->localaddress);
        he = gethostbyname(s->localaddress);
        if(he)
        {
            /* copy the network address to sockaddr_in structure */
            s->localaddr.sin_addr.s_addr = *((int*)(he->h_addr_list[0]));
        }
        else
        {
            s->localaddr.sin_addr.s_addr = inet_addr(s->localaddress);
        }
    }
    else
    {
        s->localaddr.sin_addr.s_addr = INADDR_ANY;
    }

    /* set host socket address */
    he = gethostbyname(s->authserver);
    if(he)
    {
        s->authhost.sin_addr.s_addr = *((int*)(he->h_addr_list[0]));
    }
    else
    {
        s->authhost.sin_addr.s_addr = inet_addr(s->authserver);
    }

    s->authhost.sin_port = htons(s->authport);
    s->authhost.sin_family = AF_INET;

    err = bind(s->listensock,(struct sockaddr *)&s->localaddr,sizeof(s->localaddr));
    if(err)
    {
        HERT_LOGINFO("Error binding auth socket - %.80s\n",strerror(errno));
        HERT_SOCKET_Close(s->listensock);
        return RETCODE_INTERNEAL_ERROR;
    }

    err = connect(s->listensock,(struct sockaddr *)&s->authhost,sizeof(struct sockaddr_in));
    if(err)
    {
        HERT_LOGINFO("Cant connect to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
        HERT_SOCKET_Close(s->listensock);
        return RETCODE_UNREACHABLE_ERROR;
    }

    return ret;
}

RETCODE HERT_TCP_check_Connect_Init(HERT_SESSION * s)
{
    RETCODE ret = RETCODE_SUCCESS;
    struct hostent * he;
    struct in_addr old_auth_sin_addr = { 0x0 };
    char szAuthorIP[64];
    char **pptr;
	
    /* set local socket address */
    s->localaddr.sin_family = AF_INET;
    s->localaddr.sin_port = htons(s->localport);
    if(strcmp(s->localaddress,""))
    {
        HERT_LOGINFO("Using local address %s\n",s->localaddress);
        he = gethostbyname(s->localaddress);
        if(he)
        {
            /* copy the network address to sockaddr_in structure */
            s->localaddr.sin_addr.s_addr = *((int*)(he->h_addr_list[0]));
        }
        else
        {
            s->localaddr.sin_addr.s_addr = inet_addr(s->localaddress);
        }
    }
    else
    {
        s->localaddr.sin_addr.s_addr = INADDR_ANY;
    }
    HERT_LOGINFO("Using authserver address %s\n", s->authserver);
    /* set host socket address */
    if (old_auth_sin_addr.s_addr != s->authhost.sin_addr.s_addr ||
		0 == old_auth_sin_addr.s_addr)
    {
        HERT_LOGINFO("gethostbyname(%s), sin_port %d\n", s->authserver, s->authport);
        he = gethostbyname(s->authserver);
        if(he)
        {
            pptr = he->h_addr_list;
            for(; *pptr!=NULL; pptr++)
            {
                memset(szAuthorIP, 0x0, sizeof(szAuthorIP));
                HERT_LOGERR("debug___address:%s\n", inet_ntop(he->h_addrtype, *pptr, szAuthorIP, sizeof(szAuthorIP)));
                if (0 != strcmp(szAuthorIP, "255.255.255.255"))
                {
                   s->authhost.sin_addr.s_addr = inet_addr(szAuthorIP);
                   old_auth_sin_addr.s_addr = s->authhost.sin_addr.s_addr;
                   break;
                }
            }
        }
        else
        {
            s->authhost.sin_addr.s_addr = inet_addr(s->authserver);
        }
        memset(szAuthorIP, 0x0, sizeof(szAuthorIP));
        sprintf(szAuthorIP, "%s", inet_ntoa(s->authhost.sin_addr));
        if (0 == strcmp(szAuthorIP, "255.255.255.255"))
        {
            old_auth_sin_addr.s_addr = 0;
        }
        else
        {
            old_auth_sin_addr.s_addr = s->authhost.sin_addr.s_addr;
        }
    }
    else
    {
        s->authhost.sin_addr.s_addr = old_auth_sin_addr.s_addr;
        memset(szAuthorIP, 0x0, sizeof(szAuthorIP));
        sprintf(szAuthorIP, "%s", inet_ntoa(s->authhost.sin_addr));
    }

    HERT_LOGINFO("Using authserver inet_ntoa(s->authhost.sin_addr)=(%s), sin_port %d\n", 
		inet_ntoa(s->authhost.sin_addr), s->authport);

    s->authhost.sin_port = htons(s->authport);
    s->authhost.sin_family = AF_INET;

    if (0 == strcmp(szAuthorIP, "255.255.255.255"))
    {
        ret = RETCODE_TCPSOCKET_ERROR;
    }

    return ret;
}

RETCODE HERT_TCP_check_Connect_UnInit(HERT_SESSION * s)
{
    RETCODE ret = RETCODE_SUCCESS;
    
    if(s->listensock)
    {
        HERT_SOCKET_Close(s->listensock);
    }
    return ret;
}

#define RETURN_ERR_AND_CLEAR() \
	HERT_SOCKET_Close(s->listensock); \
	s->listensock = 0; \
	return RETCODE_INTERNEAL_ERROR;

RETCODE HERT_TCP_check_Connect(HERT_SESSION * s)
{
    RETCODE ret = RETCODE_SUCCESS;
    int err;

    s->listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    HERT_LOGINFO("Using authserver auth server(%s:%d)\n", s->authserver, s->authport);
    err = bind(s->listensock,(struct sockaddr *)&s->localaddr,sizeof(s->localaddr));
    if(err)
    {
        HERT_LOGINFO("Error binding auth socket - %.80s\n",strerror(errno));
        HERT_SOCKET_Close(s->listensock);
        return RETCODE_INTERNEAL_ERROR;
    }
    HERT_LOGINFO("connect...\n");

    err = connect(s->listensock,(struct sockaddr *)&s->authhost,sizeof(struct sockaddr_in));
    if(err)
    {
        HERT_LOGINFO("Cant connect to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
        HERT_SOCKET_Close(s->listensock);
        return RETCODE_UNREACHABLE_ERROR;
    }
    HERT_SOCKET_Close(s->listensock);
    return ret;
}

RETCODE HERT_TCP_check_ConnectTimeOut(HERT_SESSION * s)
{
    RETCODE ret = RETCODE_SUCCESS;
    struct timeval tm;
    int err;
    unsigned long ul = 1;
    int error = -1;
    int len = 0;
    fd_set writeSet;
    fd_set readSet;
    int res;
    char szAuthorIP[64];
    char errorString[64];

    memset(errorString, 0x0, sizeof(errorString));
    memset(szAuthorIP, 0x0, sizeof(szAuthorIP));
    sprintf(szAuthorIP, "%s", inet_ntoa(s->authhost.sin_addr));
    if (0 == strcmp(szAuthorIP, "255.255.255.255"))
    {
        ret = HERT_TCP_check_Connect_Init(s);
        if (ret != RETCODE_SUCCESS)
        {
            HERT_LOGINFO("Failed to connect int(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
#if 0
            if(strstr(errorString,"refused"))
            {
                HERT_LOGINFO("refused also set network is reachable");
                return RETCODE_SUCCESS;
            }
#endif
            return RETCODE_UNREACHABLE_ERROR;
        }
        else
        {
            /*if analysis the ip just return SUCCESS*/
            return RETCODE_SUCCESS;
        }
    }

    s->listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    HERT_LOGINFO("Using authserver sin_port %d\n",s->authport);
    ioctl(s->listensock, FIONBIO, &ul); // set as noneblock mode
        
    //if (hertUtil_IsInFile("/var/hertbind","DEBUG")){HERT_LOGINFO("first bind...\n");
        err = bind(s->listensock,(struct sockaddr *)&s->localaddr,sizeof(s->localaddr));
        if(err)
        {
            HERT_LOGINFO("Error binding auth socket - %.80s\n",strerror(errno));
            RETURN_ERR_AND_CLEAR();
        }
    //}
    HERT_LOGINFO("connect...\n");
    errno = 0;
    err = connect(s->listensock,(struct sockaddr *)&s->authhost,sizeof(struct sockaddr_in));
    if( (err < 0) && (errno < 0) && (errno != EINPROGRESS) )
    {
        HERT_LOGINFO("Cant connect to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
        RETURN_ERR_AND_CLEAR();
    }

    tm.tv_sec = 20;
    tm.tv_usec = 0;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_SET(s->listensock, &readSet);
    FD_SET(s->listensock, &writeSet);
    if( (res = select(s->listensock + 1, &readSet, &writeSet, NULL, &tm)) <= 0)
    {
        strncpy(errorString,strerror(errno),sizeof(errorString));
        HERT_LOGINFO("Cant select to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, errorString);
        
        if(strstr(errorString,"Interrupted system call"))
        {
            HERT_LOGINFO("ignore Interrupted event");
            return RETCODE_NOIMPORTMENT_ERROR;
        }
        
        RETURN_ERR_AND_CLEAR();
    }
    /* Not necessary */
    if(res == 2) 
    {
        HERT_LOGINFO("Cant select to auth server(%s:%d) - res(%d), %.80s\n", s->authserver, s->authport, res, strerror(errno));
        RETURN_ERR_AND_CLEAR();
    }
    if(!FD_ISSET(s->listensock, &writeSet))
    {
        HERT_LOGINFO("Cant fdisset to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
        RETURN_ERR_AND_CLEAR();
    } 
    errno = 0;
    error = 0;
    if(getsockopt(s->listensock, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0)
    {
        HERT_LOGINFO("Cant getsockopt to auth server(%s:%d) - %.80s\n", s->authserver, s->authport, strerror(errno));
        RETURN_ERR_AND_CLEAR();
    }
    else if(error != 0)
    {
        HERT_LOGINFO("Can not connect to auth server(%s:%d,%d) - %.80s\n", s->authserver, s->authport, error, strerror(errno));
        RETURN_ERR_AND_CLEAR();
    }
    else
    {
        HERT_LOGINFO("___Connect to auth server(%s:%d) - %.80s_______\n", s->authserver, s->authport, strerror(errno));
    }
    ul = 0;
    ioctl(s->listensock, FIONBIO, &ul); // set as block mode

    HERT_SOCKET_Close(s->listensock);
    s->listensock = 0;

    return ret;
}

