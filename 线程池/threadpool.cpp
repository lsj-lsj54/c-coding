#include"threadpool.h"
#include<functional>
#include<thread>
const int TASK_MAX_THREADHOLD = INT32_MAX;
const int THREAD_MAX_THRESHHOLD = 100;
const int THREAD_MAX_IDLE_TIME = 60;//单位：秒


///////////////////////////////////////////////////ThreadPool的方法实现

//构造函数
ThreadPool::ThreadPool()
	:initThreadSize_(0)
	,taskSize_(0)
	, taskQueMaxThreshHold_(TASK_MAX_THREADHOLD)
	,poolMode_(PoolMode::MODE_FIXED)
	,isPoolRunning_(false)
	, idleThreadSize_(0)
	, threadSizeThreshHold_(THREAD_MAX_THRESHHOLD)
	, curThreadSize_(0)
{}

//析构函数
ThreadPool::~ThreadPool()
{
	isPoolRunning_ = false;
	//notEmpty_.notify_all();
	//等待线程池里面所有的线程返回  有两种状态：阻塞 & 正在执行任务中
	std::unique_lock lock(taskQueMtx_);
	notEmpty_.notify_all();
	exitCond_.wait(lock, [&]()->bool {return threads_.size() == 0; });
}

//获取运行状态
bool ThreadPool::checkRunningState()const
{
	return isPoolRunning_;
}

//设置模式
void ThreadPool::setMode(PoolMode mode)
{
	if (checkRunningState() == false)
		poolMode_ = mode;
}

//设置task任务队列上限阈值
void ThreadPool::setTaskQueMaxThreshHold(int threshhold)
{
	if (checkRunningState() == false)
		taskQueMaxThreshHold_ = threshhold;
}

//设置线程数量上限
void ThreadPool::setThreadSizeThreshHold(int threshhold)
{
	if (checkRunningState() == false
		&& poolMode_ == PoolMode::MODE_CACHED)
		threadSizeThreshHold_ = threshhold;
}

//给线程池提交任务  用户调用该接口传入任务对象，生产任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp)
{
	//获取锁
	std::unique_lock<std::mutex>lock(taskQueMtx_);
	//线程的通信 等待任务队列有空余
	if (!notFull_.wait_for(lock, std::chrono::seconds(1),//1秒到了条件没有达到返回false
		[&]()->bool {return taskQue_.size() < (size_t)taskQueMaxThreshHold_; }))
	{
		std::cerr << "task queue is full,submit task fill" << std::endl;
		return Result(sp, false);
	}
	//如果有空余 任务放入队列 
	taskQue_.emplace(sp);
	++taskSize_;
	//因为新放了任务 任务队列不空 notempty通知
	notEmpty_.notify_all();


	//根据任务数量和空闲线程的数量 判断是否需要创建新的线程出来
	if (poolMode_ == PoolMode::MODE_CACHED
		&& taskSize_ > idleThreadSize_//任务数量>空闲线程数量
		&& curThreadSize_ < threadSizeThreshHold_)//当前线程数量小于线程数量上限
	{
		std::cout << "create new thread" << std::endl;
		//创建新线程
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
		int threadId = ptr->getId();
		threads_.emplace(threadId, std::move(ptr));
		//启动线程
		threads_[threadId]->start();
		//修改线程个数相关变量
		++curThreadSize_;
		++idleThreadSize_;
	}

	return Result(sp);
}
//开启线程池
void ThreadPool::start(int initThreadSize)
{
	//设置线程池的运行状态
	isPoolRunning_ = true;

	//记录初始线程个数
	initThreadSize_ = initThreadSize;
	curThreadSize_ = initThreadSize;

	//创建线程对象
	for (int i = 0; i < initThreadSize_; ++i)
	{
		//创建thread线程对象的时候，把线程函数给到thread线程对象
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
		int threadId = ptr->getId();
		threads_.emplace(threadId, std::move(ptr));
	}
	//启动所有线程
	for (int i = 0; i < initThreadSize_; ++i)
	{
		threads_[i]->start();//需要去执行一个线程函数
		++idleThreadSize_;//记录初始空闲线程
	}
}
//定义线程函数  线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc(int threadid)//线程函数返回 相应的线程也就结束了
{
	auto lastTime = std::chrono::high_resolution_clock().now();

	for (;;)
	{
		std::shared_ptr<Task>task;
		{
			//先获取锁
			std::unique_lock<std::mutex>lock(taskQueMtx_);
			std::cout << std::this_thread::get_id() << "尝试获取任务..." << std::endl;
			//在cached模式下 可能创建很多线程 但是空闲时间超过60s 应该去掉多余的线程
			//超过初始线程数量的线程要进行回收
			//当前时间-上次线程执行的时间>60s
			//每一秒钟返回一次
			while (taskQue_.size() == 0)
			{
				if (!isPoolRunning_)
				{
					threads_.erase(threadid);
					std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
					exitCond_.notify_all();
					return;
				}
				if (poolMode_ == PoolMode::MODE_CACHED)
				{
					//超时返回
					if (std::cv_status::timeout ==
						notEmpty_.wait_for(lock, std::chrono::seconds(1)))
					{
						auto now = std::chrono::high_resolution_clock().now();
						auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
						if (dur.count() >= THREAD_MAX_IDLE_TIME
							&& curThreadSize_ > initThreadSize_)
						{
							//开始回收当前线程
							//记录线程数量的相关变量的值的修改
							//把线程对象从线程列表容器中删除  没有办法 threadFunc《=》thread对象
							//threadid =>thread对象
							threads_.erase(threadid);
							--curThreadSize_;
							--idleThreadSize_;
							std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
							return;
						}
					}
				}
				else
				{
					//等待notempty条件
					notEmpty_.wait(lock);
				}
				//线程池要结束回收线程资源
				//if (!isPoolRunning_)
				//{
				//	threads_.erase(threadid);
				//	std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
				//	exitCond_.notify_all();
				//	return;
				//}

			}
			//if (!isPoolRunning_)
			//{
			//	break;
			//}
			--idleThreadSize_;
			std::cout << std::this_thread::get_id() << "获取任务成功" << std::endl;
			//从任务队列取个任务出去
			task = taskQue_.front();
			taskQue_.pop();
			--taskSize_;
		}
		if (!taskQue_.size() > 0)
		{
			notEmpty_.notify_all();
		}
		notFull_.notify_all();
		//当前线程负责这个任务
		if (task != nullptr)
		{
			//task->run();//执行任务 把任务的返回值setVal方法给到Result
			task->exec();
		}


		++idleThreadSize_;
		lastTime = std::chrono::high_resolution_clock().now();//更新线程执行完任务的时间

	}
	//threads_.erase(threadid);
	//std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
	//exitCond_.notify_all();
}




//////////////////////Thread方法实现

Thread::Thread(ThreadFunc func)
	:func_(func)
	, threadId_(generateId_++)
{}
Thread::~Thread()
{}

int Thread::generateId_ = 0;

 
//启动线程
void Thread::start()
{
	//创建一个线程来执行线程函数
	std::thread t(func_,threadId_);
	t.detach();
}

int Thread::getId()const
{
	return threadId_;
}

//////////////////////task方法的实现
void Task::exec()
{
	if (result_ != nullptr)
	{
		result_->setVal(run());
	}
}

void Task::setResult(Result* res)
{
	result_ = res;
}
//////////////////////Result方法的实现
Result::Result(std::shared_ptr<Task>task, bool isValid)
	:task_(task)
	, isValid_(isValid)
{
	task_->setResult(this);
}


void Result::setVal(Any any)
{
	//存储task的返回值
	this->any_ = std::move(any);
	//已经获取返回值 增加信号量资源
	sem_.post();
}

Any Result::get()//用户调用
{
	if (!isValid_)
	{
		return "";
	}
	sem_.wait();//task任务如果没有执行完会阻塞用户线程
	return std::move(any_);
}