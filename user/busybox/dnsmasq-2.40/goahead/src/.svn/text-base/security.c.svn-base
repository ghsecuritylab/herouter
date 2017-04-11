/*
 * security.c -- Security handler
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: //WIFI_SOC/MP/SDK_4_2_0_0/RT288x_SDK/source/user/goahead/src/security.c#1 $
 */

/******************************** Description *********************************/

/*
 *	This module provides a basic security policy.
 */

/********************************* Includes ***********************************/

#include	"nvram.h"
#include	"wsIntrn.h"
#include	"um.h"
#ifdef DIGEST_ACCESS_SUPPORT
#include	"websda.h"
#endif

/********************************** Defines ***********************************/
/*
 *	The following #defines change the behaviour of security in the absence 
 *	of User Management.
 *	Note that use of User management functions require prior calling of
 *	umInit() to behave correctly
 */

#ifndef USER_MANAGEMENT_SUPPORT
#define umGetAccessMethodForURL(url) AM_FULL
#define umUserExists(userid) 0
#define umUserCanAccessURL(userid, url) 1
#define umGetUserPassword(userid) websGetPassword()
#define umGetAccessLimitSecure(accessLimit) 0
#define umGetAccessLimit(url) NULL
#endif


#define HE_ROUTER_REFERER_FLAG  "herouter"

/******************************** Local Data **********************************/

static char_t	websPassword[WEBS_MAX_PASS];	/* Access password (decoded) */
#ifdef _DEBUG
static int		debugSecurity = 1;
#else
static int		debugSecurity = 0;
#endif

/*********************************** Code *************************************/
/*
 *	Determine if this request should be honored
 */

int websSecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
						char_t *url, char_t *path, char_t *query)
{
	char_t			*type, *userid, *password, *accessLimit;
	int				flags, nRet;
	accessMeth_t	am;
	char *needWizard = NULL,*superAdmin = NULL;
	
	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path);
/*
 *	Get the critical request details
 */
	type = websGetRequestType(wp);
	password = websGetRequestPassword(wp);
	userid = websGetRequestUserName(wp);
	flags = websGetRequestFlags(wp);
/*
 *	Get the access limit for the URL.  Exit if none found.
 */
	accessLimit = umGetAccessLimit(path);
	if (accessLimit == NULL) {
		return 0;
	}
		 
/*
 *	Check to see if URL must be encrypted
 */
#ifdef WEBS_SSL_SUPPORT
	nRet = umGetAccessLimitSecure(accessLimit);
	if (nRet && ((flags & WEBS_SECURE) == 0)) {
		websStats.access++;
		websError(wp, 405, T("Access Denied\nSecure access is required."));
		trace(3, T("SEC: Non-secure access attempted on <%s>\n"), path);
      /* bugfix 5/24/02 -- we were leaking the memory pointed to by
       * 'accessLimit'. Thanks to Simon Byholm.
       */
      bfree(B_L, accessLimit);
		return 1;
	}
#endif

/*
 *	Get the access limit for the URL
 */
	am = umGetAccessMethodForURL(accessLimit);

	nRet = 0;

#ifdef DARE_CUSTOMER_WEB
	websStats.isPassAuthWeb = 0;
#if 0
	if(strstr(path,"QuickSetUp.asp")){
		needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
		if (NULL != needWizard)
		{
		  if (!strcmp(needWizard, "1"))	
		  {
		      nvram_bufset(RT2860_NVRAM, "needWizard", "2");
		      nvram_commit(RT2860_NVRAM);
		  }
		}
	}
#endif
	
#endif

	if ((flags & WEBS_LOCAL_REQUEST) && (debugSecurity == 0)) {
/*
 *		Local access is always allowed (defeat when debugging)
 */
	} else if (am == AM_NONE) {
/*
 *		URL is supposed to be hidden!  Make like it wasn't found.
 */
		websStats.access++;
		websError(wp, 404, T("Page Not Found"));
		nRet = 1;
	} else 	if (userid && *userid) {
		if (!umUserExists(userid)) {
			websStats.access++;
#ifdef DARE_CUSTOMER_WEB
			if(((clientip != NULL) && ismobile) && (strcmp(clientip,wp->ipaddr)==0) )
			{
					clientip = NULL;
					ismobile = 0;
					websRedirect(wp, T("herouter/moblogin.asp"));
			}
			else
			{
					printf("######## Unknown User redirect to login.asp\n");
					websRedirect(wp, T("herouter/login.asp"));
			}
#else
			
			websError(wp, 401, T("Access Denied\nUnknown User"));
			trace(3, T("SEC: Unknown user <%s> attempted to access <%s>\n"), 
			userid, path);
#endif
			nRet = 1;
		} else if (!umUserCanAccessURL(userid, accessLimit)) {
			websStats.access++;
#ifdef DARE_CUSTOMER_WEB
		if(((clientip != NULL) && ismobile) && (strcmp(clientip,wp->ipaddr)==0) )
		{
			clientip = NULL;
			ismobile = 0;
			websRedirect(wp, T("herouter/moblogin.asp"));
			
		}
		else
		{
				printf("######## Prohibited redirect to login.asp\n");
				websRedirect(wp, T("herouter/login.asp"));
		}
#else
			websError(wp, 403, T("Access Denied\nProhibited User"));
#endif
			nRet = 1;
		} else if (password && * password) {
			char_t * userpass = umGetUserPassword(userid);
			if (userpass) {
				if (gstrcmp(password, userpass) != 0) {
					websStats.access++;
#ifdef DARE_CUSTOMER_WEB
			if(((clientip != NULL) && ismobile) && (strcmp(clientip,wp->ipaddr)==0) )
			{
				clientip = NULL;
				ismobile = 0;
				websRedirect(wp, T("herouter/moblogin.asp"));
			}
			else
			{
    	    printf("######## Wrong Password redirect to login.asp\n");
          websRedirect(wp, T("herouter/login.asp"));
			}
#else
					websError(wp, 401, T("Access Denied\nWrong Password"));
					trace(3, T("SEC: Password fail for user <%s>")
								T("attempt to access <%s>\n"), userid, path);
#endif
					nRet = 1;
				} else {
#ifdef DARE_CUSTOMER_WEB
					  websStats.isPassAuthWeb = 1;
						needWizard = (char *) nvram_bufget(RT2860_NVRAM, "needWizard");
				    if (NULL != needWizard)
				    {
					    if (!strcmp(needWizard, "0"))	
					    {
					        nvram_bufset(RT2860_NVRAM, "needWizard", "1");
					        nvram_commit(RT2860_NVRAM);
					    }
				    }
#endif
				    //printf("######## pass auth go to default page\n");
				}

				bfree (B_L, userpass);
			}
#ifdef DIGEST_ACCESS_SUPPORT
		} else if (flags & WEBS_AUTH_DIGEST) {

			char_t *digestCalc;

/*
 *			Check digest for equivalence
 */
			wp->password = umGetUserPassword(userid);

			a_assert(wp->digest);
			a_assert(wp->nonce);
			a_assert(wp->password);
							 
			digestCalc = websCalcDigest(wp);
			a_assert(digestCalc);

			if (gstrcmp(wp->digest, digestCalc) != 0) {
				websStats.access++;
            /* 16 Jun 03 -- error code changed from 405 to 401 -- thanks to
             * Jay Chalfant.
             */
				websError(wp, 401, T("Access Denied\nWrong Password"));
				nRet = 1;
			}

			bfree (B_L, digestCalc);
#endif
    } else {
/*
 *			No password has been specified
 */
#ifdef DIGEST_ACCESS_SUPPORT
			if (am == AM_DIGEST) {
				wp->flags |= WEBS_AUTH_DIGEST;
			}
#endif
			websStats.errors++;
#ifdef DARE_CUSTOMER_WEB
    	printf("######## requires a password,redirect to login.asp\n");
      websRedirect(wp, T("herouter/login.asp"));
#else
			websError(wp, 401, 
				T("Access to this document requires a password"));
#endif
			nRet = 1;
		}
		
	} else if (am != AM_FULL) {
/*
 *		This will cause the browser to display a password / username
 *		dialog
 */
#ifdef DIGEST_ACCESS_SUPPORT
		if (am == AM_DIGEST) {
			wp->flags |= WEBS_AUTH_DIGEST;
		}
#endif

		websStats.errors++;
#ifdef DARE_CUSTOMER_WEB
    superAdmin = (char *) nvram_bufget(RT2860_NVRAM, "SUPER_ADMIN");
    if((wp->path != NULL) && (gstrcmp(wp->path,"/") == 0)){
        if(((clientip != NULL) && ismobile) && (strcmp(clientip,wp->ipaddr)==0) )
        {
            printf("is mobile redirect to moblogin.asp line ==%d\n",__LINE__);
            clientip = NULL;
            ismobile = 0;
            websRedirect(wp, T("herouter/moblogin.asp"));
			
        }
        else
        {
            printf("######## page(%s)need redirect to login.asp\n",wp->path);
            websRedirect(wp, T("herouter/login.asp"));
        }
        nRet = 1;
    }
    else if((wp->path != NULL) && ((strstr(wp->path, "login.asp") != 0) || (strstr(wp->path, "moblogin.asp") != 0))){
    	/*do nothing for login.asp*/
  	    printf("######## open login.asp\n");
  	    websStats.errors--;
    }
    else if(strcmp(superAdmin,"OPEN")==0)
    {
    	  /*if open SUPER ADMIN mode,ervery page can access directly(include source page),no need auth*/
    }
   // else if((wp->path != NULL) && (strstr(wp->path, ".asp") != 0))
    else if((wp->path != NULL) )
    {
        if((flags & WEBS_REFERER) && strstr(wp->referer,HE_ROUTER_REFERER_FLAG))
        {
		        /*this request is from main page or other vaild pages,so access directly*/	
        }
        else
        {
        	  /*if want skip the auth will be direct to login.asp*/
		        if(((clientip != NULL) && ismobile) && (strcmp(clientip,wp->ipaddr)==0) )
		        {
		            printf("is mobile redirect to moblogin.asp line ==%d\n",__LINE__);
		            clientip = NULL;
		            ismobile = 0;
		            websRedirect(wp, T("herouter/moblogin.asp"));
					
		        }
		        else
		        {
		            printf("######## no-referer page(%s)need redirect to login.asp\n",wp->path);
		            websRedirect(wp, T("herouter/login.asp"));
		        }
		        nRet = 1;
        }	
    }
    //else if(/home.asp)
#else
		websError(wp, 401, T("Access to this document requires a User ID"));
		nRet = 1;
#endif

	}
  
	bfree(B_L, accessLimit);

	return nRet;
}

/******************************************************************************/
/*
 *	Delete the default security handler
 */

void websSecurityDelete()
{
	websUrlHandlerDelete(websSecurityHandler);
}

/******************************************************************************/
/*
 *	Store the new password, expect a decoded password. Store in websPassword in 
 *	the decoded form.
 */

void websSetPassword(char_t *password)
{
	a_assert(password);

	gstrncpy(websPassword, password, TSZ(websPassword));
}

/******************************************************************************/
/*
 *	Get password, return the decoded form
 */

char_t *websGetPassword()
{
	return bstrdup(B_L, websPassword);
}

/******************************************************************************/



#ifdef DARE_CUSTOMER_WEB
/*the pages after "home.asp" add for test,all these should be remove after test.*/
int isFreePages(const char *page)
{
	
	int i = 0;
	char *HeFreePagesList[] = {NULL};

#if 0
	char *HeFreePagesList[] = {	
	"/herouter/login.asp",
	"/herouter/main_fake.asp",
	"/herouter/wifisetting1.asp",
	"/herouter/sysstate.asp",
	"/herouter/intsetting1.asp",
	"/herouter/devicemag.asp",
	"/herouter/intconfig.asp",
	"/herouter/netdiags.asp",
	"/herouter/wirelessMAC.asp",
	"/herouter/entconfig.asp",
	"/herouter/DHCPbanding.asp",
	"/herouter/MACClone.asp",
	"/herouter/wirelesschannel.asp",
	"/herouter/systemtime.asp",
	"/herouter/changepassword.asp",
	"/herouter/restoredefault.asp",
	"/herouter/sysupdate.asp",
	"/herouter/mainformobile.asp",
	"/herouter/mobchangepwd.asp",
	"/herouter/mobintsetting.asp",
	"/herouter/moblogin.asp",
	"/herouter/mobwifisetting.asp",
	"/herouter/mobilePPPoE.asp",
	"/herouter/main.asp",
	"/herouter/PPPoE.asp",
	"/herouter/QuickSetUp.asp",
	"/herouter/QuickSetwifi.asp",
	"/home.asp",
	"/treeapp.asp",
	"/overview.asp",
	"/adm/upload_firmware.asp",
	"/adm/status.asp",
	"/adm/settings.asp",
	"/wireless/basic.asp",
	"/wireless/advince.asp",
	"/wireless/security.asp",
	"/wireless/apstatistics.asp",
	"/wireless/stainfo.asp",
	"/internet/wan.asp",
	"/jsonrequest.asp",
	"/wps/wps.asp",
	NULL
  };
#endif

  if (page == NULL)
		return 0;
	 
  while (HeFreePagesList[i] != NULL)
  {
 	  if (0 == gstrcmp (HeFreePagesList[i], page))
 		  return 1;
 	  i++;
   }
   
  return 0;
  //return 1;
}
#endif

