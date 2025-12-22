#include"pch.h"
#include"head.h"


//void* allocate(size_t);
//void deallocate(void*, size_t);




int main()
{
	int* p = (int*)allocate(32);
	deallocate(p, 32);
	return 0;
}