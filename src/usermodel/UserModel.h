
#ifndef __USERMODEL_H__
#define __USERMODEL_H__

#include"DBpool.h"
#include<iostream>
#include<string.h>
using namespace std;

class CUserModel
{
public:
    static CUserModel& GetInstance();
    ~CUserModel();
    int getUserId(string classnum,string account);
    void getUserFromId(int userId,string &classNum,string &account);
    void getTouXiang(string classnum,string account,string &touxiang);
    bool userExist(string classnum,string account,string password);
    void addUserInfo(string classnum,string account,string password,string touxiang);
    void changeTouXiang(string classNum,string account,string touxiangPath);
    void GetTouXiang(string classNum,string account,string &touxiangPath);
private:
    CUserModel();

};

#endif /*defined(__USERMODEL_H__) */
