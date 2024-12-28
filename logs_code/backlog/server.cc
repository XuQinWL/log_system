#include <string>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const std::string filename = "./logfile.log";
bool file_exist(const std::string &name)
{
    struct stat exist;
    return (stat(name.c_str(), &exist) == 0);
}
int main(int argc, char *argv[])
{
    if (argc != 2){
        perror("usage: ./server port");
        assert(false);
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(atoi(argv[1]));
    if (bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr))<0)
    {
        perror("bind error");
        assert(false);
    }

    FILE *fp = NULL;
    while (true)
    {
        char buf[1024] = {0};
        struct sockaddr_in cli_addr;
        socklen_t cli_socklen = sizeof(cli_addr);
        int read_byte = recvfrom(sockfd, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&cli_addr, &cli_socklen);
        if (read_byte<0){
            perror("recvfrom error");
            assert(false);
        }
        buf[read_byte] = '\n';
        buf[read_byte+1] = '\0';
        
        //写文件
        if (file_exist(filename) == false){
            fp = fopen(filename.c_str(), "ab");
            if (fp == NULL){
                perror("fopen error: ");
                assert(false);
            }
        }
        if(fp==NULL){
            perror("fp is null, Maybe you used relative paths");
            assert(false);
        }
        int write_byte = fwrite(buf, 1, read_byte+1, fp);
        if(write_byte != read_byte+1){
            perror("fwrite error: ");
            assert(false);
        }
        fflush(fp);
    }
    fclose(fp);
    close(sockfd);
    return 0;
}
