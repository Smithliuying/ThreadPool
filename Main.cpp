#include "ThreadPool.h"
#include "MyFunction.h"
#include <utility>
#include <iostream>
using namespace std;
std::mutex g_mutex;

void priorityFunc()
{
	for (int i = 1; i < 4; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::lock_guard<std::mutex> lock(g_mutex);
		cout << "priorityFunc() [" << i << "at thread [ " << std::this_thread::get_id() << "] output" << endl;// << std::endl;
	}

}

void testFunc()
{
	// loop to print character after a random period of time
	for (int i = 1; i < 4; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::lock_guard<std::mutex> lock(g_mutex);
		cout << "testFunc() [" << i << "] at thread [ " << std::this_thread::get_id() << "] output" << endl;// << std::endl;
	}

}

class test
{
public:
	test() {}
	~test() {}
	void Add(int a)
	{
		cout << "a的值为:" << a << endl;
	}
	void Add1(int a,int b)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
		cout << "a加b的值为:" << a+b << endl;
	}
};

class test1
{
public:
	test1() {}
	~test1() {}
	void Add(int a)
	{
		cout << "a的值为:" << a << endl;
	}
};

void gAdd(int a,int b)
{
	cout << "整在执行" << a << "+" << b << endl;
	return;
}
void gAdd1(int a)
{
	return;
}
template<typename F,typename Obj, typename ...Args>
auto gen(F &&f, Obj && obj,Args &&...args)//->std::future <decltype(f(args...))>
{
	/*auto func = std::bind(std::forward<F>(f), std::forward(Args...)(args));
	func();*/
	//f(args...);
	auto func = std::bind(f, obj, args...);
	func();
	return;
}

int main()
 {
	/*ThreadPool threadPool;
	threadPool.start();

	for (int i = 0; i < 5; i++)
		threadPool.addTask(testFunc);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	threadPool.addTask(ThreadPool::TaskPair(ThreadPool::level0, priorityFunc));*/

	/*test t;
	_nmsp::NewGen ng;
	ng.gen(&test::Add1, &t, 4, 5);
	ng.run();*/
	//_nmsp::NewGen::run();
	//gen(&test::Add1,&t, 4, 5);
	test t;
	ThreadPool threadPool;
	threadPool.start();

	auto future1 = threadPool.submit(&test::Add1, &t, 4, 5);
	

	for (int i = 0; i < 1; i++)
		threadPool.addTask(testFunc);
	future1.get();
	std::this_thread::sleep_for(std::chrono::seconds(1));

	threadPool.addTask(ThreadPool::TaskPair(ThreadPool::level0, priorityFunc));

	std::cout << "main thread" << endl;
	getchar();
	return 0;
}