#include"threadpool.h"
#include<chrono>
#include<thread>



class MyTask :public Task
{
public:
	MyTask(int begin,int end)
		:begin_(begin)
		,end_(end)
	{ }
	//怎么设计run函数返回值可以表示任意的类型
	Any run()
	{
		unsigned long long sum = 0;
		std::cout << std::this_thread::get_id() << "begin" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		for (int i = begin_; i <= end_; ++i)
		{
			sum += i;
		}
		std::cout << std::this_thread::get_id() << "end" << std::endl;
		return sum;
	}
private:
	int begin_;
	int end_;
};


int main()
{
	{
		ThreadPool pool;
		//用户自己设置线程池模式
		pool.setMode(PoolMode::MODE_CACHED);
		//开始启动线程池
		pool.start(4);

		//怎么设计Result
		Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
		Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
		Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
		pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));

		pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
		pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));



		unsigned long long sum1 = res1.get().cast_<unsigned long long>();//get返回一个Any类型 怎么转成具体类型
		unsigned long long sum2 = res2.get().cast_<unsigned long long>();
		unsigned long long sum3 = res3.get().cast_<unsigned long long>();

		std::cout << sum1 + sum2 + sum3 << std::endl;
	}

	getchar();

	return 0;
}