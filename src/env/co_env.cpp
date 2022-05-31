#include<co_env.h>
int read_binary(char *filename, char *buffer) {

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int size = fread(buffer, 1, length, fp);
    if (size != length) {
        printf("fread faile\n");
        return size;
    }

    fclose(fp);

    return size;
}
Event::Event()
{

}
bool DataToMat(void* data, int nH, int nW, int nFlag, Mat& outImg)//nH,nW为BYTE*类型图像的高和宽,nFlag为通道数
{
    if (data == nullptr)
    {
        return false;
    }
    int nByte = nH * nW * nFlag / 8;//字节计算
    int nType = nFlag == 8 ? CV_8UC1 : CV_8UC3;
    outImg = Mat::zeros(nH, nW, nType);
    memcpy(outImg.data, (unsigned char*)data, nByte);
    return true;
}

int getGroupListEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    getGroupData *groupData = (getGroupData *)header;

    Json::Value json_result;
    int ret = CDetectModel::GetInstance().groupGetlist(json_result);
    cout << json_result << endl;
    if(ret < 0)
    {
        cout << "groupGetlist failed!" <<endl;
        return 0;
    }
    string result = json_result.toStyledString();
    write(val->fd,result.c_str(),result.length());
    return 1;
}
int getuserListEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    getUserData *groupData = (getUserData *)header;
    Json::Value json_result;
    int ret = CDetectModel::GetInstance().groupGetusers(json_result,groupData->m_group_id);
    if(ret < 0)
    {
        cout << "groupGetusers failed!" <<endl;
        return 0;
    }
    Json::Value user_id_list = json_result["result"]["user_id_list"];
    for(int j=0;j<user_id_list.size();j++)
    {
        string account = user_id_list[j]["account"].asString();
        pair<string,string> classnum_account = make_pair(string(groupData->m_group_id),account);
        auto iter = userMap.find(classnum_account);
        if(iter == userMap.end())
        {
            userMap[classnum_account] = 0;
        }
        CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
        char strSql[200];
        sprintf(strSql,"select islogin from user_table where classnum = '%s' and account = '%s';",
                groupData->m_group_id,account.c_str());
        CResultSet *res =  newConnect->ExecuteQuery(strSql);
        if(res->Next())
        {
            int islogin = res->GetInt("islogin");
            cout << groupData->m_group_id << "--" << account<<"-----------exist : " << islogin << endl;
            userMap[classnum_account] = islogin;
        }
        CDBPool::GetInstance().RelDBConn(newConnect);
    }
    string result = json_result.toStyledString();
    write(val->fd,result.c_str(),result.length());
    return 1;
}
int delGroupEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    delGroupData * delGroup = (delGroupData *)header;
    Json::Value json_result;
    int ret =CDetectModel::GetInstance().groupDelete(json_result,delGroup->m_group_id);
    if(!ret)
    {
        cout << "groupDelete failed!" <<endl;
        return 0;
    }
    string result = json_result.toStyledString();
    write(val->fd,result.c_str(),result.length());
    return 1;
}
int delUserEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    delUserData * delUser = (delUserData *)header;
    Json::Value json_result;
    int ret = CDetectModel::GetInstance().userDelete(json_result,delUser->m_group_id,delUser->m_user_id);
    if(!ret)
    {
        cout << "userDelete failed!" <<endl;
        return 0;
    }
    string result = json_result.toStyledString();
    write(val->fd,result.c_str(),result.length());
    return 1;
}
int detectFaceEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    cout << "detectFaceEvent " << endl;
    detectFaceData * detectData = (detectFaceData *)header;
    string classnum(detectData->m_classnum);
    string account(detectData->m_account);
    cout << classnum << endl;
    cout << account << endl;
    Mat image(detectData->m_rows,detectData->m_cols,detectData->m_type,detectData->m_facedata);
    imwrite("/home/wzx/c.jpg",image);
    image = imread("/home/wzx/c.jpg");
    tuple<string,string>res;
    res =  CDetectModel::GetInstance().detectOneFace(image,classnum,account);

    string user_id = get<1>(res);
    string group_id = get<0>(res);
    shared_ptr<detectFaceResData> detectRes = make_shared<detectFaceResData>();
    detectRes->res=DETECT_SUCCESS;
    if(user_id != account || group_id != classnum)
    {
        detectRes->res = DETECT_NOUSER;
    }
    else
    {
        detectRes->res = DETECT_SUCCESS;
    }
    SockUtil::co_write(val->fd,detectRes.get(),detectRes->m_dataLength);
}

int checkOnlineEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    checkOnlineData *check = (checkOnlineData *)header;
    Json::Value root;
    for(auto iter = userMap.begin();iter!=userMap.end();iter++)
    {
        Json::Value user;
        pair<string,string> classnum_account = iter->first;
        string classnum = classnum_account.first;
        string account = classnum_account.second;
        int islogin = iter->second;
        user["classnum"] = classnum;
        user["account"] = account;
        user["islogin"] = islogin;
        root.append(user);
    }
    //cout << root << endl;
    string res = root.toStyledString();
    SockUtil::co_write(val->fd,res.c_str(),res.length());

}

int LoginUserEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    loginData *login = (loginData *)header;
    puts(login->m_classnum);
    puts(login->m_username);
    puts(login->m_password);
    CDBConn *newConnect =  CDBPool::GetInstance().GetDBConn();
    bool bRet = false;
    char strSql[256];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select classnum,account,password from user_table where classnum = '%s' and account = '%s' and password = '%s';",
            login->m_classnum,login->m_username,login->m_password);
    CResultSet *res =  newConnect->ExecuteQuery(strSql);
    if(res->Next())
    {
        cout << "user exist" << endl;
        loginResData resData ;
        resData.m_res = LOGIN_SUCCESS;
        SockUtil::co_write(val->fd,&resData,resData.m_dataLength);
    }
    else
    {
        cout << "no user"<<endl;
        loginResData resData ;
        resData.m_res = LOGIN_NOUSER;
        SockUtil::co_write(val->fd,&resData,resData.m_dataLength);
        
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
}


int LoginAddFaceEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    addFaceData *faceInfo = (addFaceData *)header;    
    Mat image(faceInfo->m_rows,faceInfo->m_cols,faceInfo->m_type,faceInfo->m_facedata);
    bool isExist = CUserModel::GetInstance().userExist(faceInfo->m_classnum,faceInfo->m_account,faceInfo->m_password);
    if(isExist)
    {
        cout << "user already exist" << endl;
        registerResData resData;
        resData.m_res = REGISTER_USER_ALREADY_EXIST;
        SockUtil::co_write(val->fd,&resData,resData.m_dataLength);
        return 0;
    }
    else
    {
        string classnum = faceInfo->m_classnum;
        string account = faceInfo->m_account;
        string password = faceInfo->m_password;
        string touxiang = faceInfo->m_touxiang;
        CUserModel::GetInstance().addUserInfo(classnum,account,password,touxiang);
        Json::Value json_res;
        CDetectModel::GetInstance().add(json_res,image,faceInfo->m_classnum,faceInfo->m_account);
        cout << json_res << endl;
        if(json_res["error_code"].asInt()!= 0)
        {
            cout << "error code----------addFace" << endl;
            return -1;
        }
        else
        {
            cout << "register success!"<<endl;
            registerResData resData ;
            resData.m_res = REGISTER_SUCCESS;
            SockUtil::co_write(val->fd,&resData,resData.m_dataLength);
            string res = "code :" + json_res["error_code"].asString() +"msg :" +json_res["error_msg"].asString();
            cout << res << endl;
            cout << "------------------------" << endl;
        }
        
    }
    return 1;
}

int LoginOnlineEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    onlineData *online = (onlineData *)header;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    string ip = SockUtil::get_peer_ip(val->fd);
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"update user_table set ip = '%s', islogin = 1 where classnum = '%s' and account = '%s';",
            ip.c_str(),online->m_classnum,online->m_account);
    pair<string,string> classnum_account = make_pair(online->m_classnum,online->m_account);
    userMap[classnum_account] = 1;
    newConnect->ExecuteQuery(strSql);
    onlineResData onlineRes ;
    onlineRes.res=ONLINE_SUCCESS;
    SockUtil::co_write(val->fd,&onlineRes,onlineRes.m_dataLength);
    CDBPool::GetInstance().RelDBConn(newConnect);
}
int LoginOfflineEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    offlineData *offline = (offlineData *)header;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"update user_table set islogin = 0 where classnum = '%s' and account = '%s';",
            offline->m_classnum,offline->m_account);
    pair<string,string> classnum_account = make_pair(offline->m_classnum,offline->m_account);
    userMap[classnum_account] = 0;
    newConnect->ExecuteQuery(strSql);
    onlineResData *onlineRes = new onlineResData;
    onlineRes->res=OFFLINE_SUCCESS;
    SockUtil::co_write(val->fd,onlineRes,onlineRes->m_dataLength);
    CDBPool::GetInstance().RelDBConn(newConnect);
}
int MessageCreateEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    messageCreateData *messageCreate = (messageCreateData *)header;
    cout << messageCreate->m_fromclassnum << endl;
    cout << messageCreate->m_fromaccount << endl;
    cout << messageCreate->m_toclassnum << endl;
    cout << messageCreate->m_toaccount << endl;
    cout << messageCreate->m_peertouxiang<<endl;
    int fromId,toId;
    fromId = CUserModel::GetInstance().getUserId(messageCreate->m_fromclassnum,messageCreate->m_fromaccount);
    toId = CUserModel::GetInstance().getUserId(messageCreate->m_toclassnum,messageCreate->m_toaccount);
    // 0 是文本，1 是语音。
    int sessionId = CSessionModel::GetInstance().addSession(fromId,toId,0,messageCreate->m_peertouxiang);
    messageCreateResData messageCreateRes;
    if(sessionId == -1)
    {
        messageCreateRes.res = MESSAGE_CREATE_FAILED;
    }
    else
    {
        messageCreateRes.res = MESSAGE_CREATE_SUCCESS;
    }
    SockUtil::co_write(val->fd,&messageCreateRes,messageCreateRes.m_dataLength);
}
int MessageSendEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
     co_enable_hook_sys();
    messageSendData *sendData =(messageSendData *)header;
    string fromClassNum = sendData->fromClassNum;
    string fromAccount = sendData->fromAccount;
    string toClassNum = sendData->toClassNum;
    string toAccount = sendData->toAccount;
    string messageContent = sendData->messageContent;
    int updateTime = sendData->updatetime;

    int fromId,toId;
    fromId = CUserModel::GetInstance().getUserId(fromClassNum,fromAccount);
    toId = CUserModel::GetInstance().getUserId(toClassNum,toAccount);
    int relationId = CRelationModel::GetInstance().getRelationId(fromId,toId,true);
    int sessionId = CSessionModel::GetInstance().getSessionId(fromId,toId,0,false);
    CSessionModel::GetInstance().updateSession(sessionId,updateTime);
    int ret = CMessageModel::GetInstance().inserMessage(relationId,fromId,toId,messageContent,1,1);
//    messageSendRes sendRes;
//    if(ret != -1)
//    {
        
//        sendRes.res = MESSAGE_SEND_SUCCESS;
//    }
//    else
//    {
//        sendRes.res = MESSAGE_SEND_FAILED;
//    }
//    SockUtil::co_write(val->fd,&sendRes,sendRes.m_dataLength);

}
int MessageHistoryEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    messageHistoryData *historyData = (messageHistoryData *)header;
    string fromClassNum = historyData->fromClassNum;
    string fromAccount = historyData->fromAccount;
    string toClassNum = historyData->toClassNum;
    string toAccount = historyData->toAccount;
    int lastUpdateTime = historyData->lastupdatetime;
    int fromId,toId;
    fromId = CUserModel::GetInstance().getUserId(fromClassNum,fromAccount);
    toId = CUserModel::GetInstance().getUserId(toClassNum,toAccount);
    int relationId = CRelationModel::GetInstance().getRelationId(fromId,toId,true);
    vector<messageSendData> messageList;
    CMessageModel::GetInstance().getMessage(relationId,false,messageList,lastUpdateTime,(int)time(NULL));
    int count = messageList.size();
    if(count == 0)
    {
        messageHistoryCountResData historyCountRes;
        historyCountRes.m_count=count;
        historyCountRes.res = MESSAGE_HISTORY_FAILED;
        SockUtil::co_write(val->fd,&historyCountRes,historyCountRes.m_dataLength);
    }
    else
    {
        cout << "message count : " << count <<endl;
        messageHistoryCountResData historyCountRes;
        historyCountRes.m_count=count;
        historyCountRes.res = MESSAGE_HISTORY_SUCCESS;
        SockUtil::co_write(val->fd,&historyCountRes,historyCountRes.m_dataLength);
        for(int i=0;i<count;i++)
        {
            write(val->fd,&messageList[i],messageList[i].m_dataLength);
            //SockUtil::co_write(val->fd,&messageList[i],messageList[i].m_dataLength);
            //messageSendRes sendRes;
            //usleep(10000);
            //read(val->fd,&sendRes,sendRes.m_dataLength);
            //SockUtil::co_read(val->fd,&sendRes,sendRes.m_dataLength);
        }
    }

}
int RescentSessionEvent::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    co_enable_hook_sys();
    messageRecentSessionData *recentSession = (messageRecentSessionData *)header;
    string fromClassNum = recentSession->classNum;
    string fromAccount = recentSession->account;

    int lastUpdateTime = recentSession->lastupdatetime;
    int fromId = CUserModel::GetInstance().getUserId(fromClassNum,fromAccount);
    
    vector<messageRecentSessionData> recentList;
    CSessionModel::GetInstance().getRecentSession(fromId,lastUpdateTime,recentList);
    
    int count = recentList.size();
    if(count <= 0)
    {
        messageRecentSessionResData recentSessionRes;
        recentSessionRes.res = MESSAGR_RECENT_SESSION_FAILED;
        recentSessionRes.m_count = 0;
        SockUtil::co_write(val->fd,&recentSessionRes,recentSessionRes.m_dataLength);
    }
    else
    {
        messageRecentSessionResData recentSessionRes;
        recentSessionRes.res = MESSAGR_RECENT_SESSION_SUCCESS;
        recentSessionRes.m_count = count;
        SockUtil::co_write(val->fd,&recentSessionRes,recentSessionRes.m_dataLength);
        for(int i=0;i<count;i++)
        {
            SockUtil::co_write(val->fd,&recentList[i],recentList[i].m_dataLength);
            usleep(5000);
            messageRecentSessionResData sessionRes;
            SockUtil::co_read(val->fd,&sessionRes,sessionRes.m_dataLength);
            if(sessionRes.res!=MESSAGR_RECENT_SESSION_SUCCESS)
            {
                cout << "MESSAGR_RECENT_SESSION_FAILED!"<<endl;
            }
        }
    }
}

int LoginTouXiang::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    loginTouXiang *touxiang = (loginTouXiang *)header;
    string classnum = touxiang->m_classNum;
    string account = touxiang->m_account;
    string pic;
    CUserModel::GetInstance().getTouXiang(classnum,account,pic);
    loginTouXiangRes getRes;
    if(pic.length())
    {
        getRes.res = LOGIN_TOUXIANG_SUCCESS;
        memcpy(getRes.m_touxiang,pic.c_str(),pic.length());

    }
    else
    {
        getRes.res = LOGIN_TOUXIANG_FAILED;
    }
    SockUtil::co_write(val->fd,&getRes,getRes.m_dataLength);
}
int ChangeTouXiang::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    changeTouXiangData *touxiang = (changeTouXiangData *)header;
    string classNum = touxiang->m_classNum;
    string account = touxiang->m_account;
    string touxiangPath = touxiang->m_touxiang;
    CUserModel::GetInstance().changeTouXiang(classNum,account,touxiangPath);

    changeTouXiangRes res;
    res.m_res = CHANGE_TOUXIANG_SUCCESS;
    SockUtil::co_write(val->fd,&res,res.m_dataLength);
}

int GetTouXiang::doEvent(stEnv_t *val,dataHeader *header,map<pair<string,string>,int> &userMap)
{
    getTouXiangData *touxiang = (getTouXiangData *)header;
    string classNum = touxiang->m_classNum;
//    char account[20];
//    memcpy(account,touxiang->m_account,sizeof(account));
    string account = touxiang->m_account;
    string touXiangPath;
    CUserModel::GetInstance().getTouXiang(classNum,account,touXiangPath);
    getTouXiangRes res;
    if(touXiangPath.length())
    {
        cout << touXiangPath<<endl;
        memcpy(res.m_touxiang,touXiangPath.c_str(),touXiangPath.length());
        res.m_res = GET_TOUXIANG_SUCCESS;
    }
    else
    {
        memcpy(res.m_touxiang,touXiangPath.c_str(),touXiangPath.length());
        res.m_res = GET_TOUXIANG_FAILED;
    }
    SockUtil::co_write(val->fd,&res,res.m_dataLength);
}
