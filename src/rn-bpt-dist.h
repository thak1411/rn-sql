#ifndef __RN_BPT_DISK_H__
#define __RN_BPT_DISK_H__

#include "rn-file-io.h"
#include "rn-type.h"

#include <cstring>  // memcpy //

#define INTERNAL_DEGREE_BPT 248     // 8B + 8B = 16B -> 248 Block //
#define LEAF_DEGREE_BPT     31      // 8B + 120B = 128B -> 31 Block //
#define VALUE_SIZE          120     // 120B Fixed //
#define PAGE_SIZE           4096    // 4KB Fixed //


// typedef LLONG KEY_BPT;       // Key Type LLONG Fixed //
// #define KEY_BPT_FMT "%lld"   // Key Type LLONG Fixed //

namespace rn {
    class Page {
    protected:
        BYTE buf[PAGE_SIZE];
        IO* io;
        LLONG offset;
    public:
        Page(IO*, LLONG);
        virtual ~Page();
    };

    class HeaderPage : public Page {
    public:
        HeaderPage(IO*, LLONG);

        LLONG getFree();
        LLONG getRoot();
        LLONG getSize();
    };

    class FreePage : public Page {
    public:
        FreePage(IO*, LLONG);

        LLONG getNext();
    };

    class NodePage : public Page {
    public:
        NodePage(IO*, LLONG);

        LLONG getParent();
        INT getIsLeaf();
        INT getKeySize();
        LLONG getNext();
        LLONG getKey(SIZE);
        LLONG getChild(SIZE);
        VOID getValue(SIZE, LPSTR);
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
    };
}




#endif // __RN_BPT_DISK_H__ //