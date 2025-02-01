// 远程备份debug等级以上的日志信息-发送端
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../util.hpp"

extern xqlog::Util::JsonData* g_conf_data;
void start_backup(const std::string &message)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1){
        std::cout << __FILE__ << __LINE__ << "socket error" << std::endl;
        perror(NULL);
    }
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(g_conf_data->backup_port);
    inet_pton(AF_INET, g_conf_data->backup_addr.c_str(), &addr_in.sin_addr);

    // 发送备份的日志
    if (sendto(sockfd, message.c_str(), message.size(), 0,
               (struct sockaddr *)&addr_in, sizeof(addr_in)) == -1){
        std::cout << __FILE__ << __LINE__ << "sendto error" << std::endl;
        perror(NULL);
    }
    close(sockfd);

    // 1. create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        std::cout << __FILE__ << __LINE__ <<"socket error : " << strerror(errno)<< std::endl;
        perror(NULL);
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(g_conf_data->backup_port);
    inet_aton(g_conf_data->backup_addr.c_str(), &(server.sin_addr));

    int cnt = 5;
    while(connect(sock, (struct sockaddr*)&server, sizeof(server)) != 0){
        sleep(1);
        cout << "正在尝试重连,重连次数还有: " << cnt-- << endl;
        if(cnt <= 0){
            std::cout << __FILE__ << __LINE__ <<"connect error : " << strerror(errno)<< std::endl;
            perror(NULL);
            break;
        }
    }

    // 3. 连接成功
    char buffer[1024];
    if(-1==write(sock, message.c_str(), message.size())){
        std::cout << __FILE__ << __LINE__ <<"send to server error : " << strerror(errno)<< std::endl;
        perror(NULL);
    }
    close(sock);
}