#include "pch.h"
#include "pool.h"
#include "public.h"

mypool* mypool::get_instance(string s)
{
    static mypool instance(s);
    return &instance;
}

bool mypool::load_file(string s)
{
    FILE* fp = nullptr;
    fopen_s(&fp, s.c_str(), "r");
    if (fp == nullptr)
    {
        LOG("open file pool.txt failed");
        return false;
    }

    while (!feof(fp))
    {
        char temp[256];
        fgets(temp, 256, fp);
        string s = temp;
        int idx_1 = s.find('=', 0);
        int idx_2 = s.find('\n', idx_1);
        string first = s.substr(0, idx_1);
        string second = s.substr(idx_1 + 1, idx_2 - idx_1 - 1);
        if (first == "id")
        {
            _id = second;
        }
        if (first == "name")
        {
            _name = second;
        }
        if (first == "min_num")
        {
            _min_num = atoi(second.c_str());
        }
        if (first == "max_num")
        {
            _max_num = atoi(second.c_str());
        }
        if (first == "waiting_time")
        {
            _waiting_time = atoi(second.c_str());
        }
        if (first == "destroy_time")
        {
            _destroy_time = atoi(second.c_str());
        }
    }
    return true;
}


mypool::mypool(string s)
{
    load_file(s);
    for (int i = 0; i < _min_num; i++)
    {
        node* n = new node();
        n->refreshAliveTime();
        _nodes_queue.push(n);
        _connection_cnt++;
    }

    thread produce_thread(bind(&mypool::produce_nodes, this));
    produce_thread.detach();

    thread destroy_thread(bind(&mypool::destroy_nodes, this));
    destroy_thread.detach();
}

void mypool::produce_nodes()
{
    while (true)
    {
        unique_lock<mutex> lock(_queue_mutex);

        
        if (!_nodes_queue.empty())
        {
            _cv.wait(lock);
        }

        if (_nodes_queue.empty() && _connection_cnt < _max_num)
        {
            node* n = new node();
            n->refreshAliveTime();
            _nodes_queue.push(n);
            _connection_cnt++;
            cout << "产生了一个新节点" << endl;
        }
        _cv.notify_all();
    }
}

void mypool::destroy_nodes()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(_destroy_time));

        unique_lock<mutex> lock(_queue_mutex);


        if (_connection_cnt > _min_num)
        {
            node* n = _nodes_queue.front();

            if (n->getAliveeTime() >= clock_t(_destroy_time))
            {
                delete n;
                _connection_cnt--;
                _nodes_queue.pop();
                cout << "销毁了一个超时节点" << endl;
            }
            else
            {
                break;
            }
        }
    }
}

shared_ptr<node> mypool::get_node(int i)
{
    unique_lock<mutex> lock(_queue_mutex);
    while (_nodes_queue.empty())
    {
        if (cv_status::timeout == _cv.wait_for(lock, chrono::milliseconds(_waiting_time)))
        {
            if (_nodes_queue.empty())
            {
                LOG("获取空闲连接超时了...获取连接失败!");
                return nullptr;
            }
        }
    }
    shared_ptr<node> n(_nodes_queue.front(), [&](node* ptr)
        {
            unique_lock<mutex> lock(_queue_mutex);
            ptr->refreshAliveTime();
            _nodes_queue.push(ptr);
            cout << " 归还了一个节点" << endl;
        });

    _nodes_queue.pop();
    cout << "节点 " << i << "成功取到一个节点" << endl;
    _cv.notify_all();
    return n;
}