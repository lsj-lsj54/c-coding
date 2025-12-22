#pragma once
#include"pch.h"

struct node
{
	int data;
	struct node* next;
};



size_t _s_round_up(size_t size);
size_t _s_freelist_index(size_t size);
void* _s_refill(size_t _n);
void* _s_chunk_alloc(size_t, int&);
void* allocate(size_t);
void deallocate(void*, size_t);


