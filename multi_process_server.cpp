#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>


void callback(int num)
{
    while(1)
    {
        pid_t pid = waitpid(-1, NULL, WNOHANG);
        if(pid <= 0)
        {
            printf("子进程正在运行, 或者子进程被回收完毕了\n");
            break;
        }
        printf("child die, pid = %d\n", pid);
    }
}

int childWork(int cfd){
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    int len = read(cfd, buff, sizeof(buff));
    if(len > 0){
        printf("client says:%s\n",buff);
        write(cfd, buff, sizeof(buff));
    }
    else if(len == 0){
        printf("client disconnected\n");
    }
    else if(len < 0){
        perror("read");
    }
    return len;
}


int main(){

    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd == -1){
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
  

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5555);

    //允许重用本地地址和端口
    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR, &flag, sizeof(flag));

    int ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1){
        perror("bind");
        return -1;
    }
    
    //设置监听
    ret = listen(listenfd, 128);
    if(ret == -1){
        perror("listen");
        return -1;
    }

    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = callback;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);








    while(1){

        struct sockaddr_in childaddr;
        socklen_t client = sizeof(childaddr);
        int cfd = accept(listenfd, (struct sockaddr*)&childaddr, &client);
        if(cfd==-1){
            if(errno == EINTR){
                continue;
            }
            perror("accept");
            exit(0);
        }

        char ip[24] = {0};
        printf("client's id:%s, port:%d\n",inet_ntop(AF_INET, &childaddr.sin_addr.s_addr, ip,sizeof(ip)), ntohs(childaddr.sin_port));

        pid_t pid = fork();
        if(pid == 0){
            close(listenfd);
            while(1){
                int ret = childWork(cfd);
                if(ret <=0){
                    break;
                }
            }
            close(cfd);
            exit(0);
        }
        else if(pid > 0){
            close(cfd);
        }



    }





    return 0;
}