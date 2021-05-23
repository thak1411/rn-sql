ADD_COMPILE_OPTIONS ( -g -Wall -O2 -std=c++17 )
ADD_EXECUTABLE (
    insert-test.out
    main.cc
    ../../src/rn-bpt.cc
)