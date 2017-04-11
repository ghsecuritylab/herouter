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
#include <fcntl.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/vfs.h> 
#include <sys/statfs.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/wireless.h>
#include "linux/autoconf.h"

#include "hert_com.h"

#ifdef BUILD_PLAT_COM
#include "hert_app.h"
#include "hert_msg.h"
#include "hert_util.h"
#include "nvram.h"

#else
#include "wsIntrn.h"
#include "md5.h"
#include "nvram.h"
#include "herouter.h"
#include "utils.h"

#endif


int g_esw_fd;

#ifdef SUPPORT_SEM
#include <semaphore.h>

char SEM_TASK_HEROUTE_PLATFORM[]= "heroute_platform_task";
char SEM_TASK_HEROUTE_APP[]= "heroute_app_task";
sem_t *g_sem_want_read = NULL;
sem_t *g_sem_finish_read = NULL;
#endif


#define MAX_DEV_NUM  12

extern int g_nRemainTime;

static DEVINFO g_DevInfArray[MAX_DEV_NUM];
static int g_nDevNum = 0;

static RT_802_11_MAC_TABLE g_rtTable = {0};

#define USB_STORAGE_SIGN    "/media/sd"

static DHCPITEM g_WebdhcpItemArray[MAX_ITEMS_NUMBER];/* save reachable items */
static int      g_WebdhcpItemNum = 0;

extern char currMsgType[128];
extern int toAddDevNum;
extern int firstTime;

extern int mtd_open(const char *name, int flags);

char* hertUtil_getYearmonth();

char sdxList[26][5] = {"sda1","sdb1","sdc1","sdd1","sde1","sdf1","sdg1","sdh1","sdi1","sdj1","sdk1","sdl1",\
	                       "sdm1","sdn1","sdo1","sdp1","sdq1","sdr1","sds1","sdt1","sdu1","sdv1","sdw1","sdx1","sdy1","sdz1"};

void herUtil_dumpHexString(const char *pszFunction, int nLine, char *pszData)
{
    char szBuf[1024];
    char *pt = pszData;
    memset(szBuf, 0x0, sizeof(szBuf));
    while(*pt)
    {
        sprintf(szBuf+strlen(szBuf), "%x ", 0xff & (*pt) );
        pt++;
    }
    HERT_LOGINFO("[%s,%d]:\n%s\n", pszFunction, nLine, szBuf);
	//system("echo 3 > /proc/sys/vm/drop_caches");
}

#define DUMP_STR(str) \
    if (hertUtil_IsInFile("/var/hertdumpstr","DEBUG")) \
    { \
        herUtil_dumpHexString(__FUNCTION__, __LINE__, str); \
    }

char hertUtil_isWifiOn()
{
    char long_buf[1024];
    FILE *fp;

    memset(long_buf, 0, sizeof(long_buf));
    if(!(fp = popen("ifconfig ra0", "r")))
    {
        return 0;
    }
    fread(long_buf, 1, sizeof(long_buf) - 1, fp);
    pclose(fp);

    if(!strstr(long_buf, "HWaddr"))
    {
        return 0;
    }

    return 1;
}

/* Format: 8699990000001406000000 */
char *hertUtil_get2860SN()
{
    FILE *file;
    static char firmwareSn[64];
    char cmd[128];
    char *pYearMonth = NULL;
    const char *tempFileName = "/var/tempsn.txt";

    memset(firmwareSn, 0x0, sizeof(firmwareSn));
    sprintf(cmd,"nvram_get HE_ROUTE_SN > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return firmwareSn;
    }   
    if (fgets(firmwareSn, 64, file) && (strlen(firmwareSn) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(firmwareSn);
    }    
    fclose(file);
    unlink(tempFileName);

    /* update year/month to Format: 8699990000001406000000 */
    pYearMonth = hertUtil_getYearmonth();
    if ( (*pYearMonth != 0x0 ) && (strlen(firmwareSn) > 20) && (!strstr(firmwareSn, pYearMonth)) )
    {
        /* 8699990000001406000000 -> 869999000000YYMM000000 */
        strncpy(firmwareSn + 12, pYearMonth, 4);
    }
    HERT_LOGINFO("firmwareSn: (%s)\n",firmwareSn);
    return firmwareSn;
}

char *hertUtil_getSN()
{
    FILE *file;
    static char firmwareUbSn[64];
    char cmd[128];
    const char *tempFileName = "/var/temputsn.txt";

    memset(firmwareUbSn, 0x0, sizeof(firmwareUbSn));
    sprintf(cmd,"nvram_get uboot HE_ROUTE_SN > %s", tempFileName);
    system(cmd);	
    if (file = fopen(tempFileName, "r"))
    {
        if (fgets(firmwareUbSn, 64, file) && (strlen(firmwareUbSn) >= 1) ) 
        {
            /* remove 0x0d,0x0a if there is */
            REMOVE_CRLN(firmwareUbSn);
        }    
        fclose(file);
        unlink(tempFileName);
    }
	DUMP_STR(firmwareUbSn);
    HERT_LOGINFO("firmwareUbSn: (%s)\n",firmwareUbSn);
    if(0x0 == (*firmwareUbSn))
    {   
	
        return hertUtil_get2860SN();
    }

    return firmwareUbSn;
}

#define MAX_EAP_LEN                     1265 /* Size of buffer to hold EAP message. Can hold 5 EAP attributes, i.e. the max for Radius over UDP */

/* Encode an EAP msg into a string using the base64 algorithm. 
 * Returns 0 if encoding was successfull. Returns 1 if the capacity 
 * of the str is not enough to hold the encoded EAP msg*/
int base64encoder(unsigned char *pData, unsigned int nlength, char * outstr, 
			 const unsigned int max_str_out) 
{
  
  const static char table64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
  uint8_t i0;
  uint8_t i1;
  uint8_t i2;
  
  uint8_t o0;
  uint8_t o1;
  uint8_t o2;
  uint8_t o3;
  
  unsigned int x_in = 0;
  unsigned int x_out = 0; 
  
  /* input length */
  unsigned int len_in = nlength;
  
  
  unsigned int len_out = (len_in*4+2)/3;	 /* output length without padding */
  unsigned int len_out_padd = ((len_in+2)/3)*4;  /* output length with padding */
  
  /* Check if we have enough space in the output buffer to store the encoding 
     result */
  if (len_out_padd >= max_str_out){
    return 1;
  }
  
  /*
    if (optionsdebug) log_dbg("Base64 encoder: input len: %d\n", len_in);
    if (optionsdebug) log_dbg("Base64 encoder: output len without padding: %d\n", len_out);
    if (optionsdebug) log_dbg("Base64 encoder: output len with padding: %d\n", len_out_padd);
  */
  
  while(x_in < len_in) {
    
    i0 =  pData[x_in++] & 0xff;
    i1 = (x_in < len_in)?pData[x_in++] & 0xff:0;
    i2 = (x_in < len_in)?pData[x_in++] & 0xff:0;
    
    o0 = i0 >> 2;
    o1 = (((i0 & 3) << 4) | (i1 >> 4));
    o2 = ((i1 & 0xf) << 2) | (i2 >> 6);
    o3 = i2 & 0x3F;
    
    outstr[x_out++] = table64[o0];
    outstr[x_out++] = table64[o1];
    outstr[x_out] = (x_out < len_out) ? table64[o2] : '=';
    x_out++;
    outstr[x_out] = (x_out < len_out) ? table64[o3] : '=';
    x_out++;
  }
  
  outstr[x_out] = 0;
  return 0;
}


/* Decode a string using the base64 algorithm into an EAP msg. 
 * Returns 0 if decoding was successfull, returns 1 if the eapstr is too big
 * for the EAP msg. Return 2 for other errors */
int base64decoder (char * eapstr, unsigned char *pData, unsigned int *nlength)
{
  if (eapstr == NULL || pData == NULL)
    return 1;
  
  char car;
  char in64[4];
  int  compt;   
  int  x_in = 0; 
  int  x_out = 0;
  unsigned int  len_in = strlen(eapstr);
  unsigned int  len_out;
  
  *nlength = 0;       /* To avoid invalid data if decoding fails */
  
  if ((len_in % 4) != 0)
    return 2;
  
  /* Remove all trailing '=' characters */
  while (eapstr [len_in-1] == '='){
    eapstr [len_in-1] = 0;
    len_in--;
  }
  
  len_out = (len_in*3) / 4;
  
  /* Check of that the size of the resulting message fits into 
     the EAP msg buffer. */
  if (len_out > MAX_EAP_LEN)
    return 1;
  
  /* while not end of string */
  while (x_in < len_in) {
    for (compt = 0; compt < 4 && x_in < len_in; compt++) {
      car = eapstr [x_in++];
      
      /* decode the char */
      if ('A' <= car && car <= 'Z')
    in64[compt] = car - 'A';
      else if ('a' <= car && car <= 'z')
    in64[compt] = car + 26 - 'a';
      else if ('0' <= car && car <= '9')
    in64[compt] = car + 52 - '0';
      else if (car == '+')
    in64[compt] = 62;
      else if (car == '/')
    in64[compt] = 63;
      else
    return 2;   /* Invalid character */
    }
    
    
    pData[x_out++] = (in64[0] << 2) | (in64[1] >> 4);
    if (x_out < len_out) 
      pData[x_out++] = (in64[1] << 4) | (in64[2] >> 2);
    if (x_out < len_out) 
      pData[x_out++] = (in64[2] << 6) | (in64[3]);
  }
  
  *nlength = x_out;
  
  /* indicate success of decoding */
  return 0;
}

int hertUtil_isStorageExist(void)
{
    char buf[256];
    FILE *fp = fopen("/proc/scsi/scsi", "r");
    if(!fp)
    {
        perror(__FUNCTION__);
        return 0;
    }
	
#if 0	
#define USB_STORAGE_SIGN    "/media/sd"
#define SD_STORAGE_SIGN     "/media/mmc"
#endif

#define USB_STORAGE_HOST_SIGN    "Host"


    while(fgets(buf, sizeof(buf), fp))
    {
        if(strstr(buf, USB_STORAGE_HOST_SIGN))
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    HERT_LOGDEBUG("no hotplug disk found\n.");
    return 0;
}

int hertUtil_getStorageSize(char *path, long long *freeSize, long long *totalSize) 
{
    struct statfs myStatfs;  
  
    if (statfs(path, &myStatfs) == -1) 
    {  
        HERT_LOGERR("Failed at statfs!!\n");
        return -1;  
    }  
      
    //long long?  
    *freeSize = (((long long)myStatfs.f_bsize * (long long)myStatfs.f_bfree) / (long long) 1024 / (long long) 1024);  
    *totalSize = (((long long)myStatfs.f_bsize * (long long)myStatfs.f_blocks) / (long long) 1024 / (long long) 1024);  
  
    return 0;  
}

int hertUtil_GetUSBPartitionPath(int nIndex, char *outPathName)
{

    char buf[256] = {0},buftmp[256] = {0};
    char *ptr = NULL;
    char *pchr = NULL;
    FILE *fp = fopen("/proc/mounts", "r");
    int iCount = 0;
    char szUsbStag[128] = { 0x0 };

    if(!fp)
    {
        HERT_LOGERR("Maybe the usb paritition was not mounted");
        strcpy(outPathName,"/media/sda1"); 
        return 0;
    }
    memset(buf, 0x0, sizeof(buf));
    memset(szUsbStag, 0x0, sizeof(szUsbStag));
    sprintf(szUsbStag, "%s", USB_STORAGE_SIGN);

    //the mounts disk name may be /media/sda1 or /media/sda12
    while(fgets(buf, sizeof(buf), fp))
    {
        if((ptr = strstr(buf, szUsbStag)) != NULL)
        {
            pchr = strstr(ptr, " ");
            if(NULL != pchr)
            {
                *pchr = 0x0;
            }
            if (iCount == nIndex)
            {
                memset(buftmp, 0x0, sizeof(buftmp));
                strncpy(buftmp, ptr, sizeof(buftmp));
            }
            iCount++;
        }
        memset(buf, 0x0, sizeof(buf));
    }
	
    fclose(fp);


	strcpy(outPathName,buftmp); 
	HERT_LOGDEBUG("######## outPathName:%s, PartitionNum(%d)########\n", outPathName, iCount);
    return iCount;
}

int hretUtil_GetUSBPartitionSize(int nIndex, long long *freeSize, long long *totalSize, char *pszOutPathName, int nPathNameSize) 
{
    struct statfs myStatfs;
    char szPathName[256] = {0};
    int nPartitionNum = 0;

    *freeSize = 0;
    *totalSize= 0;

    /* get parition number */
    nPartitionNum = hertUtil_GetUSBPartitionPath(nIndex, szPathName);
	HERT_LOGINFO("===========>\n");

    if( (nPartitionNum > 0) && (nIndex < nPartitionNum) && ( *szPathName != 0x0) )
    {
		if (statfs(szPathName, &myStatfs) == -1)
		{
            HERT_LOGERR("Failed at statfs(%s)!!\n", szPathName);
		}
		else
		{
            memset(pszOutPathName, 0x0, nPathNameSize);
            memcpy(pszOutPathName, szPathName, nPathNameSize);
            *freeSize = (((long long)myStatfs.f_bsize * (long long)myStatfs.f_bfree) / (long long) 1024 / (long long) 1024);  
            *totalSize = (((long long)myStatfs.f_bsize * (long long)myStatfs.f_blocks) / (long long) 1024 / (long long) 1024);  
		}
    }
	HERT_LOGDEBUG("nIndex(%d), freeSize(%lld), totalSize(%lld), pszOutPathName(%s), nPathNameSize(%d)\n", 
		nIndex, *freeSize, *totalSize, pszOutPathName, nPathNameSize);

    return 0;  
}

int hretUtil_GetUSBSize(long long *freeSize, long long *totalSize) 
{
    char szPathName[256] = {0};
    int nPartitionNum = 0;
    int nIndex = 0;
    long long partyfreeSize = 0;
    long long partytotalSize= 0;

	HERT_LOGINFO("=======xxxx====>\n");

    *freeSize = 0;
    *totalSize= 0;

    /* get parition number */
    nPartitionNum = hertUtil_GetUSBPartitionPath(0, szPathName);
	
	HERT_LOGINFO("nIndex(%d), nPartitionNum(%d), freeSize(%lld), totalSize(%lld)\n", 
		nIndex, nPartitionNum, *freeSize, *totalSize);

    while(nIndex < nPartitionNum)
    {
		HERT_LOGDEBUG("nIndex(%d), nPartitionNum(%d), freeSize(%lld), totalSize(%lld)",
                     nIndex, nPartitionNum, *freeSize, *totalSize);
		hretUtil_GetUSBPartitionSize(nIndex, &partyfreeSize, &partytotalSize, szPathName, sizeof(szPathName) - 1);

		if ((hertUtil_IsEnableWritePath(szPathName) == 1) || (hertUtil_IsExsitPath(szPathName) == 1))
		{
		//long long?
		*freeSize += partyfreeSize;
		*totalSize+= partytotalSize;
		}
        nIndex++;
    }
	HERT_LOGINFO("nIndex(%d), nPartitionNum(%d), freeSize(%lld), totalSize(%lld)\n", 
		nIndex, nPartitionNum, *freeSize, *totalSize);


    return 0;  
}

int hertUtil_GetUSBPartitionMaxFreeSize(long long *freeSize, char *pszOutPathName, int nPathNameSize) 
{
    char szPathName[256] = {0};
    int nPartitionNum = 0;
    int nIndex = 0;
    long long partyfreeSize = 0;
    long long partytotalSize= 0;

    *freeSize = 0;

    /* get parition number */
    nPartitionNum = hertUtil_GetUSBPartitionPath(0, szPathName);

    memset(pszOutPathName, 0x0, nPathNameSize);
    while(nIndex < nPartitionNum)
    {
		partyfreeSize = 0;
		partytotalSize= 0;
		HERT_LOGDEBUG("nIndex(%d), nPartitionNum(%d)", nIndex, nPartitionNum);
		hretUtil_GetUSBPartitionSize(nIndex, &partyfreeSize, &partytotalSize, szPathName, sizeof(szPathName) - 1);
		//long long?
        if ((partyfreeSize > *freeSize) || (*pszOutPathName == 0x0))
        {
            *freeSize = partyfreeSize;
            if (hertUtil_IsEnableWritePath(szPathName) == 1)
            {
                memset(pszOutPathName, 0x0, nPathNameSize);
                memcpy(pszOutPathName, szPathName, nPathNameSize);
            }
            else
            {
                HERT_LOGINFO("Failed to write file to the path(%s)", szPathName);
            }
        }
        nIndex++;
    }
	HERT_LOGINFO("nIndex(%d), nPartitionNum(%d), freeSize(%lld)\n", 
		nIndex, nPartitionNum, *freeSize);

    return 0;
}

int hretUtil_GetUSBPartitionName( char *pszOutPathName, int nPathNameSize) 
{
    char szPathName[256] = {0};
    int nPartitionNum = 0;
    int nIndex = 0;
    long long partyfreeSize = 0;
    long long partytotalSize= 0;

    /* get parition number */
    nPartitionNum = hertUtil_GetUSBPartitionPath(0, szPathName);

    memset(pszOutPathName, 0x0, nPathNameSize);
    while(nIndex < nPartitionNum)
    {
        partyfreeSize = 0;
        partytotalSize= 0;
        HERT_LOGDEBUG("nIndex(%d), nPartitionNum(%d)", nIndex, nPartitionNum);
        hretUtil_GetUSBPartitionSize(nIndex, &partyfreeSize, &partytotalSize, szPathName, sizeof(szPathName) - 1);
		//long long?
        if (*pszOutPathName == 0x0)
        {
            if ((hertUtil_IsEnableWritePath(szPathName) == 1) || (hertUtil_IsExsitPath(szPathName) == 1))
            {
                memset(pszOutPathName, 0x0, nPathNameSize);
                memcpy(pszOutPathName, szPathName, nPathNameSize);
            }
            else
            {
                HERT_LOGINFO("Failed to write file to the path(%s)", szPathName);
            }
        }
        nIndex++;
    }
	HERT_LOGINFO("nIndex(%d), nPartitionNum(%d)\n", 
		nIndex, nPartitionNum);

    return 0;
}

int hertUtil_IsInFile(const char *szFile, const char *szfindstr)
{
   FILE *fs = NULL;
   char line[256];
   int nFind = 0;
   char *ptr= NULL;
   int nLineCount = 0;

   if ( !szfindstr || (*szfindstr == 0x0) )
   {
      return nFind;
   }
   
   fs =  fopen(szFile, "r");

   if(fs != NULL)
   {
      while( ( fgets(line, 256, fs) != NULL ) && (nLineCount<= 1000) )
      {
         ptr = strstr(line, szfindstr);
         if(ptr)
         {
            nFind = 1; 
            break;
         }
         nLineCount++;
      }
      fclose(fs);
   }
   if(nLineCount > 1000)
   {
      HERT_LOGERR("Much lines for file(%s): nLineCount(%d)\n",szFile, nLineCount);
   }
   return nFind;
}

long hertUtil_getFileSize(const char *filename)
{
    struct stat fileStatus;

    if(stat(filename, &fileStatus) != 0)
    {
        return 0;
    }

    return (long)(fileStatus.st_size);
}

void herUtil_dumpAesHexString(char *pszFunction, int nLine, char *pszData, int nDataLen)
{
    int i = 0;
    char szBuf[1024];
    char *pt = pszData;
    if (!hertUtil_IsInFile("/var/aesdebug","DEBUG"))
    {
        return;
    }

    memset(szBuf, 0x0, sizeof(szBuf));
    for(i = 0; i < nDataLen; i++)
    {
        sprintf(szBuf+strlen(szBuf), "%x ", 0xff & (*(pt+i)));
    }
    HERT_LOGERR("[%s,%d]:\n%s\n", pszFunction, nLine, szBuf);
}

char* hertUtil_getFirmwareVersion()
{
    FILE *file;
    static char version[64];
    char cmd[128];
    const char *tempFileName = "/var/tempver.txt";
    char buf[64] = {0};

    memset(version, 0x0, sizeof(version));
    sprintf(cmd,"nvram_get HE_ROUTE_VER > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        printf("iptables : == unable to open config file: %s\n",tempFileName);
        return version;
    }

    while(fgets(buf, sizeof(buf), file))
    {
		strcpy(version, buf);
    }

    /* remove 0x0d,0x0a if there is */
    REMOVE_CRLN(version);
   
    fclose(file);
    unlink(tempFileName);	
    return version;
}

int hertUtil_IsEnableWritePath(char *pszPath)
{
    char szFileName[128] = { 0x0 };
    char szCmd[128] = { 0x0 };
    int nRet = 0;
    
    memset(szFileName, 0x0, sizeof(szFileName));
    sprintf(szFileName, "%s/trywrite.txt", pszPath);
    
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "echo test > %s", szFileName);
    system(szCmd);
    if (hertUtil_IsInFile(szFileName, "test"))
    {
        nRet = 1;
    }
    unlink(szFileName);
    return nRet;
}

#define PATH_NOT_EXIT_INFO "FAT: Directory bread"

int hertUtil_IsExsitPath(char *pszPath)
{
    char szFileName[128] = { 0x0 };
    char szCmd[128] = { 0x0 };
    int nRet = 1;
    
    memset(szFileName, 0x0, sizeof(szFileName));
    strcpy(szFileName,"/var/tryls.txt");
    
    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "ls %s > %s",pszPath, szFileName);
    system(szCmd);
    
    if (hertUtil_IsInFile(szFileName, PATH_NOT_EXIT_INFO))
    {
        nRet = 0;
    }
    unlink(szFileName);
    return nRet;
}


int hertUtil_GetUSBPath(char *outPathName)
{

    char buf[256] = {0},buftmp[256] = {0};
	char *ptr = NULL;
    FILE *fp = fopen("/proc/mounts", "r");
	int sig_len = 0;
	
    if(!fp)
    {
        perror(__FUNCTION__);
		strcpy(outPathName,"/media/sda1"); 
        return 0;
    }

    //the mounts disk name may be /media/sda1 or /media/sda12
    sig_len = strlen(USB_STORAGE_SIGN) + 3;
    while(fgets(buf, sizeof(buf), fp))
    {
        if((ptr = strstr(buf, USB_STORAGE_SIGN)) != NULL)
        {
            memset(buftmp,0x0,sizeof(buftmp));
			strncpy(buftmp,ptr,sig_len);
        }
    }
	
    fclose(fp);

    if(buftmp[sig_len -1] == ' ')
	{
        buftmp[sig_len -1] = '\0';
	}
	
	strcpy(outPathName,buftmp); 
	HERT_LOGINFO("######## outPathName:%s########\n",outPathName);
    return 0;
	

}

#define DEFAULT_LAN_IP "192.168.8.1"
char *hertUtil_getLanIP(void)
{
    static char buf[64];
    char *nl;
    FILE *fp;

    memset(buf, 0, sizeof(buf));
    if( (fp = popen("nvram_get 2860 lan_ipaddr", "r")) == NULL )
    {
        goto error;
    }

    if(!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        goto error;
    }

    if(!strlen(buf))
    {
        pclose(fp);
        goto error;
    }
    pclose(fp);

    if(NULL != (nl = strchr(buf, '\n')))
    {
        *nl = '\0';
    }

    return buf;

error:
    HERT_LOGERR("warning, cant find lan ip!!\n");
    return DEFAULT_LAN_IP;
}



char* hertUtil_getDeviceType()
{
    FILE *file;
    static char devtype[64];
    char cmd[128];
    const char *tempFileName = "/var/tempdevtype.txt";

    memset(devtype, 0x0, sizeof(devtype));
    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"nvram_get HE_ROUTE_DevType > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return devtype;
    }   
    if (fgets(devtype, 64, file) && (strlen(devtype) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(devtype);
    }    
    fclose(file);
    unlink(tempFileName);

    return devtype;
}


char* hertUtil_getYearmonth()
{
    static char yearMonth[64];

    memset(yearMonth, 0x0, sizeof(yearMonth));
#if 0
    time_t curTime;
    struct tm *pcurTime;

    memset(&curTime, 0x0, sizeof(curTime));

    pcurTime = gmtime(&curTime);
    if ( pcurTime )
    {	
        sprintf(yearMonth, "%02d%02d", pcurTime->tm_year, pcurTime->tm_mon);
    }
#else
    char szVersion[64];
    char *pchr = NULL;

    memset(szVersion, 0x0, sizeof(szVersion));
    sprintf(szVersion, "%s", hertUtil_getFirmwareVersion());
    pchr = strstr(szVersion, "-");
    if (pchr)
    {
        *pchr = 0x0;
    }
    if ( strlen(szVersion) == 4 )
    {
        strcpy(yearMonth, szVersion);
    }
#endif
	HERT_LOGINFO("yearMonth=%s", yearMonth);
    return yearMonth;
}


int hertUtil_readMac(char *buf)
{
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
#if ! defined (NO_WIFI_SOC)
	lseek(fd, 0x2E, SEEK_SET);
#else
	lseek(fd, 0xE006, SEEK_SET);
#endif
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}

char *hertUtil_getWanInterface()
{
    static char wanInterface[64];

    const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
    if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
		|| !strncmp(cm, "3G", 3)
#endif
	){
        return "ppp0";
	}

    memset(wanInterface, 0x0, sizeof(wanInterface));
    strcpy(wanInterface, "eth2.2");
    return wanInterface;
}

DWORD hertUtil_getSeconds()
{
    struct timeval  tv;
    struct timezone tz;

    memset(&tv, 0x0, sizeof(tv));
    memset(&tz, 0x0, sizeof(tz));

    if (gettimeofday(&tv, &tz))
    {
        HERT_LOGERR("No memory, failed to print msg log!");
        return -1;
    }
    return tv.tv_sec;
}

void hertUtil_toUpper(char *pszStr)
{
    char *s1 = pszStr;
    if (!s1) return;
    while (*s1 && (*s1 = toupper(*s1)))
        s1++;
}

void hertUtil_tolower(char * pszStr)
{
    char *s1 = pszStr;
    if (!s1) return;
    while (*s1 && (*s1 = tolower(*s1)))
        s1++;
}

char *hertUtil_getBroadbandRate()
{

    return "%7"; /* TBD */
}

DWORD hertUtil_getBroadband()
{
    FILE *file;
    char cmd[128];
    char line[256];
    const char *tempFileName = "/var/tempbroadband.txt";
    char *ptr = NULL;
    char *ptrend = NULL;
    static long preTimeSecond = 0;
    static long long preKb = 0;
    long curTimeSecond = 0;
    long long dwKb = 0;
    DWORD dwBoardband = 0;

#define KEY_STR_WORD "RX bytes:"

    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"ifconfig %s > %s", hertUtil_getWanInterface(), tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 0;
    }
    while ( fgets(line, 256, file) != NULL ) 
    {
        ptr = strstr(line, KEY_STR_WORD);
        if(ptr)
        {
            break;
        }
    }
    fclose(file);
    unlink(tempFileName);

    if(!ptr)
    {
        HERT_LOGERR("can not find rx rate in  file: %s\n",tempFileName);
        return 0;
    }
    ptr += strlen(KEY_STR_WORD);
    ptrend = strstr(ptr, " ");
    if (ptrend != NULL)
    {    //3488863186
        *ptrend = 0x0;
        HERT_LOGDEBUG("ptr(%s)\n",ptr);
    }
    HERT_LOGINFO("ptr(%s)\n",ptr);
    curTimeSecond = hertUtil_getSeconds();
    HERT_LOGINFO("DEBUG: ptr(%s)\n",ptr);
    dwKb = atoll(ptr)/1024;
    HERT_LOGINFO("curTimeSecond(%ld),preTimeSecond(%ld),curTimeSecond - preTimeSecond(%ld),\n", 
                curTimeSecond, preTimeSecond, curTimeSecond - preTimeSecond);
    HERT_LOGINFO("ptr(%s),dwKb(%lld), preKb(%lld), curTimeSecond(%ld),preTimeSecond(%ld),curTimeSecond - preTimeSecond(%ld),\n", 
                ptr, dwKb, preKb, curTimeSecond, preTimeSecond, curTimeSecond - preTimeSecond);
    if ( (curTimeSecond - preTimeSecond) == 0)
    {
        dwBoardband = dwKb - preKb; 
    }
    else
    {
        dwBoardband =  (dwKb - preKb)/(curTimeSecond - preTimeSecond);
    }
    /* changed it up 0 */
    if ( dwBoardband >= 0xefffffff )
    {
        dwBoardband = 0xefffffff & dwBoardband;
    }
    dwBoardband = 0xffff & dwBoardband;
    HERT_LOGINFO("dwBoardband(%ld)\n",dwBoardband);
    preKb = dwKb;
    HERT_LOGINFO("curTimeSecond(%ld),preTimeSecond(%ld),curTimeSecond - preTimeSecond(%ld),\n", 
                curTimeSecond, preTimeSecond, curTimeSecond - preTimeSecond);
    preTimeSecond = curTimeSecond;

    return dwBoardband;
}

DWORD hertUtil_getWorkStatus()
{
    FILE *file;
    char line[256];
    char cmd[128];
    const char *tempFileName = "/var/temptop.txt";
    char *ptr = NULL;
    DWORD cpuRate = 0;
#define KEY_WORD "CPU:"

    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"top -n 1 -d 2 > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGERR("unable to open config file: %s\n",tempFileName);
        return 0;
    }
    while ( fgets(line, 256, file) != NULL ) 
    {
        ptr = strstr(line, KEY_WORD);
        if(ptr)
        {
           HERT_LOGDEBUG("line: (%s)\n",line);
           break;
        }
    }
    HERT_LOGDEBUG("line: (%s)\n",line);
    fclose(file);
    unlink(tempFileName);

    if(!ptr)
    {
        HERT_LOGERR("can not find cpu rate in  file: %s\n",tempFileName);
        return 0;
    }
    ptr += strlen(KEY_WORD);
    while(ptr && ((*ptr)== ' '))
        ptr++;

    cpuRate = atoi(ptr);
    HERT_LOGDEBUG("ptr(%s), cpuRate(%ld)\n", ptr, cpuRate);
    if (cpuRate < 30)
    {
        return 0; /* idle */
    }
    else if (cpuRate < 60)
    {
        return 1; /* good */
    }
    else
    {
        return 2; /* werse */
    }
}

int hertUtil_getWanDectTime()
{
    FILE *file;
    static char wanTime[64];
    char cmd[128];
    const char *tempFileName = "/var/tempwantime.txt";

    memset(wanTime, 0x0, sizeof(wanTime));
    sprintf(cmd,"nvram_get HE_ROUTE_WANLOOP_TIME > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 3; /* default for 3 seconds */
    }   
    if (fgets(wanTime, 64, file) && (strlen(wanTime) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(wanTime);
    }    
    fclose(file);
    unlink(tempFileName);

    return (atoi(wanTime) == 0) ? 3 : atoi(wanTime);
}

int hertUtil_getLanDectTime()
{
    FILE *file;
    static char lanTime[64];
    char cmd[128];
    const char *tempFileName = "/var/templanTime.txt";

    memset(lanTime, 0x0, sizeof(lanTime));
    sprintf(cmd,"nvram_get HE_ROUTE_LANLOOP_TIME > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 6; /* default for 6 seconds */
    }   
    if (fgets(lanTime, 64, file) && (strlen(lanTime) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(lanTime);
    }    
    fclose(file);
    unlink(tempFileName);

    return (atoi(lanTime) == 0) ? 6 : atoi(lanTime);
}

/*
** get wireless client info, 
** for some client will not be include in dhcp client
**
** return < 0 for error, else return the table number
*/
int hertUtil_UpdateWirelessClientInfo()
{
    char *ifname = "ra0";
    struct iwreq iwr;
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
    char tmpBuff[32];
    char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#endif
    int s = 0;

    memset(&g_rtTable, 0x0, sizeof(g_rtTable));

   /* get if name */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) &g_rtTable;

    if (s < 0) 
    {
        HERT_LOGERR("ioctl sock failed!");
        return -1;
    }

#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
    if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) 
    {
        HERT_LOGERR("ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
#else
    if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) 
    {
        HERT_LOGERR("ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
#endif
        close(s);
        return -1;
    }

    close(s);

    return g_rtTable.Num;
}

int hertUtil_UpdateWirelessClientItem(int nIndex, DEVINFO *pDevInfo)
{
    if(nIndex >= g_rtTable.Num)
    {
        HERT_LOGERR("invalid param, nIndex(%d), nNum(%d)!", nIndex, g_rtTable.Num);
        return -1;
    }

    memset(pDevInfo,0x0,sizeof(DEVINFO));
    sprintf(pDevInfo->szMac, "%02X:%02X:%02X:%02X:%02X:%02X",
             g_rtTable.Entry[nIndex].Addr[0], g_rtTable.Entry[nIndex].Addr[1],
             g_rtTable.Entry[nIndex].Addr[2], g_rtTable.Entry[nIndex].Addr[3],
             g_rtTable.Entry[nIndex].Addr[4], g_rtTable.Entry[nIndex].Addr[5]);
    HERT_LOGDEBUG("pDevInfo->szMac(%s)", pDevInfo->szMac);

    return 0;
}

int hertUtil_IsWirelessClientMac(unsigned char *pszMac)
{
    int  i = 0;
    char szTempMac[64];

    for(i = 0; i < g_rtTable.Num; i++)
    {
        memset(szTempMac, 0x0, sizeof(szTempMac));
        sprintf(szTempMac, "%02X:%02X:%02X:%02X:%02X:%02X",
                g_rtTable.Entry[i].Addr[0], g_rtTable.Entry[i].Addr[1],
                g_rtTable.Entry[i].Addr[2], g_rtTable.Entry[i].Addr[3],
                g_rtTable.Entry[i].Addr[4], g_rtTable.Entry[i].Addr[5]);
		HERT_LOGDEBUG("szTempMac(%s), pszMac(%s)", szTempMac, pszMac);
        if ( 0 == strcasecmp(szTempMac, pszMac) )
        {
            return 1;
        }
    }
    return 0;
}

int hertUtil_IsInSaveWirelessClientMac(unsigned char *pszMac)
{
    FILE *file;
    char line[256];
    const char *pszFileName = "/var/wirelessclient.txt";
    char *ptr = NULL;

    if (!pszMac)
    {
        HERT_LOGERR("Invalid parameters pszMac(%p)", pszMac);
        return 0;
    }
	
    HERT_LOGINFO(">>>>>szLowMac(%s)", pszMac);
    if (!(file = fopen(pszFileName, "r"))) 
    {
        HERT_LOGINFO("Failed to open file(%s)", pszFileName);
        return 0;
    }
    while ( fgets(line, 256, file) != NULL ) 
    {
        ptr = strstr(line, pszMac);
        if(ptr)
        {
            fclose(file);
            HERT_LOGINFO("ptr(%s), pszMac(%s)", ptr, pszMac);
            return 1;
        }
    }
    fclose(file);
    HERT_LOGINFO("<<<<<not wireless pszMac(%s)", pszMac);
    return 0;
}


/*
** DHCP CLIENT process, start
**
*/
#if 1 /* DHCP CLIENT, start */
/* *********** this function is only used to save option60 relate data *************** */
int  hertUtil_UpdateDevInfo()
{
    FILE* fr = NULL;
    char szCmd[256];
/* dhcpd.c write-> /var/dhcpdevinf, this file read<- /var/dhcpdevinf */
#define TEMPR_DEV_INFO_FILE     "/var/dhcpdevinfTr"
#define REAL_DEV_INFO_FILE      "/var/dhcpdevinf"

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "cp %s %s 2>/dev/null", REAL_DEV_INFO_FILE, TEMPR_DEV_INFO_FILE);
    system(szCmd);
    fr  = fopen(TEMPR_DEV_INFO_FILE, "rb");
    if ( !fr)
    {
        HERT_LOGDEBUG("Failed to open DEVINFO_FILE(%s)\n", TEMPR_DEV_INFO_FILE);
        return -2;
    }

    /* File was opened successfully. */
    memset(&g_DevInfArray[0], 0x0, sizeof(DEVINFO) * MAX_DEV_NUM);
    g_nDevNum = 0;
    /* Attempt to read => write */
    while ( (fread(&g_DevInfArray[g_nDevNum], 1, sizeof(DEVINFO), fr) == sizeof(DEVINFO)) && 
            (g_nDevNum < MAX_DEV_NUM) )
    {
        hertUtil_toUpper(g_DevInfArray[g_nDevNum].szMac);
        HERT_LOGDEBUG("g_DevInfArray[%d].szMac(%s)\n", g_nDevNum, g_DevInfArray[g_nDevNum].szMac);
        g_nDevNum++;
    }
    fclose(fr);

    memset(szCmd, 0x0, sizeof(szCmd));
    sprintf(szCmd, "rm %s 2>/dev/null", TEMPR_DEV_INFO_FILE);
    system(szCmd);

    return g_nDevNum;
}

int  hertUtil_GetDevInfo(char *pszMac, DEVINFO *pDevInfo)
{
    int  i = 0;

    for(i = 0; i < g_nDevNum; i++)
    {
        if ( 0 == strcasecmp(g_DevInfArray[i].szMac, pszMac) )
        {
            HERT_LOGDEBUG("g_DevInfArray[%d].mac(%s), pszMac(%s)!", 
                i, g_DevInfArray[i].szMac, pszMac);
            memcpy(pDevInfo, &g_DevInfArray[i], sizeof(DEVINFO));
            return 0;
        }
    }
    return 1;
}

int hertUtil_GetDhcpOption60ClientItem(int nIndex, DEVINFO *pDevInfo)
{
    if(nIndex >= g_nDevNum)
    {
        HERT_LOGERR("invalid param, nIndex(%d), nNum(%d)!", nIndex, g_nDevNum);
        return -1;
    }

    memset(pDevInfo,0x0,sizeof(DEVINFO));
    memcpy(pDevInfo, &g_DevInfArray[nIndex], sizeof(DEVINFO));

    return 0;
}

/* *********** this function is only used to save dhcp client relate data *************** */
int  hertUtil_UpdateIpReachInfo(int bDoPing)
{
    FILE *fp;
    FILEDHCPITEM lease;
    struct in_addr addr;
    char  szHostname[128];
    DHCPITEM dhcpItemArray[MAX_ITEMS_NUMBER];/* save reachable items */
    int   nDhcpItemNum = 0;
    HERT_LOGDEBUG(">>>>>>>>>>>>>>>>>\n"); 
    system("rm -rf /var/tmpudcpdlease 2>/dev/null");
    system("cp /var/udhcpd.leases /var/tmpudcpdlease 2>/dev/null");
    fp = fopen("/var/tmpudcpdlease", "r");
    if (NULL == fp)
    {
        return 0;
    }
    nDhcpItemNum = 0;
    memset(&dhcpItemArray[0], 0x0, sizeof(DHCPITEM) * MAX_ITEMS_NUMBER );
    while ( (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) && (nDhcpItemNum < MAX_ITEMS_NUMBER) )
    {
        CHECK_EMPTY_MAC(lease.mac);
        addr.s_addr = lease.ip;
        //printf("[%s,%d]: __ host(%s), lanip(%s), nPingCount(3), nTimeOut(3) ___\n", 
        //        __FUNCTION__, __LINE__, inet_ntoa(addr), hertUtil_getLanIP());
        memset(szHostname, 0x0, sizeof(szHostname));
        sprintf(szHostname, "%s", inet_ntoa(addr));
        if (bDoPing)
        {
            //if (!heroute_ping_apiLan(szHostname, hertUtil_getLanIP(), 3, 3))
            {
                dhcpItemArray[nDhcpItemNum].nReachable = 1;
            }
        }
        else
        {
            dhcpItemArray[nDhcpItemNum].nReachable = 1;
        }
        memcpy(&dhcpItemArray[nDhcpItemNum], &lease, sizeof(lease));
        nDhcpItemNum++ ;
    }
    HERT_LOGDEBUG("<<<<<<<<<<<<<<<<<\n"); 
    fclose(fp);

    /* update the result to membuf for self use */
    g_WebdhcpItemNum = nDhcpItemNum;
    memcpy(&g_WebdhcpItemArray[0], &dhcpItemArray[0], sizeof(DHCPITEM) * MAX_ITEMS_NUMBER );

    return g_WebdhcpItemNum;
}
int  hertUtil_GetIpReachInfo(char *pszMac, DEVINFO *pDevInfo)
{
    int  i = 0;
    char szTempMac[64];

    DUMP_STR(pszMac);
    for(i = 0; i < g_WebdhcpItemNum; i++)
    {
        DUMP_STR(g_WebdhcpItemArray[i].mac);
        memset(szTempMac, 0x0, sizeof(szTempMac));
        sprintf(szTempMac, "%02X:%02X:%02X:%02X:%02X:%02X", g_WebdhcpItemArray[i].mac[0],
			g_WebdhcpItemArray[i].mac[1], g_WebdhcpItemArray[i].mac[2], g_WebdhcpItemArray[i].mac[3],
			g_WebdhcpItemArray[i].mac[4], g_WebdhcpItemArray[i].mac[5]);
        HERT_LOGDEBUG("g_WebdhcpItemArray[%d].mac(%s), pszMac(%s), g_WebdhcpItemArray[%d].hostname(%s)!", 
			i, szTempMac, pszMac, i, g_WebdhcpItemArray[i].hostname);
        if ( 0 == strcasecmp(szTempMac, pszMac) )
        {
            /* update product name if need */
            if ( (pDevInfo->szProduct[0] == 0x0) &&
                 (g_WebdhcpItemArray[i].hostname[0] != 0x0) )
            {
                strcpy(pDevInfo->szProduct, g_WebdhcpItemArray[i].hostname);
            }
            HERT_LOGDEBUG("return for nIndex(%d), nReachable(%d)!", i, g_WebdhcpItemArray[i].nReachable);
            return g_WebdhcpItemArray[i].nReachable; /* it will return 0-no reachable or 1-reachable */
        }
    }
    return 2;
}

int hertUtil_GetDhcpClientItem(int nIndex, DEVINFO *pDevInfo)
{
    if(nIndex >= g_WebdhcpItemNum)
    {
        HERT_LOGERR("invalid param, nIndex(%d), nNum(%d)!", nIndex, g_WebdhcpItemNum);
        return -1;
    }

    memset(pDevInfo, 0x0, sizeof(DEVINFO));
    pDevInfo->curtime = g_WebdhcpItemArray[nIndex].expires;
    sprintf(pDevInfo->szMac, "%02X:%02X:%02X:%02X:%02X:%02X", g_WebdhcpItemArray[nIndex].mac[0],
        g_WebdhcpItemArray[nIndex].mac[1], g_WebdhcpItemArray[nIndex].mac[2], g_WebdhcpItemArray[nIndex].mac[3],
        g_WebdhcpItemArray[nIndex].mac[4], g_WebdhcpItemArray[nIndex].mac[5]);
    strcpy(pDevInfo->szProduct, g_WebdhcpItemArray[nIndex].hostname);

    return g_WebdhcpItemArray[nIndex].nReachable; /* it will return 0-no reachable or 1-reachable */
}

int hertUtil_GetDhcpClientLeaseItem(int nIndex, void *pDhcpItem)
{
    DHCPITEM *pDClientInfo = (DHCPITEM *)pDhcpItem;

    if(nIndex >= g_WebdhcpItemNum)
    {
        HERT_LOGERR("invalid param, nIndex(%d), nNum(%d)!", nIndex, g_WebdhcpItemNum);
        return -1;
    }

    memset(pDClientInfo, 0x0, sizeof(DHCPITEM));
    memcpy(pDClientInfo, &g_WebdhcpItemArray[nIndex], sizeof(DHCPITEM));
    HERT_LOGINFO("pDevInfo->hostname(%s)", pDClientInfo->hostname);

    return pDClientInfo->nReachable; /* it will return 0-no reachable or 1-reachable */
}

char*  hertUtil_GetHostName(char *pszMac)
{
    int  i = 0;
    char szTempMac[64];

    for(i = 0; i < g_WebdhcpItemNum; i++)
    {
        memset(szTempMac, 0x0, sizeof(szTempMac));
        sprintf(szTempMac, "%02X:%02X:%02X:%02X:%02X:%02X", g_WebdhcpItemArray[i].mac[0],
			g_WebdhcpItemArray[i].mac[1], g_WebdhcpItemArray[i].mac[2], g_WebdhcpItemArray[i].mac[3],
			g_WebdhcpItemArray[i].mac[4], g_WebdhcpItemArray[i].mac[5]);
        if ( 0 == strcasecmp(szTempMac, pszMac) )
        {
            HERT_LOGDEBUG("g_WebdhcpItemArray[%d].mac(%s), hostname(%s), pszMac(%s)!", 
                i, szTempMac, g_WebdhcpItemArray[i].hostname, pszMac);
            return (char*)g_WebdhcpItemArray[i].hostname;
        }
    }
    return "unknown";
}
#endif /* DHCP CLIENT END */

int  hertUtil_GetIsInDevList(char *pszMac, HERT_COM_SUBDEV_BODY *pDevList, int nNum)
{
    int  i = 0;
    HERT_COM_SUBDEV_BODY *pDevInfo = NULL;
    DUMP_STR(pszMac);
    pDevInfo = pDevList;
    for(i = 0; i < nNum; i++)
    {
        DUMP_STR(pDevInfo->mac);
        if ( strstr(pDevInfo->mac, pszMac) || strstr(pszMac, pDevInfo->mac) )
        {
            return i;
        }
        pDevInfo++;
    }
    return -1;
}


int hertUtil_IsAssicString(char *pszString)
{
    char *pchr = NULL;

#define IS_ASSCII_CHAR(a) ((a >= 32) && (a <= 126))

/*
#define IS_ASSCII_CHAR(a) \
	(((a >= 'A') && (a <= 'Z')) || ((a >= 'a') && (a <= 'z')) || ((a >= '0') && (a <= '9')) \
	   || (a == '_') || (a == '.')|| (a == '(') || (a == ')')|| (a == '_') || (a == '.'))
	   
#define IS_ASSCII_CHAR(a) \
	((a >= 32) && (a <= 126) && (a != 34) || (a != 42) || (a != 47) || (a != 58) || (a != 60) \
	      || (a != 62) || (a != 63) || (a != 92) || (a != 124))      
*/

    pchr = pszString;
    while(pchr && (*pchr != 0x0))
    {
        if (!IS_ASSCII_CHAR(*pchr))
        {
            return 0;
        }
        pchr++;
    }

    return 1;
}

char* hertUtil_GetAESPwdFromOnenet()
{
    static char szReturn[64];
    FILE *file;
    const char *tempFileName = "/var/aesonenet.txt";

    memset(szReturn, 0x0, sizeof(szReturn));
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 0;
    }   
    if (fgets(szReturn, 64, file) && (strlen(szReturn) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(szReturn);
    }    
    fclose(file);

	printf("szReturn=%s\n", szReturn);
    return szReturn;
}

void hertUtil_SetAESPwdFromOnenet(const char *szAes)
{
    char cmd[128];
    const char *tempFileName = "/var/aesonenet.txt";

    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"echo %s > %s", szAes, tempFileName);
	system(cmd);
	printf("cmd=%s\n", cmd);
}


char* hertUtil_GetDateTime()
{
    static char szReturn[64];
    FILE *file;
    char szDateTime[64];
    char cmd[128];
    const char *tempFileName = "/var/tempdate.txt";
    char *pchr = NULL;
    int   nYear = 0;
    int   nMonth = 0;
    int   nDay = 0;
    char  szTime[16];

    memset(szDateTime, 0x0, sizeof(szDateTime));
    memset(cmd, 0x0, sizeof(cmd));
    sprintf(cmd,"date > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        HERT_LOGINFO("unable to open config file: %s\n",tempFileName);
        return 0;
    }   
    if (fgets(szDateTime, 64, file) && (strlen(szDateTime) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(szDateTime);
    }    
    fclose(file);
    unlink(tempFileName);

    // get year
    pchr = strstr(szDateTime, "UTC ");
    if (pchr)
    {
        pchr += strlen("UTC ");
        nYear = atoi(pchr);
    }
    else
    {
        pchr = strstr(szDateTime, "GMT ");
        if (pchr)
        {
            pchr += strlen("GMT ");
            nYear = atoi(pchr);
        }
    }
	
    // get month
    if (NULL != (pchr = strstr(szDateTime, "Jan")))
        nMonth = 1;
    else if (NULL != (pchr = strstr(szDateTime, "Feb")))
        nMonth = 2;
    else if (NULL != (pchr = strstr(szDateTime, "Mar")))
        nMonth = 3;
    else if (NULL != (pchr = strstr(szDateTime, "Apr")))
        nMonth = 4;
    else if (NULL != (pchr = strstr(szDateTime, "May")))
        nMonth = 5;
    else if (NULL != (pchr = strstr(szDateTime, "Jun")))
        nMonth = 6;
    else if (NULL != (pchr = strstr(szDateTime, "Jul")))
        nMonth = 7;
    else if (NULL != (pchr = strstr(szDateTime, "Aug")))
        nMonth = 8;
    else if (NULL != (pchr = strstr(szDateTime, "Sep")))
        nMonth = 9;
    else if (NULL != (pchr = strstr(szDateTime, "Oct")))
        nMonth = 10;
    else if (NULL != (pchr = strstr(szDateTime, "Nov")))
        nMonth = 11;
    else if (NULL != (pchr = strstr(szDateTime, "Dec")))
        nMonth = 12;

    if (NULL == pchr) return "unknown";

    // get day
    pchr += 4;
    nDay = atoi(pchr);
    if (nDay == 0 )
    {
        pchr += 1;
        nDay = atoi(pchr);
        pchr += 1;
    }
    else
    {
        pchr += 2;
    }

    // get time
    memset(szTime, 0x0, sizeof(szTime));
    strncpy(szTime, pchr, 9);


    memset(szReturn, 0x0, sizeof(szReturn));
    sprintf(szReturn, "%d-%02d-%02d %s", nYear, nMonth, nDay, szTime);

    return szReturn;
}
 
/* 
  格式是date后跟月日时分年
  注意年是4位年，例如：2007年10月26日 10：00：00
  date 102610002007
*/
int hertUtil_SetDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin)
{
    char cmd[128];

    memset(cmd, 0x0, sizeof(cmd));

    sprintf(cmd, "date %02d%02d%02d%02d%d 2>/dev/null", nMonth, nDay, nHour, nMin, nYear);

	system(cmd);

	return 0;
}

int hertUtil_GetDeviceList(HERT_COM_SUBDEV_BODY **ppDev, DWORD *pdwDevNumber)
{
    int ret = 0;
    HERT_COM_SUBDEV_BODY *pDev = NULL;
    HERT_COM_SUBDEV_BODY *pTempDev = NULL;
    DEVINFO tDevInfo;
    int nNumWc  = 0; /* number of WireClient */
    int nNumDc  = 0; /* number of Dhcp Client */
    int nNumDoc = 0; /* number of Dhcp option60 Client */
    int nNumMax = 0;
    int nMaxBufSize = 0;
    int i = 0;

#define SAVE_ITEM() \
		if (*pdwDevNumber >= nNumMax) \
		{ \
			HERT_LOGDEBUG("*pdwDevNumber(%d) >= nNumMax(%d)", *pdwDevNumber, nNumMax); \
			continue; \
		} \
		if (tDevInfo.szProduct[0] != 0x0 ) \
		{ \
			strcpy(pDev->devName, tDevInfo.szProduct); \
		} \
		if (tDevInfo.szProdType[0] != 0x0 ) \
		{ \
			strcpy(pDev->devType, tDevInfo.szProdType); \
		} \
		if (tDevInfo.szProdSerial[0] != 0x0 ) \
		{ \
			strcpy(pDev->devID, tDevInfo.szProdSerial); \
		} \
		if (tDevInfo.szMac[0] != 0x0 ) \
		{ \
			strcpy(pDev->mac, tDevInfo.szMac); \
		} \
		sprintf(pDev->connectTime, "%ld", tDevInfo.curtime/60); \
		pTempDev = (HERT_COM_SUBDEV_BODY *)((*ppDev) + *pdwDevNumber); \
		DUMP_ITEM(pTempDev); \
		pDev++; \
		*pdwDevNumber = *pdwDevNumber + 1;

#define DUMP_ITEM(pTmpDev) \
		HERT_LOGDEBUG("mac(%s), pTmpDev(%p), pTmpDev->devName(%s),pTmpDev->devType(%s), pTmpDev->devID(%s)",  \
					pTmpDev, pTmpDev->mac, pTmpDev->devName, pTmpDev->devType, pTmpDev->devID);

    // get all clients
    nNumWc = hertUtil_UpdateWirelessClientInfo();
    HERT_LOGDEBUG("wireless client Num(%d)", nNumWc);

    nNumDc = hertUtil_UpdateIpReachInfo(0);
    HERT_LOGDEBUG("dhcp client Num(%d)", nNumDc);

    nNumDoc = hertUtil_UpdateDevInfo();
    HERT_LOGDEBUG("dhcp option60 client Num(%d)", nNumDoc);

    //nNumMax = (nNumWc > nNumDc) ? nNumWc : nNumDc;
    //nNumMax = (nNumMax > nNumDoc) ? nNumMax : nNumDoc;
    // sometimes, the mac wireless client is not in include dhcp clients
    nNumMax = nNumWc + nNumDc;

    *pdwDevNumber = 0;
    if (*ppDev)
    {
        free(*ppDev);
        *ppDev = NULL;
    }
    nMaxBufSize = sizeof(HERT_COM_SUBDEV_BODY) * nNumMax;
    *ppDev = (HERT_COM_SUBDEV_BODY *)malloc(nMaxBufSize);
    if (*ppDev)
    {
        memset(*ppDev, 0x0, nMaxBufSize);
    }
    else if(nMaxBufSize > 0)
    {
        HERT_LOGERR("No memory, nNumMax(%d).\n", nNumMax);
        return 1;
    }
    HERT_LOGDEBUG("malloc memory items: nNumMax(%d)", nNumMax);
    if(nNumMax <= 0)
    {
        return 0; /* nothing to do */
    }

    pDev = *ppDev;

    // add wireless clients
    HERT_LOGDEBUG("add wireless clients, *pdwDevNumber(%d)", *pdwDevNumber);
    for(i = 0; (i < nNumWc) && (nNumWc > 0); i++)
    {
        memset(&tDevInfo, 0x0, sizeof(tDevInfo));
        if(hertUtil_UpdateWirelessClientItem(i, &tDevInfo) < 0)
        {
            continue;
        }

        // update dhcp option60 info
        if(hertUtil_GetDevInfo(tDevInfo.szMac, &tDevInfo) == 0)
        {
            // continue;
        }

        // update dhcp and is reachable info
        if(hertUtil_GetIpReachInfo(tDevInfo.szMac, &tDevInfo) == 0)
        {
            // continue; comment this for it is wireless client, no need to check reachable.
        }

        HERT_LOGDEBUG("add tDevInfo...");
        SAVE_ITEM();
    }

    return ret;
}

static const char *codes = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
   base64 Encode a buffer (NUL terminated)
   @param in      The input buffer to encode
   @param inlen   The length of the input buffer
   @param out     [out] The destination of the base64 encoded data
   @param outlen  [in/out] The max size and resulting size
   @return CRYPT_OK if successful
*/
int hertUtil_base64_encode(const unsigned char *in,  unsigned long inlen, 
                        unsigned char *out, unsigned long *outlen)
{
   unsigned long i, len2, leven;
   unsigned char *p;

   if(in == NULL) return -1;
   if(out == NULL) return -2;
   if(outlen == NULL) return -3;

   /* valid output size ? */
   len2 = 4 * ((inlen + 2) / 3);
   if (*outlen < len2 + 1) {
      *outlen = len2 + 1;
      return -4;
   }
   p = out;
   leven = 3*(inlen / 3);
   for (i = 0; i < leven; i += 3) {
       *p++ = codes[(in[0] >> 2) & 0x3F];
       *p++ = codes[(((in[0] & 3) << 4) + (in[1] >> 4)) & 0x3F];
       *p++ = codes[(((in[1] & 0xf) << 2) + (in[2] >> 6)) & 0x3F];
       *p++ = codes[in[2] & 0x3F];
       in += 3;
   }
   /* Pad it if necessary...  */
   if (i < inlen) {
       unsigned a = in[0];
       unsigned b = (i+1 < inlen) ? in[1] : 0;

       *p++ = codes[(a >> 2) & 0x3F];
       *p++ = codes[(((a & 3) << 4) + (b >> 4)) & 0x3F];
       *p++ = (i+1 < inlen) ? codes[(((b & 0xf) << 2)) & 0x3F] : '=';
       *p++ = '=';
   }

   /* append a NULL byte */
   *p = '\0';

   /* return ok */
   *outlen = p - out;
   return 0;
}

time_t getUptime()
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

#if 1 /* USED FOR DEVLIST PARTS */
int hertUtil_InitSem()
{
#ifdef SUPPORT_SEM
    int val;

    g_sem_finish_read = sem_open(SEM_TASK_HEROUTE_APP, O_CREAT, 0644, 1);
    if (g_sem_finish_read == SEM_FAILED)
    {
        perror("unable to create semaphore");
        HERT_LOGERR("g_sem_finish_read(%p)", g_sem_finish_read);
        sem_unlink(SEM_TASK_HEROUTE_APP);
        return -1;
    }
    g_sem_want_read   = sem_open(SEM_TASK_HEROUTE_PLATFORM, O_CREAT, 0644, 0);
    if (g_sem_finish_read == SEM_FAILED)
    {
        HERT_LOGERR("g_sem_finish_read(%p)", g_sem_finish_read);
        perror("unable to create semaphore");
        return -1;
    }
    return 0;
#endif
    g_esw_fd = socket(AF_INET, SOCK_DGRAM, 0);

    return g_esw_fd;
}

int hertUtil_UnInitSem()
{
#ifdef SUPPORT_SEM
    sem_close(g_sem_finish_read);
    sem_unlink(SEM_TASK_HEROUTE_APP);
#endif
    if(g_esw_fd >= 0)
    {
        close(g_esw_fd);
    }

    return 0;
}

int hertUtil_PostSem()
{
#ifdef SUPPORT_SEM
    sem_post(g_sem_finish_read);
#else
    system("echo g_sem_finish_read > /var/finishread");
#endif
    return 0;
}

int hertUtil_WaitSem(int nSecond)
{
    int result = -1;
    HERT_LOGDEBUG("WaitSem(%d)...", nSecond);
#ifdef SUPPORT_SEM
    struct timespec ts;

    ts.tv_sec = nSecond;
    ts.tv_nsec = 0;
    result = sem_timedwait(g_sem_want_read,&ts);
    if( result == 0 )
    {
        HERT_LOGINFO("will update devlist...", result);
        return 0;
    }
    else
    {
        HERT_LOGDEBUG("will do normal backgroud serve...", result);
        return -1;
    }
#else
    int i = 0;
    for(i = 0; i < nSecond; i++)
    {
        if (hertUtil_IsInFile("/var/wantread","g_sem_want_read"))
        {
            unlink("/var/wantread");
            result = 0;
            break;
        }
        sleep(1);
    }
    return result;
#endif
}
#endif /* USED FOR DEVLIST PARTS */
