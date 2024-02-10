#include "page_layout_db.h"
#include "../../record_db/record_db.h"

namespace dbms::page {

    // Assume each slot stores an offset as an int
    int* get_slot_directory(Page* page) {
        // The slot directory starts at the end of the page data minus the space needed for the slot count
        return reinterpret_cast<int*>(page->data_ + page->page_size_ - 
                    page->slot_size_ * sizeof(int));
    }

     /**
     * Initializes a page using the given slot size
     */
    void init_fixed_len_page(Page *page, int page_size, int slot_size)
    {
        // Allocate memory for the Page *page
        // Should I initialize to 0?
        page->data_ = new char[page_size]{0};
        page->page_size_ = page_size;
        page->slot_size_ = slot_size;

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
    
    /**
     * Calculates the maximal number of records that fit in a page
     */
    int fixed_len_page_capacity(Page *page)
    {
        return (page->page_size_ - page->slot_size_)/ATTRIBUTE_FIXED_LENGTH;
    }
    
    /**
     * Calculate the free space (number of free slots) in the page
     */
    int fixed_len_page_freeslots(Page *page);
    
    /**
     * Add a record to the page
     * Returns:
     *   record slot offset if successful,
     *   -1 if unsuccessful (page full)
     */
    int add_fixed_len_page(Page *page, Record *r);
    
    /**
     * Write a record into a given slot.
     */
    void write_fixed_len_page(Page *page, int slot, Record *r);
    
    /**
     * Read a record from the page from a given slot.
     */
    void read_fixed_len_page(Page *page, int slot, Record *r);


}