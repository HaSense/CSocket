#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define TRUE 1

void error_handling(char *message);     //에러 메시지 출력을 위한 함수입니다.

int main(int argc, char const *argv[])
{
    int server_sock;        //서버소켓의 상태정보를 확인할 값입니다. 성공 시 디스크립트번호, 실패 시 -1
    int client_sock;        //클라이언트 소켓의 상태정보를 확인할 값입니다.

    //int opt = TRUE;

    struct sockaddr_in server_addr;   //C언어에서 인터넷 주소체계를 저정한 구조체 입니다. 즉, 서버의 IP주소를 뜻합니다.
    struct sockaddr_in client_addr;   //클라이언트 IP주소 입니다. C언어는 객체가 없습니다. 구조체가 클래스의 데이터부분만 담당합니다.
    socklen_t client_addr_size;       //클라이언트 주소의

    char message[] = "Hello World!";

    if(argc!=2){
        printf("사용법 : %s <port>\n", argv[0]);
        exit(1);
    }

    server_sock=socket(PF_INET, SOCK_STREAM, 0);  //성공 시 서버소켓을 제어할 디스크립트 번호를 얻게 됩니다.
    if(server_sock == -1){                        //-1을 리턴받으면 소켓생성이 실패했다는 뜻입니다. 원인을 알아봐야 됩니다.
        error_handling("socket() error");
    }

    memset(&server_addr, 0, sizeof(server_addr));    //성공했다면 서버소켓을 만들 변수를 초기화합니다.
    server_addr.sin_family = AF_INET;                //IPV4방식으로 주소체계를 초기화 합니다.
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);   //현재 서버의 IP를 호스트바이트순에서 long형 네트워크 바이트순으로 초기화 합니다.
    server_addr.sin_port=htons(atoi(argv[1]));      //문자열로 읽어온 포트번호를 정수값으로 변환한 후 호스트바이트 순에서 short형 네트워크
                                                    //바이트 순으로 초기화 합니다.

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){    //초기화된 서버주소체계를 바인딩합니다.
        error_handling("bind() error");
    }

    if(listen(server_sock, 5) == -1){       //클라이언트의 연결을 기다립니다. 단, 실패 시 에러를 출력합니다.
        error_handling("listen() error");
    }

    client_addr_size = sizeof(client_addr);
    client_sock=accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);  //클라이언트가 연결된면 클라이언트를 관리할 드스크립트 번호를 받습니다.
    if(client_sock == -1){
        error_handling("accept() error");
    }

    write(client_sock, message, sizeof(message));
    
    close(client_sock);
    close(server_sock);

    //setsockopt(server_sock , SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
