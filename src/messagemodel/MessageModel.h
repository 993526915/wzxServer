/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef MESSAGE_MODEL_H_
#define MESSAGE_MODEL_H_
#include"co_env.h"
#include <vector>
#include <string>
#include"DBpool.h"
//#include "SessionModel.h"
#include "RelationModel.h"
using namespace std;
class messageSendData;
class CMessageModel {
public:
	virtual ~CMessageModel();
	static CMessageModel& GetInstance();
    int inserMessage(int relateId,int fromId,int toId,string content,int type,int status);
    void getMessage(int relateId,bool isAll,vector<messageSendData> &messageList,int lastTime,int nowTime);
	void getLastMsg(int nFromId, int nToId, string& strMsgData, int &nMsgType);
private:
	CMessageModel();


};



#endif /* MESSAGE_MODEL_H_ */
