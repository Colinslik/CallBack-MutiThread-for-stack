#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

//length of key and value
#define KEY_LENGTH 2
#define VALUE_LENGTH 10
#define TERMINATE_TIME 10

//pair struct
typedef struct pair{
	char* key;
	char* value;
	struct pair *next;
}my_pair;

typedef void(*CBfun1)(void);
typedef bool(*CBfun2)(char*, char*);
typedef char* (*CBfun3)(void);

typedef struct CallBack{
	CBfun1 Print;
	CBfun2 Push;
	CBfun3 Pop;
}CBfun;

void delay(unsigned long);

//Exception handle 2

bool consoleHandler(int);

//Exception handle 1

void CtrlHandler(int);

my_pair* pair_find(char *);

void destructor_pair();

void random_push(CBfun2);

void recursive_pop(CBfun3);

void Thread_main(CBfun);

#endif //__SAMPLE_H__