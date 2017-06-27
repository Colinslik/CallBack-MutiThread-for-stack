#include "Sample_cpp.h"

std::function< void(void)> CallBack;

//show all entries in stack
void stack_printf(void){
	my_pair *_ptr = HEAD;
	std::cout << std::endl << std::endl << "Top  of stack." << std::endl << std::endl;

	std::cout << std::endl << std::endl << "HEAD ori: " << HEAD << std::endl << std::endl;
	for (; _ptr && _ptr != NULL; _ptr = _ptr->getNext())
	{
		std::cout << std::endl <<"KEY: " << _ptr->getKey() <<"Value: " << _ptr->getValue() <<std::endl;
	}
	std::cout <<std::endl <<"END  of stack." <<std::endl;
}

int main(int argc, char** argv)
{
	CallBack = stack_printf;
	Thread_main(CallBack);
	getchar();
	return 0;
}