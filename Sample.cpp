#include "Sample_cpp.h"

#define TERMINATE_TIME 10

my_pair::my_pair() : _key(""), _value(""), _next(NULL){}
my_pair::~my_pair(){}
void my_pair::setKey(std::string key){ _key = key; }
void my_pair::setValue(std::string value){ _value = value; }
void my_pair::setNext(my_pair * next){ _next = next; }
std::string my_pair::getKey(){ return _key; }
std::string my_pair::getValue(){ return _value; }
my_pair *my_pair::getNext(){ return _next; }


//length of key and value
const unsigned int KEY_LENGTH = 2;
const unsigned int VALUE_LENGTH = 10;

std::atomic<bool> run{ true };

std::mutex gMutex;
std::condition_variable gCV1;

void delay(unsigned long ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//Exception handle 2
bool consoleHandler(int signal){
	if (signal == SIGINT){
		std::cout <<"Signal caught2" <<std::endl;
	}
	exit(1);
}

//Exception handle 1

void CtrlHandler(int sig){
	std::cout << "Signal caught" <<std::endl;
	consoleHandler(sig);
}

//stack push
bool push(std::string key, std::string value){

	my_pair *_ptr = HEAD;

	std::cout << std::endl << std::endl << "HEAD ori: " << HEAD << std::endl << std::endl;

	if (!_ptr){//if Head pointer has already been initialized.
		if ((_ptr = new my_pair) != NULL)
		{
			_ptr->setKey(key);
			_ptr->setValue(value);
			HEAD = _ptr;

			std::cout << std::endl << std::endl << "HEAD set: " << HEAD << std::endl << std::endl;
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
}
//stack pop
std::string pop(){
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
}

//pair struct search method by key
my_pair* pair_find(std::string key){
	my_pair *_ptr = HEAD;
	if (_ptr){
		for (; _ptr != NULL; _ptr = _ptr->getNext()){
			if (key.compare(_ptr->getKey()) == 0) return _ptr;
		}
	}
	return NULL;
}

//free resource accupied by stack container
void destructor_pair(){
	my_pair *_ptr, *_tail;

	std::cout << std::endl <<"Release Resource." <<std::endl;
	for (_ptr = HEAD; _ptr;)
	{
		_tail = _ptr;
		if (_ptr->getNext()) {
			_ptr = _ptr->getNext();
			delete _tail;
		}
		else{
			delete _tail;
			break;
		}
	}
}

//Random create string pair to push to stack
void random_push(){
	std::string key = "";
	std::string value = "";
	my_pair *_ptr;

	int count = 0;

	std::random_device rd;
	std::default_random_engine gen = std::default_random_engine(rd());
	std::uniform_int_distribution<int> dis(0, 26);

	while (run){

		std::unique_lock<std::mutex> mLock(gMutex);

		key = "";
		value = "";
		_ptr = HEAD;
		for (unsigned i = 0; i < 10; i++){
			for (unsigned j = 0; j < KEY_LENGTH; j++){
				key = key + (static_cast<char>('A' + dis(gen) % 26));
			}
			if ((_ptr = pair_find(key)) == NULL) break; //check if key exist or not
			else {
				key = "";
			}
		}
		for (unsigned i = 0; i < VALUE_LENGTH; i++){
			value = value + (static_cast<char>('A' + dis(gen) % 26));
		}
		std::cout << std::endl << std::endl << "PUSH item KEY: " << key << "  Value: " << value << std::endl << std::endl;
		if (!push(key, value)){//if push fail, destory container and restart.
			destructor_pair();
		}
//		else delay(10000);
		count++;
		mLock.unlock();
		if (count > 5) {
			count = 0;
			gCV1.notify_one();
		}
		else delay(10);
	}
	std::cout << std::endl << std::endl <<"PUSH Thread is terminated." << std::endl <<std::endl;
}
//Pop entry from stack
void recursive_pop(){
	std::string value = "";
	while (run){

		std::unique_lock<std::mutex> mLock(gMutex);
		gCV1.wait(mLock);
		while ( HEAD && (!(value = pop()).empty())){
			std::cout << "POP Value:" << value << std::endl;
//			delay(5000);
		}
		mLock.unlock();
	}
	std::cout << std::endl << std::endl << "POP Thread is terminated." << std::endl << std::endl;
}
//show all entries in stack
/*void stack_printf(){
	my_pair *_ptr = HEAD;
	std::cout << std::endl << std::endl << "Top  of stack." << std::endl << std::endl;
	for (; _ptr && (_ptr != NULL); _ptr = _ptr->next)
	{
		std::cout << "KEY: " << _ptr->key << "   Value: " << _ptr->value << std::endl;
	}
	std::cout << std::endl << "END  of stack." << std::endl;
//	delay(10000);
}*/

void Thread_main(std::function< void(void)> CallBack){
	signal(SIGINT, CtrlHandler);

	auto start = std::chrono::system_clock::now();

	std::thread mThread1(random_push); //thread 1 for push entries

	std::thread mThread2(recursive_pop); // thread 2 for pop entries

	while (run){//printf entries in stack
		if (HEAD && HEAD != 0) {
			std::unique_lock<std::mutex> mLock(gMutex);
//			stack_printf();
			CallBack();
			mLock.unlock();
		}
/*		{
			[=](my_pair *_ptr) {
				for (; _ptr != NULL; _ptr = _ptr->next)
				{
					std::cout << "   PRINTER  KEY: " << _ptr->key << "   Value: " << _ptr->value  <<"   PTR: " <<_ptr << std::endl;
					delay(100);
				}
				std::cout << std::endl << std::endl << std::endl << " END  of  PRINTER " << std::endl << std::endl << std::endl;
			}(HEAD);
		*/
		else {
//			std::cout << std::endl << std::endl << "Stack is empty!" << std::endl << std::endl;
		}
		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout	<< std::endl << std::endl << "TIME is " << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
					<< std::endl <<std::endl;
		if ((double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den) > TERMINATE_TIME) run = !run;
		delay(10);
	}
//	std::cout << std::endl << std::endl << "wait for thread1!" << std::endl << std::endl;
	mThread1.join();
//	std::cout << std::endl << std::endl << "wait for thread2!" << std::endl << std::endl;
	gCV1.notify_all();
	mThread2.join();
//	std::cout << std::endl << std::endl << "Destroy stack!" << std::endl << std::endl;
	destructor_pair();
//	getchar();
//	return 0;
}