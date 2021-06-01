#ifndef __RN_FILE_IO_H__
#define __RN_FILE_IO_H__

#include "rn-bpt-dist.h"
#include "rn-type.h"

#include <unistd.h>     // pread, pwrite //
#include <stdlib.h>     // exit //
#include <stdio.h>      // fprintf //
#include <fcntl.h>      // open //

namespace rn {
    class IO {
    private:
        int fd;
    public:
        IO(LPCSTR);
        ~IO();

        SSIZE_T read(LPVOID, SIZE_T, LLONG);
        SSIZE_T readPage(LPVOID, LLONG);
        SSIZE_T write(LPCVOID, SIZE_T, LLONG);
        SSIZE_T writePage(LPCVOID, LLONG);
    };
}


#endif // __RN_FILE_IO_H__ //