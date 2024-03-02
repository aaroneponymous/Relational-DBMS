#include "heap_file_db.h"
#include <iostream>

using namespace dbms::heap_file;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <csv_file> <heapfile> <page_size>" << std::endl;
        return 1;
    }

    const char* csv_file = argv[1];
    const char* heapfile = argv[2];
    int page_size = std::stoi(argv[3]);

    // Call function to build heap file from CSV
    csv_to_heapfile(csv_file, heapfile, page_size);

    return 0;
}
