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
        meta_dir[0] = -1;

        // Write only metadata to the file
        fwrite(initial_buffer, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        // Set the file pointer back to the start of the file
        // FIXME: Making an assumption here that I'd need to access the 
        // FIXME: directory once I leave this function call
        fseek(heapfile->file_ptr_, dir_start_offset, SEEK_SET);
        delete[] initial_buffer;
    }

    // [x] : Changed Implementation to Accomodate for Executables
    PageID alloc_page(Heapfile *heapfile)
    {
        int meta_effective_size = heapfile->meta_data_size_ * sizeof(int);
        char meta_buff[meta_effective_size];
        std::memset(meta_buff, 0, meta_effective_size); 
        // Pointer Offset to the Directory
        int offset_dir = ftell(heapfile->file_ptr_); 

        fread(meta_buff, sizeof(char), meta_effective_size, heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(meta_buff);
        // Set the file pointer back to the start of the file
        // [x] : Change here
        fseek(heapfile->file_ptr_, 0, SEEK_SET);

        // First Page
        if (meta_dir[2] == 0)
        {
            // Move the file pointer to the appropriate position
            int offset = heapfile->meta_data_size_ * sizeof(int);
    
            if (fseek(heapfile->file_ptr_, offset_dir + offset, SEEK_SET) != 0) {
                // Handle error
                std::cerr << "Seek Did Not Work" << std::endl;
            }

            // Allocate New_Page
            char* new_page = new char[heapfile->page_size_];
            std::memset(new_page, 0, heapfile->page_size_);
            fwrite(new_page, sizeof(char), heapfile->page_size_, heapfile->file_ptr_);

            // Update metadata
            meta_dir[1]++; // Page Counter
            meta_dir[2] = offset_dir + offset;
            meta_dir[3] = heapfile->page_size_;

            // Rewrite the buffer into the page
            // Set the file pointer back to the start of the file
            fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
            fwrite(meta_buff, sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
            fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);

            // Clean up
            delete[] new_page;
            delete[] meta_buff;
            return 2;

        }

        else if (meta_dir[1] < heapfile_capacity(heapfile->page_size_, 32))
        {
            if (heapfile->meta_data_size_ > 4)
            {
                for (int i = 2; i < heapfile->meta_data_size_ - 2; i += 2)
                {

                    if (meta_dir[i] != 0 && meta_dir[i + 2] == 0)
                    {
                        // Move the file pointer to the appropriate position
                        int offset = meta_dir[i]; // Calculate the offset based on meta_dir[i]
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
                        meta_dir[1]++; 
                        meta_dir[i + 2] = meta_dir[i] + heapfile->page_size_;
                        meta_dir[i + 3] = heapfile->page_size_;

                        // Rewrite the buffer into the page
                        // Set the file pointer back to the start of the file
                        fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
                        fwrite(meta_buff, sizeof(char), heapfile->meta_data_size_ * sizeof(int), heapfile->file_ptr_);
                        fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);

                        // Clean up
                        delete[] new_page;
                        delete[] meta_buff;
                        return i + 2;
                    }

                }
            }

        }
        
        fseek(heapfile->file_ptr_, offset_dir, SEEK_SET);
        delete[] meta_buff;
        return -1;
    }

    // [x] OPTIMIZE THIS FFS
   /*  PageID alloc_page(Heapfile *heapfile)
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
                int offset = meta_dir[i]; // Calculate the offset based on meta_dir[i]

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
    } */

    /* void write_page(Page *page, Heapfile *heapfile, PageID pid)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Write Page Failed: Invalid input parameters" << std::endl;
            return;
        }

        // Calculate the offset of the page within the file
        int page_offset = heapfile->meta_data_size_ * sizeof(int) + (pid - 1) * heapfile->page_size_;

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

        // Update the free space slot of the page in the heapfile directory
        int slot_offset = sizeof(int) * (pid * 2 + 1); // Offset of the free space slot in the directory
        int free_space = fixed_len_page_freeslots(page);

        if (fseek(heapfile->file_ptr_, slot_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed to update free space slot" << std::endl;
            return;
        }

        // FIXME: Check this if it doesn't work
        if (fwrite(&free_space, sizeof(int), 1, heapfile->file_ptr_) != 1)
        {
            std::cerr << "Error updating free space slot" << std::endl;
            return;
        }
    } */

    void write_page(Page *page, Heapfile *heapfile, PageID pid)
    {
        if (!heapfile || !heapfile->file_ptr_ || !page)
        {
            std::cerr << "Write Page Failed: Invalid input parameters" << std::endl;
            return;
        }


        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;

        char* metadata = new char[metasize * sizeof(int)];
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);
        int page_offset = meta_dir[pid];



        // Seek to the position of the page within the file
        fseek(heapfile->file_ptr_, 0, SEEK_SET);
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

        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;
        int heapdir_offset = ftell(heapfile->file_ptr_);

        char* metadata = new char[metasize * sizeof(int)];
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);
        int page_offset = meta_dir[pid];

        // Seek to the position of the page within the file
        if (fseek(heapfile->file_ptr_, page_offset, SEEK_SET) != 0)
        {
            std::cerr << "Seek failed in read_page function" << std::endl;
            fseek(heapfile->file_ptr_, heapdir_offset, SEEK_SET);
            return;
        }

        // Allocate memory for reading the page data
        char *page_buffer = new char[heapfile->page_size_];

        // Read the page data from the file
        if (fread(page_buffer, sizeof(char), heapfile->page_size_, heapfile->file_ptr_) != heapfile->page_size_)
        {
            std::cerr << "Error reading page data from file" << std::endl;
            fseek(heapfile->file_ptr_, heapdir_offset, SEEK_SET);
            delete[] page_buffer;
            return;
        }

        // Assign the read data to the page
        std::memcpy(page->data_, page_buffer, heapfile->page_size_);

        // Cleanup dynamically allocated memory
        delete[] page_buffer;
        fseek(heapfile->file_ptr_, heapdir_offset, SEEK_SET);
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
        fseek(heapfile->file_ptr_, 0, SEEK_SET);
        delete[] metadata;
        // fclose(heapfile->file_ptr_);
    }

    int* get_heapfile_directory(Heapfile *heapfile)
    {
        int heap_dir_offset = ftell(heapfile->file_ptr_);
        int metasize = heapfile->meta_data_size_;
        int pagesize = heapfile->page_size_;

        char* metadata = new char[metasize * sizeof(int)];
        fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
        int* meta_dir = reinterpret_cast<int*>(metadata);

        // Set the file pointer back to the start of the file
        fseek(heapfile->file_ptr_, heap_dir_offset, SEEK_SET);

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
        Page page_writer;
        init_fixed_len_page(&page_writer, page_size, page_record_capacity(page_size) + 2);
        int heapfile_page_cap = heapfile_capacity(page_size, 32);
        int prev_file_ptr = ftell(heapfile_new->file_ptr_);


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


            if (add_fixed_len_page(&page_writer, &record) == -1)
            {
                if ((page_id/2) <= heapfile_page_cap)
                {
                    write_page(&page_writer, heapfile_new, page_id);
                    fseek(heapfile_new->file_ptr_, prev_file_ptr, SEEK_SET);
                    delete[] static_cast<char*>(page_writer.data_);
                    page_writer.data_ = nullptr;
                    init_fixed_len_page(&page_writer, page_size, page_record_capacity(page_size) + 2);
                    add_fixed_len_page(&page_writer, &record);
                    // FIXME: Set the file pointer to the beginning of heapfile
                    page_id = alloc_page(heapfile_new);
                }
                else
                {
                    // Create a new heapfile directory in the file
                    int new_heap_dir_offset = fseek(heapfile_new->file_ptr_, 0, SEEK_END); // Move to the end of the file
                    // Initialize Heapfile Again & Write
                    int hf_page_cap = heapfile_capacity(page_size, 32);
                    int hf_meta_size = heapfile_metadata_size(hf_page_cap);
                    int meta_effective_size = hf_meta_size * sizeof(int);
                    std::cout << "Heapfile Page Capacity: " << hf_page_cap << std::endl;
                    std::cout << "Heapfile Meta Slots: " << hf_meta_size << std::endl;
                
                    

                    // heapfile->file_ptr_ = file;
                    heapfile_new->meta_data_size_ = hf_meta_size;
                    heapfile_new->page_size_ = page_size;
                    heapfile_new->file_ptr_ = file;

                    // Allocation of char* buffer
                    char* initial_buffer = new char[meta_effective_size + page_size];
                    std::memset(initial_buffer, 0, meta_effective_size + page_size);
                    int* meta_dir = reinterpret_cast<int*>(initial_buffer);
                    std::fill_n(meta_dir, hf_meta_size, 0);
                    meta_dir[0] = -1;

                    // Write only metadata to the file
                    if (fwrite(initial_buffer, sizeof(char), meta_effective_size, heapfile_new->file_ptr_) != meta_effective_size) {
                        std::cerr << "Error writing metadata to file" << std::endl;
                        delete[] initial_buffer;
                        return;
                    }



                    // Free initial_buffer as it's no longer needed
                    delete[] initial_buffer;


                    // Allocate Page
                    page_id = alloc_page(heapfile_new);
                    add_fixed_len_page(&page_writer, &record);
                    // FIXME: NEED ATTENTION ABOVE

                    // Update the offset in the previous heap directory's first slot
                    fseek(heapfile_new->file_ptr_, prev_file_ptr, SEEK_SET);
                    if (fwrite(&new_heap_dir_offset, sizeof(int), 1, heapfile_new->file_ptr_) != 1) {
                        std::cerr << "Error updating previous heap directory's first slot" << std::endl;
                        return;
                    }

                    prev_file_ptr = new_heap_dir_offset;

                }

            }

            cleanup_record(record);
        
        }

        fclose(heapfile_new->file_ptr_);
        delete[] static_cast<char*>(page_writer.data_);
        delete heapfile_new;
    }


    void scan(const char* heapfile, const int page_size) 
    {
        FILE* file = fopen(heapfile, "r");
        if (!file) {
            std::cerr << "Error opening heapfile: " << heapfile << std::endl;
            return;
        }

        fseek(file, 0, SEEK_SET);

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