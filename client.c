#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

void do_read(int sockid,int *flag)
{
	char receive[100]={0};
	int r_size=read(sockid,receive,sizeof(receive));
	if(strcmp(receive,"quit\n")==0)
	{
		printf("对方已结束聊天\n");
		*flag=0;
		return;
	}
	if(r_size>0)
	{
		printf("\t\t\t");
		fputs(receive,stdout);
	}
}

void do_write(int sockid,int *flag)
{
	char send[100]={0};
	int w_size=read(0,send,sizeof(send));
	if(strcmp(send,"quit\n")==0)
	{
		printf("您已下线\n");
		write(sockid,send,sizeof(send));
		*flag=0;
		return;
	 }
    if(w_size>0)
	{
		write(sockid,send,sizeof(send));
		memset(send,0,strlen(send));
	}
}

int internet()
{
	int  flag=1;
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(5188);
	addr.sin_addr.s_addr=inet_addr("10.0.1.52");
	int sockid=socket(AF_INET,SOCK_STREAM,0);
	socklen_t addrlen=sizeof(addr);
	if(connect(sockid,(struct sockaddr *)&addr,addrlen)<0)
	{
		printf("connect\n");
		return 0;
	}
	int flags=fcntl(sockid,F_GETFL,0);
	fcntl(sockid,F_SETFL,flags|O_NONBLOCK);

	flags=fcntl(0,F_GETFL,0);
	fcntl(0,F_SETFL,flags|O_NONBLOCK);
	while(flag)
	{
		do_read(sockid,&flag);
		do_write(sockid,&flag);
	}
	close(sockid);
	return 0;
}

int main()
{
	internet();
	return 0;
}

