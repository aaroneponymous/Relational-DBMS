#include "heap_file_db.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath> // For log function to calculate the pages in a directory

namespace dbms::heap_file
{
    using namespace dbms::page;

    // [x] Think about an implementation where you're initializing
    // FIXME: MAKE SURE YOU CLOSE THE FILE - OTHERWISE MEMORY LEAK FROM THE POINTER      
    // [x] a new heapfile in a file which already has data too
    // [x] Implement that after you're done testing this
    // NOTE: I do that after I get out of the function call
    void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
    {
        

        if (file == nullptr)
        {
            std::cerr << "File pointer is null" << std::endl;
            return;
        }

        int dir_start_offset = ftell(file);
        
        int hf_page_cap = heapfile_capacity(page_size, 32);
        int hf_meta_size = heapfile_metadata_size(hf_page_cap);
        int meta_effective_size = hf_meta_size * sizeof(int);
        /* std::cout << "Heapfile Page Capacity: " << hf_page_cap << std::endl;
        std::cout << "Heapfile Meta Slots: " << hf_meta_size << std::endl; */
        
        heapfile->meta_data_size_ = hf_meta_size;
        heapfile->page_size_ = page_size;
        heapfile->file_ptr_ = file;

        char* initial_buffer = new char[meta_effective_size];
        std::memset(initial_buffer, 0, meta_effective_size);
        int* meta_dir = reinterpret_cast<int*>(initial_buffer);
        std::fill_n(meta_dir, hf_meta_size, 0);
        meta_dir[0] = 0;

        // Write only metadata to the file
        std::fwrite(initial_buffer, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        // Set the file pointer back to the start of the file
        // FIXME: Making an assumption here that I'd need to access the 
        // FIXME: directory once I leave this function call
        std::fseek(heapfile->file_ptr_, dir_start_offset, SEEK_SET);
        delete[] initial_buffer;
    }

    void init_heapfile_read(Heapfile *heapfile, int page_size, FILE *file)
    {
        
        if (file == nullptr)
        {
            std::cerr << "File pointer is null" << std::endl;
            return;
        }
        
        int hf_page_cap = heapfile_capacity(page_size, 32);
        int hf_meta_size = heapfile_metadata_size(hf_page_cap);
        int meta_effective_size = hf_meta_size * sizeof(int);
        
        heapfile->meta_data_size_ = hf_meta_size;
        heapfile->page_size_ = page_size;
        heapfile->file_ptr_ = file;

    }

    // [x] : Changed Implementation to Accomodate for Executables
    PageID alloc_page(Heapfile *heapfile)
    {
        // NOTE: Assumption is that offsets are relative to the beginning of dir or pages
        // Pointer Offset to the Directory
        int offset_dir = ftell(heapfile->file_ptr_); 
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);

        // First Page
        if (meta_dir[1] == 0)
        {
            // Move the file pointer to the appropriate position
            int offset = heapfile->meta_data_size_ * sizeof(int);
            // File already at beginning of heap directory so just add offset
            if (std::fseek(heapfile->file_ptr_, offset, SEEK_SET) != 0) {
                // Handle error
                std::cerr << "Seek Did Not Work" << std::endl;
                std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                delete[] meta_buff;
                return -1;

            }

            // std::cout << "Page Number : " << 2 << "\n";
            // std::cout << "File Pointer Position Page ID (Before New Page): " << " - " << ftell(heapfile->file_ptr_) << "\n";

            // Allocate New_Page
            char new_page[heapfile->page_size_];
            std::memset(new_page, 0, heapfile->page_size_);
            fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

            // std::cout << "File Pointer Position Page ID (After New Page): " << " - " << ftell(heapfile->file_ptr_) << "\n";

            // Update metadata
            meta_dir[1]++; // Page Counter
            meta_dir[2] = offset; // Page Offset Relative to the Heapfile Offset
            meta_dir[3] = heapfile->page_size_; // Free Space

            // Rewrite the buffer into the page
            // Set the file pointer back to the start of the file
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);

            // std::cout << "File Pointer Position Page ID (Rewrite Dir): " << " - " << ftell(heapfile->file_ptr_) << "\n";
            fwrite(reinterpret_cast<char*>(meta_dir), sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
            // std::cout << "File Pointer Position Page ID (After Rewrite): " << " - " << ftell(heapfile->file_ptr_) << "\n";
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            // std::cout << "File Pointer Position Page ID (Exit Function): " << " - " << ftell(heapfile->file_ptr_) << "\n" << std::endl;
            // print_heapfile_directory(heapfile);
            // std::cout << "PROBLEM NOT HERE\n\n" << std::endl;
            delete[] meta_buff;
            return 2;

        }

        else if (meta_dir[1] < heapfile_capacity(heapfile->page_size_, 32))
        {
            // If the heapfile supports more than 1 Page
            if (heapfile->meta_data_size_ > 4)
            {
                // Iteration Starting from the first page slot
                for (int i = 2; i < heapfile->meta_data_size_ - 2; i += 2)
                {
                    if (meta_dir[i] != 0 && meta_dir[i + 2] == 0)
                    {
                        
                        int offset = meta_dir[i];

                        if (std::fseek(heapfile->file_ptr_, offset + heapfile->page_size_, SEEK_SET) != 0) {
                            // Handle error
                            std::cerr << "Seek Did Not Work" << std::endl;
                            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                            delete[] meta_buff;
                            return -1;
                        }

                        // Allocate New_Page
                        char new_page[heapfile->page_size_];
                        std::memset(new_page, 0, heapfile->page_size_);
                        // std::cout << "Page Number : " << i + 2 << "\n";
                        // std::cout << "File Pointer Position Page ID (Before New Page): " << " - " << ftell(heapfile->file_ptr_) << "\n";
                        fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);
                        // std::cout << "File Pointer Position Page ID (After New Page): " << " - " << ftell(heapfile->file_ptr_) << "\n";


                        // Update metadata
                        meta_dir[1]++; 
                        meta_dir[i + 2] = meta_dir[i] + heapfile->page_size_;
                        meta_dir[i + 3] = heapfile->page_size_;

                        // Rewrite the buffer into the page
                        // Set the file pointer back to the start of the file
                        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                        // std::cout << "File Pointer Position Page ID (Rewrite Dir): " << " - " << ftell(heapfile->file_ptr_) << "\n";
                        fwrite(reinterpret_cast<char*>(meta_dir), sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
                        // std::cout << "File Pointer Position Page ID (After Rewrite): " << " - " << ftell(heapfile->file_ptr_) << "\n" << std::endl;
                        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                        // std::cout << "File Pointer Position Page ID (Exit Function): " << " - " << ftell(heapfile->file_ptr_) << "\n" << std::endl;
                        // std::cout << "\n\nPROBLEM NOT HERE PART TWO\n\n" << std::endl;
                        delete[] meta_buff;
                        return i + 2;
                    }

                }
            }

        }
        
        delete[] meta_buff;
        std::cout << "PAGES IN THIS HEAPFILE ARE FULL!" << std::endl;
        return -1;
    }

    void write_page(Page *page, Heapfile *heapfile, PageID pid)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Write Page Failed: Invalid input parameters" << std::endl;
            return;
        }

        // print_page_records(page);
        // std::cout << "\nFile Pointer Position Page ID (Initial): " << pid << " - " << ftell(heapfile->file_ptr_) << std::endl;
        int offset_dir = ftell(heapfile->file_ptr_); 
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);

        int page_offset = meta_dir[pid];

        if (std::fseek(heapfile->file_ptr_, page_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed in write_page function" << std::endl;
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            delete[] meta_buff;
            return;
        }

        // std::cout << "File Pointer Position Page ID (After Seek): " << pid << " - " << ftell(heapfile->file_ptr_) << std::endl;

        // std::cout << "\n\nPROBLEM NOT HERE\n\n" << std::endl;

        // Write the page data to the file
        if (fwrite(static_cast<char*>(page->data_), sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error writing page data to file" << std::endl;
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            delete[] meta_buff;
            return;
        }

        // std::cout << "File Pointer Position Page ID: (After Write) " << pid << " - " << ftell(heapfile->file_ptr_) << std::endl;

       std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);

    //    std::cout << "File Pointer Position Page ID: (At Return) " << pid << " - " << ftell(heapfile->file_ptr_) << "\n" << std::endl;
       delete[] meta_buff;
    }

    void read_page(Heapfile *heapfile, PageID pid, Page *page)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Read Page Failed: Invalid input parameters" << std::endl;
            return;
        }

        int offset_dir = ftell(heapfile->file_ptr_); 
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        int page_offset = meta_dir[pid];
        
        // Seek to the position of the page within the file
        if (std::fseek(heapfile->file_ptr_, page_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed in read_page function" << std::endl;
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            delete[] meta_buff;
            return;
        }

        // Allocate memory for reading the page data
        char* page_buffer = new char[heapfile->page_size_];

        // Read the page data from the file
        if (fread(page_buffer, sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error reading page data from file" << std::endl;
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            delete[] meta_buff;
            return;
        }

        // Assign the read data to the page
        std::memcpy(reinterpret_cast<char*>(page->data_), page_buffer, heapfile->page_size_);
        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
        delete[] meta_buff;
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

    void print_heapfile_directory(Heapfile *heapfile)
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
            std::cout << "Slot [" << i << "]: " << meta_dir[i] << std::endl;  
        }

        // Set the file pointer back to the start of the file
        std::fseek(heapfile->file_ptr_, 0, SEEK_SET);
        delete[] metadata;
        // fclose(heapfile->file_ptr_);
    }

    int* get_heapfile_directory(Heapfile *heapfile)
    {
        int heap_dir_offset = ftell(heapfile->file_ptr_);
        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;

        char* metadata[metasize * sizeof(int)];
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);

        // Set the file pointer back to the start of the file
        std::fseek(heapfile->file_ptr_, heap_dir_offset, SEEK_SET);

        return meta_dir;
    }

    void print_heapfile_contents(Heapfile *heapfile_obj, const int page_size) 
    {
        int offset_dir = ftell(heapfile_obj->file_ptr_); 
        int meta_effective_size = heapfile_obj->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile_obj->file_ptr_);
        std::fseek(heapfile_obj->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        int total_pages = 2 * meta_dir[1];

        for (int i = 2; i <= total_pages; i += 2)
        {
            if (meta_dir[i] > 0)
            {
                Page *page = new Page;
                init_fixed_len_page(page, page_size, page_record_capacity(page_size) + 2);
                read_page(heapfile_obj, i, page);
                print_page_records(page);
                delete[] static_cast<char*>(page->data_);
                delete page;
            }
        }

        delete[] meta_buff;
    }


    // [x] Executable Functions
    
    void csv_to_heapfile(const char* csv_file, const char* heapfile, const int page_size)
    {
        FILE* file = fopen(heapfile, "rb+");
        std::string input = csv_file;
        std::ifstream csv_input(input);

        int heapfile_page_cap = heapfile_capacity(page_size, 32);
        int heapfile_slots = heapfile_page_cap * 2 + 2;
        if (heapfile_page_cap < 1)
        {
            std::cerr << "NO RECORDS CAN BE ADDED TO PAGE SIZE: " << page_size << std::endl;
            return;
        }

        if (!csv_input.is_open())
        {
            std::cerr << "Error Opening CSV file" << std::endl;
            return;
        }

        if (file == nullptr)
        {
            // Create file - doesn't exist
            file = fopen(heapfile, "wb");
            if (file == nullptr)
            {
                std::cerr << "File Note Created: nullptr" << std::endl;
                return;
            }

            fclose(file);
            fopen(heapfile, "r+");
        }

        // Read Records and Write to File
        // If pages are full
        // Set an offset in the first slot of heapfile directory
        // to the next heapfile directory and append until no records left

        // Create a Heapfile Object
        Heapfile *heapfile_new = new Heapfile;
        init_heapfile(heapfile_new, page_size, file);
        // std::cout << "\nFile Pointer Position I: " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
        int page_id = alloc_page(heapfile_new);
        int heapfile_count = 1;
        // Page 
        Page *page_writer = new Page;
        init_fixed_len_page(page_writer, page_size, page_record_capacity(page_size) + 2);
        int prev_file_ptr = ftell(heapfile_new->file_ptr_);
        int record_count{0};
        int test_count{0};
        


        // Cases
        // 1. When Page is Full
        // 2. When Entire Directory is Full

        std::string line;

        while (std::getline(csv_input, line))
        {
            std::stringstream line_stream(line);
            std::string cell;
            Record record;

            while (std::getline(line_stream, cell, ','))
            {
                char *cell_str = new char[cell.length() + 1];
                std::strcpy(cell_str, cell.c_str());
                record.push_back(cell_str);
            }

    
            // print_record(record);

            if (add_fixed_len_page(page_writer, &record) == -1)
            {
                if ((page_id/2) <= heapfile_slots)
                {
                    // std::cout << "Page ID: " << page_id << std::endl;
                    // print_page_records(page_writer);
                    // std::cout << "\nFile Pointer Position I: " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
                    write_page(page_writer, heapfile_new, page_id);
                    delete[] static_cast<char*>(page_writer->data_);
                    /* Page *test_read_page = new Page;
                    init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
                    read_page(heapfile_new, page_id, test_read_page);
                    print_page_records(test_read_page);
                    delete[] static_cast<char*>(test_read_page->data_);
                    delete test_read_page; */
                    init_fixed_len_page(page_writer, page_size, page_record_capacity(page_size) + 2);
                    add_fixed_len_page(page_writer, &record);
                    page_id = alloc_page(heapfile_new);
                    // std::cout << "PAGE RETURNED: " << page_id << std::endl;
                
                }
                else
                {
                    // std::cout << "Page ID: " << page_id << std::endl;
                    // print_page_records(page_writer);
                    // std::cout << "\nFile Pointer Position II (before): " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
                    write_page(page_writer, heapfile_new, page_id);
                    // std::cout << "\nFile Pointer Position II: (after) " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
                    delete[] static_cast<char*>(page_writer->data_);

                    /* Page *test_read_page = new Page;
                    init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
                    read_page(heapfile_new, page_id, test_read_page);
                    print_page_records(test_read_page);
                    delete[] static_cast<char*>(test_read_page->data_);
                    delete test_read_page; */
                    
                    init_fixed_len_page(page_writer, page_size, page_record_capacity(page_size) + 2);
                    add_fixed_len_page(page_writer, &record);
                    page_id = 0;
                    int* heapfile_dir = get_heapfile_directory(heapfile_new);
                    int prev_rel_offset = heapfile_dir[0];
                    int new_heap_dir_offset = std::fseek(heapfile_new->file_ptr_, 0, SEEK_END);
                    int offset_to_newheap = new_heap_dir_offset - prev_rel_offset;
                    heapfile_dir[0] = offset_to_newheap;
                    std::fseek(heapfile_new->file_ptr_, prev_file_ptr, SEEK_SET);
                    fwrite(reinterpret_cast<char*>(heapfile_dir), sizeof(char), 
                                heapfile_new->meta_data_size_ * sizeof(int), heapfile_new->file_ptr_);
                    std::fseek(heapfile_new->file_ptr_, 0, SEEK_END);
                    
                    // Initialize Heapfile Again
                    init_heapfile(heapfile_new, page_size, file);
                    heapfile_dir = get_heapfile_directory(heapfile_new);

                    // Allocate Page
                    page_id = alloc_page(heapfile_new);
                    add_fixed_len_page(page_writer, &record);
                    prev_file_ptr = new_heap_dir_offset;
                    heapfile_count++;

                }

            }
            
            else if (csv_input.eof())
            {
                // std::cout << "Page ID: " << page_id << std::endl;
                // print_page_records(page_writer);
                // std::cout << "\nFile Pointer Position III: (before) " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
                write_page(page_writer, heapfile_new, page_id);
                // std::cout << "\nFile Pointer Position III: (after) " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
                /* Page *test_read_page = new Page;
                init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
                read_page(heapfile_new, page_id, test_read_page);
                print_page_records(test_read_page);
                delete[] static_cast<char*>(test_read_page->data_);
                delete test_read_page; */

            }

            cleanup_record(record);
        }

        /* Page *test_read_page = new Page;
        init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
        read_page(heapfile_new, 23 , test_read_page);
        print_page_records(test_read_page);
        delete[] static_cast<char*>(test_read_page->data_);
        delete test_read_page; */

        /* std::fseek(heapfile_new->file_ptr_, 0, SEEK_SET);
        int offset_dir = ftell(heapfile_new->file_ptr_); 
        int meta_effective_size = heapfile_new->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile_new->file_ptr_);
        std::fseek(heapfile_new->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        int total_pages = 2 * meta_dir[1];

        for (int i = 2; i <= total_pages; i += 2)
        {
            if (meta_dir[i] > 0)
            {
                Page *test_read_page = new Page;
                int offset_dir = ftell(heapfile_new->file_ptr_); 
                int meta_effective_size = heapfile_new->meta_data_size_ * sizeof(int);
                char* meta_buff = new char[meta_effective_size];
                std::memset(meta_buff, 0, meta_effective_size);
                std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile_new->file_ptr_);
                std::fseek(heapfile_new->file_ptr_, offset_dir, SEEK_SET);
                int* meta_dir = reinterpret_cast<int*>(meta_buff);
                int total_pages = 2 * meta_dir[1];
                std:: cout << "TOTAL PAGES: " << total_pages/2 << std::endl;
                
                init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
                read_page(heapfile_new, i, test_read_page);
                std::cout << "\n\nPage Record Count: " << get_record_count(test_read_page) << std::endl;
                print_page_records(test_read_page);
                delete[] static_cast<char*>(test_read_page->data_);
                delete test_read_page;
                
            }

        } */

        // std::cout << "\nFile Pointer Position IV: " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
        write_page(page_writer, heapfile_new, page_id);
        // std::cout << "\nFile Pointer Position IV: " << ftell(heapfile_new->file_ptr_) << "\n" << std::endl;
        /* Page *test_read_page = new Page;
        init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
        read_page(heapfile_new, page_id - 2, test_read_page);
        print_page_records(test_read_page);
        delete[] static_cast<char*>(test_read_page->data_);
        delete test_read_page;

        
        Page *test_ = new Page;
        init_fixed_len_page(test_, page_size, page_record_capacity(page_size) + 2);
        read_page(heapfile_new, page_id, test_);
        print_page_records(test_);
        delete[] static_cast<char*>(test_->data_);
        delete test_; */

        

        // print_heapfile_directory(heapfile_new);

        
        // delete[] meta_buff;

        fclose(heapfile_new->file_ptr_);
        delete[] static_cast<char*>(page_writer->data_);
        delete page_writer;
        heapfile_new->file_ptr_ = nullptr;
        delete heapfile_new;
    }

    void scan(const char* heapfile, const int page_size) 
    {
        FILE* file = fopen(heapfile, "rb");
        if (!file) {
            std::cerr << "Error opening heapfile: " << heapfile << std::endl;
            return;
        }

        std::fseek(file, 0, SEEK_SET);

        Heapfile *test_heapfile = new Heapfile;
        init_heapfile(test_heapfile, page_size, file);
        // print_heapfile_directory(test_heapfile);
        /* int offset_dir = ftell(test_heapfile->file_ptr_); 
        int meta_effective_size = test_heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, test_heapfile->file_ptr_);
        std::fseek(test_heapfile->file_ptr_, offset_dir, SEEK_SET);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        int total_pages = 2 * meta_dir[1];

        
        for (int i = 2; i <= total_pages; i += 2)
        {
            if (meta_dir[i] > 0)
            {
                Page *test_read_page = new Page;
                init_fixed_len_page(test_read_page, page_size, page_record_capacity(page_size) + 2);
                read_page(test_heapfile, i, test_read_page);
                print_page_records(test_read_page);
                delete[] static_cast<char*>(test_read_page->data_);
                delete test_read_page;
                
            }

        } */

        print_heapfile_contents(test_heapfile, page_size);

        fclose(file);
        // test_heapfile->file_ptr_ = nullptr;
        delete test_heapfile;
        // delete[] static_cast<char*>(test_read_page->data_);
        // delete[] meta_buff;
    }
     
    void insert_csv_to_heapfile(const char* heapfile, const char* csv_file, const int page_size) 
    {
        FILE* file = fopen(heapfile, "rb+");
        if (!file) {
            std::cerr << "Error opening heapfile: " << heapfile << std::endl;
            return;
        }

        int prev_file_ptr = ftell(file);
        Heapfile* heapfile_obj = new Heapfile;
        init_heapfile_read(heapfile_obj, page_size, file);
        int meta_effective_size = heapfile_obj->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile_obj->file_ptr_);
        std::fseek(heapfile_obj->file_ptr_, prev_file_ptr, SEEK_SET);
        int* heapfile_dir = reinterpret_cast<int*>(meta_buff);

        // print_heapfile_directory(heapfile_obj);
        
        int last_allocated_page = heapfile_dir[1];
        int prev_page = last_allocated_page * 2;

        std::cout << "LAST ALLOCATED PAGE: " << last_allocated_page << std::endl;
        
        int heapfile_page_cap = heapfile_capacity(page_size, 32);
        
        Page* last_page = new Page;
        init_fixed_len_page(last_page, page_size, page_record_capacity(page_size) + 2);
        read_page(heapfile_obj, prev_page, last_page);
        // print_slot_dir(last_page);
        
        int remaining_slots = fixed_len_page_freeslots(last_page);
        int next_heap_exists = heapfile_dir[0];

        // std::cout << "REMAINING SLOTS " << remaining_slots << std::endl;        



        std::ifstream csv_input(csv_file);
        std::string line;
        Record record;

        while (std::getline(csv_input, line)) {

            std::stringstream line_stream(line);
            std::string cell;
            Record record;

            while (std::getline(line_stream, cell, ','))
            {
                char *cell_str = new char[cell.length() + 1];
                std::strcpy(cell_str, cell.c_str());
                record.push_back(cell_str);
            }

            // print_record(record);

            if (remaining_slots > 0)
            {
                // Insert record into the last allocated page
                add_fixed_len_page(last_page, &record);
                remaining_slots--;

                // Check if the last page is full after inserting the record
                if (remaining_slots == 0) 
                {
                    // Write the page since it's full
                    // print_page_records(last_page);
                    // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                    // print_page_records(last_page);
                    write_page(last_page, heapfile_obj, prev_page);
                    delete[] static_cast<char*>(last_page->data_);
                }
                
            }
            else
            {
                // Allocate a new page if the heapfile is not full
                if (last_allocated_page < heapfile_page_cap) 
                {
                    // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                    if (last_page->data_)
                    {
                        delete[] static_cast<char*>(last_page->data_);
                    }

                    prev_page = alloc_page(heapfile_obj);
                    if (prev_page == -1)
                    {
                        add_new_heap(heapfile_obj, heapfile_dir[0], prev_file_ptr, heapfile_dir);
                        prev_page = alloc_page(heapfile_obj);
                        last_allocated_page = heapfile_dir[1];
                        init_fixed_len_page(last_page, page_size, page_record_capacity(page_size) + 2);
                        add_fixed_len_page(last_page, &record);

                        remaining_slots = fixed_len_page_freeslots(last_page);
                        // std::cout << "REMAINING SLOTS " << remaining_slots << std::endl;


                        // Insert record into the newly allocated page
                        add_fixed_len_page(last_page, &record);
                        // print_page_records(last_page);
                        remaining_slots--;

                        if (remaining_slots == 0) 
                        {
                            // Write the page since it's full
                            // print_page_records(last_page);
                            // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                            // print_page_records(last_page);
                            write_page(last_page, heapfile_obj, prev_page);
                            delete[] static_cast<char*>(last_page->data_);
                        }
                        
                    }

                    last_allocated_page++;
                    // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                    init_fixed_len_page(last_page, page_size, page_record_capacity(page_size) + 2);
                    remaining_slots = fixed_len_page_freeslots(last_page);
                    // std::cout << "REMAINING SLOTS " << remaining_slots << std::endl;


                    // Insert record into the newly allocated page
                    add_fixed_len_page(last_page, &record);
                    // print_page_records(last_page);
                    remaining_slots--;

                    if (remaining_slots == 0) 
                    {
                        // Write the page since it's full
                        // print_page_records(last_page);
                        // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                        // print_page_records(last_page);
                        write_page(last_page, heapfile_obj, prev_page);
                        // NOTE: REMOVED THIS AND IT WORKED
                        // delete[] static_cast<char*>(last_page->data_);
                    }
                } 

                else 
                {
                     if (last_page->data_)
                    {
                        delete[] static_cast<char*>(last_page->data_);
                    }

                    // FIXME: HAVE YET TO TEST THIS SHIT

                    add_new_heap(heapfile_obj, heapfile_dir[0], prev_file_ptr, heapfile_dir);

                    // Create  a new heapfile
                    /* int prev_rel_offset = heapfile_dir[0];
                    int new_heap_dir_offset = std::fseek(heapfile_obj->file_ptr_, 0, SEEK_END);
                    int offset_to_newheap = new_heap_dir_offset - prev_rel_offset;
                    heapfile_dir[0] = offset_to_newheap;
                    std::fseek(heapfile_obj->file_ptr_, prev_file_ptr, SEEK_SET);
                    fwrite(reinterpret_cast<char*>(heapfile_dir), sizeof(char), 
                                heapfile_obj->meta_data_size_ * sizeof(int), heapfile_obj->file_ptr_);
                    std::fseek(heapfile_obj->file_ptr_, 0, SEEK_END);
                    prev_file_ptr = ftell(heapfile_obj->file_ptr_);
                    
                    // Initialize Heapfile Again
                    init_heapfile(heapfile_obj, page_size, file);
                    std::memset(meta_buff, 0, meta_effective_size);
                    std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile_obj->file_ptr_);
                    std::fseek(heapfile_obj->file_ptr_, prev_file_ptr, SEEK_SET);
                    heapfile_dir = reinterpret_cast<int*>(meta_buff); */


                    // Allocate Page
                    prev_page = alloc_page(heapfile_obj);
                    last_allocated_page = heapfile_dir[1];
                    init_fixed_len_page(last_page, page_size, page_record_capacity(page_size) + 2);
                    add_fixed_len_page(last_page, &record);

                    remaining_slots = fixed_len_page_freeslots(last_page);
                    // std::cout << "REMAINING SLOTS " << remaining_slots << std::endl;


                    // Insert record into the newly allocated page
                    add_fixed_len_page(last_page, &record);
                    // print_page_records(last_page);
                    remaining_slots--;

                    if (remaining_slots == 0) 
                    {
                        // Write the page since it's full
                        // print_page_records(last_page);
                        // std::cout << "LAST ALLOCATED PAGE : " << last_allocated_page << " HEAPFILE PAGE CAP : " << heapfile_page_cap << " PREVIOUS PAGE INDEX : " << prev_page << std::endl;
                        // print_page_records(last_page);
                        write_page(last_page, heapfile_obj, prev_page);
                        delete[] static_cast<char*>(last_page->data_);
                    }
                }

            }

            cleanup_record(record);
        }

        if (last_page->data_)
        {
            write_page(last_page, heapfile_obj, last_allocated_page);
            delete[] static_cast<char*>(last_page->data_);
            
        }

        // Clean up resources
        // delete[] meta_buff;
        fclose(file);
        delete heapfile_obj;
        delete last_page;
    }

    /* void add_new_heap(Heapfile *heapfile, int prev_offset, int &prev_file_ptr, int* &heapfile_dir, char* &meta_buff)
    {
        int new_heap_dir_offset = std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        int offset_to_newheap = new_heap_dir_offset - prev_offset;
        heapfile_dir[0] = offset_to_newheap;
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
        fwrite(reinterpret_cast<char*>(heapfile_dir), sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        prev_file_ptr = ftell(heapfile->file_ptr_);

        // Initialize Heapfile Again
        init_heapfile(heapfile, heapfile->page_size_, heapfile->file_ptr_);

        // Delete previous buffer and allocate new one for metadata
        delete[] reinterpret_cast<char*>(heapfile_dir);
        delete[] meta_buff;
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        meta_buff = new char[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
        heapfile_dir = reinterpret_cast<int*>(meta_buff);
    } */




    /* void add_new_heap(Heapfile *heapfile, int prev_offset, int &prev_file_ptr, int* &heapfile_dir)
    {
        
        
        int new_heap_dir_offset = std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        int offset_to_newheap = new_heap_dir_offset - prev_offset;
        heapfile_dir[0] = offset_to_newheap;
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
        fwrite(reinterpret_cast<char*>(heapfile_dir), sizeof(char), 
                    heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        prev_file_ptr = ftell(heapfile->file_ptr_);
        
        // Initialize Heapfile Again
        init_heapfile(heapfile, heapfile->page_size_, heapfile->file_ptr_);
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char* meta_buff = new char[meta_effective_size];
        delete[] reinterpret_cast<char*>(heapfile_dir);
        std::memset(meta_buff, 0, meta_effective_size);
        std::fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
        heapfile_dir = reinterpret_cast<int*>(meta_buff);

    } */

    void add_new_heap(Heapfile *heapfile, int prev_offset, int &prev_file_ptr, int* &heapfile_dir)
    {
        // Calculate offset to the new heap directory
        int new_heap_dir_offset = std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        int offset_to_newheap = new_heap_dir_offset - prev_offset;
        
        // Update heapfile directory with the offset
        heapfile_dir[0] = offset_to_newheap;

        // Write the updated directory to the file
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
        fwrite(heapfile_dir, sizeof(int), heapfile->meta_data_size_, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, 0, SEEK_END);
        prev_file_ptr = ftell(heapfile->file_ptr_);
        
        // Initialize Heapfile Again
        init_heapfile(heapfile, heapfile->page_size_, heapfile->file_ptr_);

        // Deallocate memory for the old heapfile directory
        delete[] heapfile_dir;

        // Read the new heapfile directory from the file
        heapfile_dir = new int[heapfile->meta_data_size_];
        std::fread(heapfile_dir, sizeof(int), heapfile->meta_data_size_, heapfile->file_ptr_);
        std::fseek(heapfile->file_ptr_, prev_file_ptr, SEEK_SET);
    }



    /* # Insert all records in the CSV file to a heap file
    $ insert <heapfile> <csv_file> <page_size>

    # Update one attribute of a single record in the heap file given its record ID
    # <attribute_id> is the index of the attribute to be updated (e.g. 0 for the first attribute, 1 for the second attribute, etc.)
    # <new_value> will have the same fixed length (10 bytes)
    $ update <heapfile> <record_id> <attribute_id> <new_value> <page_size>

    # Delete a single record in the heap file given its record ID
    $ delete <heapfile> <record_id> <page_size> */

}