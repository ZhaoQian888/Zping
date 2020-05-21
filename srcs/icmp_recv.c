#include"icmp_recv.h"

int icmp_decode(char * buf,int len){
    int ipheader_len;
    struct ip* ip_header=(struct ip*)buf;
    ipheader_len=ip_header->ip_hl*4;
    struct icmp* icmp=(struct icmp*)(buf+ipheader_len);
    len-=ipheader_len;
    if(len<8){
        fprintf(stderr, "Invalid icmp packet.Its length is less than 8\n");
        return -1;
    }
    if((icmp->icmp_type==ICMP_ECHOREPLY)&&(icmp->icmp_id==(pid&0xffff))){
        if((icmp->icmp_seq<0)||(icmp->icmp_seq>PACKET_MAX)){
            fprintf(stderr,"icmp packet seq is out of range!\n");
            return -1;
        }
        ping_packet[icmp->icmp_seq].flag=0;
        struct timeval send_time=ping_packet[icmp->icmp_seq].send_time;
        struct timeval recv_time;
        gettimeofday(&recv_time,NULL);
        struct timeval offset_time=cal_time_offset(send_time,recv_time);
        int rtt=offset_time.tv_sec*1000+offset_time.tv_usec/1000;
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%d ms\n",
        len, inet_ntoa(ip_header->ip_src), icmp->icmp_seq, ip_header->ip_ttl, rtt);        
    }else{
        fprintf(stderr, "Invalid ICMP packet! Its id is not matched!\n");
        return -1;
    }
    return 0;
}

void ping_recv(struct recv_para *arg){
    int *alive=arg->alive;
    int rawsock=arg->rawsock;
    struct timeval tv;
    tv.tv_usec=200;
    tv.tv_sec=0;
    fd_set read_fd;
    char recv_buf[512];
    memset(recv_buf,0,sizeof(recv_buf));
    while(*alive==1){
        int ret=0;
        FD_ZERO(&read_fd);
        FD_SET(rawsock,&read_fd);
        ret=select(rawsock+1,&read_fd,NULL,NULL,&tv);
        switch(ret){
            case -1:
                fprintf(stderr,"fail to select!\n");
                break;
            case 0:
                break;
            default:
                {
                    int size=recv(rawsock,recv_buf,sizeof(recv_buf),0);
                    if(size<0){
                        fprintf(stderr,"recv data fail!\n");
                        continue;
                    }
                    ret=icmp_decode(recv_buf,size);
                    if(ret==-1){
                        continue;
                    }
                    recv_count++;
                }
        }
    }
}

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