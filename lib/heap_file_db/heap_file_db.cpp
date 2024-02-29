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
        std::cout << "Heapfile Page Capacity: " << hf_page_cap << std::endl;
        std::cout << "Heapfile Meta Slots: " << hf_meta_size << std::endl;
        
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
        if (meta_dir[2] == 0)
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

            // Allocate New_Page
            char new_page[heapfile->page_size_];
            std::memset(new_page, 0, heapfile->page_size_);
            fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

            // Update metadata
            meta_dir[1]++; // Page Counter
            meta_dir[2] = offset; // Page Offset Relative to the Heapfile Offset
            meta_dir[3] = heapfile->page_size_; // Free Space

            // Rewrite the buffer into the page
            // Set the file pointer back to the start of the file
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            fwrite(reinterpret_cast<char*>(meta_dir), sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
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
                        // Offset from the beginning of heapfile directory
                        int offset = meta_dir[i];

                        if (std::fseek(heapfile->file_ptr_, offset, SEEK_SET) != 0) {
                            // Handle error
                            std::cerr << "Seek Did Not Work" << std::endl;
                            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                            delete[] meta_buff;
                            return -1;
                        }

                        // Allocate New_Page
                        char new_page[heapfile->page_size_];
                        std::memset(new_page, 0, heapfile->page_size_);
                        fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

                        // Update metadata
                        meta_dir[1]++; 
                        meta_dir[i + 2] = meta_dir[i] + heapfile->page_size_;
                        meta_dir[i + 3] = heapfile->page_size_;

                        // Rewrite the buffer into the page
                        // Set the file pointer back to the start of the file
                        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                        fwrite(reinterpret_cast<char*>(meta_dir), sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
                        std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
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

        // std::cout << "\n\nPROBLEM NOT HERE\n\n" << std::endl;

        // Write the page data to the file
        if (fwrite(static_cast<char*>(page->data_), sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error writing page data to file" << std::endl;
            std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            delete[] meta_buff;
            return;
        }

       std::fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
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
        char page_buffer[heapfile->page_size_];

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
            std::cout << "Slot [" << i + 1 << "]: " << meta_dir[i] << std::endl;  
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


    // [x] Executable Functions
    
    void csv_to_heapfile(const char* csv_file, const char* heapfile, const int page_size)
    {
        FILE* file = fopen(heapfile, "r+");
        std::string input = csv_file;
        std::ifstream csv_input(input);

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
        int page_id = alloc_page(heapfile_new);
        int heapfile_count = 1;
        // Page 
        Page *page_writer = new Page;
        std::cout << "\nPage Record Capacity: " << page_record_capacity(page_size) << std::endl;
        init_fixed_len_page(page_writer, page_size, page_record_capacity(page_size) + 2);
        int heapfile_page_cap = heapfile_capacity(page_size, 32);
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
                if ((page_id/2) < heapfile_page_cap)
                {
                    std::cout << "Page ID: " << page_id << std::endl;
                    print_page_records(page_writer);
                    write_page(page_writer, heapfile_new, page_id);
                    delete[] static_cast<char*>(page_writer->data_);
                    init_fixed_len_page(page_writer, page_size, page_record_capacity(page_size) + 2);
                    add_fixed_len_page(page_writer, &record);
                    page_id = alloc_page(heapfile_new);
                
                }
                else
                {
                    std::cout << "Page ID: " << page_id << std::endl;
                    print_page_records(page_writer);
                    write_page(page_writer, heapfile_new, page_id);
                    delete[] static_cast<char*>(page_writer->data_);
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
                std::cout << "Page ID: " << page_id << std::endl;
                print_page_records(page_writer);
                write_page(page_writer, heapfile_new, page_id);

            }

            cleanup_record(record);
        }

        std::fseek(heapfile_new->file_ptr_, 0, SEEK_SET);
        fclose(heapfile_new->file_ptr_);
        delete[] static_cast<char*>(page_writer->data_);
        delete page_writer;
        heapfile_new->file_ptr_ = nullptr;
        delete heapfile_new;
    }


    void scan(const char* heapfile, const int page_size) 
    {
        FILE* file = fopen(heapfile, "r");
        if (!file) {
            std::cerr << "Error opening heapfile: " << heapfile << std::endl;
            return;
        }

        std::fseek(file, 0, SEEK_SET);

        Heapfile* heapfile_obj = new Heapfile;
        init_heapfile(heapfile_obj, page_size, file);
        int dir_offset = ftell(heapfile_obj->file_ptr_);

        RecordIterator record_it(heapfile_obj);

        while (record_it.hasNext())
        {
            std::cout << "RECORD PRESENT" << std::endl;
            Record record_read = record_it.next();
            print_record(record_read);
        }


        /* // Initialize a Heapfile object
        Heapfile* heapfile_obj = new Heapfile;
        init_heapfile(heapfile_obj, page_size, file);
        int dir_offset = ftell(heapfile_obj->file_ptr_);

        // Allocate a Page object
        Page* page = new Page;
        init_fixed_len_page(page, page_size, page_record_capacity(page_size) + 2);
        int* heapfile_dir = get_heapfile_directory(heapfile_obj);
        int pages_read{0};

        Record record;
        int record_no{1};

        // Loop through each page in the heapfile
        for (int pid = 2; pages_read < heapfile_dir[1]; pid += 2) {
            read_page(heapfile_obj, pid, page);
            fseek(heapfile_obj->file_ptr_, dir_offset, SEEK_SET);
            pages_read++;
            int* slot_dir = get_slot_directory(page);

            // Loop through each record in the page and print it
            for (int i = page->slot_size_ - 3; i >= 0; --i)
            {
                if (slot_dir[i] != -1)
                {
                    std::cout << "Record [" << record_no << "] :" << std::endl;
                    record_no++;
                    read_fixed_len_page(page, i, &record);
                    print_record(record);
                    cleanup_record(record);
                }
            }
        }

        // Clean up
        fclose(file);
        delete heapfile_obj;
        delete[] static_cast<char*>(page->data_);
        delete page; */
    }

}