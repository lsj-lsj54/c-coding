#include"pch.h"
#include"ngx_mem_pool.h"


//void* ngx_create_pool(size_t size);
//
//void* ngx_palloc(size_t size);
//
//void* ngx_pnalloc(size_t size);
//
//void* ngx_pcalloc(size_t size);
//
//void ngx_pfree(void* p);
//
//void ngx_reset_pool();
//
//void ngx_destroy_pool();
//
//ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);


void fun1(void* data)
{
	delete data;
}


int main()
{
	ngx_mem_pool mempool;
	if (mempool.ngx_create_pool(521) == nullptr)
	{
		cout << "createÊ§°Ü" << endl;
		return -1;
	}
	void* p1 = mempool.ngx_palloc(128);
	if (p1 == nullptr)
	{
		cout << "Ð¡pallocÊ§°Ü" << endl;
		return -1;
	}
	void* p2 = mempool.ngx_palloc(512);
	if (p2 == nullptr)
	{
		cout << "´ópallocÊ§°Ü" << endl;
		return -1;
	}
	int* lsj = new int(54);
	p2 = lsj;
	ngx_pool_cleanup_s* c1 = mempool.ngx_pool_cleanup_add(sizeof(int*));
	c1->handler = fun1;
	c1->data = p2;

	mempool.ngx_destroy_pool();


}
