#include "page_layout_db.h"
#include <iostream>
#include <string>

using namespace std;
using namespace dbms::page;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <page_file> <page_size>" << endl;
        return 1;
    }

    string page_file_name = argv[1];
    int page_size = stoi(argv[2]);

    read_fixed_len_pages(page_file_name, page_size);
    
    return 0;
}
