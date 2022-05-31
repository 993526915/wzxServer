/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：RelationModel.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <vector>


#include "RelationModel.h"
using namespace std;

CRelationModel* CRelationModel::m_pInstance = NULL;

CRelationModel::CRelationModel()
{

}

CRelationModel::~CRelationModel()
{

}

CRelationModel& CRelationModel::GetInstance()
{
    static CRelationModel m_pInstance;
	return m_pInstance;
}

/**
 *  获取会话关系ID
 *  对于群组，必须把nUserBId设置为群ID
 *
 *  @param nUserAId  <#nUserAId description#>
 *  @param nUserBId  <#nUserBId description#>
 *  @param bAdd      <#bAdd description#>
 *  @param nStatus 0 获取未被删除会话，1获取所有。
 */
int CRelationModel::getRelationId(int nUserAId, int nUserBId, bool bAdd)
{
    int nRelationId = -1;
    if (nUserAId == 0 || nUserBId == 0) {
        printf("invalied user id:%u->%u", nUserAId, nUserBId);
        return nRelationId;
    }

    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        int nBigId = nUserAId > nUserBId ? nUserAId : nUserBId;
        int nSmallId = nUserAId > nUserBId ? nUserBId : nUserAId;
        string strSql = "select id from relationShip_table where smallId=" + to_string(nSmallId) + " and bigId="+ to_string(nBigId) + " and status = 0";
        
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if (pResultSet)
        {
            while (pResultSet->Next())
            {
                nRelationId = pResultSet->GetInt("id");
            }
            delete pResultSet;
        }
        else
        {
            printf("there is no result for sql:%s\n", strSql.c_str());
        }
        CDBPool::GetInstance().RelDBConn(pDBConn);
        if (nRelationId == -1 && bAdd)
        {
            nRelationId = addRelation(nSmallId, nBigId);
        }
    }
    else
    {
        printf("no db connection for myysql\n");
    }
    return nRelationId;
}

int CRelationModel::addRelation(int nSmallId, int nBigId)
{
    int nRelationId = -1;

    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        int nTimeNow = (int)time(NULL);
        string strSql = "select id from relationShip_table where smallId=" + to_string(nSmallId) + " and bigId="+ to_string(nBigId);
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet && pResultSet->Next())
        {
            nRelationId = pResultSet->GetInt("id");
            strSql = "update relationShip_table set status=0, updated=" + to_string(nTimeNow) + " where id=" + to_string(nRelationId);
            bool bRet = pDBConn->ExecuteUpdate(strSql.c_str());
            if(!bRet)
            {
                nRelationId = -1;
            }
            printf("has relation ship set status\n");
            delete pResultSet;
        }
        else
        {
            strSql = "insert into relationShip_table (`smallId`,`bigId`,`status`,`created`,`updated`) values(?,?,?,?,?)";
            // 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
            CPrepareStatement* stmt = new CPrepareStatement();
            if (stmt->Init(pDBConn->GetMysql(), strSql))
            {
                uint32_t nStatus = 0;
                uint32_t index = 0;
                stmt->SetParam(index++, nSmallId);
                stmt->SetParam(index++, nBigId);
                stmt->SetParam(index++, nStatus);
                stmt->SetParam(index++, nTimeNow);
                stmt->SetParam(index++, nTimeNow);
                bool bRet = stmt->ExecuteUpdate();
                if (bRet)
                {
                    nRelationId = pDBConn->GetInsertId();
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
    return nRelationId;
}

bool CRelationModel::updateRelation(int nRelationId, int nUpdateTime)
{
    bool bRet = false;
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        string strSql = "update relationShip_table set `updated`="+to_string(nUpdateTime) + " where id="+to_string(nRelationId);
        bRet = pDBConn->ExecuteUpdate(strSql.c_str());
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    
    return bRet;
}

bool CRelationModel::removeRelation(int nRelationId)
{
    bool bRet = false;
    CDBConn* pDBConn = CDBPool::GetInstance().GetDBConn();
    if (pDBConn)
    {
        uint32_t nNow = (uint32_t) time(NULL);
        string strSql = "update relationShip_table set status = 1, updated="+to_string(nNow)+" where id=" + to_string(nRelationId);
        bRet = pDBConn->ExecuteUpdate(strSql.c_str());
        CDBPool::GetInstance().RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection for mysql\n");
    }
    return bRet;
}