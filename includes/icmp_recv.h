#include"icmp_send.h"
#include <netinet/ip.h>
typedef struct recv_para{
    int *alive;
    int rawsock;
}recv_para;
struct timeval cal_time_offset(struct timeval begin, struct timeval end);
void ping_recv(struct recv_para *arg);
int icmp_decode(char * buf,int len);