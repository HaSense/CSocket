#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD	100

void * thread_inc(void * arg);	// num변수를 증가 시키는 함수
void * thread_des(void * arg);  // num변수를 감소 시키는 함수

long long num=0;
pthread_mutex_t mutex;

int main(int argc, char *argv[]) 
{
	pthread_t thread_id[NUM_THREAD]; // 100개의 스레드id를 담을 변수 선언
	int i;
	
	/* 뮤텍스 생성 및 초기화
	   첫번째 매개변수 : 뮤텍스 생성시 변수 주소값 전달
	   두번째 매개변수 : 뮤텍스 특성정보 전달, 일반적으로 NULL
	*/
	pthread_mutex_init(&mutex, NULL);  

	for(i=0; i<NUM_THREAD; i++)
	{
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL); //50번 thread_inc 스레드 만듦
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL); //50번 thread_dec 스레드 만듦	
	}	

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);  // 100개의 스레드들이 모두 종료되기를 기다림

	printf("result: %lld \n", num);		// 스레드 동기화 여부 확인, 0이면 동시접근 없이 정상수행됨
	pthread_mutex_destroy(&mutex);	// 뮤텍스 종료 및 리소스 반환
	return 0;
}

void * thread_inc(void * arg) 
{
	int i;
	pthread_mutex_lock(&mutex);	//뮤텍스 실행 임계영역에 진입하면 문을 잠궈버림, 하나의 스레드만 통과
	for(i=0; i<50000000; i++)
		num+=1;			// 50000000번 1증가
	pthread_mutex_unlock(&mutex); //임계영역을 수행하고 나오면 락을 풀어줌, 이때부터 다른 스레드가 진입가능함
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
	{
		pthread_mutex_lock(&mutex);  //뮤텍스락
		num-=1;
		pthread_mutex_unlock(&mutex); //뮤텍스 언락
	}
	return NULL;
}

/* [실행법] : -D_REENTRANT 옵션은 안전하지 않은 스레드함수를 자동으로 안전한 스레드함수로 변화해준다.

$ gcc mutex.c -D_REENTRANT -o mutex -lpthread
$ ./mutex
result: 0 

*/
