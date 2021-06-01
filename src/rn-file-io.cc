#include "rn-file-io.h"

namespace rn {
    IO::IO(LPCSTR path) {
        if ((fd = open(path, O_RDWR | O_CREAT | O_SYNC | O_NONBLOCK, 0644)) <= 0) {
            fprintf(stderr, "Error! File Can Not Open\n");
            exit(-1);
        }
    }
    IO::~IO() {
        if (fd > 0) close(fd);
    }

    SSIZE_T IO::read(LPVOID buf, SIZE_T size, LLONG offset) {
        return ::pread(fd, buf, size, offset);
    }

    SSIZE_T IO::readPage(LPVOID buf, LLONG offset) {
        return ::pread(fd, buf, PAGE_SIZE, offset);
    }

    SSIZE_T IO::write(LPCVOID buf, SIZE_T size, LLONG offset) {
        return ::pwrite(fd, buf, size, offset);
    }

    SSIZE_T IO::writePage(LPCVOID buf, LLONG offset) {
        return ::pwrite(fd, buf, PAGE_SIZE, offset);
    }
}