#include "Sample.h"

//stack container pointer
my_pair* HEAD = NULL;

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
//stack push
bool push(char* key, char* value){
	my_pair *_ptr = HEAD;
	char* _key = (char *)malloc(KEY_LENGTH * sizeof(char));
	char* _value = (char *)malloc(VALUE_LENGTH * sizeof(char));

	strncpy(_key, key, KEY_LENGTH);
	_key[KEY_LENGTH] = '\0';
	strncpy(_value, value, VALUE_LENGTH);
	_value[VALUE_LENGTH] = '\0';
	if (!_ptr){//if Head pointer has already been initialized.
		if ((_ptr = (my_pair *)malloc(sizeof(my_pair))) != NULL)
		{
			_ptr->key = _key;
			_ptr->value = _value;
			_ptr->next = NULL;
			HEAD = _ptr;
			return 1;
		}
		return 0;
	}
	else{//if Head pointer has not yet been initialized.
		my_pair *_ptr = HEAD;
		for (; _ptr && _ptr->next != NULL; _ptr = _ptr->next);
		if ((_ptr->next = (my_pair *)malloc(sizeof(my_pair))) != NULL)
		{
			_ptr = _ptr->next;
			_ptr->key = _key;
			_ptr->value = _value;
			_ptr->next = NULL;
			return 1;
		}
		return 0;
	}
}
//stack pop
char* pop(void){
	char* _value = (char *)malloc((VALUE_LENGTH + 1) * sizeof(char));
	my_pair *_ptr = HEAD, *_pre_ptr = HEAD;
	if (_ptr->next != NULL) {
		_pre_ptr = _ptr;
		_ptr = _ptr->next;
	}
	for (; _ptr->next != NULL; _ptr = _ptr->next) _pre_ptr = _pre_ptr->next;
	strncpy(_value, _ptr->value, VALUE_LENGTH);
	_value[VALUE_LENGTH] = '\0';
	_pre_ptr->next = NULL;
	free(_ptr);
	if (_pre_ptr == _ptr) HEAD = NULL;//if pop the last entry, let Head pointer set to NULL
	return _value;
}


int main(int argc, char** argv)
{
	CBfun cb;
	cb.Print = stack_printf;
	cb.Push = push;
	cb.Pop = pop;
	Thread_main(cb);
	getchar();
	return 0;
}