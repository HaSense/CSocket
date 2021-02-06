#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//POSIX 운영 체제 API에 대한 액세스를 제공하는 헤더 파일
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[]) 
{
	pthread_t t_id;
	int thread_param=5;
	void * thr_ret;			// 작업이 완료된 스레드로 부터 값을 전달받을 때 사용할 예정
	
	//스레드 생성
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	}; 	
	/* pthread_join() :첫번째 매개변수의 thread id가 종료될 때 까지 기다리는 함수
	   첫번째 매개변수 : 스레드 id
	   두번째 매개변수 : 스레드 함수가 리턴될 때 전달하는 포인터 변수의 주소값, 이를 이용하여
	                    수행된 작업스레드로 부터 값을 전달 받을 수 있다.
	*/
	if(pthread_join(t_id, &thr_ret)!=0)
	{
		puts("pthread_join() error");
		return -1;
	};

	printf("Thread return message: %s \n", (char*)thr_ret); // 작업스레드의 결과를 화면에 출력
	free(thr_ret);
	return 0;
}

void* thread_main(void *arg) 
{
	int i;
	int cnt=*((int*)arg);  // 메인함수로 부터 받아온 thread_param 값을 cnt로 전달
	char * msg=(char *)malloc(sizeof(char)*50); // 50bytes 문자열공간을 힙메모리에 만들고 
	strcpy(msg, "Hello, I'am thread~ \n");

	for(i=0; i<cnt; i++) // cnt가 5 이므로 5번 반복
	{
		sleep(1);  // 1초 지연
		puts("running thread");	 
	}
	return (void*)msg; //만들어진 메세지의 주소값을 void 포인터로 만들면서 main함수로 전달, 
	                   //main에서 해당 주소를 char*로 읽으면 문자열을 읽어 낼 수 있음
}
