#include "page_layout_db.h"
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace dbms::page
{

    /**
     * Initializes a page using the given slot size
     */

    void init_fixed_len_page(Page *page, int page_size, int slot_size)
    {
        if (!page)
        {
            std::cerr << "Page *page pointer NULL" << std::endl;
        }
        else
        {
            // Handle error or allocate memory for page
            // Allocate memory for the Page *page
            // Initialize: Otherwise points to unitialised bytes
            // FIXME: MAYBE THIS IS CAUSING MEMORY LEAK IN HEAPFILE
            page->data_ = new char[page_size];
            page->page_size_ = page_size;
            page->slot_size_ = slot_size;
            std::memset(page->data_, 0, page->page_size_);

            // Creating a Slot Directory (Footer)
            // Slot Directory: Record Slots ; Free Space Pointer Slot ;
            // Number of Records Present Slot

            // Number of Records Present: Initially 0
            // Free Space Pointer Slot: Initially points to the beginning of the page
            // Number of Records Present Slot: Initially Empty Slots (NULL)

            int *slot_directory = get_slot_directory(page);
            std::fill_n(slot_directory, slot_size, -1);
            slot_directory[slot_size - 1] = 0;
            // Offset to the beginning of page (free space starts in the beginning)
            slot_directory[slot_size - 2] = 0;
        }
    }

    /**
     * Calculates the maximal number of records that fit in a page
     */
    int fixed_len_page_capacity(Page *page)
    {
        return (page->page_size_ - page->slot_size_) / (ATTRIBUTES_IN_RECORD *
                                                        ATTRIBUTE_FIXED_LENGTH);
    }

    /**
     * Calculate the free space (number of free slots) in the page
     */
    int fixed_len_page_freeslots(Page *page)
    {
        int *slot_directory = get_slot_directory(page);
        int free_slots{0};
        for (int i = 0; i < page->slot_size_ - 2; i++)
        {
            if (slot_directory[i] == -1)
            {
                free_slots++;
            }
        }
        return free_slots;
    }

    /**
     * Add a record to the page
     * Returns:
     *   record slot offset if successful,
     *   -1 if unsuccessful (page full)
     */
    int add_fixed_len_page(Page *page, Record *r)
    {
        // Get Page's Slot Directory
        int *slot_dir = get_slot_directory(page);
        
        // Space for the new record
        int required_space = ATTRIBUTE_FIXED_LENGTH * r->size();

        if (slot_dir[page->slot_size_ - 2] != -1)
        {
            // std::cout << "ENTERING HERE I" << std::endl;
            for (int i = page->slot_size_ - 3; i >= 0; i--)
            {
                // std::cout << "ENTERING HERE II" << std::endl;
                if (slot_dir[i] == -1)
                {
                    // std::cout << "ENTERING HERE III" << std::endl;
                    fixed_len_write(r, reinterpret_cast<char *>(page->data_) + slot_dir[page->slot_size_ - 2]);
                    slot_dir[i] = slot_dir[page->slot_size_ - 2];
                    slot_dir[page->slot_size_ - 1]++;
                    slot_dir[page->slot_size_ - 2] += required_space;

                    int new_free_space_offset = slot_dir[page->slot_size_ - 2] + required_space;
                    // Calculate the address of the beginning of the slot directory
                    char *slot_dir_start = reinterpret_cast<char *>(slot_dir);
                    // Calculate the address where the next free space starts (after adding
                    // record)
                    char *new_free_space_start = static_cast<char *>(page->data_) +
                                                 new_free_space_offset;

                    // Check if the new record will overlap/exceed the slot_dir_
                    if (new_free_space_start >= slot_dir_start)
                    {
                        slot_dir[page->slot_size_ - 2] = -1;
                    }

                    // std::cout << "\nPrinting Slot Directory\n" << std::endl;
                    // print_slot_dir(page);

                    return i; // Record Slot Offset
                }
            }
        }

        // std::cout << "\nPrinting Slot Directory\n" << std::endl;
        // print_slot_dir(page);

        return -1;
    }

    /**
     * Write a record into a given slot.
     */
    void write_fixed_len_page(Page *page, int slot, Record *r)
    {
        int *slot_dir = get_slot_directory(page);
        fixed_len_write(r, reinterpret_cast<char *>(page->data_) + slot_dir[slot]);
    }


    /**
     * Read a record from the page from a given slot.
     */
    void read_fixed_len_page(Page *page, int slot, Record *r)
    {
        int *slot_dir = get_slot_directory(page);
        // Deserialize the buffer and store in rec
        int record_size = ATTRIBUTE_FIXED_LENGTH * ATTRIBUTES_IN_RECORD;
        fixed_len_read(reinterpret_cast<char *>(page->data_) + slot_dir[slot], record_size, r);
    }


    // [x]: Helper Functions


    int page_record_capacity(int page_size_)
    {
        int n = 0; // Initialize number of records
        int record_size = ATTRIBUTE_FIXED_LENGTH * ATTRIBUTES_IN_RECORD;
        // Iterate to find the maximum number of records that fit the page size
        while ((n * record_size + (n + 2) * sizeof(int)) <= page_size_)
        {
            n++;
        }

        return n - 1; // Adjust for the last increment that exceeds the page size
    }

    int get_record_count(Page *page)
    {
        int* page_dir = get_slot_directory(page);
        return page_dir[page->slot_size_ - 1];
    }


    // Assume each slot stores an offset as an int
    int *get_slot_directory(Page *page)
    {
        return reinterpret_cast<int *>(reinterpret_cast<char *>(page->data_) + page->page_size_ - page->slot_size_ * sizeof(int));
    }

    void print_slot_dir(Page *page) 
    {
        int *slot_dir = get_slot_directory(page);
        int num_slots = page->slot_size_;

        for (int i = 0; i < num_slots; ++i) {
            int slot_value = slot_dir[i];
            std::cout << "Slot [" << i << "] : " << slot_value << std::endl;
        }
    }
    
    void add_records_to_page(Page *page, const std::string &csv_input_file)
    {
        std::ifstream csv_file(csv_input_file);

        int *slot_dir = get_slot_directory(page);

        std::string line;

        while (std::getline(csv_file, line))
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

            if (add_fixed_len_page(page, &record) == -1)
            {
                cleanup_record(record);
                return;
            }
           
            cleanup_record(record);
        }
    }

    void print_page_records(Page *page)
    {
        if (!page)
        {
            std::cerr << "Error: Invalid page pointer" << std::endl;
            return;
        }

        int *slot_dir = get_slot_directory(page);
        /* std::cout << "\nPrinting Slot Directory\n" << std::endl;
        print_slot_dir(page); */

        std::cout << "Printing records in the page:" << std::endl;
        int record_count{0};

        // Iterate over slots in reverse order to print records
        for (int i = page->slot_size_ - 3; i >= 0; i--)
        {
            if (slot_dir[i] != -1)
            {
                Record record;
                read_fixed_len_page(page, i, &record);
                record_count++;
                std::cout << "RECORD " << record_count << std::endl;
                print_record(record);
                cleanup_record(record);
            }
        }
    }

    void read_and_print(std::ifstream &file, size_t num_bytes)
    {
        std::vector<char> buffer(num_bytes);
        file.read(buffer.data(), num_bytes);

        // Check how many bytes were successfully read
        std::streamsize bytes_read = file.gcount();
        std::cout << "Bytes read: " << bytes_read << std::endl;

        // Print the buffer content in hexadecimal
        for (size_t i = 0; i < static_cast<size_t>(bytes_read); ++i)
        {
            std::cout << std::hex << (buffer[i] & 0xff) << " ";
        }
        std::cout << std::endl;
    }

    void write_fixed_len_pages(const std::string &csv_input_file, const std::string &output_page_file, int page_size)
    {
        std::ifstream csv_file(csv_input_file);
        std::ofstream page_file(output_page_file, std::ios::binary | std::ios::app); // Open for appending in binary mode

        if (!csv_file.is_open() || !page_file.is_open())
        {
            std::cerr << "Error opening file(s)." << std::endl;
            return;
        }

        int page_records_cap = page_record_capacity(page_size); 
        Page page;
        init_fixed_len_page(&page, page_size, page_records_cap + 2);
        int *slot_dir = get_slot_directory(&page);

        std::string line;
        int records_count = 0, pages_count = 0;
        auto start_time = std::chrono::high_resolution_clock::now();

        while (std::getline(csv_file, line))
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

            if (add_fixed_len_page(&page, &record) != -1)
            {
                records_count++;
            }
            else
            {
                // Page is full, write it out and reset for next page
                char *data = static_cast<char *>(page.data_);
                page_file.write(data, page.page_size_);
                pages_count++;
                std::memset(page.data_, 0, page.page_size_ - (page.slot_size_ * 4));
                std::fill_n(slot_dir, page.slot_size_, -1);
                slot_dir[page.slot_size_ - 1] = 0;
                slot_dir[page.slot_size_ - 2] = 0;
                // Add record to the new page
                add_fixed_len_page(&page, &record);
                records_count++;
            }

            cleanup_record(record);
        }

        // After finishing the loop, check if there are any unsaved records in the current page buffer
        if (slot_dir[page.slot_size_ - 1] > 0 && slot_dir[page.slot_size_ - 1] != page_records_cap)
        {
            // Write the final page to the file
            char *data = static_cast<char *>(page.data_);
            page_file.write(data, page.page_size_);
            pages_count++;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << "NUMBER OF RECORDS: " << records_count << std::endl;
        std::cout << "NUMBER OF PAGES: " << pages_count << std::endl;
        std::cout << "TIME: " << duration << " milliseconds" << std::endl;

        delete[] static_cast<char *>(page.data_);
    }

    void read_fixed_len_pages(const std::string &output_page_file, int page_size)
    {
        std::ifstream input_file(output_page_file, std::ios::binary);
        if (!input_file)
        {
            std::cerr << "Error opening this: " << output_page_file << std::endl;
            return;
        }
        else
        {
            Record record;
            int record_no = 1;
            int no_page = 1;
            int page_records_capacity = page_record_capacity(page_size);
            Page page;
            init_fixed_len_page(&page, page_size, page_records_capacity + 2);
            int *slot_dir = get_slot_directory(&page);
            char buffer[page_size];

            while (input_file.read(buffer, page_size))
            {
                std::memcpy(static_cast<char *>(page.data_), buffer,
                            page_size);

                std::cout << "Page No: " << no_page << std::endl;
                for (int i = page.slot_size_ - 3; i >= 0; i--)
                {
                    if (slot_dir[i] != -1)
                    {
                        std::cout << "Record [" << record_no << "] :" << std::endl;
                        record_no++;
                        read_fixed_len_page(&page, i, &record);
                        print_record(record);
                        cleanup_record(record);
                    }
                }

                std::memset(page.data_, 0, page_size);
                std::memset(buffer, 0, page_size);
                no_page++;
            }

            std::memset(reinterpret_cast<char *>(page.data_), 0, page.page_size_);
            delete[] reinterpret_cast<char *>(page.data_);
        }
    }

}