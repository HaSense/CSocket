#include <stdio.h>
#include <unistd.h>		//POSIX 운영 체제 API에 대한 액세스를 제공하는 헤더 파일
#include <pthread.h>	//리눅스에서 thread사용 시 꼭 사용해야 하는 라이브러리

void* thread_main(void *arg); //스레드 함수의 내용 , 함수선언부

int main(int argc, char *argv[]) 
{
	pthread_t t_id;		//스레드 id를 선언하기 위한 구조체변수 pthread_t
	int thread_param=5; // main stack에서 만든 그냥 일반 int변수, thread_main함수로 call by reference 예정
	
	/* pthread_create() : c에서 thread를 만들어 주는 매우 중요한 함수
	   첫번째 매개변수 : id
	   두번째 매개변수 : 스레드에부여할 특성정보, 대부분 NULL
	   세번째 매개변수 : 스레드 이름
	   네번째 매개변수 : 스레드 함수로 전달할 변수또는 구조체

	   리턴 : 0이면 성공, 실패면 0 이외의 값
	*/
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)  //스레드 실패시 예외처리
	{
		puts("pthread_create() error");
		return -1;
	}; 	
	sleep(10);  //메인스레드가 종료되면 생성된 스레드들도 모두 종료된 따라서 강제로 main스레드를 10초 sleep해서 
	            //다른 스레드들의 종료를 기다리게 함. 나중에 pthread_join 또는 pthread_detach를 사용함
	puts("end of main"); //메인스레드가 종료됨을 화면에 출력함
	return 0;
}

/* 스레드 함수의 main 함수, 이름은 수정가능, 구조체나 변수를 만든다음 void*형으로 전달한다. */
void* thread_main(void *arg) 
{
	int i;
	int cnt=*((int*)arg); // 정수형 포인트형으로 형변환(casting)하여 매개변수로 전달된 값을 복사함

	for(i=0; i<cnt; i++)
	{
		sleep(1);  // 1초 cpu시간을 정지
		puts("running thread");	 //puts--> printf의 저수준 함수
	}
	return NULL;
}
