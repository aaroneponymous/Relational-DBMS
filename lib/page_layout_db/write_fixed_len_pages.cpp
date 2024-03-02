#include "page_layout_db.h"
#include <iostream>
#include <string>

using namespace std;
using namespace dbms::page;

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <csv_file> <page_file> <page_size>" << endl;
        return 1;
    }


    std::string csv_file_name = argv[1];
    std::string output_file_name = argv[2];

    int page_size = stoi(argv[3]);

    write_fixed_len_pages(csv_file_name, output_file_name, page_size);

    return 0;
}
