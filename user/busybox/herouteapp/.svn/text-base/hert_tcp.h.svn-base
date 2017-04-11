
#ifndef __HE_ROUTE_TCP_HEADER__
#define __HE_ROUTE_TCP_HEADER__

/*
** status return codes
*/
#define T_STATUS_SUCCESS 0
#define T_STATUS_USERNAME_NOT_FOUND 1
#define T_STATUS_INCORRECT_PASSWORD 2
#define T_STATUS_ACCOUNT_DISABLED 3
#define T_STATUS_USER_DISABLED 4
#define T_STATUS_LOGIN_SUCCESSFUL_ALREADY_LOGGED_IN 100
#define T_STATUS_LOGIN_RETRY_LIMIT 101
#define T_STATUS_LOGIN_SUCCESSFUL_SWVER 102
#define T_STATUS_LOGIN_FAIL_SW 103
#define T_STATUS_LOGOUT_SUCCESSFUL_ALREADY_DISCONNECTED 200
#define T_STATUS_LOGOUT_AUTH_RETRY_LIMIT 201
#define T_STATUS_LOGIN_SUCCESS_SWVER 300
#define T_STATUS_LOGIN_FAIL_SWVER 301
#define T_STATUS_LOGIN_FAIL_INV_PROT 302
#define T_STATUS_LOGIN_UNKNOWN 500
#define T_STATUS_FAIL_USERNAME_VALIDATE 501
#define T_STATUS_FAIL_PASSWORD_VALIDATE 502

#define MAXUSERNAME 25
#define MAXPASSWORD 25
#define MAXAUTHSERVER 80
#define MAXAUTHDOMAIN 80
#define MAXLOGINPROG 256
#define MAXCONFFILE 256
#define MAXLOCALADDRESS 32
#define MAXDDNSCONFFILE 256


#define DEFAULT_DEBUG         1
#define DEFAULT_AUTHSERVER    "sm-server"
#define DEFAULT_AUTHDOMAIN    ""
#define DEFAULT_AUTHPORT      1122
#define DEFAULT_CONFFILE      "/usr/local/etc/bpalogin.conf"

typedef unsigned short INT2;
typedef unsigned int INT4;

typedef struct tagTransaction
{
    char data[1204];
    DWORD length;
}HERT_TRANSACTION;

/*
**  This structure holds all information necessary to connect/disconnect
*/
typedef struct tagSession
{
    /*
    **  Control paramters
    */
    char authserver[MAXAUTHSERVER];
    char authdomain[MAXAUTHDOMAIN];
    unsigned short authport;
    char localaddress[MAXLOCALADDRESS];
    unsigned short localport;
    char remoteaddress[MAXLOCALADDRESS];
    unsigned short remoteport;
    int minheartbeat, maxheartbeat;

    /*
    **  Internal data
    */
    INT4 sessionid;
    INT2 listenport;
    struct sockaddr_in authhost;
    int listensock;
    struct sockaddr_in localaddr;
    struct sockaddr_in localipaddress;
}HERT_SESSION;

RETCODE HERT_TCP_Init(HERT_SESSION * s);
RETCODE HERT_TCP_SendTransaction(HERT_SESSION *s, HERT_TRANSACTION * t);
RETCODE HERT_TCP_SendTransactionExt(HERT_SESSION *s, char *pData, int length);
RETCODE HERT_TCP_RecvTransaction(HERT_SESSION *s, HERT_TRANSACTION * t);

void HERT_Dump_Transaction(HERT_TRANSACTION * t);

RETCODE HERT_TCP_check_Connect_Init(HERT_SESSION * s);
RETCODE HERT_TCP_check_Connect_UnInit(HERT_SESSION * s);
RETCODE HERT_TCP_check_Connect(HERT_SESSION * s);
RETCODE HERT_TCP_check_ConnectTimeOut(HERT_SESSION * s);

#endif /* __HE_ROUTE_TCP_HEADER__ */

