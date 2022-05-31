#ifndef TIMEWHEEL_H
#define TIMEWHEEL_H

#include"spinlock.h"
//#include"co_routine.h"
//#include"co_epoll.h"

#include <poll.h>
#include <sys/time.h>
#include <errno.h>

#include <assert.h>
#include<pthread.h>
#include<iostream>
#include<list>
#include <queue>
#include<string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <limits.h>
using namespace  std;

#define TIME_NEAR_SHIFT 8
#define TIME_NEAR (1 << TIME_NEAR_SHIFT)
#define TIME_LEVEL_SHIFT 6
#define TIME_LEVEL (1 << TIME_LEVEL_SHIFT)
#define TIME_NEAR_MASK (TIME_NEAR-1)
#define TIME_LEVEL_MASK (TIME_LEVEL-1)

struct timeNode;

typedef void (*OnPreparePfn_t)( timeNode *node,struct epoll_event &ev, list<timeNode *> *active );
typedef void (*OnProcessPfn_t)( timeNode *node);
//typedef void (*handler_pt) (struct timeNode *node);



struct link_list {
    list<timeNode *> head;
};
class timeWheel
{
public:
    //增加定时任务
    timeNode* add_timer(int time, OnProcessPfn_t func, void *args);
    timeNode* add_timer(int time, OnPreparePfn_t func, void *args);
    static timeWheel* GetInstance();
    void timer_update();
    void del_timer(timeNode *node);
    void clear_timer();
    void expire_timer();
    void get_timer(int time,list<timeNode *> t);
private:
    timeWheel();
    uint64_t gettime();
    void link(link_list *list, timeNode *node);
    void add_node(timeNode *node);
    void move_list(int level, int idx);
    void timer_shift();
    void dispatch_list(list<timeNode *> *current);
    void  timer_execute();
    list<timeNode *> link_clear(link_list *list);
private:
    static timeWheel *m_Instance;
    link_list m_near[TIME_NEAR];
    link_list m_t[4][TIME_LEVEL];
    struct spinlock m_lock;
    uint32_t m_time;
    uint64_t m_current_point;
};

struct timeNode
{
    uint32_t m_expire;  //超时时间

    OnPreparePfn_t pfnPrepare;
    OnProcessPfn_t pfnProcess;
    bool m_Timeout;
    bool m_cancel; //是否取消
    void *pArg; // routine
};
class stPoll_t;
class stPollItem_t : public timeNode
{
public:
    struct pollfd *pSelf;
    stPoll_t *pPoll;

    struct epoll_event *stEvent;
};
class stPoll_t : public timeNode
{
public:
    struct pollfd *fds;
    nfds_t nfds; // typedef unsigned long int nfds_t;

    stPollItem_t *pPollItems;

    int iAllEventDetach;

    int iEpollFd;

    int iRaiseCnt;


};

#endif // TIMEWHEEL_H
