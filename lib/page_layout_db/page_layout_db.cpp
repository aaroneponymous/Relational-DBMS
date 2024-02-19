#include "page_layout_db.h"
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace dbms::page
{

    // FIXME: Passing page_size: Does it have to be interpreted as size_t
    // [x]: I am dumb: sizeof(page_size) would just give 4 bytes lmao
    // FIXME: Or char[int characters (page_size)]

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
            // FIXME - Initialize to int 0, not '\0' gives error
            // BUG - Conditional jump or move depends on uninitialised values
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

            for (int i = 0; i < slot_size; i++)
            {
                slot_directory[i] = -1;
            }

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
            for (int i = page->slot_size_ - 3; i >= 0; i--)
            {
                if (slot_dir[i] == -1)
                {
                    fixed_len_write(r, reinterpret_cast<char *>(page->data_) + slot_dir[page->slot_size_ - 2]);

                    /* [x]: Inserting of Records Working Perfectly
                    // [x]: Tested with deserialization right afterwards
                    // Print the deserialized records
                    Record record_1_deserialized;
                    fixed_len_read(page->data_, 30, &record_1_deserialized);
                    std::cout << "Deserialized Record 1: ";
                    print_record(record_1_deserialized);
                    cleanup_record(record_1_deserialized); */

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

                    return i; // Record Slot Offset
                }
            }
        }

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
        // Deserialize the buffer and store in record
        // [x]: Testing set size to 5 * 10 = 50
        int record_size = ATTRIBUTE_FIXED_LENGTH * ATTRIBUTES_IN_RECORD;
        fixed_len_read(reinterpret_cast<char *>(page->data_) + slot_dir[slot], record_size, r);
    }

    // Assume each slot stores an offset as an int
    int *get_slot_directory(Page *page)
    {
        // char* data = static_cast<char*>(page->data_);
        // The slot directory starts at the end of the page data minus
        // the space needed for the slot count
        // NOTE: Make sure to reinterpret the cast of void* page->data_
        return reinterpret_cast<int *>(reinterpret_cast<char *>(page->data_) + page->page_size_ - page->slot_size_ * sizeof(int));
    }

    // TODO Experiment 3.2: Appending Pages to a Binary File Functions

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
        Page *page = new Page;
        // page_records_cap + 2 ---> Need two more slots for free offset 
        // pointer and number of records
        init_fixed_len_page(page, page_size, page_records_cap + 2);
        int *slot_dir = get_slot_directory(page);

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

            if (add_fixed_len_page(page, &record) != -1)
            {
                records_count++;
            }
            else
            {
                // Page is full, write it out and reset for next page
                char *data = static_cast<char *>(page->data_);
                page_file.write(data, page->page_size_);
                pages_count++;
                // FIXME: Making a Change Here
                std::memset(page->data_, 0, page->page_size_ - (page->slot_size_ * 4));
                std::fill_n(slot_dir, page->slot_size_, -1);
                slot_dir[page->slot_size_ - 1] = 0;
                slot_dir[page->slot_size_ - 2] = 0;
                // Add record to the new page
                add_fixed_len_page(page, &record);
                records_count++;
            }

            cleanup_record(record);
        }

        // After finishing the loop, check if there are any unsaved records in the current page buffer
        if (slot_dir[page->slot_size_ - 1] > 0 && slot_dir[page->slot_size_ - 1] != page_records_cap)
        {
            // Write the final page to the file
            char *data = static_cast<char *>(page->data_);
            page_file.write(data, page->page_size_);
            pages_count++;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << "NUMBER OF RECORDS: " << records_count << std::endl;
        std::cout << "NUMBER OF PAGES: " << pages_count << std::endl;
        std::cout << "TIME: " << duration << " milliseconds" << std::endl;

        delete[] static_cast<char *>(page->data_);
        delete page;
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
            int page_records_capacity = page_record_capacity(page_size);
            Page *page = new Page;
            init_fixed_len_page(page, page_size, page_records_capacity + 2);
            int *slot_dir = get_slot_directory(page);
            char buffer[page_size];

            // NOTE: Bytes reading Test
            // std::cout << "First " << page_size << " Bytes:" << std::endl;
            // read_and_print(input_file, page_size);
            // While not End of File
            while (input_file.read(buffer, page_size))
            {
                std::memcpy(static_cast<char *>(page->data_), buffer,
                            page_size);

                for (int i = page->slot_size_ - 2; i >= 0; i--)
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

                std::memset(page->data_, 0, page_size);
                std::memset(buffer, 0, page_size);
            }

            std::memset(reinterpret_cast<char *>(page->data_), 0, page->page_size_);
            delete[] reinterpret_cast<char *>(page->data_);
            delete page;
        }
    }

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

}