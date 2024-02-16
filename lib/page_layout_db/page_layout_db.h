#include "../record_db/record_db.h"
#include <string>
namespace dbms::page {

    using namespace dbms::record_codec;
    
    struct Page 
    {
        void *data_;
        int page_size_;
        int slot_size_;
    };

    int* get_slot_directory(Page* page);
    
    /**
     * Length of a Attributes in a Record as an inlined 
     * constexpr with type size_t
    */

    inline constexpr std::size_t ATTRIBUTES_IN_RECORD = 100;

    /**
     * Initializes a page using the given slot size
     */
    void init_fixed_len_page(Page *page, int page_size, int slot_size);
    
    /**
     * Calculates the maximal number of records that fit in a page
     */
    int fixed_len_page_capacity(Page *page);
    
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


    // Experiment 3.2: Appending Pages to a Binary File

    int page_record_capacity(int page_size);
    
    void write_fixed_len_pages(const std::string& csv_file_name, const std::string& output_page_file, int page_size_);

    
}