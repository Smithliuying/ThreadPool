#pragma once
#include <vector>
#include <utility>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <future>

//#include "Condition.hh"

class ThreadPool {
public:
	static const int kInitThreadsSize = 3;
	enum taskPriorityE { level0, level1, level2, };
	typedef std::function<void()> Task;
	typedef std::pair<taskPriorityE, Task> TaskPair;

	ThreadPool();
	~ThreadPool();

	void start();
	void stop();
	void addTask(const Task&);
	void addTask(const TaskPair&);

	template<typename F, typename Obj, typename ...Args>
	auto submit(F &&f, Obj && obj, Args &&...args) -> std::future <decltype((obj->*f)(args...))>
	{
		auto func = std::bind(f, obj, args...);
		auto task_ptr = std::make_shared<std::packaged_task<decltype((obj->*f)(args...))()>>(func);
		std::function<void()> warpper_func = [task_ptr]()
		{
			(*task_ptr)();
		};
		TaskPair taskPair(level2, warpper_func);
		m_tasks.push(taskPair);
		m_cond.notify_one();
		return task_ptr->get_future();
	}


private:
	ThreadPool(const ThreadPool&);//禁止复制拷贝.
	const ThreadPool& operator=(const ThreadPool&);

	struct TaskPriorityCmp
	{
		bool operator()(const ThreadPool::TaskPair p1, const ThreadPool::TaskPair p2)
		{
			return p1.first > p2.first; //first的小值优先
		}
	};

	void threadLoop();
	Task take();

	typedef std::vector<std::thread*> Threads;
	typedef std::priority_queue<TaskPair, std::vector<TaskPair>, TaskPriorityCmp> Tasks;

	Threads m_threads;
	Tasks m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_cond;
	bool m_isStarted;
};