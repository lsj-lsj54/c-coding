#pragma once
#include"pch.h"

class node
{
public:

	void setData(const string& s) { _data = s; }


	// 刷新一下连接的起始的空闲时间点
	void refreshAliveTime() { _alivetime = clock(); }
	// 返回存活的时间
	clock_t getAliveeTime()const { return clock() - _alivetime; }
private:
	clock_t _alivetime; // 记录进入空闲状态后的起始存活时间
	string _data; // 节点数据
};