#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#pragma once
#endif
#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#define TERMINATE_TIME 10
#else
#include <pthread.h>
#define TERMINATE_TIME 1
#endif


//sleep for win32 and linux
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)

#include <windows.h>

void delay(unsigned long ms)
{
	Sleep(ms);
}

#else  /* presume POSIX */

#include <unistd.h>
#include <stdlib.h>

void delay(unsigned long ms)
{
	usleep(ms * 1000);
}

#endif 

typedef void(*CBfun)(void);

//pair struct
struct pair;

struct pair* HEAD;

//Exception handle 2

bool consoleHandler(int signal){
//#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
//	if (signal == CTRL_C_EVENT){
//#else
	if (signal == SIGINT){
//#endif
		printf("Signal caught2\n");
		//exit(0);
	}
	printf("signal:%d  SIGINT: %d\n",signal, SIGINT);
	exit(1);
}

//Exception handle 1

void CtrlHandler(int sig){
	printf("Signal caught\n");
	consoleHandler(sig);
}

bool push(char*, char*);

char* pop();

struct pair* pair_find(char *);

void destructor_pair();

void random_push();

void recursive_pop();

void stack_printf();

void Thread_main(CBfun);

#endif //__SAMPLE_H__