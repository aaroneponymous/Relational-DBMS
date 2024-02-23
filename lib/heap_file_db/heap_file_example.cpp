#include "heap_file_db.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring> // For std::strcpy
#include <cmath>
#include <cstdio>

using namespace dbms::heap_file;

    /* */

int main() {


    // BUG: Undefined Behaviour at INT MAX
    // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-test.csv";
    std::string output_file_name = "page_output"; 

    // write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    // read_fixed_len_pages(output_file_name, 100000);
    Heapfile *test_heapfile = new Heapfile;
    FILE *file = fopen("page_output", "a");
    if (file != nullptr)
    {
        std::cout << "Opened" << std::endl;
    }

    init_heapfile(test_heapfile, 10000, file);
    delete test_heapfile;

    fclose(file);




    return 0;
}

