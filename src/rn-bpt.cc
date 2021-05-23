#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"
#include "rn-bpt.h"



namespace rn {
    SIZE searchBucket(BPlusNode* ptr, KEY_BPT key) {
        if (ptr == NULL) return 0;
        INT l = 0, r = ptr->getKeySize() - 1;
        for (; l < r; ) {
            INT mid = (l + r + 1) / 2;
            if (ptr->getKey(mid) < key) l = mid; // compare function //
            else r = mid - 1;
        }
        if (l < ptr->getKeySize() && ptr->getKey(l) < key) ++l;
        return l;
    }

    SIZE insertBucket(BPlusNode* ptr, KEY_BPT key) {
        if (ptr == NULL) return FALSE;
        INT idx = searchBucket(ptr, key);
        ptr->moveKey(idx, ptr->getKeySize() - idx);
        ptr->moveChild(idx, ptr->getKeySize() - idx + 1);
        ptr->incKeySize();
        ptr->setKey(idx, key);
        return idx;
    }

    BOOL eraseBucket(BPlusNode* ptr, KEY_BPT key) {
        if (ptr == NULL) return FALSE;
        SIZE idx = searchBucket(ptr, key);
        if (ptr->getKey(idx) != key) return FALSE;
        ptr->rmoveKey(idx, ptr->getKeySize() - idx);
        ptr->rmoveChild(idx, ptr->getKeySize() - idx + 1);
        ptr->decKeySize();
        return TRUE;
    }

    BOOL replaceBucket(BPlusNode* ptr, KEY_BPT key, KEY_BPT newKey) { // Legacy //
        if (ptr == NULL) return FALSE;
        SIZE idx = searchBucket(ptr, key);
        if (ptr->getKey(idx) != key) return FALSE;
        ptr->setKey(idx, newKey);
        if (idx == 0 && ptr->getParent()) {
            replaceBucket(ptr->getParent(), key, newKey);
        }
        return TRUE;
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
    BPlusNode::~BPlusNode() {
        if (isLeaf || keySize <= 0) return;
        for (INT i = 0; i <= keySize; ++i) {
            delete child[i];
        }
    }

    BYTE BPlusNode::getIsLeaf() { return isLeaf; }
    INT BPlusNode::getKeySize() { return keySize; }
    KEY_BPT BPlusNode::getKey(SIZE i) { return key[i]; }
    BPlusNode* BPlusNode::getParent() { return parent; }
    BPlusNode* BPlusNode::getNext() { return next; }
    BPlusNode* BPlusNode::getChild(SIZE i) { return child[i]; }

    VOID BPlusNode::incKeySize() { ++keySize; }
    VOID BPlusNode::decKeySize() { --keySize; }
    VOID BPlusNode::setKeySize(INT keySize) { this->keySize = keySize; }
    VOID BPlusNode::setKey(SIZE i, KEY_BPT key) { this->key[i] = key; }
    VOID BPlusNode::setNext(BPlusNode* next) { this->next = next; }
    VOID BPlusNode::setChild(SIZE i, BPlusNode* child) { this->child[i] = child; }
    VOID BPlusNode::setParent(BPlusNode* parent) { this->parent = parent; }

    VOID BPlusNode::moveKey(SIZE piv, SIZE sz) {
        memmove(key + piv + 1, key + piv, sizeof *key * sz);
    }
    VOID BPlusNode::rmoveKey(SIZE piv, SIZE sz) {
        memmove(key + piv, key + piv + 1, sizeof *key * sz);
    }

    VOID BPlusNode::moveChild(SIZE piv, SIZE sz) {
        memmove(child + piv + 1, child + piv, sizeof *child * sz);
    }
    VOID BPlusNode::rmoveChild(SIZE piv, SIZE sz) {
        memmove(child + piv, child + piv + 1, sizeof *child * sz);
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

    BOOL BPlusNode::redistribute(SIZE node, SIZE mnode) {
        BPlusNode* ptr = parent->getChild(mnode);
        BPlusNode* nptr = parent->getChild(node);
        
        if (node > mnode) {
            KEY_BPT pkey = parent->getKey(mnode);
            BPlusNode* tmp = nptr->getChild(nptr->getKeySize());
            INT idx = insertBucket(nptr, pkey);
            if (!nptr->getIsLeaf()) {
                nptr->setChild(idx, ptr->getChild(ptr->getKeySize()));
                nptr->setChild(idx + 1, tmp);
                nptr->getChild(idx)->setParent(nptr);
                nptr->getChild(idx + 1)->setParent(nptr);
            } else {
                eraseBucket(ptr, pkey);
            }
            parent->setKey(mnode, ptr->getKey(ptr->getKeySize() - 1));
            eraseBucket(ptr, ptr->getKey(ptr->getKeySize() - 1));
        } else {
            KEY_BPT pkey = parent->getKey(node);
            BPlusNode* tmp = nptr->getChild(nptr->getKeySize());
            INT idx = insertBucket(nptr, pkey);
            if (!nptr->getIsLeaf()) {
                nptr->setChild(idx, tmp);
                nptr->setChild(idx + 1, ptr->getChild(0));
                nptr->getChild(idx)->setParent(nptr);
                nptr->getChild(idx + 1)->setParent(nptr);
            } else {
                eraseBucket(ptr, pkey);
            }
            parent->setKey(node, ptr->getKey(0));
            eraseBucket(ptr, ptr->getKey(0));
        }
        return TRUE;
    }

    BOOL BPlusNode::mer(BPlusNode*& root, SIZE node, SIZE prev) {
        BPlusNode* ptr = parent->getChild(prev);
        BPlusNode* nptr = parent->getChild(node);
        KEY_BPT pkey = parent->getKey(prev);

        if (!isLeaf) {
            INT idx = insertBucket(ptr, pkey);
            ptr->setChild(idx, ptr->getChild(ptr->getKeySize()));
            ptr->getChild(idx)->setParent(ptr);
        }
        for (INT i = 0; i < nptr->getKeySize(); ++i) { // TODO: Update to memmove //
            INT idx = insertBucket(ptr, nptr->getKey(i));
            if (!isLeaf) {
                ptr->setChild(idx, nptr->getChild(i));
                ptr->getChild(idx)->setParent(ptr);
            }
        }
        if (!isLeaf) {
            ptr->setChild(ptr->getKeySize(), nptr->getChild(nptr->getKeySize()));
            ptr->getChild(ptr->getKeySize())->setParent(ptr);
        }
        if (isLeaf) {
            eraseBucket(parent, pkey);
            ptr->setNext(nptr->getNext());
        } else {
            eraseBucket(parent, pkey);
        }
        if (parent->getKeySize() == 0) { // Warning: That Pointer Sometime Can't Use Before Redistribution(parent) //
            parent->setChild(0, ptr);
            if (parent->getParent() == NULL) {
                root = ptr;
                ptr->setParent(NULL);
                delete parent;
                goto Exit;
            }
        }
        parent->merge(root);
Exit:
        nptr->setKeySize(0); // Block Delete Recursion //
        delete nptr;
        return TRUE;
    }

    BOOL BPlusNode::merge(BPlusNode*& root) {
        if (root == NULL) return FALSE;
        if (parent == NULL) {
            if (keySize == 0) {
                delete root;
                root = NULL;
            }
            return FALSE;
        }
        if (keySize > 0) return FALSE; // TODO: Check UnderFlow Condition - Now: Key is Not Exist in One Bucket //
        INT cidx = -1;
        for (INT i = 0; i <= parent->getKeySize(); ++i) {
            if (parent->getChild(i) == this) cidx = i;
        }
        if (cidx == -1) {
            fprintf(stderr, "Exception in Merge\n");
            exit(-1);
        }
        if (cidx - 1 >= 0 && cidx - 1 <= (INT)parent->getKeySize()) {
            if (parent->getChild(cidx - 1)->getKeySize() + keySize + !isLeaf < DEGREE_BPT * 2) {
                puts("MER1");
                return mer(root, cidx, cidx - 1);
            }
        }
        if (cidx + 1 >= 0 && cidx + 1 <= (INT)parent->getKeySize()) {
            if (parent->getChild(cidx + 1)->getKeySize() + keySize + !isLeaf < DEGREE_BPT * 2) {
                puts("MER2");
                return mer(root, cidx + 1, cidx);
            }
        }
        if (cidx - 1 >= 0 && cidx - 1 <= (INT)parent->getKeySize()) {
            if (parent->getChild(cidx - 1)->getKeySize() > 1) { // TODO: Check Condition With UnderFlow - Now: Key is Exist More Than 1 //
                puts("RED3");
                return redistribute(cidx, cidx - 1);
            }
        }
        if (cidx + 1 >= 0 && cidx + 1 <= (INT)parent->getKeySize()) {
            if (parent->getChild(cidx + 1)->getKeySize() > 1) { // TODO: Check Condition With UnderFlow - Now: Key is Exist More Than 1 //
                puts("RED4");
                return redistribute(cidx, cidx + 1);
            }
        }
        fprintf(stderr, "Exception in Mer");
        exit(-1);
        return FALSE; // Exception //
    }


    BPlusTree::BPlusTree() : root(NULL) {}
    BPlusTree::~BPlusTree() {
        delete root;
    }

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
            puts(" |");
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

    BOOL BPlusTree::erase(KEY_BPT key) {
        if (root == NULL) return FALSE;
        BPlusNode* ptr = root;
        for (; !ptr->getIsLeaf(); ) {
            BOOL flag = 0;
            INT n = ptr->getKeySize();
            for (INT i = 0; i < n; ++i) {
                if (key < ptr->getKey(i)) {  // compare function //
                    ptr = ptr->getChild(i); ++flag;
                    break;
                }
            }
            if (!flag) ptr = ptr->getChild(n);
        }
        if (eraseBucket(ptr, key) == FALSE) return FALSE;
        ptr->merge(root);
        return TRUE;
    }

    std::vector < KEY_BPT > BPlusTree::getList() {
        if (root == NULL) return {};
        std::vector < KEY_BPT > v;
        BPlusNode* ptr = root;
        for (; !ptr->getIsLeaf(); ) {
            ptr = ptr->getChild(0);
        }
        for (; ptr != NULL; ptr = ptr->getNext()) {
            for (INT i = 0; i < ptr->getKeySize(); ++i) {
                v.push_back(ptr->getKey(i));
            }
        }
        return v;
    }
}