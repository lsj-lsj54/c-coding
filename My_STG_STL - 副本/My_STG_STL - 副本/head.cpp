#include"head.h"

node* _s_free_list[16];
void* _s_start_free = nullptr;
void* _s_end_free = nullptr;
int _s_heap_size = 0;

size_t _s_round_up(size_t size)
{
	return (size + ALIGN - 1) & ~(ALIGN - 1);
}

size_t _s_freelist_index(size_t size)
{
	return (size + ALIGN - 1) / ALIGN - 1;
}
void* _s_chunk_alloc(size_t _n, int& nobjs)
{
	char* _result;
	size_t _total_bytes = _n * nobjs;
	size_t _bytes_left = (size_t)_s_end_free - (size_t)_s_start_free;
	if (_total_bytes <= _bytes_left)
	{
		_result = (char*)_s_start_free;
		_s_start_free = (char*)_s_start_free + _total_bytes;
		return _result;
	}
	else if (_n <= _bytes_left)
	{
		nobjs = (int)(_bytes_left / _n);
		_total_bytes = _n * nobjs;
		_result = (char*)_s_start_free;
		_s_end_free = (char*)_s_start_free + _total_bytes;
		return _result;
	}
	else
	{
		size_t _bytes_to_get = 2 * _total_bytes + _s_round_up(_s_heap_size >> 4);
		if (_bytes_left)
		{
			node** _my_free_list = _s_free_list + _s_freelist_index(_bytes_left);
			((node*)_s_start_free)->next = *_my_free_list;
			*_my_free_list = (node*)_s_start_free;
		}
		_s_start_free = (char*)malloc(_bytes_to_get);


		if (_s_start_free == nullptr)
		{
			int i;
			for (i = _n; i <= MAX_BYTES; i += ALIGN)
			{
				node** _my_free_list = _s_free_list + _s_freelist_index(i);
				node* _p = *_my_free_list;
				if (_p != nullptr)
				{
					*_my_free_list = _p->next;
					_s_start_free = (char*)_p;
					_s_end_free = (char*)_s_start_free + i;
					return _s_chunk_alloc(_n, nobjs);
				}
			}
		}
		//_s_end_free = nullptr;
		//_s_start_free = nullptr;//这里再次尝试其他方法腾出空间，作为最后的尝试，应该可以成功

		_s_end_free = (char*)_s_start_free + _bytes_to_get;
		_s_heap_size += _bytes_to_get;
		return _s_chunk_alloc(_n, nobjs);
	}
}
void* _s_refill(size_t _n)
{
	int _nobjs = 20;
	char* _chunk = (char*)_s_chunk_alloc(_n, _nobjs);
	node** _my_free_list;
	node* _result;
	node* _current_node;
	node* _next_node;
	int _i;
	if (_nobjs == 1)return (node*)_chunk;
	_my_free_list = _s_free_list + _s_freelist_index(_n);
    _result = (node*)_chunk;
	*_my_free_list = _next_node = (node*)(_chunk + _n);
	for (_i = 1; _i != _nobjs; ++_i)
	{
		_current_node = _next_node;
		_next_node = (node*)((char*)_next_node + _n);
		if (_i == _nobjs - 1)
		{
			_current_node->next = nullptr;
		}
		else
		{
			_current_node->next = _next_node;
		}
	}
	return _result;
}
void* allocate(size_t _n)
{
	void* _ret;
	if (_n > MAX_BYTES)
	{
		_ret = malloc(_n);
	}
	else
	{
		node** _my_free_list = _s_free_list + _s_freelist_index(_n);
		node* _result = *_my_free_list;
		if (_result == nullptr)
		{
			_ret = _s_refill(_s_round_up(_n));
		}
		else
		{
			*_my_free_list = _result->next;
			_ret = _result;
		}
	}
	return _ret;
}
void deallocate(void* _p, size_t _n)
{
	if (_n > MAX_BYTES)free(_p);
	else
	{
		node** _my_free_list = _s_free_list + _s_freelist_index(_n);
		node* _q = *_my_free_list;
		((node*)_p)->next = _q;
		*_my_free_list = (node*)_p;
	}
}