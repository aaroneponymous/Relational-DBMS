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
        // FIXME: Think about an implementation where you're initializing
        // FIXME: a new heapfile in a file which already has data too
        // FIXME: Implement that after you're done testing this

        if (file == nullptr)
        {
            std::cerr << "File pointer is null" << std::endl;
            return;
        }

        int hf_page_cap = heapfile_capacity(page_size, 32);
        int hf_meta_size = heapfile_metadata_size(hf_page_cap);
        int meta_effective_size = hf_meta_size * sizeof(int);
        std::cout << "Heapfile Page Capacity: " << hf_page_cap << std::endl;
        std::cout << "Heapfile Meta Slots: " << hf_meta_size << std::endl;

        heapfile->file_ptr_ = file;
        heapfile->meta_data_size_ = hf_meta_size;
        heapfile->page_size_ = page_size;

        // Allocation of char* buffer
        char* initial_buffer = new char[meta_effective_size + page_size];
        std::memset(initial_buffer, 0, meta_effective_size + page_size);
        int* meta_dir = reinterpret_cast<int*>(initial_buffer);
        std::fill_n(meta_dir, hf_meta_size, 0);
        meta_dir[1]++;
        meta_dir[2] = meta_effective_size;
        meta_dir[3] = page_size;

        // Write only metadata to the file
        fwrite(initial_buffer, sizeof(char), meta_effective_size, file);
        delete[] initial_buffer;
        // FIXME: MAKE SURE YOU CLOSE THE FILE - OTHERWISE MEMORY LEAK FROM THE POINTER      
    }


    int heapfile_capacity(int page_size, int address_size)
    {
        return (page_size * 8) / static_cast<int>(ceil(log2(page_size) + address_size));
    }
    
    int heapfile_metadata_size(int heapfile_page_cap)
    {
        return (1 + 1 + (2 * heapfile_page_cap));
    }



    void get_heapfile_directory(Heapfile *heapfile)
    {

        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;

        char* metadata = new char[metasize * sizeof(int)];
        // Use memcpy
        // std::memcpy(metadata, heapfile->file_ptr_, metasize * sizeof(int));
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);
        for (int i = 0; i < heapfile->meta_data_size_; i++)
        {
            std::cout << "Slot [" << i + 1 << "]: " << meta_dir[i] << std::endl;  
        }
        delete[] metadata;
    }

}