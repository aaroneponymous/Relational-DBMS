#include "page_layout_db.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring> // For std::strcpy
#include <cmath>
#include <cstdio>

using namespace dbms::page;
using namespace dbms::record_codec;

    int return_heapfile_cap(int page_size, int address_size)
    {
        return (page_size * 8)/int((ceil(log2(page_size) + address_size)));
    }

int main() {


    // BUG: Undefined Behaviour at INT MAX
    // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-100.csv";
    std::string output_file_name = "page_output"; 

    // write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    // read_fixed_len_pages(output_file_name, 100000);    


    /* int page_size = 16 * 1024;
    int bits_free_space = ceil(log(page_size)/log(2));
    int bits_page_addr = 32;
    int total_bits_needed = bits_free_space + bits_page_addr;
    int page_size_bits = page_size * 8;
    int pages_cap_dir = page_size * 8/total_bits_needed;
    std::cout << "Bits for Free Space: " << bits_free_space << "\n";
    std::cout << "Total Bits: " << total_bits_needed << "\n";
    std::cout << "Page Size in Bits: " << page_size_bits << "\n";
    std::cout << "Pages Directory can Index: " << return_heapfile_cap(page_size, bits_page_addr) << std::endl;
 */

    std::cout << "Size of int as integer: " << static_cast<int>(sizeof(int)) << std::endl;
    


    return 0;
}

