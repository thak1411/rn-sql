#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"
#include "rn-bpt.h"



namespace rn {
    SIZE insertBucket(BPlusNode* ptr, KEY_BPT key) {
        INT l = 0, r = ptr->getKeySize() - 1;
        for (; l < r; ) {
            INT mid = (l + r + 1) / 2;
            if (ptr->getKey(mid) < key) l = mid; // compare function //
            else r = mid - 1;
        }
        if (ptr->getKey(l) < key) ++l;
        ptr->moveKey(l, ptr->getKeySize() - l);
        ptr->moveChild(l, ptr->getKeySize() - l + 1);
        ptr->incKeySize();
        ptr->setKey(l, key);
        return l;
    }

    BPlusNode::BPlusNode() : isLeaf(0), keySize(0), parent(NULL) {
        memset(key, 0, sizeof key);
        memset(child, 0, sizeof child);
    }
    BPlusNode::BPlusNode(BYTE isLeaf) : isLeaf(isLeaf), keySize(0), parent(NULL) {
        memset(key, 0, sizeof key);
        memset(child, 0, sizeof child);
    }
    BPlusNode::BPlusNode(BYTE isLeaf, KEY_BPT key) : isLeaf(isLeaf), keySize(1), parent(NULL) {
        memset(this->key, 0, sizeof this->key);
        memset(child, 0, sizeof child);
        this->key[0] = key;
    }

    BYTE BPlusNode::getIsLeaf() { return isLeaf; }
    INT BPlusNode::getKeySize() { return keySize; }
    KEY_BPT BPlusNode::getKey(SIZE i) { return key[i]; }
    BPlusNode* BPlusNode::getNext() { return next; }
    BPlusNode* BPlusNode::getChild(SIZE i) { return child[i]; }

    VOID BPlusNode::incKeySize() { ++keySize; }
    VOID BPlusNode::setKeySize(INT keySize) { this->keySize = keySize; }
    VOID BPlusNode::setKey(SIZE i, KEY_BPT key) { this->key[i] = key; }
    VOID BPlusNode::setNext(BPlusNode* next) { this->next = next; }
    VOID BPlusNode::setChild(SIZE i, BPlusNode* child) { this->child[i] = child; }
    VOID BPlusNode::setParent(BPlusNode* parent) { this->parent = parent; }

    VOID BPlusNode::moveKey(SIZE piv, SIZE sz) {
        memmove(key + piv + 1, key + piv, sizeof *key * sz);
    }

    VOID BPlusNode::moveChild(SIZE piv, SIZE sz) {
        memmove(child + piv + 1, child + piv, sizeof *child * sz);
    }

    BOOL BPlusNode::split(BPlusNode*& root) {
        if (getKeySize() < DEGREE_BPT * 2) return FALSE;
        BPlusNode* splt = new("NEW NODE BPT::SPLIT - SPLT") BPlusNode(isLeaf);
        if (parent == NULL) {
            parent = root = new("NEW NODE BPT::SPLIT - ROOT") BPlusNode(0, key[DEGREE_BPT]);
            root->setChild(0, this);
            root->setChild(1, splt);
        } else {
            INT piv = insertBucket(parent, key[DEGREE_BPT]);
            parent->setChild(piv, this);
            parent->setChild(piv + 1, splt);
        }
        splt->setParent(parent);
        if (isLeaf) {
            splt->setNext(next);
            this->setNext(splt);
        }
        this->setKeySize(DEGREE_BPT);
        if (!isLeaf) {
            splt->setChild(0, child[DEGREE_BPT + 1]);
            child[DEGREE_BPT + 1]->setParent(splt);
        }
        for (INT i = !isLeaf, j = 0; i < DEGREE_BPT; ++i, ++j) {
            splt->incKeySize();
            splt->setKey(j, key[DEGREE_BPT + i]);
            if (!isLeaf) {
                splt->setChild(j + 1, child[DEGREE_BPT + i + 1]);
                child[DEGREE_BPT + i + 1]->setParent(splt);
            }
        }
        parent->split(root);
        return TRUE;
    }


    BPlusTree::BPlusTree() : root(NULL) {}

    VOID BPlusTree::print() {
        if (root == NULL) return;
        BPlusNode* ptr = root;
        for (; !ptr->getIsLeaf(); ) {
            ptr = ptr->getChild(0);
        }
        for (; ptr != NULL; ptr = ptr->getNext()) {
            for (INT i = 0; i < ptr->getKeySize(); ++i) {
                printf(KEY_BPT_FMT " ", ptr->getKey(i));
            }
        }
        puts("");
    }

    BOOL BPlusTree::insert(KEY_BPT key) {
        if (root == NULL) {
            root = new("NEW NODE IN BPT::INSERT") BPlusNode(1, key);
            return TRUE;
        }
        BPlusNode* ptr = root;
        for (; !ptr->getIsLeaf(); ) {
            BOOL flag = 0;
            INT n = ptr->getKeySize();
            for (INT i = 0; i < n; ++i) {
                if (key <= ptr->getKey(i)) {  // compare function //
                    ptr = ptr->getChild(i); ++flag;
                    break;
                }
            }
            if (!flag) ptr = ptr->getChild(n);
        }
        insertBucket(ptr, key);
        ptr->split(root);
        return TRUE;
    }
}