#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
	
#define BUF_SIZE 100
#define NAME_SIZE 20
	
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);
	
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];
	
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;			// 스레드로 부터 받을 리턴변수로 사용될 변수
	if(argc!=4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	 }
	
	sprintf(name, "[%s]", argv[3]);
	sock=socket(PF_INET, SOCK_STREAM, 0);		//소켓생성
	
	memset(&serv_addr, 0, sizeof(serv_addr));	//주소체계 생성
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)		//서버에 연결
		error_handling("connect() error");
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);	// 전송스레드 생성
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);	// 수신스레드 생성
	pthread_join(snd_thread, &thread_return);	// 메인스레드 종료전 snd_thread 기다림 
	pthread_join(rcv_thread, &thread_return);   // 메인스레드 종료전 rcv_thread 기다림 
	close(sock);  
	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
	int sock=*((int*)arg);	//클라이언트 소켓정보 call by reference
	char name_msg[NAME_SIZE+BUF_SIZE];	// 이름과 메시지가 모두 들어갈 크기
	while(1) // 무한루프
	{
		fgets(msg, BUF_SIZE, stdin); //키보드로 메시지 입력 
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))  // q Q 입력시 빠짐
		{
			close(sock);
			exit(0);
		}
		sprintf(name_msg,"%s %s", name, msg); //메시지 출력  name변수와 msg변수를 첫번째 매개변수 name_msg로 합침
		write(sock, name_msg, strlen(name_msg)); //소켓에 write
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);	//소켓 정보 call by reference
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
	while(1)	//문한루프
	{
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);  //서버로부터 메시지 읽어옮
		if(str_len==-1) 		//EOF면 리턴
			return (void*)-1;
		name_msg[str_len]=0;	//문자열 배열 마지막에 널(\0)문자 삽입
		fputs(name_msg, stdout); //stdout(모니터)에 출력
	}
	return NULL;
}
	
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
