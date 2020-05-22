#include"icmp_send.h"

//发送icmp数据报
void send_query(struct send_para *arg){
    //处理参数
    int *alive=arg->alive;
    int rawsock=arg->rawsock;
    struct sockaddr_in *dest=arg->dest;
    //数据报buf
    char send_buf[128];
    memset(send_buf,0,sizeof(send_buf));
    //开始时间
    gettimeofday(&start_time,NULL);
    //初始化缓存信息
    for(int i=0;i<PACKET_MAX;i++){
        ping_packet[i].sendflag=0;
        ping_packet[i].recvflag=0;
    }
    while(*alive==1){
        int size=0;
        //记录发送时间
        gettimeofday(&(ping_packet[send_count].send_time),NULL);
        //构造icmp数据包
        icmp_create((struct icmp*)send_buf,send_count,64);
        //发送数据报
        size=sendto(rawsock,send_buf,64,0,(struct sockaddr*)dest,sizeof(dest));
        if(size<0){
            fprintf(stderr, "send icmp packet fail!     :     %s\n",strerror(errno));
        }else{
            //将该标记为设置为该包已发送
            ping_packet[send_count].sendflag=1;
            //发送计数
            send_count++;
        }
        sleep(1);
    }
}
