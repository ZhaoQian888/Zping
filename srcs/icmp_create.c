#include "icmp.h"

void icmp_create(struct icmp* icmp_header,int seq, int total_len){
    //icmp类型值
    icmp_header->icmp_type=ICMP_ECHO;
    //icmp代码
    icmp_header->icmp_code=0;
    //icmp校验和
    icmp_header->icmp_cksum=0;
    //   icmp_seq  : icmp_hun.ih_idseq.icd_seq
    icmp_header->icmp_seq=seq;
    //  icmp_id : 
    icmp_header->icmp_id=pid&0xffff;
    // 填充data部分
    for(int i=0;i<total_len;i++){
        icmp_header->icmp_data[i]=i;
    }
    icmp_header->icmp_cksum=calculate_cksum((unsigned short*)icmp_header,total_len);

}

unsigned short calculate_cksum(unsigned short *addr,int len){
    int sum=0;
    unsigned short *w=addr;
    unsigned short answer=0;
    //计算2字节单位和
    int i;
    for(i=len;i>=2;i-=2){
        sum+=*w;
        w++;
    }
    //处理奇数字节的情况
    if(i==1){
        *(unsigned char*)(&answer)=*(unsigned char *)w;
        sum+=answer;
    }
    //高16位和低16位相加
    sum=(sum>>16)+(sum&0xffff);
    //取高十六位
    sum+=(sum>>16);
    //取反
    answer=~sum;
    return answer;
}