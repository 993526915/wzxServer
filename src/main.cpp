
#include"socket/co_socktool.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <stack>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include<stdio.h>
#include <string>
#include<iostream>
#include<thread>
#include"myserver.h"
#include"CDetectModel.h"
#include"RelationModel.h"
using namespace std;

static int g_listen_fd = -1;
typedef int NCALLBACK(int fd, int event, void *arg);
void *client_loop(void *args)
{
    co_enable_hook_sys();
    stEnv_t *env = (stEnv_t *)args;
    myServer server(env);
    server.run();
}
int co_accept(int fd, struct sockaddr *addr, socklen_t *len );
void *accept_routine( void * args)
{
    co_enable_hook_sys();
    printf("accept_routine\n");
    fflush(stdout);
    stEnv_t *event = (stEnv_t *)args;
    for(;;)
    {
        struct sockaddr_in addr; //maybe sockaddr_un;
        memset( &addr,0,sizeof(addr) );
        socklen_t len = sizeof(addr);

        int fd = co_accept(g_listen_fd, (struct sockaddr *)&addr, &len);
        if( fd < 0 )
        {
            struct pollfd pf = { 0 };
            pf.fd = g_listen_fd;
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll( co_get_epoll_ct(),&pf,1,1000 );
            continue;
        }
        SockUtil::setNoBlocked(fd);
        SockUtil::setNoDelay(fd);
        std::cout << "accept success" << endl;
        stEnv_t *newclient = new stEnv_t;
        newclient->fd=fd;
        co_create( &newclient->co,NULL,client_loop,newclient);
        co_resume(newclient->co);
    }
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <time.h>
int main()
{
    int  connfd, n;
    const char *ip = "127.0.0.1";
    int port = atoi( "8888" );
    g_listen_fd = SockUtil::co_listen(port,ip);
    printf("listen %d %s:%d\n",g_listen_fd,ip,port);
    stEnv_t *event = new stEnv_t;
    event->fd = -1;
    event->msgLength=0;
    stCoRoutine_t *accept_co = NULL;
    co_create( &accept_co,NULL,accept_routine,event);
    co_resume( accept_co );
    co_eventloop( co_get_epoll_ct(),0,0 );
    return 0;

	// struct tm t;   //tm结构指针
	// time_t now;  //声明time_t类型变量
	// now = (int)time(NULL);
    // char* curr_time = ctime(&now);
    //  cout << curr_time <<endl;  // Mon Apr 05 15:23:17 2021    
}
