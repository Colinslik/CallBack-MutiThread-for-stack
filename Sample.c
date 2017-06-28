#include "Sample.h"

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
HANDLE hMutex1 = NULL;
HANDLE hEvent1 = NULL;
HANDLE hEvent2 = NULL;
HANDLE hThread1 = NULL, hThread2 = NULL;
HANDLE hEvent_1[2];
HANDLE hEvent_2[2];
#else
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
#endif

//stack container pointer
extern my_pair* HEAD;

bool run = 1;

//sleep for win32 and linux
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
void delay(unsigned long ms)
{
	Sleep(ms);
}
#else  /* presume POSIX */
void delay(unsigned long ms)
{
	usleep(ms);
}
#endif 

bool consoleHandler(int signal){
	if (signal == SIGINT){
		printf("Signal caught2\n");
	}
	printf("signal:%d  SIGINT: %d\n", signal, SIGINT);
	exit(1);
}

//Exception handle 1

void CtrlHandler(int sig){
	printf("Signal caught\n");
	consoleHandler(sig);
}

//pair struct search method by key
my_pair* pair_find(char *key){

	my_pair *_ptr = HEAD;
	if (_ptr){
		for (; _ptr != NULL; _ptr = (*_ptr).next){
			if (strcmp((*_ptr).key, key) == 0) return _ptr;
		}
	}
	return NULL;
}

//free resource accupied by stack container
void destructor_pair(){

	my_pair *_ptr, *_tail;

	printf("\nRelease Resource.\n");
	for (_ptr = HEAD; _ptr;)
	{
		_tail = _ptr;
		if (_ptr->next) {
			_ptr = _ptr->next;
			free(_tail);
		}
		else{
			free(_tail);
			break;
		}
	}

}

//Random create string pair to push to stack
void random_push(CBfun2 fun){
	char key[KEY_LENGTH+1];
	char value[VALUE_LENGTH+1];
	char temp[2];
	time_t t;
	my_pair *_ptr;

	int count = 0;

	srand((unsigned)time(&t));

	while (run){
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
		WaitForSingleObject(hMutex1, INFINITE);
#else
		pthread_mutex_lock(&mutex1);
#endif
		strcpy(key, "\0");
		strcpy(value, "\0");
		strcpy(temp, "\0");
		_ptr = HEAD;
		for (unsigned i = 0; i < 10; i++){
			for (unsigned j = 0; j < KEY_LENGTH; j++){
				temp[0] = (char)('A' + (rand() % 26));
				temp[1] = '\0';
				strcat(key, temp);
			}
			if ((_ptr = pair_find(key)) == NULL) break; //check if key exist or not
			else {
				strcpy(key, "\0");
			}
		}
		strcpy(temp, "\0");
		for (unsigned i = 0; i < VALUE_LENGTH; i++){
			temp[0] = (char)('A' + (rand() % 26));
			temp[1] = '\0';
			strcat(value, temp);
		}
//		printf("\n\nPUSH item KEY: %s   Value: %s\n\n", key, value);
		if (!fun(key, value)){//if push fail, destory container and restart.
			destructor_pair();
		}
		count++;
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
		ReleaseMutex(hMutex1);
#else
		pthread_mutex_unlock(&mutex1);
#endif
		if (count > 5) {
			count = 0;
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
			SetEvent(hEvent1);
		}
		else SetEvent(hEvent2);
#else
			pthread_cond_signal (&cond1);
		}
		delay(10);
#endif
	}
	printf("\n\nPUSH Thread is terminated.\n\n");
}
//Pop entry from stack
void recursive_pop(CBfun3 fun){
	char *value;
	while (run){
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
		WaitForMultipleObjects(2, hEvent_1, FALSE, INFINITE);
		WaitForSingleObject(hMutex1, INFINITE);
#else
		pthread_mutex_lock(&mutex1);
		pthread_cond_wait(&cond1, &mutex1);
#endif
		while (HEAD && (value = fun()) != NULL){
//			printf("POP Value: %s\n", value);
		}
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
		ReleaseMutex(hMutex1);
#else
		pthread_mutex_unlock(&mutex1);
#endif
	}
	printf("\n\nPOP Thread is terminated.\n\n");
}

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
DWORD WINAPI Thread1(LPVOID lpParam){
	random_push(lpParam);
	return 0;
}
DWORD WINAPI Thread2(LPVOID lpParam){
	recursive_pop(lpParam);
	return 0;
}
#endif

void Thread_main(CBfun fun){

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
	//	HANDLE hThread1, hThread2;
	DWORD threadID1, threadID2;

	hMutex1 = CreateMutex(NULL, FALSE, "MyMutex1");
	hEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread1 = CreateThread(NULL, // security attributes ( default if NULL )
		0, // stack SIZE default if 0
		Thread1, // Start Address
		(LPVOID)fun.Push, // input data
		0, // creational flag ( start if  0 )
		&threadID1); // thread ID

	hThread2 = CreateThread(NULL, // security attributes ( default if NULL )
		0, // stack SIZE default if 0
		Thread2, // Start Address
		(LPVOID)fun.Pop, // input data
		0, // creational flag ( start if  0 )
		&threadID2); // thread ID

	hEvent_1[0] = hEvent1;
	hEvent_1[1] = hThread1;
	hEvent_2[0] = hEvent2;
	hEvent_2[1] = hThread1;

#else
	void *ret;
	pthread_t thread1;
	pthread_t thread2;

	pthread_create(&thread1, NULL, (void *)random_push, (void*)fun.Push); //thread 1 for push entries

	pthread_create(&thread2, NULL, (void *)recursive_pop, (void*)fun.Pop); // thread 2 for pop entries
#endif
	clock_t start, now;
	signal(SIGINT, CtrlHandler);

	start = clock();

	while (run){//printf entries in stack
		if (HEAD && HEAD != 0) {
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
			WaitForMultipleObjects(2, hEvent_2, FALSE, INFINITE);
			WaitForSingleObject(hMutex1, INFINITE);
			fun.Print();
			ReleaseMutex(hMutex1);
#else
			pthread_mutex_lock(&mutex1);
			fun.Print();
			pthread_mutex_unlock(&mutex1);
#endif			
		}
		else {
//			printf("\n\nStack is empty!\n\n");
		}
		now = clock();
		printf("\n\nTIME is %f\n\n", (now - start) / (double)(CLOCKS_PER_SEC));
		if (((now - start) / (double)(CLOCKS_PER_SEC)) > TERMINATE_TIME) run = !run;
		delay(10);
	}

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
	WaitForSingleObject(hThread1, INFINITE);
	CloseHandle(hThread1);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread2);
	CloseHandle(hEvent1);
	CloseHandle(hEvent2);
	CloseHandle(hMutex1);
#else
	pthread_cond_broadcast(&cond1);
	pthread_join(thread1, &ret);
	pthread_join(thread2, &ret);
	pthread_mutex_destroy(&mutex1);
	pthread_cond_destroy(&cond1);
#endif
	destructor_pair();
	printf("\n\nTerminate.\n\n");
}