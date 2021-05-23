#ifndef __RN_BPT_H__
#define __RN_BPT_H__

#include "rn-type.h"

#include <cstring>  // memset, memmove //
#include <cstdio>   // printf, puts //
#include <vector>   // std::vector //

#define DEGREE_BPT      2 // must be bigger than 1

typedef INT KEY_BPT;
#define KEY_BPT_FMT "%d"

namespace rn {
    class BPlusNode;
    class BPlusTree;

    SIZE insertBucket(BPlusNode*, KEY_BPT);
    SIZE searchBucket(BPlusNode*, KEY_BPT);
    BOOL eraseBucket(BPlusNode*, KEY_BPT);
    BOOL replaceBucket(BPlusNode*, KEY_BPT, KEY_BPT);

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
        ~BPlusNode();

        BYTE getIsLeaf();
        INT getKeySize();
        KEY_BPT getKey(SIZE);
        BPlusNode* getParent();
        BPlusNode* getNext();
        BPlusNode* getChild(SIZE);

        VOID incKeySize();
        VOID decKeySize();
        VOID setKeySize(INT);
        VOID setKey(SIZE, KEY_BPT);
        VOID setNext(BPlusNode*);
        VOID setChild(SIZE, BPlusNode*);
        VOID setParent(BPlusNode*);

        VOID moveKey(SIZE, SIZE);
        VOID rmoveKey(SIZE, SIZE);
        VOID moveChild(SIZE, SIZE);
        VOID rmoveChild(SIZE, SIZE);
        BOOL split(BPlusNode*&);
        BOOL redistribute(SIZE, SIZE);
        BOOL mer(BPlusNode*&, SIZE, SIZE);
        BOOL merge(BPlusNode*&);
    };

    class BPlusTree {
    private:
        BPlusNode* root;
    public:
        BPlusTree();
        ~BPlusTree();

        VOID print();
        BOOL erase(KEY_BPT);
        BOOL insert(KEY_BPT);

        std::vector < KEY_BPT > getList();
    };
}

#endif // __RN_BPT_H__ //