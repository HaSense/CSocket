#include <stdio.h>
#include <pthread.h>
#include <unistd.h>		//POSIX 운영 체제 API에 대한 액세스를 제공하는 헤더 파일
#include <semaphore.h>

void * read(void * arg);	// 스레드로 전달된 변수값을 읽어오는 함수
void * accu(void * arg);	// 읽어온 변수값의 합을 계산하는 함수
static sem_t sem_one;	// 첫번째 세마포어
static sem_t sem_two;	// 두번째 세마포어
static int num;		// 프로그램 실행과 동시에 변수를 메모리에 등록하기 위해 정적(static)으로 생성
                    // 정적으로 만들면 프로그램 종료시에 해당 변수가 메모리에서 소멸된다.

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;				//스레드 id 2개 생성
	/* 세마포어가 뮤텍스와 다른점은 뮤텍스는 스레드와 관련된 것이고 
	   세마포어는 프로세스와 관련이 있다. 즉, 세마포어는 프로세스들의 동시접근을 막는다.
	   세마포어의 세번째 매개변수는 동시접근 가능한 스레드의 개수를 입력할 수 있는데 
	   3을 입력하면  3, 2, 1 3개의 스레드를 허용한다.
	   1을 입력하면 한개의 스레드만 허용하고 이는 뮤텍스와 동일하다. 이를 바이너리(binary)세마포어라 한다.
	*/
	sem_init(&sem_one, 0, 0);	/* 세마포어 초기화 함수
								  첫번째 매개변수 : 세마포어 id
								  두번째 매개변수 : 0 - 하나의 프로세스에 접근 가능한 세마포어 생성
								                   1이상 - 둘 이상의 프로세스에 접근 가능한 세마포어 생성
								  세번째 매개변수 : 허용가능한 스레드 개수 지정 */
	sem_init(&sem_two, 0, 1);	

	pthread_create(&id_t1, NULL, read, NULL); // 변수를 입력받는 스레드 생성
	pthread_create(&id_t2, NULL, accu, NULL); // 연산을 수행하는 스레드 생성

	pthread_join(id_t1, NULL);	//스레드 종료 기다림
	pthread_join(id_t2, NULL);  //스레드 종료 기다림

	sem_destroy(&sem_one); // 세마포어 리소스 반환
	sem_destroy(&sem_two); // 세마포어 리소스 반환
	return 0;
}

void * read(void * arg)
{
	int i;
	for(i=0; i<5; i++)
	{
		fputs("Input num: ", stdout);

		sem_wait(&sem_two);	// 현재 함수가 호출될 당시 sem_two함수의 value가 0이라면 더이상 감소하지 않고 block되어 기다린다.
		                   // 초기 sem_post(&sem_two)가 1의 값을 가지고 있으므로 수행되지만 값은 0이 되어버리고
						   // 혹 한번 더 수행된다면 block된다. 하지만 accu함수에서 post함수가 수행된다면 다시 1이 되므로 수행되고
						   // value값을 0으로 감소 시킨다.
		scanf("%d", &num);
		sem_post(&sem_one); // post함수가 수행되어 sem_one 세마포어의  value가 1로 증가 됩니다.
	}
	return NULL;	
}
void * accu(void * arg)
{
	int sum=0, i;
	for(i=0; i<5; i++)
	{
		sem_wait(&sem_one); //세마포어 sem_one이 1로 증가되어 있다면 진입가능. 그렇지 않다면 대기
		sum+=num;
		sem_post(&sem_two); //sem_two value값을 1증가 시킴
	}
	printf("Result: %d \n", sum);
	return NULL;
}