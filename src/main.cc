#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"

#define __RN_ON_DISK__  // Use On Disk BPlusTree //
#ifdef __RN_ON_DISK__   // Use On Disk BPlusTree //
    #include "rn-bpt-disk.h"
#else   // Use In Memory BPlusTree //
    #include "rn-bpt.h"
#endif  // __RN_ON_DISK__ //

#include "rn-type.h"

#include <cstdio>   // scanf, printf, fprintf, puts //

INT input() {
    INT x;
    puts("1. PRINT TREE");
    puts("2. INSERT NODE");
    puts("3. DELETE NODE");
    puts("4. EXIT");
    printf(" >>> ");
    scanf("%d", &x);
    return x;
}

int main() {
#ifdef __RN_ON_DISK__   // Use On Disk BPlusTree //
    rn::BPlusTree* bpt = new("NEW BPT IN MAIN") rn::BPlusTree("rn-bpt.dat");
    BYTE val[VALUE_SIZE] = { 0, };
    LLONG key;
    for (INT x; (x = input()); ) {
        switch (x) {
        case 1:
            bpt->print();
            break;
        case 2:
            scanf("%lld%s", &key, val);
            bpt->insert(key, val);
            break;
        case 3:
            scanf("%lld", &key);
            bpt->erase(key);
            break;
        default:
            puts("Exit...");
            goto Exit;
        }
    }
Exit:
    delete bpt;
#else   // Use In Memory BPlusTree //
    rn::BPlusTree* bpt = new("NEW BPT IN MAIN") rn::BPlusTree();
    for (INT x, key; (x = input()); ) {
        switch (x) {
        case 1:
            bpt->printDetail();
            bpt->print();
            break;
        case 2:
            scanf("%d", &key);
            bpt->insert(key);
            break;
        case 3:
            scanf("%d", &key);
            bpt->erase(key);
            break;
        default:
            puts("Exit...");
            goto Exit;
        }
    }
Exit:
    delete bpt;
#endif  // __RN_ON_DISK__ //

#ifdef __MEMDEBUG__
    if (rn::getMemoryCount()) {
        fprintf(stderr, "Memory Leak Count: %lu\n\n", rn::getMemoryCount());
        rn::getMemoryInfo();
    }
#endif // __MEMDEBUG__ //
    return 0;
}