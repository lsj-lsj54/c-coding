#pragma once
#include"pch.h"
#include"node.h"


class mypool
{
public:
	static mypool* get_instance(string s);

	shared_ptr<node> get_node(int i);
private:

	bool load_file(string s);

	mypool(string s);

	void produce_nodes();

	void destroy_nodes();


	string _id;
	string _name;
	int _min_num;
	int _max_num;
	int _waiting_time;
	int _destroy_time;

	queue<node*> _nodes_queue;
	mutex _queue_mutex;
	atomic_int _connection_cnt;
	condition_variable _cv;
};