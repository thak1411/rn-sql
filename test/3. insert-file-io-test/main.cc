// #define __MEMDEBUG__
#include "../../src/rn-bpt-disk.h"
#include "../../src/rn-memory.hpp"
#include "../../src/rn-type.h"

#include <random>
#include <chrono>
#include <cstdio>
#include <set>

#define TC_L        -50
#define TC_R        50
#define TC_COUNT    10
#define TC_SMALL    1000
#define TC_BIG      3000000

BOOL comp(std::multiset < LLONG > s, std::vector < LLONG > v) {
    std::vector < LLONG > w;
    for (auto& it : s) w.push_back(it);
    if (v.size() != w.size()) return FALSE;
    for (int i = 0; i < v.size(); ++i) {
        if (v[i] != w[i]) return FALSE;
    }
    return TRUE;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(TC_L, TC_R);
    std::multiset < LLONG > s;
    puts(" [ START INSERT TESTING ] ");
    for (INT t = TC_COUNT; t--; ) {
        rn::BPlusTree* bpt = new("NEW BPT IN INSERT-TEST MAIN") rn::BPlusTree("rn-bpt.dat");
        printf(" >> Start %d / %d test ", TC_COUNT - t, TC_COUNT);
        for (INT i = 0; i < TC_SMALL; ++i) {
            LLONG rnd = dis(gen);
            s.insert(rnd);
            bpt->insert(rnd, "test");
            if (!comp(s, bpt->getList())) {
                puts("CONFLICT");
                delete bpt;
                goto Exit;
            }
        }
        puts("SUCCESS");
        delete bpt;
    }
    if (1) {
        std::chrono::time_point < std::chrono::high_resolution_clock > start = std::chrono::high_resolution_clock::now();
        puts("\n [ TIME TEST ]");
        rn::BPlusTree* bpt = new("NEW BPT IN TIME INSERT-TEST MAIN") rn::BPlusTree("rn-bpt-big.dat");
        printf(" >> Start Time test ");
        for (INT i = 0; i < TC_BIG; ++i) {
            bpt->insert(dis(gen), "test");
        }
        puts("FINISH");
        delete bpt;
        std::chrono::time_point < std::chrono::high_resolution_clock > end = std::chrono::high_resolution_clock::now();
        std::chrono::duration diff = end - start;
        printf(" >> DURATION TIME %lldus -> %lldms -> %llds\n", 
            std::chrono::duration_cast < std::chrono::microseconds >(diff).count(),
            std::chrono::duration_cast < std::chrono::milliseconds >(diff).count(),
            std::chrono::duration_cast < std::chrono::seconds >(diff).count()
        );
    }
Exit:
#ifdef __MEMDEBUG__
    fprintf(stderr, "Memory Leak Count: %lu\n\n", rn::getMemoryCount());
    rn::getMemoryInfo();
#endif
    return 0;
}