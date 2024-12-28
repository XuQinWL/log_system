#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>    
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

std::string ser_addr= "47.116.74.254";
size_t ser_port = 8080;
int main(){
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        std::cout << "socket error" << std::endl;
        return -1;
    }
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(ser_port);
    inet_pton(AF_INET, ser_addr.c_str(), &addr_in.sin_addr);
    
    //发送测试消息
    int n=0;
    std::string mes = "[asynclogger][FATAL][test.cpp-10]    测试日志";
    while(n++<100){
        sendto(sockfd, mes.c_str(), mes.size(), 0, 
                (struct sockaddr*)&addr_in, sizeof(addr_in));
    }
    close(sockfd);
    return 0;
}