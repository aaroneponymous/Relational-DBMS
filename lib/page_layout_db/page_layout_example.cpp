#include "page_layout_db.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring> // For std::strcpy
#include <iostream>

using namespace dbms::page;
using namespace dbms::record_codec;

int main() {


    // BUG: Undefined Behaviour at INT MAX
    // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-100.csv";
    std::string output_file_name = "page_output"; 

    write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    read_fixed_len_pages(output_file_name, 100000);

    return 0;
}

