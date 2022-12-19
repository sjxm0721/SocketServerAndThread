#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<iostream>
#include<pthread.h>

using namespace std;

//信息结构体
struct SockInfo
{
    sockaddr_in addr;
    int fd;
};

SockInfo infos[512];

void* working(void* arg) {
    SockInfo* pinfo=(SockInfo*)arg;
    //连接建立成功，打印客户端的IP和端口信息
    char ip[32];
    cout << "客户端的IP " << inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)) << " 端口: "
         << ntohs(pinfo->addr.sin_port) << endl;

    //通信

    while (1) {
        char buff[1024]={0};
        int len=recv(pinfo->fd, buff, sizeof(buff), 0);
        if (len> 0) {
            cout << "client( "<<"IP: "<<inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip))<<
                 " Port: "<<ntohs(pinfo->addr.sin_port)<< " )say: " << buff<<endl;
            send(pinfo->fd, buff, len, 0);
        } else if (len == 0) {
            cout << "客户端: "<<inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip))
                 << " 端口: "<< ntohs(pinfo->addr.sin_port)<< " 已经断开了连接..." << endl;
            break;
        } else {
            perror("recv");
            break;
        }
    }
    //关闭文件描述符
    close(pinfo->fd);
    pinfo->fd=-1;
    return NULL;
}

int main() {
    //创建监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    //绑定本地的IP port
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = INADDR_ANY;    //0=0.0.0.0
    int ret = bind(fd, (sockaddr *) &saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    //设置监听
    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

    //初始化结构体数组
    int max=sizeof(infos)/sizeof(infos[0]);
    for(int i=0;i<max;i++)
    {
      memset(&infos[i],0,sizeof(infos[i]));
      infos[i].fd=-1;
    }

    //阻塞并等待客户端的连接
        struct sockaddr_in caddr;
        int addrlen=sizeof(sockaddr_in);
        while(1)
        {
            SockInfo* pinfo;
            for(int i=0;i<max;i++)
            {
                if(infos[i].fd==-1)
                {
                    pinfo=&infos[i];
                    break;
                }
            }
            pinfo->fd = accept(fd, (sockaddr *) &pinfo->addr, (socklen_t *) &addrlen);
            if (pinfo->fd == -1) {
                perror("accept");
                break;
            }
            pthread_t tid;
            pthread_create(&tid,NULL,working,pinfo);
            pthread_detach(tid);
        }
        //关闭文件描述符
        close(fd);
    }



