#include"icmp_recv.h"
#include<netdb.h>
#include<pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
//进程指针，用来判断是否进程被杀死
int alive=1;
//
void icmp_sigint(int signo)
{
    alive = 0;
    gettimeofday(&end_time, NULL);
    time_interval = cal_time_offset(start_time, end_time);
}
//
void ping_stats_show()
{
    long time = time_interval.tv_sec*1000+time_interval.tv_usec/1000;
    /*注意除数不能为零，这里send_count有可能为零，所以运行时提示错误*/
    printf("%d packets transmitted, %d recieved, %d%c packet loss, time %ldms\n",
        send_count, recv_count, (send_count-recv_count)*100/send_count, '%', time);
}
//socket
int rawsocket;    
int main(int argc,char *argv[]){
    pid=getpid();
    //协议指针
    struct protoent* protocol;
    //地址字符串
    char dese_addr_str[80];
    memset(dese_addr_str,0,sizeof(dese_addr_str));
    //两个线程
    pthread_t send_id,recv_id;
    //参数检查
    if(argc<2){
        printf("NO ip address \n");
        return -1;
    }
    //获取协议类型ICMP
    protocol=getprotobyname("icmp");
    if(protocol == NULL){
        printf("Fail to getprotobyname!\n");
        return -1;
    }
    //获取地址信息
    memcpy(dese_addr_str,argv[1],strlen(argv[1])+1);
    //创建一个socket
    rawsocket=socket(AF_INET,SOCK_RAW,protocol->p_proto);
    if(rawsocket < 0){
        printf("Fail to create socket!\n");
        return -1;
    }
    struct hostent* host = NULL;
    int size=128*1024;
    setsockopt(rawsocket,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
    struct sockaddr_in dest;
    bzero(&dest,sizeof(dest));
    dest.sin_family=AF_INET;
    unsigned int inaddr=inet_addr(argv[1]);
    //输入的是域名
    if(inaddr==INADDR_NONE){
        host = gethostbyname(argv[1]);
        if(host==NULL){
            printf("Fail to gethostbyname!\n");
            return -1;
        }
        
        memcpy((char*)&dest.sin_addr, host->h_addr, host->h_length);
    }else{
        memcpy((char*)&dest.sin_addr,&inaddr,sizeof(inaddr));
    }
    inaddr =dest.sin_addr.s_addr;
    printf("ping %s,(%d.%d.%d.%d)56 bytes of data.\n",dese_addr_str,(inaddr&0x000000ff),
    (inaddr&0x0000ff00)>>8,(inaddr&0x00ff0000)>>16,(inaddr&0xff000000)>>24);


    signal(SIGINT, icmp_sigint);
    //void send_query(int *alive,int rawsock,const struct sockaddr_in *dest)
    struct send_para send_arg;
    send_arg.alive=&alive;
    send_arg.rawsock=rawsocket;
    send_arg.dest=&dest;
    if(pthread_create(&send_id,NULL,(void*)send_query,&(send_arg))){
        printf("Fail to create ping send thread!\n");
        return -1;
    }
    struct recv_para recv_arg;
    recv_arg.alive=&alive;
    recv_arg.rawsock=rawsocket;
    if(pthread_create(&recv_id,NULL,(void*)ping_recv,&(recv_arg))){
        printf("Fail to create ping recv thread!\n");
        return -1;
    }
    pthread_join(send_id, NULL);//等待send ping线程结束后进程再结束
    pthread_join(recv_id, NULL);//等待recv ping线程结束后进程再结束

    ping_stats_show();

    close(rawsocket);
    return 0;

}