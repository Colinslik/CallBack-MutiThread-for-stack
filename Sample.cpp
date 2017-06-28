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

CallBack::CallBack(){}
CallBack::~CallBack(){}
void CallBack::setPrint(std::function< void(void)> print){ _print = print; }
void CallBack::setPush(std::function< bool(std::string, std::string)> push){ _push = push; }
void CallBack::setPop(std::function< std::string(void)> pop){ _pop = pop; }
std::function< void(void)> CallBack::getPrint(){ return _print; }
std::function< bool(std::string, std::string)> CallBack::getPush(){ return _push; }
std::function< std::string(void)> CallBack::getPop(){ return _pop; }

//length of key and value
const unsigned int KEY_LENGTH = 2;
const unsigned int VALUE_LENGTH = 10;

std::atomic<bool> run{ true };

std::mutex gMutex;
std::condition_variable gCV1;

my_pair* HEAD = NULL;

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
void random_push(std::function< bool(std::string, std::string)> CallBack){
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
		if (!CallBack(key, value)){//if push fail, destory container and restart.
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
void recursive_pop(std::function< std::string(void)> CallBack){
	std::string value = "";
	while (run){

		std::unique_lock<std::mutex> mLock(gMutex);
		gCV1.wait(mLock);
		while (HEAD && (!(value = CallBack()).empty())){
			std::cout << "POP Value:" << value << std::endl;
//			delay(5000);
		}
		mLock.unlock();
	}
	std::cout << std::endl << std::endl << "POP Thread is terminated." << std::endl << std::endl;
}

void Thread_main(CallBack CBfun){
	signal(SIGINT, CtrlHandler);

	auto start = std::chrono::system_clock::now();

	std::function< bool(std::string, std::string)> Push = CBfun.getPush();
	std::function< std::string(void)> Pop = CBfun.getPop();
	std::function< void(void)> Print = CBfun.getPrint();

	std::thread mThread1(random_push, Push); //thread 1 for push entries

	std::thread mThread2(recursive_pop, Pop); // thread 2 for pop entries

	while (run){//printf entries in stack
		if (HEAD && HEAD != 0) {
			std::unique_lock<std::mutex> mLock(gMutex);
//			stack_printf();
			Print();
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