#include "heap_file_db.h"
#include <iostream>
#include <cstring>
#include <cmath> // For log function to calculate the pages in a directory

namespace dbms::heap_file
{
    using namespace dbms::page;

    void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
    {
        // Initialization of a Heapfile
        // Directory Based Approach:
            // 1. Pages Linked List
            // 2. Heapfile Directory to Heapfile Directory Linked List

        if (file == nullptr)
        {
            std::cerr << "File pointer is null" << std::endl;
            return;
        }

        // Pages a Directory can Index & Heapfile Directory Header
        heapfile->file_ptr_ = file;
        heapfile->meta_data_size_= heapfile_metadata_size(heapfile_capacity
                                    (page_size, 32));
        heapfile->page_size_ = page_size;
        
        char* heapfile_initial = new char[heapfile->meta_data_size_ + heapfile->page_size_];
        std::memset(heapfile_initial, 0, heapfile->meta_data_size_ + heapfile->page_size_);
        int* metadata_dir = reinterpret_cast<int*>(heapfile_initial + heapfile->meta_data_size_);
        std::fill_n(metadata_dir, heapfile->meta_data_size_, 0);
        
    }

    /**
     * Lambda Expression for Calculating the Heap File Page Capacity
    */

    auto heapfile_capacity = [](int page_size, int address_size) 
    {
        return (page_size * 8) / static_cast<int>(ceil(log2(page_size) + address_size));
    };

    auto heapfile_metadata_size = [](int heapfile_page_cap) 
    {
        return (1 + 1 + (2 * heapfile_page_cap)) * static_cast<int>(sizeof(int));
    };

    // Assume each slot stores an offset as an int
    // FIXME: Change the implementation when you get here
    int *get_heapfile_directory(Page *page)
    {
        return reinterpret_cast<int *>(reinterpret_cast<char *>(page->data_) + page->page_size_ - page->slot_size_ * sizeof(int));
    }


}