#include "heap_file_db.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath> // For log function to calculate the pages in a directory

namespace dbms::heap_file
{
    using namespace dbms::page;

    void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
    {
        if (file == nullptr)
        {
            std::cerr << "File pointer is null" << std::endl;
            return;
        }

        int heapfile_page_cap = heapfile_capacity(page_size, 32);
        int heapfile_meta_size = heapfile_metadata_size(heapfile_page_cap);


        // Pages a Directory can Index & Heapfile Directory Header
        heapfile->file_ptr_ = file;
        heapfile->meta_data_size_= heapfile_meta_size;
        heapfile->page_size_ = page_size;
        
        char* heapfile_initial = new char[heapfile_page_cap + heapfile_meta_size + page_size];
        std::memset(heapfile_initial, 0, heapfile->meta_data_size_ + heapfile->page_size_);
        // [x] Redundant to do the following:
        /* int* metadata_dir = reinterpret_cast<int*>(heapfile_initial + heapfile->meta_data_size_);
        std::fill_n(metadata_dir, heapfile->meta_data_size_, 0); */
        
        // Write to File *file
        fwrite(heapfile_initial, sizeof(char), heapfile_page_cap + heapfile_meta_size, file);
        delete[] heapfile_initial;  
        // FIXME: MAKE SURE YOU CLOSE THE FILE - OTHERWISE MEMORY LEAK FROM THE POINTER      
    }

    int heapfile_capacity(int page_size, int address_size)
    {
        return (page_size * 8) / static_cast<int>(ceil(log2(page_size) + address_size));
    }
    
    int heapfile_metadata_size(int heapfile_page_cap)
    {
        return (1 + 1 + (2 * heapfile_page_cap)) * static_cast<int>(sizeof(int));
    }



    // Assume each slot stores an offset as an int
    // FIXME: Change the implementation when you get here
    void get_heapfile_directory(Heapfile *heapfile)
    {
        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;
        char metadata[metasize];
        fread(metadata, sizeof(char), sizeof(metadata), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);
        for (int i = 0; i < heapfile->meta_data_size_; i++)
        {
            std::cout << "Slot [" << i << "]: " << meta_dir[i] << std::endl;  
        }
    }

}