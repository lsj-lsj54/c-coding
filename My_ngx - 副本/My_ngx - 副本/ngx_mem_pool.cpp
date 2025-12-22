#include"pch.h"
#include"ngx_mem_pool.h"



// 使用 uintptr_t 保持指针宽度
size_t ngx_align(size_t size, size_t align)
{
	return (size + align - 1) & ~(align - 1);
}

char* ngx_align_ptr(void* ptr, size_t align)
{
	uintptr_t p = (uintptr_t)ptr;
	uintptr_t a = (uintptr_t)align;
	uintptr_t aligned = (p + a - 1) & ~(a - 1);
	return (char*)aligned;
}

//创建指定size大小的内存池，小块内存不超过一个页面的大小
void* ngx_mem_pool::ngx_create_pool(size_t size)
{
	pool = (ngx_pool_t*)malloc(size);
	if (pool == nullptr)
	{
		return nullptr;
	}
	pool->d.last = (char*)pool + sizeof(ngx_pool_t);
	pool->d.end = (char*)pool + size;
	pool->d.next = nullptr;
	pool->d.fail = 0;

	size = size - sizeof(ngx_pool_t);
	pool->max = (size<NGX_MAX_ALLOC_FROM_POOL)?size: NGX_MAX_ALLOC_FROM_POOL;

	pool->current = pool;
	pool->large = nullptr;
	pool->cleanup = nullptr;

	return pool;
}
//考虑内存字节对齐，申请内存
void* ngx_mem_pool::ngx_palloc(size_t size)
{
	if (size <= pool->max)
		return ngx_palloc_small(size,1);
	else
		return ngx_palloc_large(size);
}
//不考虑内存字节对齐，申请内存
void* ngx_mem_pool::ngx_pnalloc(size_t size)
{
	if (size <= pool->max)
		return ngx_palloc_small(size, 0);
	else
		return ngx_palloc_large(size);
}
//调用ngx_palloc，会初始化0
void* ngx_mem_pool::ngx_pcalloc(size_t size)
{
	void* p;
	p = ngx_palloc(size);
	if (p)
	{
		memset(p, 0, size);
	}
	return p;
}
void* ngx_mem_pool::ngx_palloc_small(size_t size,bool align)
{
	ngx_pool_t* p;
	char* m;

	p = pool->current;
	do
	{
		m = p->d.last;
		if (align)
		{
			m = ngx_align_ptr(m, NGX_ALIGNMENT);
		}
		if ((p->d.end - m) >= size)
		{
			p->d.last = m + size;
			return m;
		}
		p = p->d.next;
	} while (p);

	return ngx_palloc_block(size);
}
void* ngx_mem_pool::ngx_palloc_block(size_t size)
{
	ngx_pool_t* m, * n, * t;
	size_t psize;
	psize = (size_t)(pool->d.end - (char*)pool);
	n= (ngx_pool_t*)malloc(psize);
	if (n == nullptr)
	{
		return nullptr;
	}
	m = n;
	m->d.end = (char*)n + psize;
	m->d.next = nullptr;
	m->d.fail = 0;

	m = (ngx_pool_t*)((char*)m + sizeof(ngx_pool_data_t));
	m = (ngx_pool_t*)ngx_align_ptr(m, NGX_ALIGNMENT);

	n->d.last = (char*)m + size;

	for (t = pool->current; t->d.next; t = t->d.next)
	{
		if (t->d.fail++ > 4)
		{
			pool->current = t->d.next;
		}
	}
	t->d.next = n;
	return (void*)m;
}
void* ngx_mem_pool::ngx_palloc_large(size_t size)
{
	int x = 0;
	ngx_pool_large_t* n;
	void* p;
    p=malloc(size);
	if (p == nullptr)
	{
		return nullptr;
	}
	for (ngx_pool_large_t* m = pool->large; m; m = m->next)
	{
		if (m->alloc == nullptr)
		{
			m->alloc = p;
			return p;
		}
		if (++x > 4)
		{
			break;
		}
	}
    n = (ngx_pool_large_t*)ngx_palloc_small(sizeof(ngx_pool_large_t),1);
	if (n == nullptr)
	{
		free(p); 
		return nullptr;
	}
	n->alloc = p;
	n->next = pool->large;
	pool->large = n;
	return p;
}
//释放大块内存
void ngx_mem_pool::ngx_pfree(void* p)
{
	for (ngx_pool_large_t* m = pool->large; m; m = m->next)
	{
		if (p == m->alloc)
		{
			free(m->alloc);
			m->alloc = nullptr;
		}
	}
}
//内存重置函数
void ngx_mem_pool::ngx_reset_pool()
{
	for (ngx_pool_large_t* m = pool->large; m; m = m->next)
	{
		if (m->alloc)
		{
			free(m->alloc);
		}
	}
	ngx_pool_t* m = pool;
	m->d.fail = 0;
	m->d.last = (char*)m + sizeof(ngx_pool_t);
	
	for (m = m->d.next; m; m = m->d.next)
	{
		m->d.fail = 0;
		m->d.last = (char*)m + sizeof(ngx_pool_data_t);
	}

	pool->current = pool;
	pool->large = nullptr;
}
//内存池销毁
void ngx_mem_pool::ngx_destroy_pool()
{
	ngx_pool_t* p, * n;
	ngx_pool_large_t* l;
	ngx_pool_cleanup_s* c;

	for (c = pool->cleanup; c; c = c->next)
	{
		if (c->handler)
		{
			c->handler(c->data);
		}
	}
	for (l= pool->large; l; l = l->next)
	{
		if (l->alloc)
		{
			free(l->alloc);
		}
	}

	for (p = pool, n = p->d.next; ; p = n, n = p->d.next)
	{
		free(p);
		if (n == nullptr)
		{
			break;
		}
	}
}
//添加回调清理函数
ngx_pool_cleanup_s* ngx_mem_pool::ngx_pool_cleanup_add(size_t size)
{
	ngx_pool_cleanup_s *c;

	c = (ngx_pool_cleanup_s*)ngx_palloc(sizeof(ngx_pool_cleanup_s));
	if (c == nullptr)
	{
		return nullptr;
	}
	if (size)
	{
		c->data = ngx_palloc(size);
	}
	else
	{
		c->data = nullptr;
	}

	c->handler = nullptr;
	c->next = pool->cleanup;
	pool->cleanup = c;
	return c;
}