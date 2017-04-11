
#ifndef __HE_ROUTE_API_HEADER__
#define __HE_ROUTE_API_HEADER__

void HERT_APP_OnSignal(int i);
RETCODE HERT_APP_INIT();
RETCODE Msg_Process_Entry(IN CHAR *ptMsg, IN DWORD nMsgDataLen);
RETCODE Msg_Process_Internel();
RETCODE Msg_Process_Internel_SendPushInitReq();
RETCODE Msg_Process_Internel_SendConnReq();
RETCODE Msg_Process_Internel_SendHeartbeatReq();

void Msg_Process_PushData_DownLoad_Thread(void * arg);

RETCODE Msg_Process_ConnResp(HERT_MSG_OPTION_CONN_RSP *ptMsgOption);

RETCODE Msg_Process_PushData(IN CHAR *ptMsg, IN DWORD nMsgDataLen);

RETCODE Msg_Process_PushData_InitInfoRsp(IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_AbilityNotifyRsp(IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_RouterStatusNotifyRsp(IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_Internel_SendPushAbiNtfReq();

RETCODE Msg_Process_PushData_RouterStatusReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody,int isSendRequest);

RETCODE Msg_Process_PushData_RouterSpaceReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_DownLoadImgReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_DownloadCompNotifyRsp(IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_GetUpdateInfoRsp(IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_AddDownloadMissionReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_DeviceOperateReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody);

RETCODE Msg_Process_PushData_UnknowDataReq(HERT_MSG_OPTION_PUSH_DATA *ptMsgOption, IN CHAR *ptMsgCnstBody);

#endif /* __HE_ROUTE_API_HEADER__ */

