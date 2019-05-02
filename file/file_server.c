#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int server_socket, client_socket;
    FILE *fp;
    char buf[BUF_SIZE];
    int read_cnt;

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;

    if(argc!=2){
        printf("사용법 : %s <PORT>\n", argv[0]);
        exit(1);
    }

    fp = fopen("file_server.c", "rb");
    server_socket=socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(atoi(argv[1]));

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    client_addr_size = sizeof(client_addr);
    client_socket=accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    
    while(1){
        read_cnt=fread((void*)buf, 1, BUF_SIZE, fp);
        if(read_cnt<BUF_SIZE){
            write(client_socket, buf, read_cnt);
            break;
        }
        write(client_socket, buf, BUF_SIZE);    
    }

    shutdown(client_socket, SHUT_WR);
    read(client_socket, buf, BUF_SIZE);
    printf("Message from client: %s \n", buf);

    fclose(fp);
    close(client_socket);
    close(server_socket);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
