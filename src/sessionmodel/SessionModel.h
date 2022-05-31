
#ifndef __SESSIONMODEL_H__
#define __SESSIONMODEL_H__

#include"DBpool.h"
#include"RelationModel.h"
#include"co_env.h"
class messageRecentSessionData;
class CSessionModel
{
public:
    static CSessionModel& GetInstance();
    ~CSessionModel() {}
    void getRecentSession(int nUserId,int lastTime, vector<messageRecentSessionData>& lsContact);
    int getSessionId(int nUserId, int nPeerId, int nType, bool isAll);
    bool updateSession(int nSessionId, int nUpdateTime);
    bool removeSession(int nSessionId);
    int addSession(int nUserId, int nPeerId, int nType,string peerTouXiang);
    
private:
    CSessionModel() {};

};

#endif /*defined(__SESSIONMODEL_H__) */
