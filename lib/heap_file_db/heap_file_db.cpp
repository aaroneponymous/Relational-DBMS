#include "heap_file_db.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath> // For log function to calculate the pages in a directory

namespace dbms::heap_file
{
    using namespace dbms::page;

    // [x] Think about an implementation where you're initializing
    // [x] a new heapfile in a file which already has data too
    // [x] Implement that after you're done testing this
    void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
    {
        

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
      
        

        // heapfile->file_ptr_ = file;
        heapfile->meta_data_size_ = hf_meta_size;
        heapfile->page_size_ = page_size;
        heapfile->file_ptr_ = file;

        // Allocation of char* buffer
        char* initial_buffer = new char[meta_effective_size + page_size];
        std::memset(initial_buffer, 0, meta_effective_size + page_size);
        int* meta_dir = reinterpret_cast<int*>(initial_buffer);
        std::fill_n(meta_dir, hf_meta_size, 0);

        // Write only metadata to the file
        fwrite(initial_buffer, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        // Set the file pointer back to the start of the file
        fseek(heapfile->file_ptr_, 0, SEEK_SET);

        delete[] initial_buffer;
        // FIXME: MAKE SURE YOU CLOSE THE FILE - OTHERWISE MEMORY LEAK FROM THE POINTER      
    }

    // [x] OPTIMIZE THIS FFS
    PageID alloc_page(Heapfile *heapfile)
    {
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size]; 
        fseek(heapfile->file_ptr_, 0, SEEK_SET);   
        fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        // Set the file pointer back to the start of the file
        fseek(heapfile->file_ptr_, 0, SEEK_SET);

        for (int i = 2; i < heapfile->meta_data_size_ - 2; i += 2)
        {

            if (meta_dir[i] > 0 && meta_dir[i + 2] == 0)
            {
                // Move the file pointer to the appropriate position
                long offset = meta_dir[i]; // Calculate the offset based on meta_dir[i]

                if (fseek(heapfile->file_ptr_, offset, SEEK_SET) != 0) {
                    // Handle error
                    std::cerr << "Seek Did Not Work" << std::endl;
                }

                // Allocate New_Page
                char* new_page = new char[heapfile->page_size_];
                std::memset(new_page, 0, heapfile->page_size_);
                fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

                // Update metadata
                meta_dir[1]++; 
                meta_dir[i + 2] = meta_dir[i] + heapfile->page_size_;
                meta_dir[i + 3] = heapfile->page_size_;

                // Rewrite the buffer into the page
                // Set the file pointer back to the start of the file
                fseek(heapfile->file_ptr_, 0, SEEK_SET);
                fwrite(meta_buff, sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
                fseek(heapfile->file_ptr_, 0, SEEK_SET);

                // Clean up
                delete[] new_page;
                delete[] meta_buff;
                return i + 2;
            }

            else if (meta_dir[i] == 0)
            {
                // Move the file pointer to the appropriate position
                long offset = heapfile->meta_data_size_ * sizeof(int);
                // Set the file pointer back to the start of the file
                fseek(heapfile->file_ptr_, 0, SEEK_SET);

                if (fseek(heapfile->file_ptr_, offset, SEEK_SET) != 0) {
                    // Handle error
                    std::cerr << "Seek Did Not Work" << std::endl;
                }

                // Allocate New_Page
                char* new_page = new char[heapfile->page_size_];
                std::memset(new_page, 0, heapfile->page_size_);
                fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

                // Update metadata
                meta_dir[1]++; // Page Counter
                meta_dir[i] = offset;
                meta_dir[i + 1] = heapfile->page_size_;

                // Rewrite the buffer into the page
                // Set the file pointer back to the start of the file
                fseek(heapfile->file_ptr_, 0, SEEK_SET);
                fwrite(meta_buff, sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
                fseek(heapfile->file_ptr_, 0, SEEK_SET);

                // Clean up
                delete[] new_page;
                delete[] meta_buff;
                return i;

            }

        }
        
        fseek(heapfile->file_ptr_, 0, SEEK_SET);
        delete[] meta_buff;
        return -1;
    }

    void write_page(Page *page, Heapfile *heapfile, PageID pid)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Write Page Failed: Invalid input parameters" << std::endl;
            return;
        }

        // Calculate the offset of the page within the file
        int page_offset = heapfile->meta_data_size_ * sizeof(int) + (pid) * heapfile->page_size_;

        // Seek to the position of the page within the file
        if (fseek(heapfile->file_ptr_, page_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed in write_page function" << std::endl;
            return;
        }

        // Write the page data to the file
        if (fwrite(page->data_, sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error writing page data to file" << std::endl;
            return;
        }
    }

    void read_page(Heapfile *heapfile, PageID pid, Page *page)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Read Page Failed: Invalid input parameters" << std::endl;
            return;
        }

        // Calculate the offset of the page within the file
        int page_offset = heapfile->meta_data_size_ * sizeof(int) + (pid) * heapfile->page_size_;

        // Seek to the position of the page within the file
        if (fseek(heapfile->file_ptr_, page_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed in read_page function" << std::endl;
            return;
        }

        // Allocate memory for reading the page data
        char *page_buffer = new char[heapfile->page_size_];

        // Read the page data from the file
        if (fread(page_buffer, sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error reading page data from file" << std::endl;
            delete[] page_buffer;
            return;
        }

        // Assign the read data to the page
        std::memcpy(page->data_, page_buffer, heapfile->page_size_);

        // Cleanup dynamically allocated memory
        delete[] page_buffer;
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
        /* fclose(heapfile->file_ptr_);
        FILE *new_file = fopen("page_output", "rb");
        heapfile->file_ptr_ = new_file; */
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);
        for (int i = 0; i < heapfile->meta_data_size_; i++)
        {
            std::cout << "Slot [" << i + 1 << "]: " << meta_dir[i] << std::endl;  
        }

        // Set the file pointer back to the start of the file
        fseek(heapfile->file_ptr_, 0, SEEK_SET);
        delete[] metadata;
        // fclose(heapfile->file_ptr_);
    }

}