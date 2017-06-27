#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#pragma once
#endif
#ifndef __SAMPLE_CPP_H__
#define __SAMPLE_CPP_H__

#include <iostream>
#include <string>
#include <functional>
#include <csignal> 
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <random>

//pair object
class my_pair{
public:
	my_pair();
	~my_pair();
	void setKey(std::string);
	void setValue(std::string);
	void setNext(my_pair *);
	std::string getKey();
	std::string getValue();
	my_pair *getNext();
private:
	std::string _key;
	std::string _value;
	my_pair *_next;
};

static my_pair* HEAD;

//sleep for milliseconds
void delay(unsigned long);

//Exception handle
bool consoleHandler(int);

void CtrlHandler(int);
//=====================

bool push(std::string, std::string);

std::string pop();

my_pair* pair_find(std::string);

void destructor_pair();

void random_push();

void recursive_pop();

void stack_printf();

void Thread_main(std::function< void(void)>);

#endif //__SAMPLE_H__