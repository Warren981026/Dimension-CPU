#ifndef DISTRIBUTE_MMAP_H
#define DISTRIBUTE_MMAP_H

#include <fcntl.h>
#include <stdint.h>

#include <iostream>

using namespace std;

struct LoadResult{
    void* start;
    uint64_t length;
};

LoadResult Load(const char* PATH);
void UnLoad(LoadResult result);


#ifdef _WIN64
    //Load into an array and return as the same struct
#else
#include <sys/stat.h>
#include <sys/mman.h>

LoadResult Load(const char* PATH){
    LoadResult ret;

    int32_t fd = open(PATH, O_RDONLY);
    if(fd == -1) {
        cerr << "Cannot open " << PATH << endl;
        throw;
    }

    struct stat sb;
    if(fstat(fd, &sb) == -1)
        cerr << "Fstat Error\n";

    ret.length = sb.st_size;
    ret.start = mmap(nullptr, ret.length, PROT_READ, MAP_PRIVATE, fd, 0u);

    if (ret.start == MAP_FAILED) {
        cerr << "Cannot mmap " << PATH << " of length " << ret.length << endl;
        throw;
    }

    return ret;
}

void UnLoad(LoadResult result){
    munmap(result.start, result.length);
}
#endif

#endif //DISTRIBUTE_MMAP_H
