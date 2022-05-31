#include "timewheel.h"
timeWheel* timeWheel::m_Instance = NULL;
timeWheel::timeWheel()
{
    for(int i=0;i<TIME_NEAR;i++)
    {
        m_near->head.clear();
    }
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<TIME_LEVEL;j++)
        {
            m_t[i][j].head.clear();
        }
    }
    spinlock_init(&m_lock);
    m_time = 0;
    m_current_point = gettime();
}
timeWheel* timeWheel::GetInstance()
{
     if (m_Instance == NULL) //判断是否第一调用
         m_Instance = new timeWheel();
     return m_Instance;
}

timeNode* timeWheel::add_timer(int time, OnProcessPfn_t func, void *args)
{
    timeNode *node = (timeNode *)malloc(sizeof(*node));
    spinlock_lock(&m_lock);
    node->m_expire = time+m_time;
    node->pfnProcess = func;
    node->pArg=args;
    if (time <= 0) {
        node->pfnProcess(node);
        spinlock_unlock(&m_lock);
        free(node);
        return NULL;
    }
    add_node(node);
    spinlock_unlock(&m_lock);
    return node;
}
timeNode* timeWheel::add_timer(int time, OnPreparePfn_t func, void *args)
{
    timeNode *node = (timeNode *)malloc(sizeof(*node));
    spinlock_lock(&m_lock);
    node->m_expire = time+m_time;
    node->pfnPrepare = func;
    node->pArg=args;
    if (time <= 0) {
//        node->pfnPrepare(node);
//        spinlock_unlock(&m_lock);
//        free(node);
//        return NULL;
    }
    add_node(node);
    spinlock_unlock(&m_lock);
    return node;
}

void timeWheel::timer_update()
{
    spinlock_lock(&m_lock);
    timer_execute();//这里两次timer_execute（）是因为
    timer_shift(); // 可能出现tick在第二层2处执行完timer_shift()后
    timer_execute();//时间任务被映射到第一层2处，若没有第二次则会缺失一次时间任务
    spinlock_unlock(&m_lock);
}
void timeWheel::del_timer(timeNode *node)
{
    node->m_cancel=1;
}
uint64_t timeWheel::gettime()
{
    uint64_t t;
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    t = (uint64_t)ti.tv_sec * 100;
    t += ti.tv_nsec / 10000000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (uint64_t)tv.tv_sec * 100;
    t += tv.tv_usec / 10000;
#endif
    return t;
}
void timeWheel::expire_timer()
{
    uint64_t cp = gettime();
    if (cp != m_current_point) {
        int diff = (uint32_t)(cp - m_current_point);
        m_current_point = cp;
        int i;
        for (i=0; i<diff; i++) {
            timer_update();
        }
    }
}
void timeWheel::clear_timer()
{
    for(int i=0;i<TIME_NEAR;i++)
    {
        m_near->head.clear();
    }
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<TIME_LEVEL;j++)
        {
            m_t[i][j].head.clear();
        }
    }
}
void timeWheel::link(link_list *list, timeNode *node)
{
    list->head.push_back(node);
}
void timeWheel::add_node(timeNode *node)
{
    uint32_t time=node->m_expire;
    uint32_t current_time=m_time;
    uint32_t msec = time - current_time;
    if (msec < TIME_NEAR) { //[0, 0x100)
        link(&m_near[time&TIME_NEAR_MASK],node);
    } else if (msec < (1 << (TIME_NEAR_SHIFT+TIME_LEVEL_SHIFT))) {//[0x100, 0x4000)
        link(&m_t[0][((time>>TIME_NEAR_SHIFT) & TIME_LEVEL_MASK)],node);
    } else if (msec < (1 << (TIME_NEAR_SHIFT+2*TIME_LEVEL_SHIFT))) {//[0x4000, 0x100000)
        link(&m_t[1][((time>>(TIME_NEAR_SHIFT + TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);
    } else if (msec < (1 << (TIME_NEAR_SHIFT+3*TIME_LEVEL_SHIFT))) {//[0x100000, 0x4000000)
        link(&m_t[2][((time>>(TIME_NEAR_SHIFT + 2*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);
    } else {//[0x4000000, 0xffffffff]
        link(&m_t[3][((time>>(TIME_NEAR_SHIFT + 3*TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)],node);
    }
}
void timeWheel::move_list(int level, int idx)
{
    //list<timeNode *> current = link_clear(&m_Instance->m_t[level][idx]);
    list<timeNode *> *current = &m_t[level][idx].head;
    while (!current->empty()) {
        timeNode *temp=current->front();
        current->pop_front();
        add_node(temp);
    }
    current->clear();
}
void timeWheel::timer_shift()
{
    int mask = TIME_NEAR;
    uint32_t ct = ++m_time;
    if (ct == 0) {  //因为会出现tick = 2^32-1 , delay = 3这种溢出int32数据类型的情况
        move_list(3, 0); //时间轮的m_t[3][0]处存放的就是超出2^32*20ms -->(2^32+255*64*64*64)*20ms延时的情况
    } else {
        // ct / 256
        uint32_t time = ct >> TIME_NEAR_SHIFT; // /256
        int i=0;
        // ct % 256 == 0
        while ((ct & (mask-1))==0) {//%256
            int idx=time & TIME_LEVEL_MASK;//%64
            if (idx!=0) {
                move_list(i, idx);
                break;
            }
            mask <<= TIME_LEVEL_SHIFT; // *64
            time >>= TIME_LEVEL_SHIFT; // /64
            ++i;
        }
    }
}
void timeWheel::dispatch_list(list<timeNode *> *current)
{
    while (!current->empty())
    {
        timeNode * temp = current->front();
        current->pop_front();
        if (temp->m_cancel == false)
            temp->pfnProcess(temp);
        free(temp);
    }
}
void timeWheel::timer_execute()
{
    int idx = m_time & TIME_NEAR_MASK; //%256
    while (!m_near[idx].head.empty())
    {
//        list<timeNode *> current = link_clear(&m_Instance->m_near[idx]);
        list<timeNode *> *current = &m_near[idx].head;
        spinlock_unlock(&m_lock);
        dispatch_list(current);
        current->clear();
        spinlock_lock(&m_lock);
    }
}
list<timeNode *> timeWheel::link_clear(link_list *list)
{
    std::list<timeNode *> head=list->head;
    list->head.clear();
    return head;
}
