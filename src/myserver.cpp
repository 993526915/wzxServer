#include "myserver.h"
map<pair<string,string>,int> myServer::m_userMap;

myServer::myServer(stEnv_t *env)
{
    this->m_env = env;
}

void myServer::run()
{
    while(1)
    {
        struct pollfd pf = { 0 };
        pf.fd = m_env->fd;
        pf.events = (POLLIN|POLLERR|POLLHUP);
        co_poll( co_get_epoll_ct(),&pf,1,1000);
        if(pf.revents & POLLIN)
        {
            if(-1 == co_recvData(m_env))//处理请求 客户端退出的话
            {
                close(m_env->fd);
                cout << "client leave! : "<<m_env->fd << endl;
                break;
            }
        }
        if(pf.revents & POLLHUP)
        {
            cout << "POLLHUP" << endl;
        }
        if(pf.revents & POLLERR)
        {
            cout << "POLLERR" << endl;
        }



    }
}


int myServer::co_work(stEnv_t *env,dataHeader *header)
{
    //printf("接收到包头，命令：%d，数据长度：%d\n",header->m_cmd,header->m_dataLength);
    switch (header->m_cmd)
    {
        case NOOPERATION:
        {
            cout << "NOOPERATION"<<endl;
            break;
        }
        case ADDFACE :
        {
            cout << "ADDFACE" << endl;
            shared_ptr<Event> commend = make_shared<LoginAddFaceEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case GETGROUP :
        {
            cout << "GETGROUP" << endl;
            shared_ptr<Event> commend = make_shared<getGroupListEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case GETUSER :
        {
            cout << "GETUSER" << endl;
            shared_ptr<Event> commend = make_shared<getuserListEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case DELUSER :
        {
            cout << "DELUSER" << endl;
            shared_ptr<Event> commend = make_shared<delUserEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case DELGROUP :
        {
            cout << "DELGROUP" << endl;
            shared_ptr<Event> commend = make_shared<delGroupEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case LOGIN:
        {
            cout << "LOGIN" << endl;
            shared_ptr<Event> commend = make_shared<LoginUserEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case DETECT:
        {
            cout << "DETECTFACE" << endl;
            shared_ptr<detectFaceEvent> commend = make_shared<detectFaceEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case MESSAGE_CREATE:
        {
            cout << "MESSAGE_CREATE"<<endl;
            shared_ptr<MessageCreateEvent> commend = make_shared<MessageCreateEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case MESSAGE_SEND:
        {
            cout << "MESSAGE_SEND"<<endl;
            shared_ptr<MessageSendEvent> commend = make_shared<MessageSendEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case MESSAGE_HISTORY:
        {
            //cout << "MESSAGE_HISTORY"<<endl;
            shared_ptr<MessageHistoryEvent> commend = make_shared<MessageHistoryEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case MESSAGR_RECENT_SESSION:
        {
            //cout << "MESSAGR_RECENT_SESSION"<<endl;
            shared_ptr<RescentSessionEvent> commend = make_shared<RescentSessionEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case ONLINE:
        {
            cout << "ONLINE"<<endl;
            shared_ptr<Event> commend = make_shared<LoginOnlineEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case OFFLINE:
        {
            cout << "OFFLINE"<<endl;
            shared_ptr<Event> commend = make_shared<LoginOfflineEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case CHECK_ISONLINE:
        {
            //cout << "CHECK_ISONLINE"<<endl;
            shared_ptr<Event> commend = make_shared<checkOnlineEvent>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case LOGIN_TOUXIANG:
        {
            cout << "LOGIN_TOUXIANG"<<endl;
            shared_ptr<Event> commend = make_shared<LoginTouXiang>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case CHANGE_TOUXIANG:
        {
            cout << "CHANGE_TOUXIANG"<<endl;
            shared_ptr<Event> commend = make_shared<ChangeTouXiang>();
            commend->doEvent(env,header,m_userMap);
            break;
        }
        case GET_TOUXIANG:
        {
            cout << "GET_TOUXIANG"<<endl;
            shared_ptr<Event> commend = make_shared<GetTouXiang>();
            commend->doEvent(env,header,m_userMap);
            break;
        }

    }
}
int myServer::co_recvData(stEnv_t *env)
{
    //接收客户端发送的数据
    co_enable_hook_sys();
    //memset(env->rbuffer,0,BUFFER_LENGTH);
    int recv_len = read(env->fd, env->rbuffer, BUFFER_LENGTH);
    if(recv_len <= 0)
    {
        printf("fd : %d 与服务器断开连接,任务结束\n",env->fd);
        return -1;
    }
    //将接收缓冲区的数据拷贝到消息缓冲区
    memcpy(env->msgBuffer+env->msgLength, env->rbuffer, recv_len);
    //消息缓冲区的数据末尾后移
    env->msgLength += recv_len;
    //判断消息缓冲区的数据长度是否大于等于包头长度
    while(env->msgLength >= sizeof(dataHeader))//处理粘包问题
    {
        //选出包头数据
        dataHeader* header = (dataHeader*)env->msgBuffer;
        //判断消息缓冲区内数据长度是否大于等于报文长度 避免少包问题
        if(env->msgLength >= header->m_dataLength)
        {
            //计算出消息缓冲区内剩余未处理数据的长度
            int size = env->msgLength - header->m_dataLength;
            //响应数据
            co_work(env,header);
            //将消息缓冲区剩余未处理的数据前移
            memcpy(env->msgBuffer, env->msgBuffer + header->m_dataLength, size);

            //消息缓冲区的数据末尾前移
            env->msgLength = size;
        }
        else
        {
            //消息缓冲区数据不足
            break;
        }
    }

    return 0;
}
