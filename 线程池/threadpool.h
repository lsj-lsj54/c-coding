#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<iostream>
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
#include<functional>
#include <chrono>
#include<unordered_map>

//Any类型 可以接收任意数据类型
class Any
{
public:
	Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;
	//这个构造函数可以让Any类型接收任意其他数据
	template<typename T>
	Any(T data) :base_(std::make_unique<Derive<T>>(data))
	{
	}
	template<typename T>
	T cast_()
	{
		//我们怎么从base_找到它指向的Derive对象 取出data_成员变量
		//基类指针=》派生类指针
		Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
		if (pd == nullptr)
		{
			throw "type is unmatch!!!";
		}
		return pd->data_;
	}

private:
	//基类类型
	class Base
	{
	public:
		virtual ~Base() = default;
	};
	//派生类类型
	template<typename T>
	class Derive :public Base
	{
	public:
		Derive(T data) :data_(data) {}
		T data_;
	};
private:
	std::unique_ptr<Base> base_;
};

class Semaphore
{
public:
	Semaphore(int limit = 0)
		:resLimit_(limit)
	{
	}
	~Semaphore() = default;

	//获取一个信号量资源
	void wait()
	{
		std::unique_lock<std::mutex>lock(mtx_);
		cond_.wait(lock, [&]()->bool {return resLimit_ > 0; });
		--resLimit_;
	}
	//增加一个信号量资源
	void post()
	{
		std::unique_lock<std::mutex>lock(mtx_);
		++resLimit_;
		cond_.notify_all();
	}
private:
	int resLimit_;
	std::mutex mtx_;
	std::condition_variable cond_;
};

//实现接收提交到线程池的task任务执行完成后的返回值类型
class Task;
class Result
{
public:
	Result(std::shared_ptr<Task>task, bool isValid = true);
	~Result() = default;

	//问题一： setVal方法，获取任务执行完的返回值
	void setVal(Any any);
	//问题二： get方法，用户调用这个方法获取task的返回值
	Any get();
private:
	Any any_;//存储任务返回值
	Semaphore sem_;//线程通信信号量
	std::shared_ptr<Task>task_;//指向对应返回值的任务对象
	std::atomic_bool isValid_;//返回值是否有效
};

//线程池支持模式
enum class PoolMode
{
	MODE_FIXED,//固定数量的线程
	MODE_CACHED//线程数量可以动态增长
};


//任务抽象基类
//用户可以自定义任务类型，从Task继承，重写run方法
class Task
{
public:
	Task()
		:result_(nullptr)
	{
	}
	~Task() = default;
	void exec();
	void setResult(Result* res);
	virtual Any run() = 0;
private:
	Result* result_;
};

//线程类型
class Thread
{
public:
	using ThreadFunc = std::function<void(int)>;

	Thread(ThreadFunc);
	~Thread();
	//启动线程
	void start();
	int getId()const;
private:
	ThreadFunc func_;
	static int generateId_;
	int threadId_;//保存线程id
};


//线程池类型
class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	//设置模式
	void setMode(PoolMode);
	//设置task任务队列上限阈值
	void setTaskQueMaxThreshHold(int);
	//给线程池提交任务
	Result submitTask(std::shared_ptr<Task>);
	//开启线程池
	void start(int initThreadSize = 4);
	//设置线程数量上限
	void setThreadSizeThreshHold(int);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

private:
	//定义线程函数
	void threadFunc(int);
	//检查pool的运行状态
	bool checkRunningState()const;
private:
	//std::vector<std::unique_ptr<Thread>>threads_;//线程列表
	std::unordered_map<int, std::unique_ptr<Thread>>threads_;


	size_t initThreadSize_;//初始的线程数量
	size_t threadSizeThreshHold_; //线程数量上限
	std::atomic_int curThreadSize_; //线程当前数量
	std::atomic_int idleThreadSize_;//记录空闲线程的数量

	std::queue<std::shared_ptr<Task>>taskQue_;//任务队列
	std::atomic_int taskSize_;//任务数量
	int taskQueMaxThreshHold_;//任务队列数量上限

	std::mutex taskQueMtx_;//保证任务队列的线程安全
	std::condition_variable notFull_;//任务队列不满
	std::condition_variable notEmpty_;//任务队列不空
	std::condition_variable exitCond_;//等待线程资源全部回收

	PoolMode poolMode_;
	std::atomic_bool isPoolRunning_;//表示当前线程池的启动状态

};


#endif