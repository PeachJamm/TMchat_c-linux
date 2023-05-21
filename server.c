#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define clientA 0
#define clientB 1
#define max_client 2  //最大连接客户端数量
#define SIZE 1024

typedef struct clientNew
{
    char     *addr;
    int      sockid;
    uint16_t port;
} client_new; //存放所连客户端的地址和socket等信息

void client_new_init(client_new c_new[])
{
    int i = 0;
    for (; i < max_client; i++)
    {
        c_new[i].addr = NULL;
        c_new[i].sockid = 0;
        c_new[i].port = 0;
    }
}

void setsocket_noblock(client_new c_new[])
{
    //设置所连的两个客户端socket非阻塞
    int flags = fcntl(c_new[clientA].sockid, F_GETFL, 0);
    fcntl(c_new[clientA].sockid, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(c_new[clientB].sockid, F_GETFL, 0);
    fcntl(c_new[clientB].sockid, F_SETFL, flags | O_NONBLOCK);
}

void read_clientA(client_new c_new[], int *flag)
{
    char receive[SIZE] = {0};
    int n = read(c_new[clientA].sockid, receive, sizeof(receive));
    if (n > 0)
    {
        time_t timep;
        time(&timep);
        if (strcmp(receive, "Quit") == 0)
        {
            printf("ip=%s %s 用户发起退出\n", c_new[clientA].addr, ctime(&timep));
            write(c_new[clientB].sockid, receive, strlen(receive));
            usleep(1000);
            *flag = 0;
            return;
        }
        printf("ip=%s %s: ", c_new[clientA].addr, ctime(&timep));
        printf("%s\n", receive);
        write(c_new[clientB].sockid, receive, n); // 将A客户端发来的信息转发给B客户端
        fflush(stdout); // 刷新标准输出缓冲区
    }
}

void read_clientB(client_new c_new[], int *flag)
{
    char receive[SIZE] = {0};
    int n = read(c_new[clientB].sockid, receive, sizeof(receive));
    if (n > 0)
    {
        time_t timep;
        time(&timep);
        if (strcmp(receive, "Quit\n") == 0)
        {
            printf("ip=%s %s 用户发起退出\n", c_new[clientA].addr, ctime(&timep));
            write(c_new[clientA].sockid, receive, strlen(receive));
            usleep(1000);
            *flag = 0;
            return;
        }
        printf("ip=%s: %s:", c_new[clientB].addr, ctime(&timep));
        printf("%s\n", receive);
        write(c_new[clientA].sockid, receive, n); // 将B客户端发来的信息转发给A客户端
        fflush(stdout); // 刷新标准输出缓冲区
    }
}

void communication(client_new c_new[])
{
    int flag = 1;
    while (flag)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(c_new[clientA].sockid, &fds);
        FD_SET(c_new[clientB].sockid, &fds);

        int max_fd = (c_new[clientA].sockid > c_new[clientB].sockid) ? c_new[clientA].sockid : c_new[clientB].sockid;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(c_new[clientA].sockid, &fds))
        {
            read_clientA(c_new, &flag);
        }

        if (FD_ISSET(c_new[clientB].sockid, &fds))
        {
            read_clientB(c_new, &flag);
        }
    }

    close(c_new[clientA].sockid);
    close(c_new[clientB].sockid);
}



int internet(client_new c_new[])
{
    struct sockaddr_in sockaddr;
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_port=htons(5188);
    sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    int server_sockid=socket(AF_INET,SOCK_STREAM,0);
    const int on=1;
    if(setsockopt(server_sockid,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)//设置端口可重复利用
    {
        printf("setsockopt failed\n");
        return 0;
    }
    if(bind(server_sockid,(struct sockaddr *)&sockaddr,sizeof(sockaddr))<0)
    {
        printf("bind failed\n");
        return 0;
    }
    if(listen(server_sockid,SOMAXCONN)<0)
    {
        printf("listen failed\n");
        return 0;
    }
    struct sockaddr_in other_sock_addr;
    socklen_t other_sock_addr_len=sizeof(other_sock_addr);
    int j=0;
    while(j!=max_client)//连接两个客户端
    {
        int sockid_client=accept(server_sockid,(struct sockaddr *)&other_sock_addr,&other_sock_addr_len);
        c_new[j].sockid =sockid_client;
        c_new[j].addr=inet_ntoa(other_sock_addr.sin_addr);
        c_new[j].port=ntohs(other_sock_addr.sin_port);
        printf("ip=%s,port=%d  已连接\n",c_new[j].addr,c_new[j].port);
        j++;
    }
    return server_sockid;
}


int main()
{
    client_new c_new[max_client];        //定义结构体数组
    client_new_init(c_new);              //初始化结构体数组
    int server_sockid = internet(c_new); //网络连接并返回服务器socket
    setsocket_noblock(c_new);            //设置所连的两个客户端socket非阻塞
    communication(c_new);                //通信
    return 0;
}