#ifndef _CO_PARAMS_H_
#define _CO_PARAMS_H_
#include<jsoncpp/json/json.h>
#include<string>
#include<fstream>
#include<memory>
#include<iostream>
using namespace std;
struct baiduAPIParams
{
    std::string app_id;
    std::string api_key;
    std::string secret_key;
    std::string group_id_list;
    std::string image_type;
};
struct mysqlParams
{
    std::string HostIp;
    int HostPort;
    std::string DataBaseName;
    std::string UserName;
    std::string PassWord;
    std::string DBPoolName;
    int DBPoolMaxNum;
//    "HostIp":"127.0.0.1",
//    "HostPort":3306,
//    "DataBaseName":"face_detect",
//    "UserName":"root",
//    "PassWord":"991214",
//    "DBPoolName":"wzx",
//    "DBPoolMaxNum":4
};
struct arcSoftParams
{
    string appID;
    string SDKKey;
    int NScale;
    int FaceNum;
};
class initParams
{
public:
    static initParams& GetInstance();
    shared_ptr<arcSoftParams> getArcSoftParams();
    shared_ptr<mysqlParams> getMysqlParams();
private:
    initParams();
    int init(string jsonPath);
private:
    shared_ptr<arcSoftParams> m_arcSoftParams;
    shared_ptr<mysqlParams> m_mysqlParams;
    std::ifstream ifs;
};


#endif
