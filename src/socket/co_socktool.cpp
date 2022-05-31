#include"co_socktool.h"
std::string SockUtil::inet_ntoa(struct in_addr &addr)
{
    char buf[20];
    unsigned char *p = (unsigned char *) &(addr);
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
    return string(buf);
}
int SockUtil::co_connect(const char *host, uint16_t port)
{
    co_enable_hook_sys();
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int nIP = 0;
    if( !host || '\0' == *host
            || 0 == strcmp(host,"0") || 0 == strcmp(host,"0.0.0.0")
            || 0 == strcmp(host,"*")
      )
    {
        nIP = htonl(INADDR_ANY);
    }
    else
    {
        nIP = inet_addr(host);
    }
    addr.sin_addr.s_addr = nIP;
    return connect(fd,(struct sockaddr*)&addr,sizeof(addr));
}
int SockUtil::co_listen(const uint16_t port, const char *local_ip , int back_log )
{
    co_enable_hook_sys();
    int fd = -1;
    if ((fd = (int) socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        cout << "创建套接字失败:" << fd << endl;
        return -1;
    }

    setReuseable(fd, true, false);
    setNoBlocked(fd);
    if (bindSock(fd, local_ip, port) == -1) {
        close(fd);
        return -1;
    }

    //开始监听
    if (::listen(fd, back_log) == -1) {
        cout << "开始监听失败:port " << port << " used " << endl;;
        close(fd);
        return -1;
    }

    return fd;
}

int SockUtil::co_read( int fd, void *buf, size_t nbyte )
{
    co_enable_hook_sys();
    ssize_t readret = read( fd,(char*)buf ,nbyte );

    if( readret < 0 )
    {
        cout << "read error!" << endl;
    }

    return readret;
}
int SockUtil::co_write(int fd, const void *buf, size_t nbyte)
{
    co_enable_hook_sys();
    int totalLen = 0;
    int writeLen = write(fd, buf, nbyte);
    if(writeLen < 0)
    {
        cout << "write error!" << endl;
        return writeLen;
    }
    if( writeLen > 0 )
    {
        totalLen += writeLen;
    }
    while( totalLen < (int)nbyte )
    {

        writeLen = write( fd,(const char *)buf + writeLen,nbyte - writeLen );

        if( writeLen <= 0 )
        {
            break;
        }
        totalLen += writeLen ;
    }
    return totalLen;
}

int SockUtil::bindSock(int fd, const char *local_op, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(local_op);
    bzero(&(addr.sin_zero), sizeof addr.sin_zero);
    //绑定监听
    if (::bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        cout << "绑定套接字失败:" << fd << endl;
        return -1;
    }
    return 0;
}

int SockUtil::setNoDelay(int fd, bool on)
{
    co_enable_hook_sys();
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 NoDelay 失败!" << endl;
    }
    return ret;
}
int SockUtil::setNoBlocked(int fd, bool noblock )
{
    int iFlags;
    int ret = -1;
    int opt = noblock ? 1 : 0;
    if(opt)
    {
        iFlags = fcntl(fd, F_GETFL, 0);
        iFlags |= O_NONBLOCK;
    //    iFlags |= O_NDELAY;
        ret = fcntl(fd, F_SETFL, iFlags);
    }
    else
    {
        iFlags = fcntl(fd, F_GETFL, 0);
        iFlags &= ~O_NONBLOCK;
        ret = fcntl(fd, F_SETFL, iFlags);
    }
    if (ret == -1) {
        cout << "设置 NoBlocked 失败!" << endl;
    }
    return ret;
}
int SockUtil::setRecvBuf(int fd, int size )
{
    co_enable_hook_sys();
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
        cout << "设置接收缓冲区失败!" << endl;
    }
    return ret;
}

int SockUtil::setSendBuf(int fd, int size)
{
    int ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(size));
    if (ret == -1) {
        cout << "设置发送缓冲区失败!" << endl;
    }
    return ret;
}
int SockUtil::setReuseable(int fd, bool on , bool reuse_port)
{
    co_enable_hook_sys();
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 SO_REUSEADDR 失败!"<< endl;
        return ret;
    }
#if defined(SO_REUSEPORT)
    if (reuse_port) {
        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
        if (ret == -1) {
            cout << "设置 SO_REUSEPORT 失败!"<<endl;
        }
    }
#endif
    return ret;
}
int SockUtil::setKeepAlive(int fd, bool on)
{
    co_enable_hook_sys();
    int opt = on ? 1 : 0;
    int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, static_cast<socklen_t>(sizeof(opt)));
    if (ret == -1) {
        cout << "设置 SO_KEEPALIVE 失败!" << endl;
    }
    return ret;
}

int SockUtil::setCloseWait(int sock, int second )
{
    linger m_sLinger;
    //在调用closesocket()时还有数据未发送完，允许等待
    // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
    m_sLinger.l_onoff = (second > 0);
    m_sLinger.l_linger = second; //设置等待时间为x秒
    int ret = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &m_sLinger, sizeof(linger));
    if (ret == -1) {
#ifndef _WIN32
        cout << "设置 SO_LINGER 失败!" << endl;
#endif
    }
    return ret;
}

bool getSystemDomainIP(const char *host, sockaddr &item) {
    struct addrinfo *answer = nullptr;
    //阻塞式dns解析，可能被打断
    int ret = -1;
    do {
        ret = getaddrinfo(host, nullptr, nullptr, &answer);
    } while (ret == -1);

    if (!answer) {
        cout << "域名解析失败:" << host;
        return false;
    }
    item = *(answer->ai_addr);
    freeaddrinfo(answer);
    return true;
}

bool SockUtil::getDomainIP(const char *host, uint16_t port, struct sockaddr &addr)
{
    bool flag = getSystemDomainIP(host, addr);
    if (flag) {
        ((sockaddr_in *) &addr)->sin_port = htons(port);
    }
    return flag;
}

std::string SockUtil::get_local_ip(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取local ip and port
    memset(&addr, 0, sizeof(addr));
    if (0 == getsockname(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return SockUtil::inet_ntoa(addr_v4->sin_addr);
        }
    }
    return "";
}


uint16_t SockUtil::get_local_port(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getsockname(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return ntohs(addr_v4->sin_port);
        }
    }
    return 0;
}


std::string SockUtil::get_peer_ip(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getpeername(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return SockUtil::inet_ntoa(addr_v4->sin_addr);
        }
    }
    return "";
}


uint16_t SockUtil::get_peer_port(int sock)
{
    struct sockaddr addr;
    struct sockaddr_in *addr_v4;
    socklen_t addr_len = sizeof(addr);
    //获取remote ip and port
    if (0 == getpeername(sock, &addr, &addr_len)) {
        if (addr.sa_family == AF_INET) {
            addr_v4 = (sockaddr_in *) &addr;
            return ntohs(addr_v4->sin_port);
        }
    }
    return 0;
}


