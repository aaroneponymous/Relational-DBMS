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
using namespace dbms::page;

    /* */

int main() {

    csv_to_heapfile("people-test.csv", "heapfile-output", 1500);
    scan("heapfile-output", 1500);


    // BUG: Undefined Behaviour at INT MAX
    /* // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-test.csv";
    std::string output_file_name = "page_output";

    // write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    // read_fixed_len_pages(output_file_name, 100000); 

    Page *test_page = new Page;
    // Page *test_read_page = new Page;
    int page_size = 16384;
    int page_cap = page_record_capacity(page_size);
    init_fixed_len_page(test_page, page_size, page_cap + 2);
    // init_fixed_len_page(test_read_page, page_size, page_cap + 2);
    add_records_to_page(test_page, csv_file_name);
    // print_page_records(test_page);


    Heapfile *test_heapfile = new Heapfile;
    FILE *file = fopen("heapfile_output", "r+");
    if (file != nullptr)
    {
        std::cout << "Opened" << std::endl;
    }

    init_heapfile(test_heapfile, page_size, file);
    // print_heapfile_directory(test_heapfile);
    // std::cout << "\n\n\n\nAfter Allocation of a Page:" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        alloc_page(test_heapfile);
    }

    write_page(test_page, test_heapfile, 2);
    // read_page(test_heapfile, 2, test_read_page);
    // print_page_records(test_read_page);
    // std::cout << "\n\nPage Record Count: " << get_record_count(test_read_page) << std::endl;

    RecordIterator test_iterator(test_heapfile);

    while(test_iterator.hasNext())
    {
        Record record_tester = test_iterator.next();
        print_record(record_tester);
    }
 */

   





    
    /* FILE *new_file = fopen("page_output", "rb+");
    test_heapfile->file_ptr_ = new_file;
    int page_id = alloc_page(test_heapfile);
    std::cout << "\nPage Slot Index: " << page_id << std::endl;
    FILE *another_file = fopen("page_output", "rb+");
    get_heapfile_directory(test_heapfile); */
    
    /* file = fopen("page_output", "rb");
    int page_id = alloc_page(test_heapfile);
    get_heapfile_directory(test_heapfile);
    std::cout << "Page ID: " << page_id << std::endl; */
    // fclose(file);
    // FILE *new_file = fopen("page_output", "r");
    // test_heapfile->file_ptr_ = new_file;
    // get_heapfile_directory(test_heapfile);
    
    /* delete[] static_cast<char*>(test_page->data_);
    // delete[] static_cast<char*>(test_read_page->data_);
    delete test_page;
    // delete test_read_page;
    fclose(file);
    delete test_heapfile; */
    return 0;
}

