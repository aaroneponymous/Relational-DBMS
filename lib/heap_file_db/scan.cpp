#include "heap_file_db.h"
#include <iostream>

using namespace dbms::heap_file;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <heapfile> <page_size>" << std::endl;
        return 1;
    }

    const char* heapfile = argv[1];
    int page_size = std::stoi(argv[2]);

    // Call function to scan and print all records in heap file
    scan(heapfile, page_size);

    return 0;
}
