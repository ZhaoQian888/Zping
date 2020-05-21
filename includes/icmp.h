
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
pid_t pid;
void icmp_create(struct icmp* icmp_header,int seq, int total_len);
unsigned short calculate_cksum(unsigned short *addr,int len);
