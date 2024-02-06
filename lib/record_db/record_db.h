#pragma once

#include <vector>

namespace dbms::record_codec {

    /**
     * Record Representation: Using Abstract Records as a Tuple of Values
     * 
     * For simplicity, we assume that records are maps mapping attribute names to values. 
     * The attribute names are stored as part of the schema, which will not be stored as part 
     * of the record serialization.
    */

   
    typedef const char* V;              // Each value stored as an immutable char array V
    typedef std::vector<V> Record;      // Each tuple (Record) is a vector of char arrays V (all the values in the tuple)

    /**
     * Length of a Fixed Attribute as an inlined 
     * constexpr with type size_t
    */

    inline constexpr std::size_t ATTRIBUTE_FIXED_LENGTH = 10;

    // Fixed Length Serialization & Deserialization

    // Functions for Serializing Records
    /**
    * Compute the number of bytes required to serialize record
    */
    int fixed_len_sizeof(Record *record);

    /**
    * Serialize the record to a byte array to be stored in buf.
    */
    void fixed_len_write(Record *record, void *buf);

    // Functions for Deserializing Records
    /**
    * Deserializes `size` bytes from the buffer, `buf`, and
    * stores the record in `record`.
    */
    void fixed_len_read(void *buf, int size, Record *record);

    
    // Variable Length Serialization & Deserialization

    // Utility Functions
    void print_record(const Record& record);

    // Function to clean up dynamically allocated Records
    void cleanup_record(Record& record);

}