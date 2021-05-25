#define __STRICT__   // Show All Memory Alloc & DeAlloc //
#define __MEMDEBUG__ // Use Memory Debuging //
#include "rn-memory.hpp"
#include "rn-type.h"
#include "rn-bpt.h"

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
#ifdef __MEMDEBUG__
    if (rn::getMemoryCount()) {
        fprintf(stderr, "Memory Leak Count: %lu\n\n", rn::getMemoryCount());
        rn::getMemoryInfo();
    }
#endif
    return 0;
}