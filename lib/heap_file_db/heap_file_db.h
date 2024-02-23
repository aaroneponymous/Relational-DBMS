#include "../record_db/record_db.h"
#include "../page_layout_db/page_layout_db.h"

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
    };

    // Abstraction of page ID and record ID
    typedef int PageID;
    struct RecordIDbits_free_space
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

    // The central functionality of a heap file is enumeration of records
    // [ ]: Implement the record iterator class

    class RecordIterator {
    private:
        Heapfile *heapfile;
        Page *curPage;

    public:
        RecordIterator(Heapfile *heapfile);
        Record next();
        bool hasNext();
    }

    
    
}