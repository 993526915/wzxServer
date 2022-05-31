#include"co_params.h"
initParams& initParams::GetInstance()
{
    static initParams instance;
    return instance;
}
initParams::initParams()
{
    m_arcSoftParams = make_shared<arcSoftParams>();
    m_mysqlParams = make_shared<mysqlParams>();
    init("/home/wzx/netlearning/wzxServer/src/params.json");
}
int initParams::init(string jsonPath)
{
    Json::Value root;
    Json::Reader reader;
    ifs.open(jsonPath.c_str());
    if(!ifs.is_open())
    {
        cout << "Json文件打开错误--------initParams::init()" << endl;
        return 0;
    }
    if(!reader.parse(ifs, root)){
       cout << "Json解析错误--------initParams::init()" << endl;
       return 0;
    }
    
    m_arcSoftParams->appID = root["arcsoft"]["AppID"].asString();
    m_arcSoftParams->SDKKey = root["arcsoft"]["SDKKey"].asString();
    m_arcSoftParams->NScale = root["arcsoft"]["NScale"].asInt();
    m_arcSoftParams->FaceNum = root["arcsoft"]["FaceNum"].asInt();

    m_mysqlParams->HostIp = root["mysql"]["HostIp"].asString();
    m_mysqlParams->HostPort = root["mysql"]["HostPort"].asInt();
    m_mysqlParams->PassWord = root["mysql"]["PassWord"].asString();
    m_mysqlParams->UserName = root["mysql"]["UserName"].asString();
    m_mysqlParams->DBPoolName = root["mysql"]["DBPoolName"].asString();
    m_mysqlParams->DBPoolMaxNum = root["mysql"]["DBPoolMaxNum"].asInt();
    m_mysqlParams->DataBaseName = root["mysql"]["DataBaseName"].asString();
}

shared_ptr<arcSoftParams> initParams::getArcSoftParams()
{
    return this->m_arcSoftParams;
}
shared_ptr<mysqlParams> initParams::getMysqlParams()
{
    return  this->m_mysqlParams;
}
