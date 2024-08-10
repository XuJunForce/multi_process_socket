#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>







int main(){
    //创建通信套接字

    int fd  = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        perror("socket");
    }

    struct sockaddr_in caddr;
    caddr.sin_family=AF_INET;
    //caddr.sin_addr.s_addr = htonl('127.0.0.1');
    caddr.sin_port = htons(4444);
    inet_pton(AF_INET, "127.0.0.1", &caddr.sin_addr.s_addr);

    int ret = connect(fd,(struct sockaddr*)&caddr, sizeof(caddr));    
    if(ret == -1){
        perror("connect");
        return -1;
    }

    int number = 1;


    struct sigaction act;
    



    while(1){
        if(number == 20){
            close(fd);
            break;
        }
        //send data
        char buff[1024];
        sprintf(buff, "你好, hello,world, %d...\n",number++);
        send(fd, buff, strlen(buff + 1), 0);

        //清空数据并接收新的数据
        memset(buff,0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        if(len > 0){
            printf("server says:%s\n",buff);
        }
        else if(len==0){
            printf("server disconnected\n");
            break;
        }
        else if(len < 0){
            perror("recv");
            break;
        }

        sleep(1);
    }



    return 0 ;
}