#include "Sample_cpp.h"

extern my_pair* HEAD;

int main(int argc, char** argv)
{
	CallBack cb;
	cb.setPrint([]() {
		my_pair *_ptr = HEAD;
		std::cout << std::endl << std::endl << "Top  of stack." << std::endl << std::endl;
		for (; _ptr && _ptr != NULL; _ptr = _ptr->getNext())
		{
			std::cout << std::endl << "KEY: " << _ptr->getKey() << "   Value: " << _ptr->getValue() << std::endl;
		}
		std::cout << std::endl << "END  of stack." << std::endl;
	});

	cb.setPush([=](std::string key, std::string value)->bool{
		my_pair *_ptr = HEAD;
		if (!_ptr){//if Head pointer has already been initialized.
			if ((_ptr = new my_pair) != NULL)
			{
				_ptr->setKey(key);
				_ptr->setValue(value);
				HEAD = _ptr;
				return 1;
			}
			return 0;
		}
		else{//if Head pointer has not yet been initialized.
			for (; _ptr && _ptr->getNext() != NULL; _ptr = _ptr->getNext());
			my_pair* obj = new my_pair;
			if (obj)
			{
				_ptr->setNext(obj);
				obj->setKey(key);
				obj->setValue(value);
				return 1;
			}
			return 0;
		}
	});

	cb.setPop([]()->std::string{
		my_pair *_ptr = HEAD, *_pre_ptr = HEAD;
		if (_ptr->getNext() != NULL) {
			_pre_ptr = _ptr;
			_ptr = _ptr->getNext();
		}
		for (; _ptr->getNext() != NULL; _ptr = _ptr->getNext()) _pre_ptr = _pre_ptr->getNext();
		std::string value(_ptr->getValue());
		_pre_ptr->setNext(NULL);
		if (_pre_ptr == _ptr) HEAD = NULL;//if pop the last entry, let Head pointer set to NULL
		delete _ptr;
		return value;
	});

	Thread_main(cb);
	getchar();
	return 0;
}