#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <unistd.h>
#include <limits.h>

#include "utility.h"

/* BSD hack */
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

void printMessage (void) {
    /* we need to get system page size information */
    const long PAGE_SIZE = sysconf(_SC_PAGESIZE);
    union {
        void * real;
        void (* fake)(int (*)(const char *, va_list), const char *, ...);
    } heap;

    /* enable execute premission to a page */
    heap.real = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    /* extract machine code */
    extractCode(heap.real, PAGE_SIZE);
    /* close read/write premission to prevent abuse */
    mprotect(heap.real, PAGE_SIZE, PROT_EXEC);

    /* call the heap as a function */
    heap.fake(vprintf, "Ethan Hunt, Run!\n");

    /* free the allocated page */
    munmap(heap.real, PAGE_SIZE);
}

void deleteSelf (void) {
    char path[PATH_MAX];

    readlink("/proc/self/exe", path, sizeof(path));
    unlink(path);
}

void blockByMilliseconds (unsigned long ms) {
    sleep( ms / 1000 );
}
