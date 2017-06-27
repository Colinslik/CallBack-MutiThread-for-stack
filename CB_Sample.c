#ifndef __SAMPLE_H__
#define __SAMPLE_H__
#include "Sample.h"
#endif

#include <stdio.h>

//pair struct
typedef struct pair{
	char* key;
	char* value;
	struct pair *next;
}my_pair;


//stack container pointer
my_pair* HEAD = NULL;

typedef void(*Fun)(void);

void Thread_main(Fun);

//show all entries in stack
void stack_printf(void){
	my_pair *_ptr = HEAD;
	printf("\n\nTop  of stack.\n\n");
	for (; _ptr && _ptr != NULL; _ptr = _ptr->next)
	{
		printf("KEY: %s    Value: %s\n", _ptr->key, _ptr->value);
	}
	printf("\nEND  of stack.\n");
}

int main(int argc, char** argv)
{
	Fun cb;
	cb = stack_printf;
	Thread_main(cb);
	getchar();
	return 0;
}