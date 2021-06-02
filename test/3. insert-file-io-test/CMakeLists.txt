ADD_COMPILE_OPTIONS ( -g -Wall -O2 -std=c++17 )
ADD_EXECUTABLE (
    insert-file-io-test.out
    main.cc
    ../../src/rn-bpt-disk.cc
    ../../src/rn-file-io.cc
)