#include "page_layout_db.h"
#include <cstring>
#include <iostream>


namespace dbms::page {


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
            // Should I initialize to 0?
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

            int* slot_directory = get_slot_directory(page);

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
        return (page->page_size_ - page->slot_size_)/(ATTRIBUTES_IN_RECORD * 
                    ATTRIBUTE_FIXED_LENGTH);
    }
    
    /**
     * Calculate the free space (number of free slots) in the page
     */
    int fixed_len_page_freeslots(Page *page)
    {
        int* slot_directory = get_slot_directory(page);
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
        int* slot_dir = get_slot_directory(page);
        
        // Space for the new record
        int required_space = ATTRIBUTE_FIXED_LENGTH * r->size();

        if (slot_dir[page->slot_size_ - 2] != -1) {
            for (int i = page->slot_size_ - 3; i >= 0; i--)
            {
                if (slot_dir[i] == -1)
                {
                    fixed_len_write(r, reinterpret_cast<char*>(page->data_) + slot_dir[page->slot_size_ - 2]);
                    
                    /* [x]: Inserting of Records Working Perfectly
                       [x]: Tested with deserialization right afterwards
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
                    char* slot_dir_start = reinterpret_cast<char*>(slot_dir);
                    // Calculate the address where the next free space starts (after adding
                    // record)
                    char* new_free_space_start = static_cast<char*>(page->data_) +
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
        


        



        



    }
    
    /**
     * Read a record from the page from a given slot.
     */
    void read_fixed_len_page(Page *page, int slot, Record *r) 
    {
        int* slot_dir = get_slot_directory(page);
        int record_offset = slot_dir[slot];
        // [x]: Fixed the issue of not being able to retrieve serialized data
        // The problem was that I was reinterpret_cast(ing) the void*
        // and adding the offset as an argument to function // BUG: BUT WHY?
        char* buffer = static_cast<char *>(page->data_);
        char* offset_buffer = buffer + record_offset;

        // Deserialize the buffer and store in record
        // [x]: Testing set size to 5 * 10 = 50
        fixed_len_read(offset_buffer, 50, r);

    }


    // Assume each slot stores an offset as an int
    int* get_slot_directory(Page* page) {
        // NOTE: Cast the void* data_ to char* before using it
        char* data = static_cast<char*>(page->data_);
        // The slot directory starts at the end of the page data minus 
        // the space needed for the slot count
        return reinterpret_cast<int*>(data + page->page_size_ - 
                    page->slot_size_ * sizeof(int));
    }

}