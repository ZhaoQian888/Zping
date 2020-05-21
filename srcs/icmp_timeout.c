#include"icmp_recv.h"
// 超时计数器
void check_timeout(int *alive){
    //时间间隔
    struct timeval interval;
    //当前时间
    struct timeval now;
    //超时记录buf
    int timeout[PACKET_MAX];
    for(int j=0;j<PACKET_MAX;j++){
        timeout[j]=-1;
    }
    //未收到中断信号
    while(*alive==1){
        // 遍历packetinfobuffer
        for(int i=0;i<PACKET_MAX;i++){
            //已发送但未接收回复
            if(ping_packet[i].sendflag==1 && ping_packet[i].recvflag==0){
                //计算时间间隔
                gettimeofday(&now,NULL);
                interval=cal_time_offset(ping_packet[i].send_time,now);
                int ms=interval.tv_sec*1000+interval.tv_usec/1000;
                //时间间隔大于500ms且还未输出超时信息
                if(ms>500&&timeout[i]==-1){
                    //超时信息置位
                    timeout[i]=0;
                    printf("Request timeout for icmp_seq %d\n",i);
                }

            }
        }
        usleep(100);
    }
}