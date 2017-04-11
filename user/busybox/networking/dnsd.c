/* vi: set sw=4 ts=4: */
/*
 * Mini DNS server implementation for busybox
 *
 * Copyright (C) 2005 Roberto A. Foglietta (me@roberto.foglietta.name)
 * Copyright (C) 2005 Odd Arild Olsen (oao at fibula dot no)
 * Copyright (C) 2003 Paul Sheer
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 *
 * Odd Arild Olsen started out with the sheerdns [1] of Paul Sheer and rewrote
 * it into a shape which I believe is both easier to understand and maintain.
 * I also reused the input buffer for output and removed services he did not
 * need.  [1] http://threading.2038bug.com/sheerdns/
 *
 * Some bugfix and minor changes was applied by Roberto A. Foglietta who made
 * the first porting of oao' scdns to busybox also.
 */

#include "libbb.h"
#include <syslog.h>
#include <net/if.h>

#define DEBUG_LOG(title) \
	if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG")) \
	{ \
		printf("[%s,%d]:%s\n", __FUNCTION__, __LINE__, title);  \
	}

//#define DEBUG 1
#define DEBUG 0

enum {
	MAX_HOST_LEN = 16,      // longest host name allowed is 15
	IP_STRING_LEN = 18,     // .xxx.xxx.xxx.xxx\0

//must be strlen('.in-addr.arpa') larger than IP_STRING_LEN
	MAX_NAME_LEN = (IP_STRING_LEN + 13),

/* Cannot get bigger packets than 512 per RFC1035
   In practice this can be set considerably smaller:
   Length of response packet is  header (12B) + 2*type(4B) + 2*class(4B) +
   ttl(4B) + rlen(2B) + r (MAX_NAME_LEN =21B) +
   2*querystring (2 MAX_NAME_LEN= 42B), all together 90 Byte
*/
	MAX_PACK_LEN = 512,

	DEFAULT_TTL = 30,       // increase this when not testing?

	REQ_A = 1,
	REQ_PTR = 12
};

struct dns_head {		// the message from client and first part of response mag
	uint16_t id;
	uint16_t flags;
	uint16_t nquer;		// accepts 0
	uint16_t nansw;		// 1 in response
	uint16_t nauth;		// 0
	uint16_t nadd;		// 0
};
struct dns_prop {
	uint16_t type;
	uint16_t class;
};
struct dns_entry {		// element of known name, ip address and reversed ip address
	struct dns_entry *next;
	char ip[IP_STRING_LEN];		// dotted decimal IP
	char rip[IP_STRING_LEN];	// length decimal reversed IP
	char name[MAX_HOST_LEN];
};

static struct dns_entry *dnsentry;
static uint32_t ttl = DEFAULT_TTL;

static const char *fileconf = "/etc/dnsd.conf";

// Must match getopt32 call
#define OPT_daemon  (option_mask32 & 0x10)
#define OPT_verbose (option_mask32 & 0x20)


/*
 * Convert host name from C-string to dns length/string.
 */
static void convname(char *a, uint8_t *q)
{
	int i = (q[0] == '.') ? 0 : 1;
	for (; i < MAX_HOST_LEN-1 && *q; i++, q++)
		a[i] = tolower(*q);
	a[0] = i - 1;
	a[i] = 0;
}

/*
 * Insert length of substrings instead of dots
 */
static void undot(uint8_t * rip)
{
	int i = 0, s = 0;
	while (rip[i])
		i++;
	for (--i; i >= 0; i--) {
		if (rip[i] == '.') {
			rip[i] = s;
			s = 0;
		} else s++;
	}
}

/*
 * Read hostname/IP records from file
 */
static void dnsentryinit(void)
{
	char *token[2];
	parser_t *parser;
	struct dns_entry *m, *prev;

	prev = dnsentry = NULL;
	parser = config_open(fileconf);
	while (config_read(parser, token, 2, 2, "# \t", PARSE_NORMAL)) {
		unsigned a, b, c, d;
		/*
		 * Assumes all host names are lower case only
		 * Hostnames with more than one label are not handled correctly.
		 * Presently the dot is copied into name without
		 * converting to a length/string substring for that label.
		 */
//		if (!token[1] || sscanf(token[1], ".%u.%u.%u.%u"+1, &a, &b, &c, &d) != 4)
		if (sscanf(token[1], ".%u.%u.%u.%u"+1, &a, &b, &c, &d) != 4)
			continue;

		m = xzalloc(sizeof(*m));
		/*m->next = NULL;*/
		sprintf(m->ip, ".%u.%u.%u.%u"+1, a, b, c, d);
		sprintf(m->rip, ".%u.%u.%u.%u", d, c, b, a);
		undot((uint8_t*)m->rip);
		convname(m->name, (uint8_t*)token[0]);

		if (OPT_verbose)
			fprintf(stderr, "\tname:%s, ip:%s\n", &(m->name[1]), m->ip);

		if (prev == NULL)
			dnsentry = m;
		else
			prev->next = m;
		prev = m;
	}
	config_close(parser);
}
void DNSD_Dump_HexData(uint8_t *buf, char *pszFunc, int nLine)
{
    int i = 0;
    int iLength = strlen(buf);
    char * p = buf;
    char szLogBuf[128] = {0x0};

    printf("\n[%s,%d]: ==================================\n", pszFunc, nLine);
    memset(szLogBuf, 0x0, sizeof(szLogBuf));
    for(i = 0; i < 16; i++)
    {
        sprintf(szLogBuf + strlen(szLogBuf), "%02x ",i);
    }
    printf("[%s,%d]: %s\n", __FUNCTION__, __LINE__, szLogBuf);

    i = 0;
    memset(szLogBuf, 0x0, sizeof(szLogBuf));
    while(i < iLength)
    {
        sprintf(szLogBuf + strlen(szLogBuf), "%02x ",(unsigned char)*(p));
        if ((i%16 == 0x0f) && (i != 0))
        {
            printf("[%s,%d]: %s\n", __FUNCTION__, __LINE__, szLogBuf);
            memset(szLogBuf, 0x0, sizeof(szLogBuf));
        }
        p++;
        i++;
    }
    if (*szLogBuf != 0x0)
    {
        printf("[%s,%d]: %s\n", __FUNCTION__, __LINE__, szLogBuf);
    }
    printf("[%s,%d]: \n", __FUNCTION__, __LINE__);
}
#define DEFAULT_LAN_IP "192.168.1.1"

#define REMOVE_CRLN(data) \
        if( (*(data + strlen(data) - 1) == 0x0a) || \
            (*(data + strlen(data) - 1) == 0x0d) ) \
        { \
            *(data + strlen(data) - 1) = 0x0; \
        } \
        if( (*(data + strlen(data) - 2) == 0x0a) || \
            (*(data + strlen(data) - 2) == 0x0d) ) \
        { \
            *(data + strlen(data) - 2) = 0x0; \
        }

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

    if(nl = strchr(buf, '\n'))
    {
        *nl = '\0';
    }

    return buf;

error:
    printf("warning, cant find lan ip!!\n");
    return DEFAULT_LAN_IP;
}
extern int hertUtil_IsInFile(const char *szFile, const char *szfindstr);

unsigned short hertUtil_getDnsdDebug()
{
    FILE *file;
    static char DnsdDebug[64];
    char cmd[128];
    const char *tempFileName = "/var/tempDnsdDebug.txt";

    memset(DnsdDebug, 0x0, sizeof(DnsdDebug));
    sprintf(cmd,"nvram_get HE_ROUTE_DNSDDEBUG > %s", tempFileName);
    system(cmd);	
    if (!(file = fopen(tempFileName, "r"))) 
    {
        printf("unable to open config file: %s\n",tempFileName);
        return atoi(DnsdDebug);
    }   
    if (fgets(DnsdDebug, 64, file) && (strlen(DnsdDebug) >= 1) ) 
    {
    }    
    fclose(file);
    unlink(tempFileName);	
    return atoi(DnsdDebug);
}

int hertUtil_getOperationMode()
{
    FILE *file;
    static char OpMode[64];
    char cmd[128];
    const char *tempFileName = "/var/tempOpMode.txt";

    memset(OpMode, 0x0, sizeof(OpMode));
    sprintf(cmd,"nvram_get OperationMode > %s", tempFileName);
    system(cmd);
    if (!(file = fopen(tempFileName, "r"))) 
    {
        printf("unable to open config file: %s\n",tempFileName);
        return 0; /* default for 0 */
    }   
    if (fgets(OpMode, 64, file) && (strlen(OpMode) >= 1) ) 
    {
        /* remove 0x0d,0x0a if there is */
        REMOVE_CRLN(OpMode);
    }    
    fclose(file);
    unlink(tempFileName);

    return atoi(OpMode);
}

static int hertUtil_Port_read(int offset, int *value,char *portStatus)
{
	struct ifreq ifr;
	static int esw_fd = 0;
#define RAETH_PORTSTATE_READ  0x89FC

	if(esw_fd <= 0)
	{	
		esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (esw_fd <= 0) 
		{
			perror("socket");
			return -1;
		}
	}
	
	if (value == NULL)
		return -1;
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = portStatus;
	if (-1 == ioctl(esw_fd, RAETH_PORTSTATE_READ, &ifr)) 
	{
		perror("ioctl");
		close(esw_fd);
		return -1;
	}
	return 0;
}

int fomart_data(uint8_t *src, uint8_t *dst, int nbufLen)
{
    int nItemLen = 0;
    int nsrcLen = 0;
    int ncurLen = 0;
    int i = 0;
    uint8_t *ptsrc;
    uint8_t *ptdst;
    if (!src || !dst)
    {
        return -1;
    }
    memset(dst, 0x0, nbufLen);

    ptsrc = src;
    ptdst = dst;
    nsrcLen = strlen(src);
    while( (ptsrc <= src + nsrcLen) && (nsrcLen - (ptsrc - src) >= *ptsrc) )
    {
        nItemLen = *ptsrc;
        ncurLen +=nItemLen;
        if ( nItemLen >= 64 || nbufLen <= ncurLen)
        {
            printf("[%s,%d]:----------- nItemLen(%d), ncurLen(%d) ----------------\n", __FUNCTION__, __LINE__, nItemLen, ncurLen);  
            return -2;
        }

        ptsrc = ptsrc + 1; /* point to data */
        strncpy(ptdst, ptsrc, nItemLen);
        ptsrc = ptsrc + nItemLen;/* point to data-length */
        if (*ptsrc > 0 )
        {
            strcat(ptdst, ".");
        }
        ptdst += strlen(ptdst);
        if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
        {
            printf("[%s,%d]:----------- dst(%s) ----------------\n", __FUNCTION__, __LINE__, dst);  
        }
    }
    if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
    {
        printf("[%s,%d]:----------- dst(%s) ----------------\n", __FUNCTION__, __LINE__, dst);  
    }
    return 0;
}

/*
 * Look query up in dns records and return answer if found
 * qs is the query string, first byte the string length
 */
static int table_lookup(uint16_t type, uint8_t * as, uint8_t * qs)
{
	int i;
	struct dns_entry *d = dnsentry;
	char *str = NULL;

	do {
#if DEBUG
		char *p,*q;
		q = (char *)&(qs[1]);
		p = &(d->name[1]);
		fprintf(stderr, "\n%s: %d/%d p:%s q:%s %d",
			__FUNCTION__, (int)strlen(p), (int)(d->name[0]),
			p, q, (int)strlen(q));
#endif
		if (type == REQ_A) { /* search by host name */
/*			
			for (i = 1; i <= (int)(d->name[0]); i++)
				if (tolower(qs[i]) != d->name[i])
					break;
*/
			DEBUG_LOG("================0==================\n");
			
		str=strstr(qs,"www");
		if(str != NULL)
		{
			str+=4;
			if(!strncmp(str,"heluyou",7) && strlen(str)==11)
			{
				str+=8;
				printf("check step 2\n");	
				if(!strncmp(str,"com",3))	
				{	
					//strcpy((char *)as, "192.168.1.1");
					strcpy((char *)as, hertUtil_getLanIP());
					if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
					{
						printf("this is heroute hostname str(%d)\n",strlen(str));
					}
					return 0;
				}
				else
				{
					if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
					{
						printf("not heroute hostname str is %s:%d...\n",str,strlen(str));
					}
				}
			}
			else
			{
				if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
				{
					printf("check step 2 false str is %s:len=%d\n",str,strlen(str));	
				}
			}	
		}
		else
		{
			if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
			{
				printf("no www str is %s\n",str);
			}
		}

if (hertUtil_getOperationMode() != 2 ) /* it is STA mode if 2 */
{
		char portStatus[4] = {0};   
		int  value;
		#define REG_ESW_VLAN_ID_BASE		0x50
  
		hertUtil_Port_read(REG_ESW_VLAN_ID_BASE, &value,portStatus);
		if(strlen(portStatus) > 0)
		{
			if(portStatus[0] != 'u')
			{
				DEBUG_LOG("---------WAN PORT DOWN-------------\n");  
				return 0;
			}
		}
}
		if ( hertUtil_IsInFile("/var/wannetpingsts", "failed"))
		{
			DEBUG_LOG("---------INTERNET NOT PASS-------------\n");  
			return 0;
		}
			
		/* it is not www.heluyou.com */
		struct hostent * he;
		struct sockaddr_in host;
		if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
		{
			printf("[%s,%d]:-----------parse1111... ----------------\n", __FUNCTION__, __LINE__);  
			DNSD_Dump_HexData(qs, __FUNCTION__, __LINE__);
		}

		uint8_t outdata[256];
		int nOutBufLen = sizeof(outdata);
		char *pstr = NULL;

		fomart_data(qs, outdata, nOutBufLen);

		DEBUG_LOG("-----------parse2222... ----------------");  

		he = gethostbyname(outdata);
		if(he && he->h_addr_list[0])
		{
			host.sin_addr.s_addr = *((int*)(he->h_addr_list[0]));
			pstr = inet_ntoa(host.sin_addr);
			if (pstr)
			{
				strcpy((char *)as, pstr);
			}
			if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
			{
				printf("-----------dnsd: %s, as(%s) 0 ----------------\n",pstr ? pstr : "NULL", as);  
			}
			return 0;
		}
		else
		{
			// do nothing if failed to get the ip
			//host.sin_addr.s_addr = inet_addr(qs);
			if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
			{
				printf("[%s,%d]:-----------dnsd: qs(%s), no return ----------------\n", __FUNCTION__, __LINE__, qs);  
			}
			return -1;
		}
/*
		if(!strncmp(str,"heroute",7) && strlen(str)==11)	
		{
			str+=8;
			printf("check step 2\n");	
		}else
		{
			printf("check step 2 false str is %s:len=%d\n",str,strlen(str));	
		}			
		if(!strncmp(str,"com",3))	
		{	
			strcpy((char *)as, "10.10.10.254");
			printf("this is heroute hostname str(%d)\n",strlen(str));
			return 0;
		}else
			printf("not heroute hostname str is %s:%d...\n",str,strlen(str));
*/

/*			
			if (i > (int)(d->name[0]) ||
			    (d->name[0] == 1 && d->name[1] == '*')) {
				strcpy((char *)as, d->ip);
*/
#if DEBUG
				fprintf(stderr, " OK as:%s\n", as);
#endif
		} else if (type == REQ_PTR) { /* search by IP-address */
			if ((d->name[0] != 1 || d->name[1] != '*') &&
			    !strncmp((char*)&d->rip[1], (char*)&qs[1], strlen(d->rip)-1)) {
				strcpy((char *)as, d->name);
				return 0;
			}
		}
		d = d->next;
	} while (d);
	return -1;
}


/*
 * Decode message and generate answer
 */
static int process_packet(uint8_t *buf)
{
	uint8_t answstr[MAX_NAME_LEN + 1];
	struct dns_head *head;
	struct dns_prop *qprop;
	uint8_t *from, *answb;
	uint16_t outr_rlen;
	uint16_t outr_flags;
	uint16_t flags;
	int lookup_result, type, packet_len;
	int querystr_len;

	answstr[0] = '\0';

	head = (struct dns_head *)buf;
	if (head->nquer == 0) {
		bb_error_msg("no queries");
		return -1;
	}

	if (head->flags & 0x8000) {
		bb_error_msg("ignoring response packet");
		return -1;
	}

	from = (void *)&head[1];	//  start of query string
//FIXME: strlen of untrusted data??!
	querystr_len = strlen((char *)from) + 1 + sizeof(struct dns_prop);
	answb = from + querystr_len;   // where to append answer block

	outr_rlen = 0;
	outr_flags = 0;

	qprop = (struct dns_prop *)(answb - 4);
	type = ntohs(qprop->type);

	// only let REQ_A and REQ_PTR pass
	if (!(type == REQ_A || type == REQ_PTR)) {
		goto empty_packet;	/* we can't handle the query type */
	}

	if (ntohs(qprop->class) != 1 /* class INET */ ) {
		outr_flags = 4; /* not supported */
		goto empty_packet;
	}
	/* we only support standard queries */

	if ((ntohs(head->flags) & 0x7800) != 0)
		goto empty_packet;

	// We have a standard query
	if ( hertUtil_IsInFile("/var/dnsddebug", "DEBUG"))
	{
		bb_info_msg("[%s,%d]:%s", __FUNCTION__, __LINE__, (char *)from);
	}

	lookup_result = table_lookup(type, answstr, from);
	if (lookup_result != 0) {
		outr_flags = 3 | 0x0400;	// name do not exist and auth
		goto empty_packet;
	}
	if (type == REQ_A) {    // return an address
		struct in_addr a; // NB! its "struct { unsigned __long__ s_addr; }"
		uint32_t v32;
		if (!inet_aton((char*)answstr, &a)) { //dotted dec to long conv
			outr_flags = 1; /* Frmt err */
			goto empty_packet;
		}
		v32 = a.s_addr; /* in case long != int */
		memcpy(answstr, &v32, 4);
		outr_rlen = 4;			// uint32_t IP
	} else
		outr_rlen = strlen((char *)answstr) + 1;	// a host name
	outr_flags |= 0x0400;			/* authority-bit */
	// we have an answer
	head->nansw = htons(1);

	// copy query block to answer block
	memcpy(answb, from, querystr_len);
	answb += querystr_len;

	// and append answer rr
// FIXME: unaligned accesses??
	*(uint32_t *) answb = htonl(ttl);
	answb += 4;
	*(uint16_t *) answb = htons(outr_rlen);
	answb += 2;
	memcpy(answb, answstr, outr_rlen);
	answb += outr_rlen;

 empty_packet:

	flags = ntohs(head->flags);
	// clear rcode and RA, set responsebit and our new flags
	flags |= (outr_flags & 0xff80) | 0x8000;
	head->flags = htons(flags);
	head->nauth = head->nadd = 0;
	head->nquer = htons(1);

	packet_len = answb - buf;
	return packet_len;
}

/*
 * Exit on signal
 */
static void interrupt(int sig)
{
	/* unlink("/var/run/dnsd.lock"); */
	bb_error_msg("interrupt, exiting\n");
	kill_myself_with_sig(sig);
}

int dnsd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int dnsd_main(int argc UNUSED_PARAM, char **argv)
{
	const char *listen_interface = "0.0.0.0";
	char *sttl, *sport;
	len_and_sockaddr *lsa, *from, *to;
	unsigned lsa_size;
	int udps;
	uint16_t port = 53;
	/* Paranoid sizing: querystring x2 + ttl + outr_rlen + answstr */
	/* I'd rather see process_packet() fixed instead... */
	uint8_t buf[MAX_PACK_LEN * 2 + 4 + 2 + (MAX_NAME_LEN+1)];

	if (hertUtil_getDnsdDebug())
	{
		system("echo DEBUG > /var/dnsddebug"); 
	}

	getopt32(argv, "i:c:t:p:dv", &listen_interface, &fileconf, &sttl, &sport);
	//if (option_mask32 & 0x1) // -i
	//if (option_mask32 & 0x2) // -c
	if (option_mask32 & 0x4) // -t
		ttl = xatou_range(sttl, 1, 0xffffffff);
	if (option_mask32 & 0x8) // -p
		port = xatou_range(sport, 1, 0xffff);

	if (OPT_verbose) {
		bb_info_msg("listen_interface: %s", listen_interface);
		bb_info_msg("ttl: %d, port: %d", ttl, port);
		bb_info_msg("fileconf: %s", fileconf);
	}

	if (OPT_daemon) {
		bb_daemonize_or_rexec(DAEMON_CLOSE_EXTRA_FDS, argv);
		openlog(applet_name, LOG_PID, LOG_DAEMON);
		logmode = LOGMODE_SYSLOG;
	}

	dnsentryinit();

	signal(SIGINT, interrupt);
	bb_signals(0
		/* why? + (1 << SIGPIPE) */
		+ (1 << SIGHUP)
#ifdef SIGTSTP
		+ (1 << SIGTSTP)
#endif
#ifdef SIGURG
		+ (1 << SIGURG)
#endif
		, SIG_IGN);

	lsa = xdotted2sockaddr(listen_interface, port);
	udps = xsocket(lsa->u.sa.sa_family, SOCK_DGRAM, 0);
	xbind(udps, &lsa->u.sa, lsa->len);
	socket_want_pktinfo(udps); /* needed for recv_from_to to work */
	lsa_size = LSA_LEN_SIZE + lsa->len;
	from = xzalloc(lsa_size);
	to = xzalloc(lsa_size);

	bb_info_msg("Accepting UDP packets on %s",
			xmalloc_sockaddr2dotted(&lsa->u.sa));

	while (1) {
		int r;
		/* Try to get *DEST* address (to which of our addresses
		 * this query was directed), and reply from the same address.
		 * Or else we can exhibit usual UDP ugliness:
		 * [ip1.multihomed.ip2] <=  query to ip1  <= peer
		 * [ip1.multihomed.ip2] => reply from ip2 => peer (confused) */
		memcpy(to, lsa, lsa_size);
		r = recv_from_to(udps, buf, MAX_PACK_LEN + 1, 0, &from->u.sa, &to->u.sa, lsa->len);
		if (r < 12 || r > MAX_PACK_LEN) {
			bb_error_msg("invalid packet size");
			continue;
		}
		if (OPT_verbose)
			bb_info_msg("Got UDP packet");
		buf[r] = '\0'; /* paranoia */
		r = process_packet(buf);
		if (r <= 0)
			continue;
		send_to_from(udps, buf, r, 0, &from->u.sa, &to->u.sa, lsa->len);
	}
	return 0;
}
