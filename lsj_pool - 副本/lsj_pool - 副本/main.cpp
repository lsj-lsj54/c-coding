#include"pch.h"
#include"pool.h"
#include"node.h"

int main()
{
	mypool* p = mypool::get_instance("text.text");

	thread t1([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(1);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});

	thread t2([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(2);
			//cout << "thread 2 got node with data: " << "thread 2 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});

	thread t3([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(3);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t4([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(4);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t5([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(5);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t6([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(6);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t7([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(7);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t8([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(8);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t9([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(9);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});
	thread t10([&]() {
		for (int i = 0; i < 10; i++)
		{
			shared_ptr<node> n = p->get_node(10);
			//cout << "thread 1 got node with data: " << "thread 1 data " << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
	t10.join();

	return 0;
}