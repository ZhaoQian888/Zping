#include"icmp_send.h"
void send_query(struct send_para *arg){
    int *alive=arg->alive;
    int rawsock=arg->rawsock;
    struct sockaddr_in *dest=arg->dest;
    char send_buf[128];
    memset(send_buf,0,sizeof(send_buf));
    gettimeofday(&start_time,NULL);
    while(*alive==1){
        int size=0;
        gettimeofday(&(ping_packet[send_count].send_time),NULL);
        icmp_create((struct icmp*)send_buf,send_count,64);
        size=sendto(rawsock,send_buf,64,0,(struct sockaddr*)dest,sizeof(dest));
        send_count++;
        if(size<0){
            fprintf(stderr, "send icmp packet fail!\n");
            continue;  
        }
        sleep(1);
    }
}
