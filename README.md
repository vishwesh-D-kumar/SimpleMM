# SimpleMM
A Simple Implementation of malloc and myfree in C.Attempts to understand the working behind the Memory management in C

## Working
SimpleMM uses alloc from ram and free ram APIs for allocation and dealloca- tion of RAM space from OS. alloc from ram can be used to allocate memory of size multiple of 4096 bytes. The memory address returned by alloc from ram is always aligned to 4096 (i.e., the address is divisible by 4096). A memory area of size 4096, whose starting address is aligned to 4096, is called a page. In other words, alloc from ram can be used to allocate contiguous page(s). free ram API can be used to deallocate memory returned by alloc from ra

### mymalloc
mymalloc is the memory allocation API of SimpleMM, which is similar to malloc API of standard C library. mymalloc takes the size of the buffer as input and returns a memory buffer of the input size. SimpleMM maintains multiple lists. Each list contains free objects that are available for allocation. All objects in a given list are of the same size. A list of free objects is also called a bucket. SimpleMM maintains nine buckets that contain objects of size 16, 32, 64, 128, 256, 512, 1024, 2048, and 4080 bytes, respectively. Bucket size is the size of memory objects in a given bucket.
If the allocation size is less than or equal to 4080 bytes, then the allocation size is rounded up to the nearest bucket size, and the allocation takes place from the corresponding bucket.
If the bucket is empty, then mymalloc allocates a page (using alloc from ram API). The first 16 bytes on the allocated page is reserved for metadata; the rest of the page is called data area. All objects on a page are used by the same bucket. Page metadata contains the bucket size and the number of available bytes on a given page (i.e., the number of bytes on the page that the available for allocation). mymalloc divides the data area on the page into memory objects o

### myfree

SimpleMM memory deallocation API is myfree. myfree takes a memory object (allocated using mymalloc) as input. myfree fetches the page metadata, which is stored on the top of the current page (page corresponding to the input object). If the current page belongs to the large allocation (>4080 bytes), then myfree obtains the allocation size from the page metadata and immediately frees the page using free ram API. Otherwise, myfree updates the available size on the page and inserts the object to the corresponding bucket (list). If all the bytes on the current page are available, then myfree removes all the objects on the current page from the corresponding bucket and frees the page using free ram API.

Implemented based on [the following repo](https://github.com/Systems-IIITD/SimpleMM/blob/master/assignment1.pdf)


Made By , 
Vishwesh Kumar 2018119
