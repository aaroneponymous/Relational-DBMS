#include "record_db.h"
#include <vector>
#include <cstring>
#include <iostream> // Logging & Printing Error: std::cerr


namespace dbms::record_codec {
    
    // Fixed Length Serialization & Deserialization

    /**
     * Comments include the following aliases:
     * typedef const char* V;              // Each value stored as an immutable char array V
     * typedef std::vector<V> Record;      // Each tuple (Record) is a vector of char arrays V (all the values in the tuple)
    */

    // Functions for Serializing Records

    /**
     *  Compute the number of bytes required to serialize record
    */

    // [x] The attribute length is fixed but entries can be shorter than the max length
    // [x] Calculate the length of each entry iteratively

    int fixed_len_sizeof(Record *record) {
        auto fixed_size = 0;
        for (const auto& entry: *record) {
            fixed_size += sizeof(char) * strlen(entry);
        }
        // std::cout << fixed_size << std::endl;
        return fixed_size;
    }

    /**
    * Implementation: void fixed_len_write(Record *record, void *buf) 
    * 
    * @Parameters: Record *record and void *buf
    * Since *buf is of void type, to be able to write the serialized
    * record into the buffer have to static_cast it to type char* initialized
    * locally declared as char* byte_buffer
    * 
    * Writing Bytestream from vector<V> Record to static_cast<*char> buf
    * 
    * 
    * std::memcpy: void *memcpy(void *__restrict__ __dest, const void *__restrict__ __src, size_t __n) noexcept(true)
    * Copy the n bytes (size to be copied: ATTRIBUTE_FIXED_LENGTH) in src "entry (V)" to  dest "byte_buffer" 
    * 
    * 
    * Three Cases:
    * 1. entry in Record is equal to ATTRIBUTE_FIXED_LENGTH
    * 
    *    std::memcpy: void *memcpy(void *__restrict__ __dest, const void *__restrict__ __src, size_t __n) noexcept(true)
    *    Copy the n bytes (size to be copied: entry_length) in src "entry (V)" to  dest "byte_buffer" 
    * 
    * 2. entry is less than ATTRIBUTE_FIXED_LENGTH
    * 
    *    Since we have handled the copying of the entry (even though it is less than the ATTRIBUTE_FIXED_LENGTH in this
    *    case), we need to consider the remaining bytes.
    *    std::memset: void *memset(void *__s, int __c, size_t __n) noexcept(true)
    * 
    *    Set the remaining bytes size_t __n "computed by "ATTRIBUTE_FIXED_LENGTH - entry_length" in void *__s "byte_buffer" 
    *    offset-ed by the entry_length (which is less than ATTRIBUTE_FIXED_LENGTH) with a nullable character '\0'
    * 
    * 3. entry is greater than ATTRIBUTE_FIXED_LENGTH
    *    Case doesn't occur according to the specifications of the project?
    * 
    * After each iteration of the entry V in vector<V> Record, increment the byte_buffer by the ATTRIBUTE_FIXED_LENGTH
   */

    void fixed_len_write(Record *record, void *buf) {
        // @Parameter 2: void type *buf casted to char* type
        char* byte_buffer = static_cast<char*>(buf);

        for (const auto& entry: *record) {
            auto entry_length = std::strlen(entry);

            if (entry_length <= ATTRIBUTE_FIXED_LENGTH) {
                // [x]: Initialization to 0 important otherwise points to uninitialized bytes
                std::memset(byte_buffer + ATTRIBUTE_FIXED_LENGTH, 0, ATTRIBUTE_FIXED_LENGTH);
                std::memcpy(byte_buffer, entry, entry_length);
                if (entry_length < ATTRIBUTE_FIXED_LENGTH) {
                    std::memset(byte_buffer + ATTRIBUTE_FIXED_LENGTH, '\0', 
                    ATTRIBUTE_FIXED_LENGTH - entry_length);
                }
            }
            else {
            // Handle the case where attribute length exceeds ATTRIBUTE_FIXED_LENGTH
            // Option 1: Log an error/warning
            // Option 2: Skip this attribute with or without marking its space
            // This example simply logs and skips to the next attribute space
                std::cerr << "Attribute length exceeds fixed limit. Truncation or specific handling required.\n";
                print_record_pointer(record);
                std::memset(byte_buffer, '\0', ATTRIBUTE_FIXED_LENGTH); // Optionally mark the space as unused
            }

            byte_buffer += ATTRIBUTE_FIXED_LENGTH;
        }
    }
        

    // Functions for Deserializing Records
    /**
    * Deserializes `size` bytes from the buffer, `buf`, and
    * stores the record in `record`.
    */

    void fixed_len_read(void *buf, int size, Record *record) {
        char* byte_buffer = reinterpret_cast<char*>(buf);
        int num_attributes = size / ATTRIBUTE_FIXED_LENGTH; // Calculate the number of attributes

        for (int i = 0; i < num_attributes; ++i) {
            // Allocate space for a new attribute, +1 for null-terminator
            char* attribute = new char[ATTRIBUTE_FIXED_LENGTH + 1];

            // Copy ATTRIBUTE_FIXED_LENGTH bytes from the buffer into attribute
            std::strncpy(attribute, byte_buffer + (i * ATTRIBUTE_FIXED_LENGTH), ATTRIBUTE_FIXED_LENGTH);

            // Ensure null-termination
            attribute[ATTRIBUTE_FIXED_LENGTH] = '\0';

            // Add the attribute to the record
            record->push_back(attribute);
        }
    }
   
    // Variable Length Serialization & Deserialization

    // Utility Functions
    void print_record(const Record& record) {
        for (const auto& value : record) {
            std::cout << "\"" << value << "\" ";
        }
        std::cout << "\n\n" << std::endl;
    }
    // Function to clean up dynamically allocated Records
    void cleanup_record(Record& record) {
        for (auto& value : record) {
            delete[] value; // Free the dynamically allocated c-strings
        }
        record.clear(); // Clear the vector for good measure
    }


    void print_record_pointer(const Record *record)
    {
        for (const auto& entry: *record)
        {
            std::cout << entry << "; ";
        }
        std::cout << std::endl;
    }


    void delete_record_pointer(Record *record)
    {
        delete record;
    }

}