#include "heap_file_db.h"
#include <iostream>

using namespace dbms::heap_file;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <heapfile> <csv_file> <page_size>" << std::endl;
        return 1;
    }

    const char* heapfile = argv[1];
    const char* csv_file = argv[2];
    int page_size = std::stoi(argv[3]);

    // Call function to insert all records from CSV file to heap file
    insert_csv_to_heapfile(heapfile, csv_file, page_size);

    return 0;
}
