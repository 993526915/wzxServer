
#include "SessionModel.h"


CSessionModel& CSessionModel::GetInstance()
{
    static CSessionModel m_pInstance;
    
    return m_pInstance;
}

void CSessionModel::getRecentSession(int nUserId ,int lastTime, vector<messageRecentSessionData>& lsContact)
{
    
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        string strSql = "select * from recentSession_table where userId = " + to_string(nUserId) + " and status = 0 and updated <" + to_string(lastTime) + " order by updated desc limit 100";
        
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if (pResultSet)
        {
            while (pResultSet->Next())
            {
                string lastMsg;
                int MsgType;
                string peerClassNum;
                string peerAccount;
                messageRecentSessionData temp;
                temp.lastupdatetime = pResultSet->GetInt("updated");
                char * peer = pResultSet->GetString("peerTouXiang");
                string peerTouXiang;
                if(peer)
                    peerTouXiang = peer;
                else peerTouXiang ="";
                memcpy(temp.touxiang,peerTouXiang.c_str(),peerTouXiang.length());
                int peerId = pResultSet->GetInt("peerId");
                CUserModel::GetInstance().getUserFromId(peerId,peerClassNum,peerAccount);
                CMessageModel::GetInstance().getLastMsg(nUserId,peerId,lastMsg,MsgType);
                memcpy(temp.lastMessage,lastMsg.c_str(),lastMsg.length());
                memcpy(temp.classNum,peerClassNum.c_str(),peerClassNum.length());
                memcpy(temp.account,peerAccount.c_str(),peerAccount.length());
                lsContact.push_back(temp);
            }
            delete pResultSet;
        }
        else
        {
            printf("no result set for sql: %s\n", strSql.c_str());
        }
        CDBPool::GetInstance().RelDBConn(pDBConn);

    }
    else
    {
        printf("no db connection for mysql\n");
    }
}

int CSessionModel::getSessionId(int nUserId, int nPeerId, int nType, bool isAll)
{
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    uint32_t nSessionId = -1;
    if(pDBConn)
    {
        string strSql;
        if (isAll) {
            strSql= "select id from recentSession_table where userId=" + to_string(nUserId) + " and peerId=" + to_string(nPeerId) + " and type=" + to_string(nType);
        }
        else
        {
            strSql= "select id from recentSession_table where userId=" + to_string(nUserId) + " and peerId=" + to_string(nPeerId) + " and type=" + to_string(nType) + " and status=0";
        }
        
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next()) {
                nSessionId = pResultSet->GetInt("id");
            }
            delete pResultSet;
        }
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return nSessionId;
}

bool CSessionModel::updateSession(int nSessionId, int nUpdateTime)
{
    bool bRet = false;
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        string strSql = "update recentSession_table set `updated`="+to_string(nUpdateTime) + " where id="+to_string(nSessionId);
        bRet = pDBConn->ExecuteUpdate(strSql.c_str());
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return bRet;
}

bool CSessionModel::removeSession(int nSessionId)
{
    bool bRet = false;

    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        uint32_t nNow = (uint32_t) time(NULL);
        string strSql = "update recentSession_table set status = 1, updated="+to_string(nNow)+" where id=" + to_string(nSessionId);
        bRet = pDBConn->ExecuteUpdate(strSql.c_str());
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return bRet;
}

int CSessionModel::addSession(int nUserId, int nPeerId, int nType,string peerTouXiang)
{
    int nSessionId = -1;
    nSessionId = getSessionId(nUserId, nPeerId, nType, true);
    int nTimeNow = time(NULL);
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        if(-1 != nSessionId)
        {
            char strSql[200]={0};
            sprintf(strSql,"update recentSession_table set status=0, peerTouXiang = '%s' , updated = '%d' where id = %d",peerTouXiang.c_str(),nTimeNow,nSessionId);
           // string strSql = "update recentSession_table set status=0, peerTouXiang = "+ peerTouXiang +",updated=" + to_string(nTimeNow) + " where id=" + to_string(nSessionId);
            bool bRet = pDBConn->ExecuteUpdate(strSql);
            if(!bRet)
            {
                nSessionId = -1;
            }
            printf("has relation ship set status\n");
        }
        else
        {
            string strSql = "insert into recentSession_table (`userId`,`peerId`,`type`,`status`,`created`,`updated`,`peerTouXiang`) values(?,?,?,?,?,?,?)";
            // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
            CPrepareStatement* stmt = new CPrepareStatement();
            if (stmt->Init(pDBConn->GetMysql(), strSql))
            {
                uint32_t nStatus = 0;
                uint32_t index = 0;
                stmt->SetParam(index++, nUserId);
                stmt->SetParam(index++, nPeerId);
                stmt->SetParam(index++, nType);
                stmt->SetParam(index++, nStatus);
                stmt->SetParam(index++, nTimeNow);
                stmt->SetParam(index++, nTimeNow);
                stmt->SetParam(index++, peerTouXiang);
                bool bRet = stmt->ExecuteUpdate();
                if (bRet)
                {
                    nSessionId = pDBConn->GetInsertId();
                }
                else
                {
                    printf("insert message failed. %s\n", strSql.c_str());
                }
            }
            delete stmt;
        }
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return nSessionId;
}



// void CSessionModel::fillSessionMsg(uint32_t nUserId, list<IM::BaseDefine::ContactSessionInfo>& lsContact)
// {
//     for (auto it=lsContact.begin(); it!=lsContact.end();)
//     {
//         uint32_t nMsgId = 0;
//         string strMsgData;
//         IM::BaseDefine::MsgType nMsgType;
//         uint32_t nFromId = 0;
//         if( it->session_type() == IM::BaseDefine::SESSION_TYPE_SINGLE)
//         {
//             nFromId = it->session_id();
//             CMessageModel::getInstance()->getLastMsg(it->session_id(), nUserId, nMsgId, strMsgData, nMsgType);
//         }
//         else
//         {
//             CGroupMessageModel::getInstance()->getLastMsg(it->session_id(), nMsgId, strMsgData, nMsgType, nFromId);
//         }
//         if(!IM::BaseDefine::MsgType_IsValid(nMsgType))
//         {
//             it = lsContact.erase(it);
//         }
//         else
//         {
//             it->set_latest_msg_from_user_id(nFromId);
//             it->set_latest_msg_id(nMsgId);
//             it->set_latest_msg_data(strMsgData);
//             it->set_latest_msg_type(nMsgType);
//             ++it;
//         }
//     }
// }
















