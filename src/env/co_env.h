#ifndef _CO_REACTOR_H_
#define _CO_REACTOR_H_
#include<string>

#include"co_params.h"
#include"DBpool.h"
#include<memory>
#include<jsoncpp/json/json.h>
#include"co_routine.h"
#include"co_socktool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include"CDetectModel.h"
#include"RelationModel.h"
#include"UserModel.h"
#include"MessageModel.h"
#include"SessionModel.h"
#include<tuple>
#include<map>
#define BUFFER_LENGTH		1024*1024
class dataHeader;
using namespace  std;
enum
{
    //face
    NOOPERATION = 0,
    ADDFACE ,
    GETGROUP,
    GETUSER,
    DELUSER,
    DELGROUP,
//------------------------------------
    //login
    LOGIN,
    //login_res
    LOGIN_RES,
    //signal
    LOGIN_SUCCESS,
    LOGIN_NOUSER,
    LOGIN_ERRPASSWORD,
//------------------------------------
    //REGISTER
    REGISTER_RES,
    //signal
    REGISTER_SUCCESS,
    REGISTER_USER_ALREADY_EXIST,
//------------------------------------
    DETECT,
    //DETECT
    DETECT_RES,
    //signal
    DETECT_SUCCESS,
    DETECT_NOUSER,
//------------------------------------
    MESSAGE_CREATE,
    MESSAGE_CREATE_RES,
    MESSAGE_CREATE_SUCCESS,
    MESSAGE_CREATE_FAILED,
//------------------------------------
    ONLINE,
    ONLINE_RES,
    ONLINE_SUCCESS,
    ONLINE_FAILED,
//------------------------------------
    OFFLINE,
    OFFLINE_RES,
    OFFLINE_SUCCESS,
    OFFLIBE_FAILED,
//------------------------------------
    CHECK_ISONLINE,
    CHECK_ISONLINE_RES,
    CHECK_ISONLINE_SUCCESS,
    CHECK_ISONLINE_FAILED,
//------------------------------------
    MESSAGE_SEND,
    MESSAGE_SEND_RES,
    MESSAGE_SEND_SUCCESS,
    MESSAGE_SEND_FAILED,
//------------------------------------
    MESSAGE_HISTORY,
    MESSAGE_HISTORY_RES,
    MESSAGE_HISTORY_COUNT_RES,
    MESSAGE_HISTORY_SUCCESS,
    MESSAGE_HISTORY_FAILED,
//------------------------------------
    MESSAGR_RECENT_SESSION,
    MESSAGR_RECENT_SESSION_RES,
    MESSAGR_RECENT_SESSION_SUCCESS,
    MESSAGR_RECENT_SESSION_FAILED,

//------------------------------------
    LOGIN_TOUXIANG,
    LOGIN_TOUXIANG_RES,
    LOGIN_TOUXIANG_SUCCESS,
    LOGIN_TOUXIANG_FAILED,
//------------------------------------
    CHANGE_TOUXIANG,
    CHANGE_TOUXIANG_RES,
    CHANGE_TOUXIANG_SUCCESS,
    CHANGE_TOUXIANG_FAILED,
//------------------------------------
    GET_TOUXIANG,
    GET_TOUXIANG_RES,
    GET_TOUXIANG_SUCCESS,
    GET_TOUXIANG_FAILED
};
struct stEnv_t { // fd
    stEnv_t()
    {
        rbuffer=(unsigned char *)malloc(BUFFER_LENGTH*sizeof (unsigned char));
        msgBuffer=(unsigned char *)malloc(8*BUFFER_LENGTH*sizeof (unsigned char));
        msgLength=0;
        memset(rbuffer,0,BUFFER_LENGTH);
        memset(msgBuffer,0,8*BUFFER_LENGTH);
    }
    int fd;
    stCoRoutine_t *co;
    unsigned char *rbuffer;
    unsigned char *msgBuffer;
    int msgLength;
};
class Event
{
public:
    Event();
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap) = 0;
    virtual ~Event(){}


};

class addFaceEvent : public Event
{
public:
    addFaceEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
private:

};

class getGroupListEvent : public Event
{
public:
    getGroupListEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);

private:
};

class getuserListEvent : public Event
{
public:
    getuserListEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);

private:
};

class delGroupEvent : public Event
{
public:
    delGroupEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);

private:
};

class delUserEvent : public Event
{
public:
    delUserEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);

private:
};

class detectFaceEvent : public Event
{
public:
    detectFaceEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~detectFaceEvent(){}
private:

};

class checkOnlineEvent : public Event
{
public:
    checkOnlineEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~checkOnlineEvent(){}
private:

};


class LoginUserEvent :public Event
{
public:
    LoginUserEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~LoginUserEvent(){}
private:

};

class LoginAddFaceEvent : public Event
{
public:
    LoginAddFaceEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~LoginAddFaceEvent(){}
private:

};

class LoginOnlineEvent : public Event
{
public:
    LoginOnlineEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~LoginOnlineEvent(){}
private:

};

class LoginOfflineEvent : public Event
{
public:
    LoginOfflineEvent(){}
    virtual int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~LoginOfflineEvent(){}
private:

};

class MessageCreateEvent :public Event
{
public:
    MessageCreateEvent(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~MessageCreateEvent(){}
};

class MessageSendEvent : public Event
{
public:
    MessageSendEvent(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~MessageSendEvent(){}
};

class MessageHistoryEvent : public Event
{
public:
    MessageHistoryEvent(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~MessageHistoryEvent(){}
};

class RescentSessionEvent : public Event
{
public:
    RescentSessionEvent(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~RescentSessionEvent(){}
};

class LoginTouXiang : public Event
{
public:
    LoginTouXiang(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~LoginTouXiang(){}
};
class ChangeTouXiang : public Event
{
public:
    ChangeTouXiang(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~ChangeTouXiang(){}
};

class GetTouXiang : public Event
{
public:
    GetTouXiang(){}
    int doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap);
    virtual ~GetTouXiang(){}
};

class dataHeader
{
public:
    dataHeader(){}
//protected:
    int m_cmd;
    int m_dataLength;
};

class addFaceData : public dataHeader
{
public:
    addFaceData()//初始化包头
    {
        this->m_cmd = ADDFACE;
        this->m_dataLength = sizeof(addFaceData);
        memset(m_classnum,0,sizeof(m_classnum));
        memset(m_account,0,sizeof(m_account));
        memset(m_password,0,sizeof(m_password));
        memset(m_facedata,0,sizeof(m_facedata));
        memset(m_touxiang,0,sizeof(m_touxiang));
    }
    char m_classnum[20];
    char m_account[20];
    char m_password[20];
    char m_facedata[1024*1024];
    int m_cols;
    int m_rows;
    int m_type;
    char m_touxiang[50];
    
};

class getGroupData : public dataHeader
{
public:
    getGroupData()//初始化包头
    {
        this->m_cmd = GETGROUP;
        this->m_dataLength = sizeof(getGroupData);
    }
    //char m_group_id[20];

};

class getUserData : public dataHeader
{
public:
    getUserData()//初始化包头
    {
        this->m_cmd = GETUSER;
        this->m_dataLength = sizeof(getUserData);
    }
    char m_group_id[20];

};

class delUserData : public dataHeader
{
public:
    delUserData()//初始化包头
    {
        this->m_cmd = DELUSER;
        this->m_dataLength = sizeof(delUserData);
        memset(m_group_id,0,sizeof (m_group_id));
        memset(m_user_id,0,sizeof (m_user_id));
    }
    char m_group_id[20];
    char m_user_id[20];
};

class delGroupData : public dataHeader
{
public:
    delGroupData()//初始化包头
    {
        this->m_cmd = DELGROUP;
        this->m_dataLength = sizeof(delGroupData);
        memset(m_group_id,0,sizeof (m_group_id));
    }
    char m_group_id[20];
};


class loginData :public dataHeader
{
public:
    loginData()
    {
        this->m_cmd = LOGIN;
        this->m_dataLength = sizeof(loginData);
        memset(m_classnum,0,sizeof (m_classnum));
        memset(m_username,0,sizeof (m_username));
        memset(m_password,0,sizeof (m_password));
        memset(m_ip,0,sizeof (m_ip));
    }
    char m_classnum[20];
    char m_username[20];
    char m_password[20];
    char m_ip[20];

};
class loginResData :public dataHeader
{
public:
    loginResData()
    {
        this->m_cmd = LOGIN_RES;
        this->m_dataLength = sizeof(loginResData);
    }
    int m_res;
};
class registerResData :public dataHeader
{
public:
    registerResData()
    {
        this->m_cmd = REGISTER_RES;
        this->m_dataLength = sizeof(registerResData);
    }
    int m_res;
};

class detectFaceData :public dataHeader
{
public:
    detectFaceData()
    {
        this->m_cmd = DETECT;
        this->m_dataLength = sizeof(detectFaceData);
        memset(m_facedata,0,sizeof(m_facedata));
        memset(m_classnum,0,sizeof(m_classnum));
        memset(m_account,0,sizeof(m_account));
    }
    char m_classnum[20];
    char m_account[20];
    char m_facedata[1024*1024];
    int m_cols;
    int m_rows;
    int m_type;
};
class detectFaceResData :public dataHeader
{
public:
    detectFaceResData()
    {
        this->m_cmd = DETECT_RES;
        this->m_dataLength = sizeof(detectFaceResData);
    }
    int res;
};
class messageCreateData :public dataHeader
{
public:
    messageCreateData()
    {
        this->m_cmd = MESSAGE_CREATE;
        this->m_dataLength = sizeof(messageCreateData);
        memset(m_fromclassnum,0,sizeof(m_fromclassnum));
        memset(m_fromaccount,0,sizeof(m_fromaccount));
        memset(m_toclassnum,0,sizeof(m_toclassnum));
        memset(m_toaccount,0,sizeof(m_toaccount));
        memset(m_peertouxiang,0,sizeof(m_peertouxiang));
    }
    char m_fromclassnum[20];
    char m_toclassnum[20];
    char m_fromaccount[20];
    char m_toaccount[20];
    char m_peertouxiang[50];
};
class messageCreateResData :public dataHeader
{
public:
    messageCreateResData()
    {
        this->m_cmd = MESSAGE_CREATE_RES;
        this->m_dataLength = sizeof(messageCreateResData);
    }
    int res;
};

class onlineData :public dataHeader
{
public:
    onlineData()
    {
        this->m_cmd = ONLINE;
        this->m_dataLength = sizeof(onlineData);
        memset(m_classnum,0,sizeof(m_classnum));
        memset(m_account,0,sizeof(m_account));
    }
    char m_classnum[20];
    char m_account[20];
};

class onlineResData :public dataHeader
{
public:
    onlineResData()
    {
        this->m_cmd = ONLINE_RES;
        this->m_dataLength = sizeof(onlineResData);
    }
    int res;
};

class offlineData :public dataHeader
{
public:
    offlineData()
    {
        this->m_cmd = OFFLINE;
        this->m_dataLength = sizeof(offlineData);
        memset(m_classnum,0,sizeof(m_classnum));
        memset(m_account,0,sizeof(m_account));
    }
    char m_classnum[20];
    char m_account[20];
};

class offlineResData :public dataHeader
{
public:
    offlineResData()
    {
        this->m_cmd = OFFLINE_RES;
        this->m_dataLength = sizeof(offlineResData);
    }
    int res;
};

class checkOnlineData :public dataHeader
{
public:
    checkOnlineData()
    {
        this->m_cmd = CHECK_ISONLINE;
        this->m_dataLength = sizeof(checkOnlineData);
    }
};

//class checkOnlineResData :public dataHeader
//{
//public:
//    checkOnlineResData()
//    {
//        this->m_cmd = CHECK_ISONLINE_RES;
//        this->m_dataLength = sizeof(checkOnlineResData);
//    }
//    int res;
//};


class messageSendData :public dataHeader
{
public:
    messageSendData()
    {
        this->m_cmd = MESSAGE_SEND;
        this->m_dataLength = sizeof(messageSendData);
        memset(fromClassNum,0,sizeof(fromClassNum));
        memset(toClassNum,0,sizeof(toClassNum));
        memset(fromAccount,0,sizeof(fromAccount));
        memset(toAccount,0,sizeof(toAccount));
        memset(messageContent,0,sizeof(messageContent));
    }
    char fromClassNum[20];
    char toClassNum[20];
    char fromAccount[20];
    char toAccount[20];
    int updatetime;
    char messageContent[4096];
};

class messageSendRes :public dataHeader
{
public:
    messageSendRes()
    {
        this->m_cmd = MESSAGE_SEND_RES;
        this->m_dataLength = sizeof(messageSendRes);
        res = 0;
    }
    int res;
};
class messageHistoryData :public dataHeader
{
public:
    messageHistoryData()
    {
        this->m_cmd = MESSAGE_HISTORY;
        this->m_dataLength = sizeof(messageHistoryData);
        memset(fromClassNum,0,sizeof(fromClassNum));
        memset(toClassNum,0,sizeof(toClassNum));
        memset(fromAccount,0,sizeof(fromAccount));
        memset(toAccount,0,sizeof(toAccount));
        lastupdatetime = 0;
    }
    char fromClassNum[20];
    char toClassNum[20];
    char fromAccount[20];
    char toAccount[20];
    int lastupdatetime;
};
class messageHistoryCountResData :public dataHeader
{
public:
    messageHistoryCountResData()
    {
        this->m_cmd = MESSAGE_HISTORY_COUNT_RES;
        this->m_dataLength = sizeof(messageHistoryCountResData);
        m_count = 0;
    }
    int m_count;
    int res;
};
class messageHistoryResData :public dataHeader
{
public:
    messageHistoryResData()
    {
        this->m_cmd = MESSAGE_HISTORY_RES;
        this->m_dataLength = sizeof(messageHistoryCountResData);
    }
    int res;
};

class messageRecentSessionData :public dataHeader
{
public:
    messageRecentSessionData()
    {
        this->m_cmd = MESSAGR_RECENT_SESSION;
        this->m_dataLength = sizeof(messageRecentSessionData);
        memset(classNum,0,sizeof(classNum));
        memset(account,0,sizeof(account));
        memset(lastMessage,0,sizeof(lastMessage));
        memset(touxiang,0,sizeof(touxiang));
        lastupdatetime = 0;
    }
    char classNum[20];
    char account[20];
    char lastMessage[1024];
    char touxiang[50];
    int lastupdatetime;
};


class messageRecentSessionResData :public dataHeader
{
public:
    messageRecentSessionResData()
    {
        this->m_cmd = MESSAGR_RECENT_SESSION_RES;
        this->m_dataLength = sizeof(messageRecentSessionResData);
        res = 0;
        m_count=0;
    }
    int res;
    int m_count;
};

class loginTouXiang :public dataHeader
{
public:
    loginTouXiang()
    {
        this->m_cmd = LOGIN_TOUXIANG;
        this->m_dataLength = sizeof(loginTouXiang);
        memset(m_classNum,0,sizeof(m_classNum));
        memset(m_account,0,sizeof(m_account));
    }
    char m_classNum[20];
    char m_account[20];
};

class loginTouXiangRes :public dataHeader
{
public:
    loginTouXiangRes()
    {
        this->m_cmd = LOGIN_TOUXIANG_RES;
        this->m_dataLength = sizeof(loginTouXiangRes);
        memset(m_touxiang,0,sizeof(m_touxiang));
    }

    char m_touxiang[50];
    int res;
};
class changeTouXiangData :public dataHeader
{
public:
    changeTouXiangData()
    {
        this->m_cmd = CHANGE_TOUXIANG;
        this->m_dataLength = sizeof(changeTouXiangData);
        memset(m_classNum,0,sizeof(m_classNum));
        memset(m_account,0,sizeof(m_account));
        memset(m_touxiang,0,sizeof(m_touxiang));
    }
    char m_classNum[20];
    char m_account[20];
    char m_touxiang[50];
};

class changeTouXiangRes :public dataHeader
{
public:
    changeTouXiangRes()
    {
        this->m_cmd = CHANGE_TOUXIANG_RES;
        this->m_dataLength = sizeof(changeTouXiangRes);
        m_res = 0;
    }
    int m_res;
};

class getTouXiangData :public dataHeader
{
public:
    getTouXiangData()
    {
        this->m_cmd = GET_TOUXIANG;
        this->m_dataLength = sizeof(getTouXiangData);
        memset(m_classNum,0,sizeof(m_classNum));
        memset(m_account,0,sizeof(m_account));
    }
    char m_classNum[20];
    char m_account[20];
};

class getTouXiangRes :public dataHeader
{
public:
    getTouXiangRes()
    {
        this->m_cmd = GET_TOUXIANG_RES;
        this->m_dataLength = sizeof(getTouXiangRes);
        memset(m_touxiang,0,sizeof(m_touxiang));
        m_res = 0;
    }
    char m_touxiang[50];
    int m_res;
};
#endif
