#ifndef __RN_BPT_H__
#define __RN_BPT_H__

#include "rn-type.h"

#include <cstring>  // memset, memmove //
#include <cstdio>   // printf, puts //

#define DEGREE_BPT      2

typedef INT KEY_BPT;
#define KEY_BPT_FMT "%d"

namespace rn {
    BOOL insertBucket(KEY_BPT);

    class BPlusNode {
    private:
        BYTE isLeaf;
        INT keySize;
        KEY_BPT key[DEGREE_BPT * 2];
        BPlusNode* parent;
        union {
            BPlusNode* next;
            BPlusNode* child[DEGREE_BPT * 2 + 1];
        };
    public:
        BPlusNode();
        BPlusNode(BYTE);
        BPlusNode(BYTE, KEY_BPT);
        BYTE getIsLeaf();
        INT getKeySize();
        KEY_BPT getKey(SIZE);
        BPlusNode* getNext();
        BPlusNode* getChild(SIZE);

        VOID incKeySize();
        VOID setKeySize(INT);
        VOID setKey(SIZE, KEY_BPT);
        VOID setNext(BPlusNode*);
        VOID setChild(SIZE, BPlusNode*);
        VOID setParent(BPlusNode*);

        VOID moveKey(SIZE, SIZE);
        VOID moveChild(SIZE, SIZE);
        BOOL split(BPlusNode*&);
    };

    class BPlusTree {
    private:
        BPlusNode* root;
    public:
        BPlusTree();
        VOID print();
        BOOL insert(KEY_BPT);
    };
}

#endif // __RN_BPT_H__ //