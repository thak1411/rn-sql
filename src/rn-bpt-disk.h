#ifndef __RN_BPT_DISK_H__
#define __RN_BPT_DISK_H__

#include "rn-file-io.h"
#include "rn-type.h"

#include <unordered_map>    // unordered_map //
#include <utility>          // pair //
#include <cstring>          // memcpy, memmove //
#include <queue>            // queue //
#include <list>             // list //

#define INTERNAL_DEGREE_BPT 248     // 8B + 8B = 16B -> 248 Block //
#define LEAF_DEGREE_BPT     31      // 8B + 120B = 128B -> 31 Block //
#define VALUE_SIZE          120     // 120B Fixed //
#define PAGE_SIZE           4096    // 4KB Fixed //

#define CACHE_SIZE          8192    // CACHE Memory Size - Max Page Size //

// typedef LLONG KEY_BPT;       // Key Type LLONG Fixed //
// #define KEY_BPT_FMT "%lld"   // Key Type LLONG Fixed //

namespace rn {
    class Page;
    class HeaderPage;
    class FreePage;
    class NodePage;

    class Cache {
    private:
        IO* io;
        std::list < Page* > used;
        std::unordered_map < LLONG, std::list < Page* >::iterator > cache;
    public:
        Cache(IO*);
        ~Cache();

        Page* get(LLONG);
        HeaderPage* getHeader(LLONG);
        FreePage* getFree(LLONG);
        NodePage* getNode(LLONG);
        VOID flush();
    };

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

        LLONG newPage(Cache*);
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
        Cache* cache;
        HeaderPage* header;
    public:
        BPlusTree(LPCSTR);
        ~BPlusTree();

        BOOL split(NodePage*);

        INT find(LLONG, LPSTR);
        INT insert(LLONG, LPCSTR);
        INT erase(LLONG);

        VOID print();
        std::vector < LLONG > getList();
    };

    SIZE searchBucket(NodePage*, LLONG);
}




#endif // __RN_BPT_DISK_H__ //