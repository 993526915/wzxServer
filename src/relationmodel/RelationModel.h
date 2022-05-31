/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：RelationModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef RELATION_SHIP_H_
#define RELATION_SHIP_H_

#include <list>
#include"DBpool.h"
#include<iostream>
using namespace std;


class CRelationModel {
public:
	virtual ~CRelationModel();

	static CRelationModel& GetInstance();
    int getRelationId(int nUserAId, int nUserBId, bool bAdd);
    bool updateRelation(int nRelationId, int nUpdateTime);
    bool removeRelation(int nRelationId);
    
private:
	CRelationModel();
    int addRelation(int nSmallId, int nBigId);
private:
	static CRelationModel*	m_pInstance;
};
#endif
