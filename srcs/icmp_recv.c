#include"icmp_recv.h"
//解码收到的ip包
int icmp_decode(char * buf,int len){
    int ipheader_len;
    // 读取信息ip包
    struct ip* ip_header=(struct ip*)buf;
    //计算首部长度
    ipheader_len=ip_header->ip_hl*4;
    //去掉首部，读取ip包的数据部分
    struct icmp* icmp=(struct icmp*)(buf+ipheader_len);
    //计算数据部分大小
    len-=ipheader_len;
    if(len<8){
        fprintf(stderr, "Invalid icmp packet.Its length is less than 8\n");
        return -1;
    }
    switch (icmp->icmp_type)
    {
    case ICMP_UNREACH:
        /* code */
        printf("destnation is no rachable\n");
        break;
    case ICMP_SOURCEQUENCH :
        /* code */
        printf("packet lost, slow down\n");
        break;
    case ICMP_TIMXCEED  :
        printf("time exceeded\n");
        /* code */
        break;
    case ICMP_PARAMPROB:
        /* code */
        printf("ip header bad\n");
        break;
    case ICMP_REDIRECT:
        printf("shorter route\n");
        /* code */
        break;
    default:
        break;
    }
    //检查报文类型，以及进程号是否相同
    if((icmp->icmp_type==ICMP_ECHOREPLY)&&(icmp->icmp_id==(pid&0xffff))){
        //检查包的序号
        if((icmp->icmp_seq<0)||(icmp->icmp_seq>PACKET_MAX)){
            fprintf(stderr,"icmp packet seq is out of range!\n");
            return -1;
        }
        //将读取的包序号标记为已接受状态
        ping_packet[icmp->icmp_seq].recvflag=1;
        //计算时间差
        struct timeval send_time=ping_packet[icmp->icmp_seq].send_time;
        struct timeval recv_time;
        gettimeofday(&recv_time,NULL);
        struct timeval offset_time=cal_time_offset(send_time,recv_time);
        int rtt=offset_time.tv_sec*1000+offset_time.tv_usec/1000;
        //打印信息
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%d ms\n",
        len, inet_ntoa(ip_header->ip_src), icmp->icmp_seq, ip_header->ip_ttl, rtt);        
    }else{
        //不是icmp报文。丢弃
        fprintf(stderr, "Invalid ICMP packet! Its id is not matched!\n");
        return -1;
    }
    return 0;
}
// 接收回信
void ping_recv(struct recv_para *arg){
    //处理参数
    int *alive=arg->alive;
    int rawsock=arg->rawsock;
    //select轮询间隔时间
    struct timeval tv;
    tv.tv_usec=200;
    tv.tv_sec=0;
    fd_set read_fd;
    //准备缓存
    char recv_buf[512];
    memset(recv_buf,0,sizeof(recv_buf));
    while(*alive==1){
        //设置select
        int ret=0;
        FD_ZERO(&read_fd);
        FD_SET(rawsock,&read_fd);
        //select轮询
        ret=select(rawsock+1,&read_fd,NULL,NULL,&tv);
        switch(ret){
            case -1:
                fprintf(stderr,"fail to select!\n");
                break;
            case 0:
                break;
            default:
                {
                    //读取数据报
                    int size=recv(rawsock,recv_buf,sizeof(recv_buf),0);
                    if(size<0){
                        fprintf(stderr,"recv data fail!\n");
                        continue;
                    }
                    //解码数据报
                    ret=icmp_decode(recv_buf,size);
                    if(ret==-1){
                        continue;
                    }
                    //接受收计数
                    recv_count++;
                }
        }
    }
}
//计算时间差
struct timeval cal_time_offset(struct timeval begin, struct timeval end)
{
    struct timeval ans;
    ans.tv_sec = end.tv_sec - begin.tv_sec;
    ans.tv_usec = end.tv_usec - begin.tv_usec;
    if(ans.tv_usec < 0) //如果接收时间的usec小于发送时间的usec，则向sec域借位
    {
        ans.tv_sec--;
        ans.tv_usec+=1000000;
    }
    return ans;
}