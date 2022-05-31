/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：UserModel.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年01月05日
*   描    述：
*
================================================================*/
#include "UserModel.h"


CUserModel::CUserModel()
{

}

CUserModel::~CUserModel()
{
    
}

CUserModel& CUserModel::GetInstance()
{
    static CUserModel instance;
    return instance;
}



int CUserModel::getUserId(string classnum,string account)
{
    int idRes = -1;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[100];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select id from user_table where classnum = '%s' and account = '%s';",classnum.c_str(),account.c_str());
    //cout << strSql <<endl;
    CResultSet *res = newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        idRes = res->GetInt("id");
    }
    else
    {
        cout << "no id" << endl;
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
    return idRes;
}
void CUserModel::getUserFromId(int userId,string &classNum,string &account)
{
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select classnum,account from user_table where id = %d",userId);
    //cout << strSql <<endl;
    CResultSet *res = newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        classNum = res->GetString("classnum");
        account = res->GetString("account");
    }
    else
    {
        cout << "no id" << endl;
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
}


void CUserModel::getTouXiang(string classnum,string account,string &touxiang)
{
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select touxiang from user_table where classnum = '%s' and account = '%s'; ",classnum.c_str(),account.c_str());
    CResultSet *res = newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        touxiang= res->GetString("touxiang");
    }
    else
    {
        cout << "no touxiang" << endl;
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
}

bool CUserModel::userExist(string classnum,string account,string password)
{
    CDBConn * newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[256];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select classnum,account from user_table where classnum = '%s' and account = '%s' and password = '%s';",
            classnum.c_str(),account.c_str(),password.c_str());
    CResultSet *res =  newConnect->ExecuteQuery(strSql);
    if(res->Next())
    {
        cout << "user already exist" << endl;
        CDBPool::GetInstance().RelDBConn(newConnect);
        return true;
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
    return false;
}

void CUserModel::addUserInfo(string classnum,string account,string password,string touxiang)
{
    CDBConn * newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"insert into user_table(classnum, account, password,islogin,touxiang) values('%s','%s','%s',0,'%s')",
            classnum.c_str(),account.c_str(),password.c_str(),touxiang.c_str());

    newConnect->ExecuteQuery(strSql);

    CDBPool::GetInstance().RelDBConn(newConnect);
}
void CUserModel::changeTouXiang(string classNum,string account,string touxiangPath)
{
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"update user_table set touxiang = '%s' where classnum = '%s' and account = '%s';",
            touxiangPath.c_str(),classNum.c_str(),account.c_str());
    newConnect->ExecuteQuery(strSql);
    CDBPool::GetInstance().RelDBConn(newConnect);
}
void CUserModel::GetTouXiang(string classNum,string account,string &touxiangPath)
{
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[200];
    memset(strSql,0,sizeof(strSql));
    sprintf(strSql,"select touxiang from user_table where classnum = '%s' and account = '%s';",
            classNum.c_str(),account.c_str());
    CResultSet *res =  newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        char *path =  res->GetString("touxiang");
        if(path)
        {
            touxiangPath = path;
        }
        else
        {
            touxiangPath = "";
        }
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
    
}
