#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);		//클라이언트 소켓의 동작을 스레드로 구현 
void send_msg(char * msg, int len); //메세지 보내는 함수
void error_handling(char * msg); 

int clnt_cnt=0;	
int clnt_socks[MAX_CLNT];		//클라이언트의 개수를 256개 수용
pthread_mutex_t mutx;	//mutex를 선언

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;		//서버와 클라이언트를 위한 디스크립트
	struct sockaddr_in serv_adr, clnt_adr;	// 서버와 클라이언트를 위한 주소 구조체변수
	int clnt_adr_sz;
	pthread_t t_id;	//스레드 id
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);	//뮤텍스 생성
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);	//서버소켓 생성

	//IPv4 주소체계 만들기
	memset(&serv_adr, 0, sizeof(serv_adr));	
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	//bind
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	//listen
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);	//클라이언트 주소체계 구조체 크기 저장, accept 함수의 세번째 매개변수로 사용
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);	//임계영역 lock
		clnt_socks[clnt_cnt++]=clnt_sock; //접속한 클라이언트의 정보를 clnt_socks배열에 차례로 저장, 0 ~ 255개 가능
		pthread_mutex_unlock(&mutx); //임계영역 unlock
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);	//클라언트의 동작을 스레드로 만듦
		pthread_detach(t_id); //다 사용된 스레드를 현 프로세스에서 떼어냄, 스레드 종료 유도 
		                     // thread_join과는 다른게 join함수가 호출되지 않아도 block되는 일은 없다. 
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)		//클라이언트 소켓의 동작
{
	int clnt_sock=*((int*)arg);		// 소켓 복사 call by reference
	int str_len=0, i;
	char msg[BUF_SIZE];
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)	//클라이언트로 부터 메시지를 읽음
		send_msg(msg, str_len);		//읽은 메시지를 str_len만큼 다시 재전송, 
	
	/*채팅을 다 끝내고 클라이언트를 종료하려면 */
	pthread_mutex_lock(&mutx);	//뮤텍스 lock
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{	
		if(clnt_sock==clnt_socks[i]) //현재 소켓값과 같은 값을 클라이언트 소켓배열에서 찾음
		{
			while(i++<clnt_cnt-1) 		
				clnt_socks[i]=clnt_socks[i+1]; //현재 소켓이 빠진 부분부터 가장 뒤부분까지 소켓집합 배열을 한칸씩 당김
			break;
		}
	}
	clnt_cnt--; //클라이언트 카운트 개수 한개 줄임
	pthread_mutex_unlock(&mutx);	//뮤텍스 unlock / 임계영역 실행 보장
	close(clnt_sock);	//클라이언트 소켓 해제
	return NULL;		//스레드 호출한 쪽으로 돌려줄 값은 없다.
}
void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);		//소켓에 write
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
