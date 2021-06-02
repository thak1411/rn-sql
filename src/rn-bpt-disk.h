#ifndef __RN_BPT_DISK_H__
#define __RN_BPT_DISK_H__

#include "rn-file-io.h"
#include "rn-type.h"

#include <utility>  // pair //
#include <cstring>  // memcpy, memmove //
#include <queue>    // queue //

#define INTERNAL_DEGREE_BPT 4     // 8B + 8B = 16B -> 248 Block //
#define LEAF_DEGREE_BPT     4      // 8B + 120B = 128B -> 31 Block //
#define VALUE_SIZE          120     // 120B Fixed //
#define PAGE_SIZE           4096    // 4KB Fixed //


// typedef LLONG KEY_BPT;       // Key Type LLONG Fixed //
// #define KEY_BPT_FMT "%lld"   // Key Type LLONG Fixed //

namespace rn {
    class Page {
    protected:
        BYTE buf[PAGE_SIZE];
        IO* io;
        LLONG page, offset;
    public:
        Page(IO*);
        Page(IO*, LLONG);
        virtual ~Page();

        IO* getIO();
        LLONG getPage();
        LLONG getOffset();

        VOID flush();
    };

    class HeaderPage : public Page {
    public:
        HeaderPage(IO*);
        HeaderPage(IO*, LLONG);
        virtual ~HeaderPage();

        LLONG getFree();
        LLONG getRoot();
        LLONG getSize();

        VOID setFree(LLONG);
        VOID setRoot(LLONG);
        VOID setSize(LLONG);

        LLONG newPage();
    };

    class FreePage : public Page {
    public:
        FreePage(IO*);
        FreePage(IO*, LLONG);
        virtual ~FreePage();

        LLONG getNext();

        VOID setNext(LLONG);
    };

    class NodePage : public Page {
    public:
        NodePage(IO*);
        NodePage(IO*, LLONG);
        virtual ~NodePage();

        LLONG getParent();
        INT getIsLeaf();
        INT getKeySize();
        LLONG getNext();
        LLONG getKey(SIZE);
        LLONG getChild(SIZE);
        VOID getValue(SIZE, LPSTR);

        VOID setParent(LLONG);
        VOID setIsLeaf(INT);
        VOID incKeySize();
        VOID decKeySize();
        VOID setKeySize(INT);
        VOID setNext(LLONG);
        VOID setKey(SIZE, LLONG);
        VOID setChild(SIZE, LLONG);
        VOID setValue(SIZE, LPCSTR);

        VOID moveKey(SIZE, SIZE);

        VOID operator=(LLONG);
    };

    class BPlusTree {
    private:
        IO* io;
        HeaderPage* header;
    public:
        BPlusTree(LPCSTR);
        ~BPlusTree();

        INT find(LLONG, LPSTR);
        INT insert(LLONG, LPCSTR);
        INT erase(LLONG);

        VOID print();
    };

    SIZE searchBucket(NodePage*, LLONG);
    BOOL split(HeaderPage*, NodePage*);
}




#endif // __RN_BPT_DISK_H__ //