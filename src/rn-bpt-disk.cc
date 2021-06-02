#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"
#include "rn-bpt-disk.h"

namespace rn {
    Page::Page(IO* io) : io(io), page(-1), offset(-1) { }
    Page::Page(IO* io, LLONG page) : io(io), page(page), offset(page * PAGE_SIZE) {
        io->readPage(buf, offset);
    }
    Page::~Page() { }

    IO* Page::getIO() {
        return io;
    }
    LLONG Page::getPage() {
        return page;
    }
    LLONG Page::getOffset() {
        return offset;
    }

    VOID Page::flush() {
        io->writePage(buf, offset);
    }

    HeaderPage::HeaderPage(IO* io) : Page(io) { }
    HeaderPage::HeaderPage(IO* io, LLONG page) : Page(io, page) {
        if (io->getSize() < 1) {
            0[(LPLLONG)buf] = 1;
            1[(LPLLONG)buf] = 0;
            2[(LPLLONG)buf] = 1;
            io->write(buf, 24, offset);
            LLONG nxt = 0;
            io->write(&nxt, sizeof(LLONG), offset + PAGE_SIZE);
        }
    }
    HeaderPage::~HeaderPage() { }
    LLONG HeaderPage::getFree() {
        return 0[(LPLLONG)buf];
    }
    LLONG HeaderPage::getRoot() {
        return 1[(LPLLONG)buf];
    }
    LLONG HeaderPage::getSize() {
        return 2[(LPLLONG)buf];
    }

    LLONG HeaderPage::newPage() {
        LLONG page = getFree();
        FreePage fp(io, page);
        if (fp.getNext() > 0) setFree(fp.getNext());
        else {
            setFree(page + 1);
            LLONG nxt = 0;
            io->write(&nxt, sizeof(LLONG), (page + 1) * PAGE_SIZE);
        }
        fp.flush();
        return page;
    }

    VOID HeaderPage::setFree(LLONG free) {
        0[(LPLLONG)buf] = free;
        // io->write(&free, sizeof(LLONG), offset);
    }
    VOID HeaderPage::setRoot(LLONG root) {
        1[(LPLLONG)buf] = root;
        // io->write(&root, sizeof(LLONG), offset + 8);
    }
    VOID HeaderPage::setSize(LLONG size) {
        2[(LPLLONG)buf] = size;
        // io->write(&size, sizeof(LLONG), offset + 16);
    }

    FreePage::FreePage(IO* io) : Page(io) { }
    FreePage::FreePage(IO* io, LLONG page) : Page(io, page) { }
    FreePage::~FreePage() { }
    LLONG FreePage::getNext() {
        return 0[(LPLLONG)buf];
    }

    VOID FreePage::setNext(LLONG next) {
        0[(LPLLONG)buf] = next;
        // io->write(&next, sizeof(LLONG), offset);
    }

    NodePage::NodePage(IO* io) : Page(io) { }
    NodePage::NodePage(IO* io, LLONG page) : Page(io, page) { }
    NodePage::~NodePage() { }

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
        if (getIsLeaf()) return 16[(LPLLONG)buf + 16 * idx];    // Warning: Hard Coding With Key, Value Size //
        return 16[(LPLLONG)buf + 2 * idx];                      // Warning: Hard Coding With Key, Page Number Size //
    }
    LLONG NodePage::getChild(SIZE idx) {
        return 15[(LPLLONG)buf + 2 * idx];  // Warning: Hard Coding With Key, Page Number Size //
    }
    VOID NodePage::getValue(SIZE idx, LPSTR ret) {
        memcpy(ret, (LPCSTR)buf + 128 + 128 * idx + 8, VALUE_SIZE);   // Warning: Hard Coding With Key Size & CHAR Size -> sizeof(CHAR) Must be 1 //
    }

    VOID NodePage::setParent(LLONG parent) {
        0[(LPLLONG)buf] = parent;
        // io->write(&parent, sizeof(LLONG), offset);
    }
    VOID NodePage::setIsLeaf(INT isLeaf) {
        2[(LPINT)buf] = isLeaf;
        // io->write(&isLeaf, sizeof(INT), offset + 8);
    }
    VOID NodePage::incKeySize() {
        3[(LPINT)buf]++;
        // io->write((LPINT)buf + 3, sizeof(INT), offset + 12);
    }
    VOID NodePage::decKeySize() {
        3[(LPINT)buf]--;
        // io->write((LPINT)buf + 3, sizeof(INT), offset + 12);
    }
    VOID NodePage::setKeySize(INT keySize) {
        3[(LPINT)buf] = keySize;
        // io->write(&keySize, sizeof(INT), offset + 12);
    }
    VOID NodePage::setNext(LLONG next) {
        15[(LPLLONG)buf] = next;
        // io->write(&next, sizeof(LLONG), offset + 120);
    }
    VOID NodePage::setKey(SIZE idx, LLONG key) {
        if (getIsLeaf()) {  // Warning: Hard Coding With Key, Value Size //
            16[(LPLLONG)buf + 16 * idx] = key;
            // io->write(&key, sizeof(LLONG), offset + 128 + 128 * idx);
        } else {            // Warning: Hard Coding With Key, Page Number Size //
            16[(LPLLONG)buf + 2 * idx] = key;
            // io->write(&key, sizeof(LLONG), offset + 128 + 16 * idx);
        }
    }
    VOID NodePage::setChild(SIZE idx, LLONG child) {
        15[(LPLLONG)buf + 2 * idx] = child;
        // io->write(&child, sizeof(LLONG), offset + 120 + 16 * idx);
    }
    VOID NodePage::setValue(SIZE idx, LPCSTR value) {
        memcpy((LPSTR)buf + 128 + 128 * idx + 8, value, VALUE_SIZE);
    }

    VOID NodePage::moveKey(SIZE piv, SIZE sz) {
        // Warning: Hard Coding With Key, Value Size //
        if (getIsLeaf()) memmove(buf + 128 + 128 * (piv + 1), buf + 128 + 128 * piv, 128 * sz);
        else memmove(buf + 128 + 16 * (piv + 1), buf + 128 + 16 * piv, 16 * sz);
    }

    VOID NodePage::operator=(LLONG page) {
        this->page = page;
        offset = page * PAGE_SIZE;
        io->readPage(buf, offset);
    }


    BPlusTree::BPlusTree(LPCSTR path) {
        io = new("NEW IO IN BPT CONSTRUCTOR") IO(path);
        header = new("NEW HEADER IN BPT CONSTRUCTOR") HeaderPage(io, 0);
    }
    BPlusTree::~BPlusTree() {
        if (io != NULL) delete io;
        if (header != NULL) delete header;
    }

    INT BPlusTree::find(LLONG key, LPSTR ret) {

    }

    SIZE searchBucket(NodePage* ptr, LLONG key) {
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

    BOOL split(HeaderPage* header, NodePage* ptr) {
        INT ptrDegree = (ptr->getIsLeaf() ? LEAF_DEGREE_BPT : INTERNAL_DEGREE_BPT);
        INT ptrDegreeMid = ((ptr->getIsLeaf() ? LEAF_DEGREE_BPT : INTERNAL_DEGREE_BPT) + 1) / 2;
        if (ptr->getKeySize() < ptrDegree) return 1;
        NodePage splt(header->getIO(), header->newPage());
        splt.setParent(0);
        splt.setIsLeaf(ptr->getIsLeaf());
        splt.setKeySize(0);
        NodePage par(header->getIO());
        if (ptr->getParent() == 0) {
            BYTE val[VALUE_SIZE];
            par = header->newPage();
            par.setParent(0);
            par.setIsLeaf(0);
            par.setKeySize(1);
            par.setKey(0, ptr->getKey(ptrDegreeMid));
            ptr->getValue(ptrDegreeMid, val);
            par.setValue(0, val);
            par.setChild(0, ptr->getPage());
            par.setChild(1, splt.getPage());
            header->setRoot(par.getPage());
        } else {
            par = ptr->getParent();
            INT piv;
            for (piv = 0; piv <= par.getKeySize(); ++piv) {
                if (par.getChild(piv) == ptr->getPage()) break;
            }
            par.moveKey(piv, par.getKeySize() - piv);
            par.incKeySize();
            par.setKey(piv, ptr->getKey(ptrDegreeMid));

            par.setChild(piv, ptr->getPage());
            par.setChild(piv + 1, splt.getPage());
        }
        splt.setParent(par.getPage());
        ptr->setParent(par.getPage());
        ptr->setKeySize(ptrDegreeMid);
        if (ptr->getIsLeaf()) {
            splt.setNext(ptr->getNext());
            ptr->setNext(splt.getPage());
        } else {
            splt.setChild(0, ptr->getChild(ptrDegreeMid + 1));
            NodePage np(header->getIO(), ptr->getChild(ptrDegreeMid + 1));
            np.setParent(splt.getPage());
            np.flush();
        }
        for (INT i = !ptr->getIsLeaf(), j = 0; i < ptrDegreeMid; ++i, ++j) {
            splt.incKeySize();
            splt.setKey(j, ptr->getKey(ptrDegreeMid + i));
            if (ptr->getIsLeaf()) {
                BYTE val[VALUE_SIZE];
                ptr->getValue(ptrDegreeMid + i, val);
                splt.setValue(j, val);
            } else {
                splt.setChild(j + 1, ptr->getChild(ptrDegreeMid + i + 1));
                NodePage np(header->getIO(), ptr->getChild(ptrDegreeMid + i + 1));
                np.setParent(splt.getPage());
                np.flush();
            }
        }

        header->flush();
        ptr->flush();
        par.flush();
        splt.flush();
        split(header, &par);
        return 0;
    }

    INT BPlusTree::insert(LLONG key, LPCSTR value) {
        if (header->getRoot() == 0) {
            header->setRoot(header->newPage());
            NodePage np(io, header->getRoot());
            np.setParent(0);
            np.setIsLeaf(1);
            np.setKeySize(1);
            np.setKey(0, key);
            np.setValue(0, value);
            np.flush();
            header->flush();
            return 0;
        }
        NodePage ptr(io, header->getRoot());
        for (; !ptr.getIsLeaf(); ) {
            BOOL flag = 0;
            for (INT i = 0; i < ptr.getKeySize(); ++i) {
                if (key <= ptr.getKey(i)) {
                    ptr = ptr.getChild(i); ++flag;
                    break;
                }
            }
            if (!flag) ptr = ptr.getChild(ptr.getKeySize());
        }
        INT idx = searchBucket(&ptr, key);
        
        ptr.moveKey(idx, ptr.getKeySize() - idx);
        ptr.incKeySize();
        ptr.setKey(idx, key);
        ptr.setValue(idx, value);
        ptr.flush();
        split(header, &ptr);
        return 0;
    }

    INT BPlusTree::erase(LLONG key) {

    }

    VOID BPlusTree::print() {
        if (header->getRoot() == 0) return;
        std::queue < std::pair < LLONG, INT > > q;
        INT lv = 0;
        q.emplace(header->getRoot(), 0);
        puts("\n [ TREE DETAIL ]");
        for (; !q.empty(); ) {
            auto [x, c] = q.front(); q.pop();
            if (lv != c) lv = c, puts("");
            NodePage ptr(io);
            ptr = x;
            if (ptr.getKeySize() > 0) {
                BYTE val[VALUE_SIZE];
                ptr.getValue(0, val);
                if (ptr.getIsLeaf()) printf("(%lld/%s)", ptr.getKey(0), val);
                else printf("(%lld)", ptr.getKey(0));
            }
            for (INT i = 1; i < ptr.getKeySize(); ++i) {
                BYTE val[VALUE_SIZE];
                ptr.getValue(i, val);
                if (ptr.getIsLeaf()) printf(", (%lld/%s)", ptr.getKey(i), val);
                else printf(", (%lld)", ptr.getKey(i));
            }
            printf(" | ");
            if (ptr.getIsLeaf()) continue;
            for (INT i = 0; i <= ptr.getKeySize(); ++i) {
                q.emplace(ptr.getChild(i), c + 1);
            }
        }
        puts("\n");
    }
}