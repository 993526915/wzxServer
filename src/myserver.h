#ifndef MYSERVER_H
#define MYSERVER_H
#include"DBpool.h"
#include"co_env.h"
#include "co_routine.h"
#include"co_socktool.h"
#include"co_params.h"
#include<map>
class myServer
{
public:
    myServer(stEnv_t *env);
    void run();
    int co_read( int fd, void *buf, size_t nbyte );
    int co_write(int fd, const void *buf, size_t nbyte);
    int co_work(stEnv_t *env,dataHeader *header);
private:
    int co_recvData(stEnv_t *env);
    int serverWork(stEnv_t *env,dataHeader *header);
private:
    shared_ptr<CDBPool> m_dbpool;
    shared_ptr<initParams> m_params;
    stEnv_t *m_env;
    static map<pair<string,string>,int> m_userMap;
};

#endif // MYSERVER_H
