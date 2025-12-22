#include"pch.h"
struct ngx_pool_t;
struct ngx_pool_data_t;
struct ngx_pool_large_t;
typedef void(*ngx_pool_cleanup_pt)(void* date);

struct ngx_pool_cleanup_s
{
	ngx_pool_cleanup_pt handler;
	void* data;
	ngx_pool_cleanup_s* next;
};
struct ngx_pool_large_t
{
	void* alloc;
	ngx_pool_large_t* next;
};
struct ngx_pool_data_t
{
	char* last;
	char* end;
	ngx_pool_t* next;
	int fail;
};
struct ngx_pool_t
{
	ngx_pool_data_t d;
	size_t max;
	ngx_pool_t* current;
	ngx_pool_large_t* large;
	ngx_pool_cleanup_s* cleanup;
};




size_t ngx_align(size_t size, size_t align);
char* ngx_align_ptr(void* ptr, size_t align);






//一个物理页面的大小
const int ngx_pagasize = 4096;
//ngx小块内存池可分配最大空间
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagasize - 1;

const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//16k
//按照16字节进行对齐
const int NGX_POOL_ALIGNMENT = 16;
const int NGX_MIN_POOL_SIZE = ngx_align(sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t), NGX_POOL_ALIGNMENT);
#define NGX_ALIGNMENT sizeof(unsigned long)

class ngx_mem_pool
{
public:
	//创建指定size大小的内存池，小块内存不超过一个页面的大小
	void* ngx_create_pool(size_t size);
	//考虑内存字节对齐，申请内存
	void* ngx_palloc(size_t size);
	//不考虑内存字节对齐，申请内存
	void* ngx_pnalloc(size_t size);
	//调用ngx_palloc，会初始化0
	void* ngx_pcalloc(size_t size);
	//释放大块内存
	void ngx_pfree(void* p);
	//内存重置函数
	void ngx_reset_pool();
	//内存池销毁
	void ngx_destroy_pool();
	//添加回调清理函数
	ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);
private:
	ngx_pool_t* pool;
	void* ngx_palloc_small(size_t size,bool);
	void* ngx_palloc_block(size_t size);
	void* ngx_palloc_large(size_t size);
};