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


    // BUG: Page Size Has to be 16384 to Insert Records?
    int page_size = 1500;
    int page_cap = page_record_capacity(page_size);
    csv_to_heapfile("people-100.csv", "heapfile-output", page_size);
    scan("heapfile-output", page_size);
    // insert_csv_to_heapfile("heapfile-output", "people-test.csv", page_size);
    // scan("heapfile-output", page_size);
    
    /* Heapfile *tester_it = new Heapfile;
    FILE *file = fopen("heapfile-output", "r+");

    init_heapfile(tester_it, page_size, file);
    RecordIterator test_iterator(tester_it);

    fclose(file);
    delete tester_it; */ 





   /*  Heapfile *test_heapfile = new Heapfile;
    FILE *file = fopen("heapfile-output", "rb");
    init_heapfile(test_heapfile, page_size, file);
    // print_heapfile_directory(test_heapfile);
    int offset_dir = ftell(test_heapfile->file_ptr_); 
    int meta_effective_size = test_heapfile->meta_data_size_ * sizeof(int);
    char* meta_buff = new char[meta_effective_size];
    std::memset(meta_buff, 0, meta_effective_size);
    std::fread(meta_buff, sizeof(char), meta_effective_size, test_heapfile->file_ptr_);
    std::fseek(test_heapfile->file_ptr_, offset_dir, SEEK_SET);
    int* meta_dir = reinterpret_cast<int*>(meta_buff);
    int total_pages = 2 * meta_dir[1];
    print_heapfile_directory(test_heapfile);
    int record_count{0};

    
    for (int i = 2; i <= total_pages; i += 2)
    {
        if (meta_dir[i] > 0)
        {
            Page *test_read_page = new Page;
            init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
            read_page(test_heapfile, i, test_read_page);
            record_count += get_record_count(test_read_page);
            print_page_records(test_read_page);
            delete[] static_cast<char*>(test_read_page->data_);
            delete test_read_page;
            
        }

    }

    std::cout << "Total Records: " << record_count << std::endl;

    fclose(file);
    // test_heapfile->file_ptr_ = nullptr;
    delete test_heapfile;
    // delete[] static_cast<char*>(test_read_page->data_);
    delete[] meta_buff; */

    

    /* init_fixed_len_page(test_read_page, page_size, page_cap + 2);
    read_page(test_heapfile, 2, test_read_page);
    print_page_records(test_read_page);
    std::cout << "\n\nPage Record Count: " << get_record_count(test_read_page) << std::endl;
    delete[] static_cast<char*>(test_read_page->data_);
    init_fixed_len_page(test_read_page, page_size, page_cap + 2);
    read_page(test_heapfile, 4, test_read_page);
    print_page_records(test_read_page);


    delete[] static_cast<char*>(test_read_page->data_);

    delete test_read_page;
    // delete test_read_page;
    fclose(file);
    delete test_heapfile;

    
    


    // BUG: Undefined Behaviour at INT MAX
    /* // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-test.csv";
    std::string output_file_name = "heapfile-output";

    // write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    // read_fixed_len_pages(output_file_name, 100000); 

    Page *test_page = new Page;
    // Page *test_read_page = new Page;
    int page_size = 500;
    int page_cap = page_record_capacity(page_size);
    init_fixed_len_page(test_page, page_size, page_cap + 2);
    // init_fixed_len_page(test_read_page, page_size, page_cap + 2);
    add_records_to_page(test_page, csv_file_name);
    // print_page_records(test_page);


    Heapfile *test_heapfile = new Heapfile;
    FILE *file = fopen("heapfile-output", "r+");
    int dir_offset = ftell(file);
    if (file != nullptr)
    {
        std::cout << "Opened" << std::endl;
    }

    init_heapfile(test_heapfile, page_size, file);
    // print_heapfile_directory(test_heapfile);
    // std::cout << "\n\n\n\nAfter Allocation of a Page:" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        // fseek(test_heapfile->file_ptr_, dir_offset, SEEK_SET);
        alloc_page(test_heapfile);
    }

    write_page(test_page, test_heapfile, 2);
    // read_page(test_heapfile, 2, test_read_page);
    // print_page_records(test_read_page);
    // std::cout << "\n\nPage Record Count: " << get_record_count(test_read_page) << std::endl;

    // RecordIterator test_iterator(test_heapfile);

    // while(test_iterator.hasNext())
    // {
    //     Record record_tester = test_iterator.next();
    //     print_record(Heapfile *test_heapfile = new Heapfile;
    FILE *file = fopen("heapfile-output", "rb");
    init_heapfile(test_heapfile, page_size, file);
    // print_heapfile_directory(test_heapfile);
    int offset_dir = ftell(test_heapfile->file_ptr_); 
    int meta_effective_size = test_heapfile->meta_data_size_ * sizeof(int);
    char* meta_buff = new char[meta_effective_size];
    std::memset(meta_buff, 0, meta_effective_size);
    std::fread(meta_buff, sizeof(char), meta_effective_size, test_heapfile->file_ptr_);
    std::fseek(test_heapfile->file_ptr_, offset_dir, SEEK_SET);
    int* meta_dir = reinterpret_cast<int*>(meta_buff);
    int total_pages = 2 * meta_dir[1];
    std:: cout << "TOTAL PAGES: " << total_pages/2 << std::endl;
    print_heapfile_directory(test_heapfile);
    int record_count{0};

    
    for (int i = 2; i < 24; i += 2)
    {
        if (meta_dir[i] > 0)
        {
            std::cout << "Printing Record " << ++record_count << "\n" << std::endl;
            Page *test_read_page = new Page;
            init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
            read_page(test_heapfile, i, test_read_page);
            print_page_records(test_read_page);
            delete[] static_cast<char*>(test_read_page->data_);
            delete test_read_page;
            
        }

    }

    fclose(file);
    // test_heapfile->file_ptr_ = nullptr;
    delete test_heapfile;
    // delete[] static_cast<char*>(test_read_page->data_);
    delete[] meta_buff;record_tester);
    // } */


   





    
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
    
    // delete[] static_cast<char*>(test_read_page->data_);
    // delete[] static_cast<char*>(test_read_page->data_);
    // delete test_read_page;
    // delete test_read_page;
    // fclose(file);
    // delete test_heapfile;
    return 0;
}

