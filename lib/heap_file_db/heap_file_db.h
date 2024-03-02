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

    void init_heapfile_read(Heapfile *heapfile, int page_size, FILE *file);

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

    void add_new_heap(Heapfile *heapfile, int prev_offset, int &prev_file_ptr, int* &heapfile_dir);

    int calculate_base_page_id(int page_id, int page_cap);
    
    int calculate_heap_id(int page_id, int page_cap);


    // [x]: Executables
    void csv_to_heapfile(const char* csv_file, const char* heapfile, const int page_size);
    void scan(const char* heapfile, const int page_size);
    void insert_csv_to_heapfile(const char* heapfile, const char* csv_file, const int page_size);
    bool update(const char* heapfile, const RecordID& record_id, int attribute_id, const char* new_value, const int page_size);

    // The central functionality of a heap file is enumeration of records
    // [ ]: RecordIterator Class Implementation

    class RecordIterator 
    {
        private:
            Heapfile *heapfile_;
            Page *curPage_;

        public:

            int total_pages{0};
            int pages_read{0};
            PageID next_page_index{0};
        
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
                total_pages = heap_file_dir[1];
                next_page_index = 2;
                curPage_ = new Page;

                if (total_pages > 0)
                {
                    
                    init_fixed_len_page(curPage_, heapfile_->page_size_,page_record_capacity(heapfile_->page_size_) + 2);
                    read_page(heapfile_, next_page_index, curPage_);
                    pages_read++;
                    page_dir = get_slot_directory(curPage_);
                    total_records = get_record_count(curPage_);
                    next_record_index = curPage_->slot_size_ - 2;

                }
                
            }


            // Destructor
            ~RecordIterator()
            {
                // Delete the dynamically allocated Page object
                if (curPage_ != nullptr) {
                    delete curPage_;
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
                next_record_index--;
                records_read++;
                Record record;
                std::cout << "Record Index: " << next_record_index << std::endl;
                read_fixed_len_page(curPage_, next_record_index, &record);
                return record;
            }

            bool hasNext() 
            {
                // Check if there are more records in the current page
                if (records_read <= total_records)
                {
                    return true;
                }

                // Check if there are more pages in the heapfile
                /* if (pages_read < total_pages)
                {
                    // Move to the next page
                    next_page_index += 2;
                    int offset_page = heap_file_dir[next_page_index];
                    delete[] static_cast<char*>(curPage_->data_);
                    curPage_->data_ = nullptr;
                    init_fixed_len_page(curPage_, heapfile_->page_size_, page_record_capacity(heapfile_->page_size_) + 2);
                    read_page(heapfile_, next_page_index, curPage_);
                    pages_read++;
                    
                    records_read = 0;
                    page_dir = get_slot_directory(curPage_);
                    total_records = get_record_count(curPage_);
                    next_record_index = curPage_->slot_size_ - 2;

                    return true;

                } */

                // No more records in the heapfile
                return false;
            }
    };

    /* class RecordIterator
    {
        private:
            Heapfile *heapfile_;
            Page *curPage_;

        public:

            int* heapfile_dir;
            int* page_dir;



            RecordIterator(Heapfile *heapfile): heapfile_(heapfile), curPage_(nullptr)
            {
                int heap_dir_offset = ftell(heapfile->file_ptr_);
                int metasize = heapfile->meta_data_size_;
                int pagesize = heapfile->page_size_;

                char* metadata[metasize * sizeof(int)];
                fread(metadata, sizeof(char), metasize * sizeof(int), heapfile->file_ptr_);
                int* meta_dir = reinterpret_cast<int*>(metadata);
                heapfile_dir = meta_dir;
                // Set the file pointer back to the start of the file
                std::fseek(heapfile->file_ptr_, heap_dir_offset, SEEK_SET);

                curPage_ = new Page;
                init_fixed_len_page(curPage_, heapfile_->page_size_, page_record_capacity(heapfile_->page_size_) + 2);
                read_page(heapfile_, 2, curPage_);
                print_page_records(curPage_);
                page_dir = reinterpret_cast<int *>(reinterpret_cast<char *>(curPage_->data_) + curPage_->page_size_ - 
                            curPage_->slot_size_ * sizeof(int));

            }


            // Copy Constructor and Assignment Operator
            // [ ] Disable for now
            RecordIterator(const RecordIterator &other) = delete;
            RecordIterator &operator=(const RecordIterator &other) = delete;

            // Move Constructor and Copy Constructor (R Values)
            RecordIterator(RecordIterator &&other) noexcept = default;
            RecordIterator &operator=(RecordIterator &&other) noexcept = default;

            ~RecordIterator()
            {
                // Delete the dynamically allocated Page object
                if (curPage_ != nullptr) {
                    delete[] static_cast<char*>(curPage_->data_);
                    // delete curPage_;
                    // delete[] heap_file_dir;

                }

                delete[] reinterpret_cast<char*>(heapfile_dir);
                delete[] reinterpret_cast<char*>(page_dir);

            }



    }; */


    
}