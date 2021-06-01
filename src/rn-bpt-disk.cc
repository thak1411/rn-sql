#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"
#include "rn-bpt-dist.h"

namespace rn {
    Page::Page(IO* io, LLONG offset) : io(io), offset(offset) {
        io->readPage(buf, offset);
    }

    HeaderPage::HeaderPage(IO* io, LLONG offset) : Page(io, offset) { }
    LLONG HeaderPage::getFree() {
        return 0[(LPLLONG)buf];
    }
    LLONG HeaderPage::getRoot() {
        return 1[(LPLLONG)buf];
    }
    LLONG HeaderPage::getSize() {
        return 2[(LPLLONG)buf];
    }

    FreePage::FreePage(IO* io, LLONG offset) : Page(io, offset) { }
    LLONG FreePage::getNext() {
        return 0[(LPLLONG)buf];
    }

    NodePage::NodePage(IO* io, LLONG offset) : Page(io, offset) { }
    LLONG NodePage::getParent() {
        return 0[(LPLLONG)buf];
    }
    INT NodePage::getIsLeaf() {
        return 2[(LPINT)buf];
    }
    INT NodePage::getKeySize() {
        return 3[(LPINT)buf];
    }
    LLONG NodePage::getNext() {
        return 15[(LPLLONG)buf];
    }
    LLONG NodePage::getKey(SIZE idx) {
        if (getIsLeaf()) return 16[(LPLLONG)buf + idx + 15 * idx];  // Warning: Hard Coding With Key, Value Size //
        return 16[(LPLLONG)buf + 2 * idx];                          // Warning: Hard Coding With Key, Page Number Size //
    }
    LLONG NodePage::getChild(SIZE idx) {
        return 15[(LPLLONG)buf + 2 * idx];  // Warning: Hard Coding With Key, Page Number Size //
    }
    VOID NodePage::getValue(SIZE idx, LPSTR ret) {
        memcpy(ret, (LPCSTR)buf + 128 + 8 * idx, VALUE_SIZE);   // Warning: Hard Coding With Key Size & CHAR Size -> sizeof(CHAR) Must be 1 //
    }


    BPlusTree::BPlusTree(LPCSTR path) {
        io = new IO(path);
        header = new HeaderPage(io, 0);
    }
    BPlusTree::~BPlusTree() {
        if (io != NULL) delete io;
    }

    INT BPlusTree::find(LLONG key, LPSTR ret) {

    }

    INT BPlusTree::insert(LLONG key, LPCSTR value) {

    }

    INT BPlusTree::erase(LLONG key) {

    }
}