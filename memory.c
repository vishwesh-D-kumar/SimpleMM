#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "memory.h"

#define PAGE_SIZE 4096


static void *alloc_from_ram(size_t size)
{
	assert((size % PAGE_SIZE) == 0 && "size must be multiples of 4096");
	void* base = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (base == MAP_FAILED)
	{
		printf("Unable to allocate RAM space\n");
		exit(0);
	}
	return base;
}
struct node {
	struct node * prev;
	struct node * next;
};
struct metadata{
	long bucketSize;
	long availableBytes;
};
struct metadata * check;
struct node * buckets[9];
long bucketSizes[]={16,  32,  64,  128,256,  512,  1024,  2048,4080};
int getIndexFromSize(size_t size){
	for (int i = 0; i < 9; ++i)
	{
		if (size/bucketSizes[i]<=1)
		{
			return i;
		}
	}
	return -1;

}
void debug(struct metadata * check){
	long availableBytes=check->availableBytes;
	// printf("%ld\n",availableBytes);
}
void addNode(int bucketIndex,struct node * toAdd){
	if (buckets[bucketIndex]==NULL)
	{
		buckets[bucketIndex]=toAdd;
		toAdd->next=NULL;
		toAdd->prev=NULL;
		/* code */
	}
	else{
		toAdd->next=buckets[bucketIndex];
		buckets[bucketIndex]->prev=toAdd;
		toAdd->prev==NULL;

	}
}
void removeNode(int bucketIndex, struct node * toRemove){
	if (buckets[bucketIndex]==toRemove)
	{
		if (buckets[bucketIndex]->next==NULL)
		{
			buckets[bucketIndex]=NULL;
		}
		else{
			buckets[bucketIndex]=buckets[bucketIndex]->next;
		}
		
	}
	else{
		if (toRemove->next=NULL)
		{
			toRemove->prev->next=NULL;
		}
		else{
			toRemove->next->prev=toRemove->prev;
			toRemove->prev->next=toRemove->next;
		}
	}
}
void fillBucket(int bucketIndex){
	void * baseAddr=alloc_from_ram(4096);
	check=baseAddr;
	// printf("%p baseAddr\n",baseAddr);
	struct metadata * pageMetadata=((struct metadata *)baseAddr);
	pageMetadata->availableBytes=(4080/bucketSizes[bucketIndex])*bucketSizes[bucketIndex];
	pageMetadata->bucketSize=bucketSizes[bucketIndex];
	// printf("%ld\n",pageMetadata.availableBytes );
	struct metadata lol2=*((struct metadata * ) baseAddr);
	// printf("%ld\n", lol2.availableBytes);
	// space=4080;
	debug(check);
	void * currAddr=baseAddr+16;

	for (int i = 0; i < 4080/bucketSizes[bucketIndex]; ++i)
	{
		addNode(bucketIndex,currAddr);
		currAddr=currAddr+bucketSizes[bucketIndex];

	}
}
void * getMetadataFromNode(void * ptr){
	void * metadataAddr=(void *)(((long) ptr) & (0xFFFFFFFFFFFFF000));
	// printf("%p metadataAddr\n",metadataAddr );
	return metadataAddr;

}
static void free_ram(void *addr, size_t size)
{
	munmap(addr, size);
}
int isInBucket(int bucketIndex,void * ptr){

	struct node * curr =buckets[bucketIndex];
	if (curr==NULL)
	{
		return 0;
	}
	while(curr->next=NULL){
		if (curr==ptr)
		{
			return 1;
		}
		curr=curr->next;
	}
	return 0;
}
void myfree(void *ptr)
{	
	void * baseAddr=getMetadataFromNode(ptr);
	struct metadata * pageMetadata=getMetadataFromNode(ptr);
	// pageMetadata->availableBytes=pageMetadata->availableBytes-pageMetadata->bucketSize;
	long availableBytesAfterFreeing=pageMetadata->availableBytes+pageMetadata->bucketSize;
	long size=pageMetadata->bucketSize;
	int bucketIndex=getIndexFromSize(size);
	long maxSizeOfPage=(4080/size)*size;
	if (size>4080)
	{
		free_ram(baseAddr,4096);
		return;

	}
	else{

		
		if (availableBytesAfterFreeing==maxSizeOfPage)
		{	
			void * currAddr=baseAddr+16;
			for (int i = 0; i < 4080/bucketSizes[bucketIndex]; ++i)
			{
				if(isInBucket(bucketIndex,currAddr)){
					removeNode(bucketIndex,currAddr);
				}


			}
			free_ram(baseAddr,4096);
			return;
		}
		else{
			addNode(bucketIndex,ptr);
			pageMetadata->availableBytes=availableBytesAfterFreeing;
			return ;
		}
	}



	printf("myfree is not implemented\n");
	abort();
}

void *mymalloc(size_t size)
{
	if (size>4080)
	{
		int a=size/4096;
		long reqPageSize=(a+1)*4096;
		void * baseAddr=alloc_from_ram(reqPageSize);
		struct metadata * pageMetadata=((struct metadata *)baseAddr);
		pageMetadata->availableBytes=reqPageSize-16;
		pageMetadata->bucketSize=reqPageSize;
		return baseAddr+16;

	}
	else{
		int bucketIndex=getIndexFromSize(size);
		if (buckets[bucketIndex]==NULL)
		{
			fillBucket(bucketIndex);
		}
		void * toReturn=buckets[bucketIndex];
		removeNode(bucketIndex,buckets[bucketIndex]);
		struct metadata * pageMetadata=getMetadataFromNode(toReturn);
		pageMetadata->availableBytes=pageMetadata->availableBytes-pageMetadata->bucketSize;


		return toReturn;


	}

	printf("mymalloc is not implemented\n");
	abort();
	return NULL;
}
int main(int argc, char const *argv[])
{
	void * lol=mymalloc(100);
	// struct metadata * pageMetadata2=(getMetadataFromNode(lol));
	// debug(pageMetadata2)
	// printf("%p pageMetadata2\n",pageMetadata2);
	// printf("%l\n",pageMetadata2->bucketSize );
	// debug(check);/*/
	// printf("%p lol\n",lol);

	return 0;
}