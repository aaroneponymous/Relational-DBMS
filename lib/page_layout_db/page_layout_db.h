#include "../record_db/record_db.h"
#include <string>
namespace dbms::page 
{

    using namespace dbms::record_codec;
    
    struct Page 
    {
        void *data_;
        int page_size_;
        int slot_size_;
    };

    
    
    /**
     * Length of a Attributes in a Record as an inlined 
     * constexpr with type size_t
    */

    inline constexpr int ATTRIBUTES_IN_RECORD = 100;

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

    // [x]: Helper Functions

    /**
     * Calculates the maximum capacity of records that can be stored in the page
     * Takes into account the allocation of memory space for a slot directory
     * Inequality Used: n x R + (n + 2) x 4 <= S
     * Where:
     * S ---> page size
     * R ---> size of each record (n + 2) x 4
     * n ---> no of records
    */
    int page_record_capacity(int page_size);

    /**
     * Returns an int* to the beginning of the slot directory
     * NOTE: When traversing do in reverse: slot_dir[page->size - 1]
    */
    int* get_slot_directory(Page* page);

    void read_and_print(std::ifstream& file, size_t num_bytes);

    void add_records_to_page(Page *page, const std::string &csv_input_file);

    void print_page_records(Page *page);



    // Experiment 3.2: Appending Pages to a Binary File and Reading from a Binary File
    // [x]: Functions for the executables

    /**
     * Write the fixed length pages to a binary output file from csv file as an input
    */
    void write_fixed_len_pages(const std::string& input_csv_file, 
                        const std::string& output_page_file, int page_size);

    /**
     * Read the fixed length pages that are in binary format in-memory (output file)
    */
    void read_fixed_len_pages(const std::string& output_page_file, int page_size);
    
}