#include "../record_db/record_db.h"
#include "../page_layout_db/page_layout_db.h"
#include "../record_db/record_db.h"
#include <iostream>
#include <cmath>

namespace dbms::heap_file
{
    using namespace dbms::page;

    /** // [x] Assumptions to Assign Unique Identifiers to Records in the Heapfile 
     * 
     *  ** Every page p has an entry in the heap directory of // [x] (page_offset, freespace)
     *  ** The page ID of p can be the index of its entry in the directory // [x] ID(p)
     * 
     *  ** Every record r is stored at some slot in some page p
     *  ** The record ID // [x] ID(r)
     *  ** is the concatenation of // [x] ID(p)
     *  ** and the slot index in p
    */


    struct Heapfile
    {
        FILE *file_ptr_;
        int meta_data_size_;
        int page_size_;
    };

    // Abstraction of page ID and record ID
    typedef int PageID;
    struct RecordID
    {
        PageID page_id_;
        int slot_;
    };





    // NOTE: Directory Based implementation of a Heap File
    // NOTE: with directory pages organized as a linked list
    // NOTE: and data pages that store records

    /**
     * Initalize a heapfile to use the file and page size given.
    */
    void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

    /**
     * Allocate another page in the heapfile.  This grows the file by a page.
     */
    PageID alloc_page(Heapfile *heapfile);

    /**
     * Read a page into memory
     */
    void read_page(Heapfile *heapfile, PageID pid, Page *page);

    /**
     * Write a page from memory to disk
     */
    void write_page(Page *page, Heapfile *heapfile, PageID pid);

    

    // [x]: Helper Functions

    void print_heapfile_directory(Heapfile *heapfile);

    int* get_heapfile_directory(Heapfile *heapfile);

    int heapfile_capacity(int page_size, int address_size);
    
    int heapfile_metadata_size(int heapfile_page_cap);

    // [x]: Executables
    void csv_to_heapfile(const char* csv_file, const char* heapfile, const int page_size);
    void scan(const char* heapfile, const int page_size);



    // The central functionality of a heap file is enumeration of records
    // [ ]: RecordIterator Class Implementation

    class RecordIterator 
    {
        private:
            Heapfile *heapfile_;
            Page *curPage_;

        public:

            int total_records{0};
            int records_read{0};
            int next_record_index{0};
            int* heap_file_dir{nullptr};
            int* page_dir{nullptr};

            // Constructor
            // Constructor
            RecordIterator(Heapfile *heapfile): heapfile_(heapfile), curPage_(nullptr)
            {
                heap_file_dir = get_heapfile_directory(heapfile_);
                curPage_ = new Page;
                init_fixed_len_page(curPage_, heapfile_->page_size_, 
                    page_record_capacity(heapfile_->page_size_) + 2);
                read_page(heapfile_, 2, curPage_);
                page_dir = get_slot_directory(curPage_);
                total_records = get_record_count(curPage_);
                next_record_index = curPage_->slot_size_ - 2;  
            }


            // Destructor
            ~RecordIterator()
            {
                // Delete the dynamically allocated Page object
                if (curPage_ != nullptr) {
                    delete[] static_cast<char*>(curPage_->data_);
                    delete curPage_;
                    delete[] heap_file_dir;

                }
            }


            // Copy Constructor and Assignment Operator
            // [ ] Disable for now
            RecordIterator(const RecordIterator &other) = delete;
            RecordIterator &operator=(const RecordIterator &other) = delete;

            // Move Constructor and Copy Constructor (R Values)
            RecordIterator(RecordIterator &&other) noexcept = default;
            RecordIterator &operator=(RecordIterator &&other) noexcept = default;

            Record next()
            {
                records_read++;
                Record return_record;
                read_fixed_len_page(curPage_, next_record_index, &return_record);
                return return_record;
            }

            bool hasNext() 
            {
                // Check if all records have been read
                if (records_read >= total_records) {
                    return false;
                }

                // Check if there are more records in the current page
                while (next_record_index >= 0) {
                    if (page_dir[next_record_index] != -1) {
                        // Found the next record in the current page
                        next_record_index--;
                        return true;
                    }
                    next_record_index--;
                }


                // No more records in the heapfile
                return false;
            }



            /* bool nextPage() 
            {
                if (!hasNextPage()) {
                    // No more pages available
                    return false;
                }

                // Move to the next page
                curr_page_index += 2;
                // Read the next page
                read_page(heapfile, curr_page_index, curPage);
                // Update record indices and counts for the new page
                total_records = get_record_count(curPage);
                next_record_index = curPage->slot_size_ - 2;
                curr_record_index = next_record_index;
                return true;
            } */

            /* bool hasNextPage() 
            {
                // Check if there are more pages in the heapfile to read
                int next_page_index = curr_page_index + 2;
                if (next_page_index >= heapfile->meta_data_size_ - 2) {
                    return false; // No more pages available
                }

                // Check if the next page's free space is less than the page size
                int next_page_offset = heap_file_dir[next_page_index];
                int next_page_free_space = heap_file_dir[next_page_index + 1];
                return next_page_free_space < heapfile->page_size_;
            } */

            /* bool hasNextHeapfile() 
            {
                // Check if there is a next heapfile directory available
                int next_heapfile_offset = heap_file_dir[0];
                return next_heapfile_offset != -1;
            }

            void nextHeapfile() {
                // Update heapfile pointer to point to the next heapfile directory
                int next_heapfile_offset = heap_file_dir[0];
                fseek(heapfile->file_ptr_, next_heapfile_offset, SEEK_SET);

                // Read the next heapfile directory into memory
                fread(heap_file_dir, sizeof(int), heapfile->meta_data_size_, heapfile->file_ptr_);

                // Update iterator state to point to the first page of the new heapfile
                curr_page_index = 2;
                pages_read = 0;
                records_read = 0;
                total_records = heap_file_dir[1];
                curPage = new Page;
                init_fixed_len_page(curPage, heapfile->page_size_, page_record_capacity(heapfile->page_size_) + 2);
                read_page(heapfile, 2, curPage);
                next_record_index = curPage->slot_size_ - 2;
            } */


            /* # Build heap file from CSV file
            $ csv2heapfile <csv_file> <heapfile> <page_size>

            



            # Print out all records in a heap file
            $ scan <heapfile> <page_size>

            # Insert all records in the CSV file to a heap file
            $ insert <heapfile> <csv_file> <page_size>

            # Update one attribute of a single record in the heap file given its record ID
            # <attribute_id> is the index of the attribute to be updated (e.g. 0 for the first attribute, 1 for the second attribute, etc.)
            # <new_value> will have the same fixed length (10 bytes)
            $ update <heapfile> <record_id> <attribute_id> <new_value> <page_size>

            # Delete a single record in the heap file given its record ID
            $ delete <heapfile> <record_id> <page_size> */

    };

    
}