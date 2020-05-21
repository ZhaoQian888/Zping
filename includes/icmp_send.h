#include"icmp.h"
#include<string.h>
#include<sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#define PACKET_MAX 64
typedef struct ping_packet_info{
    struct timeval send_time;
    struct timeval recv_time;
    int flag;
    int seq;
}ping_packet_info;

ping_packet_info ping_packet[PACKET_MAX];

struct timeval start_time;
struct timeval end_time;
struct timeval time_interval;

int send_count;
int recv_count;

typedef struct send_para{
    int *alive;
    int rawsock;
    struct sockaddr_in *dest;
}send_para;
void send_query(struct send_para *arg);