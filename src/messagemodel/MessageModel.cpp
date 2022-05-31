/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageModel.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <map>
#include <set>

#include"MessageModel.h"

using namespace std;


CMessageModel::CMessageModel()
{

}

CMessageModel::~CMessageModel()
{

}

CMessageModel& CMessageModel::GetInstance()
{
    static CMessageModel instance;
    return instance;
}

int CMessageModel::inserMessage(int relateId,int fromId,int toId,string content,int type,int status)
{
    int nMsgId = -1;
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        string strSql = "insert into message_table (relateId,fromId,toId,msgId,content,type,status,updated,created) values(?,?,?,?,?,?,?,?,?);";
        // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
        CPrepareStatement* stmt = new CPrepareStatement();
        if (stmt->Init(pDBConn->GetMysql(), strSql))
        {
            uint32_t index = 0;
            stmt->SetParam(index++, relateId);
            stmt->SetParam(index++, fromId);
            stmt->SetParam(index++, toId);
            stmt->SetParam(index++, 0);
            stmt->SetParam(index++, content);
            stmt->SetParam(index++, type);
            stmt->SetParam(index++, status);
            int nTimeNow = (int)time(NULL);
            stmt->SetParam(index++, nTimeNow);
            stmt->SetParam(index++, nTimeNow);
            bool bRet = stmt->ExecuteUpdate();
            if (bRet)
            {
                nMsgId = pDBConn->GetInsertId();
            }
            else
            {
                printf("insert message failed. %s\n", strSql.c_str());
            }
        }
        delete stmt;
        
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return nMsgId;
}

void CMessageModel::getMessage(int relateId,bool isAll,vector<messageSendData> &messageList,int lastTime,int nowTime)
{
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        int type = 0;
        if(!type) type =1;
        char strSql[200];
        memset(strSql,0,sizeof(strSql));
        //sprintf(strSql,"select fromId,toId,content,type,status,updated,created from message_table where relateId = %d and type = %d and updated <= %d;",relateId,type,nowTime);
         sprintf(strSql,"select fromId,toId,content,type,status,updated,created from message_table where relateId = %d and type = %d and updated <= %d and updated > %d",relateId,type,nowTime,lastTime);
        //string strSql = "select fromId,toId,content,type,status,updated,created from message_table where relateId = "+to_string(relateId) +"and type = "+to_string(type)+" order by created desc , id desc;";
        // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
        CResultSet *res = pDBConn->ExecuteQuery(strSql);
        while(res && res->Next())
        {
            messageSendData sendData;
            string fromClassNum;
            string fromAccount;
            string toClassNum;
            string toAccount;
            string content;
            content = res->GetString("content");
            CUserModel::GetInstance().getUserFromId(res->GetInt("fromId"),fromClassNum,fromAccount);
            CUserModel::GetInstance().getUserFromId(res->GetInt("toId"),toClassNum,toAccount);
            memcpy(sendData.fromClassNum,fromClassNum.c_str(),fromClassNum.length());
            memcpy(sendData.fromAccount,fromAccount.c_str(),fromAccount.length());
            memcpy(sendData.toClassNum,toClassNum.c_str(),toClassNum.length());
            memcpy(sendData.toAccount,toAccount.c_str(),toAccount.length());
            memcpy(sendData.messageContent,content.c_str(),content.length());
            sendData.updatetime = res->GetInt("created");
            messageList.push_back(sendData);
        }
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
}


void CMessageModel::getLastMsg(int nFromId, int nToId,  string& strMsgData, int & nMsgType)
{
    uint32_t nRelateId = CRelationModel::GetInstance().getRelationId(nFromId, nToId, false);
    if (nRelateId != -1)
    {
        CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
        if (pDBConn)
        {
            string strSql = "select type,content from message_table force index (idx_relateId_status_created) where relateId= " + to_string(nRelateId) + " and status = 1 order by created desc, id desc limit 1";
            CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
            if (pResultSet)
            {
                while (pResultSet->Next())
                {
                    nMsgType = pResultSet->GetInt("type");
                    strMsgData = pResultSet->GetString("content");
                }
                delete pResultSet;
            }
            else
            {
                printf("no result set: %s\n", strSql.c_str());
            }
            CDBPool::GetInstance().RelDBConn(pDBConn);
        }
        else
        {
            printf("no db mysql\n");
        }
    }
    else
    {
        printf("no relation between %lu and %lu\n", nFromId, nToId);
    }
}