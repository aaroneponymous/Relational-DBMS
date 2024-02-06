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

    // Lambda Expression Implementation to be used if local heavy usage
    // None?
    auto fixed_len_sizeof_lambda = [] (Record *record) {
        // sizeof(char); record->size(); ATTRIBUTE_FIXED_LENGTH;
        return sizeof(char) * record->size() * ATTRIBUTE_FIXED_LENGTH;
    };

    int fixed_len_sizeof(Record *record) {
        int fixed_size = 0;
        for (const auto& entry: *record) {
            fixed_size += sizeof(char) * strlen(entry);
        }
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
    * 
    * 
    * 
    * **** Following Implementation Commented Out in the Code Block (Use if Needed: Refer to the comment within the code block) ******
    * 
    * Three Cases:
    * 1. entry in Record is equal to ATTRIBUTE_FIXED_LENGTH
    * 
    *    std::memcpy: void *memcpy(void *__restrict__ __dest, const void *__restrict__ __src, size_t __n) noexcept(true)
    *    Copy the n bytes (size to be copied: entry_length) in src "entry (V)" to  dest "byte_buffer" (Why size entry_length: 
    *    so if we have to handle the case where entry length is less than ATTRIBUTE_FIXED_LENGTH: see below 2)
    * 
    * 2. entry is less than ATTRIBUTE_FIXED_LENGTH
    * 
    *    Since we have handled the copying of the entry (even though it is less than the ATTRIBUTE_FIXED_LENGTH in this
    *    case), we need to consider the remaining bytes.
    *    std::memset: void *memset(void *__s, int __c, size_t __n) noexcept(true)
    *    Set the remaining bytes size_t __n "computed by "ATTRIBUTE_FIXED_LENGTH - entry_length" in void *__s "byte_buffer" 
    *    offset-ed by the entry_length (which is less than ATTRIBUTE_FIXED_LENGTH) with a nullable character '\0'
    * 
    * 3. entry is greater than ATTRIBUTE_FIXED_LENGTH
    *    Case doesn't occur according to the specifications of the project?
    * 
    * After each iteration of the entry V in vector<V> Record, increment the byte_buffer by the ATTRIBUTE_FIXED_LENGTH
   */

    // FIXME: Don't allocate ATTRIBUTE_FIXED_LENGTH for entry in the buffer when copying the contents over : Unused Space
    // NOTE: Initial Fixes Done

    void fixed_len_write(Record *record, void *buf) {
        // @Parameter 2: void type *buf casted to char* type
        char* byte_buffer = static_cast<char*>(buf);

        for (const auto& entry: *record) {
            if (strlen(entry) > ATTRIBUTE_FIXED_LENGTH) {
                std::cerr << "Error: Entry: " << entry << " exceeded ATTRIBUTE_FIXED_LENGTH" << std::endl;
            }
            std::memcpy(byte_buffer, entry, strlen(entry));
            byte_buffer += strlen(entry);


            // NOTE: Old Implementation which allocates ATTRIBUTE_FIXED_LENGTH for entry in the buffer
            /* if (strlen(entry) > ATTRIBUTE_FIXED_LENGTH) {
                std::cerr << "Error: Entry " << entry << " exceeded ATTRIBUTE_FIXED_LENGTH" << std::endl;
            }
            else {
                // Copy the bytes in entry to the byte_buffer
                std::memcpy(byte_buffer, entry, ATTRIBUTE_FIXED_LENGTH);
                // Offset the byte_buffer by ATTRIBUTE_FIXED_LENGTH to copy next entry
                byte_buffer += ATTRIBUTE_FIXED_LENGTH;
            } */
        }

        /* 
        // Commented Out: Can be used if the records might contain entries
        //                larger than the fixed length as defined in 
        //                ATTRIBUTE_FIXED_LENGTH constexpr
        // 
        // Loop over each value/entry (char* V) in Record *record vector<V>
        for (auto& entry: *record) {
            size_t entry_length = std::strlen(entry);
            // 3 Cases to be Handled
            // 1. entry size == ATTRIBUTE_FIXED_LENGTH
            if (sizeof(entry) <= ATTRIBUTE_FIXED_LENGTH) {
                // Copy the bytes of entry to byte_buffer
                std::memcpy(byte_buffer, entry, ATTRIBUTE_FIXED_LENGTH);
                // 2. entry len < ATTRIBUTE_FIXED_LENGTH
                if (sizeof(entry) < ATTRIBUTE_FIXED_LENGTH) {
                    // set the unused bytes to null char
                    std::memset(byte_buffer + ATTRIBUTE_FIXED_LENGTH, '\0',
                    ATTRIBUTE_FIXED_LENGTH - sizeof(entry));
                }
            }
            // 3. entry len > ATTRIBUTE_FIXED_LENGTH
            else {
                // Error Handling: 
                std::cerr << "Error: Entry size exceeds the size of ATTRIBUTE_FIXED_LENGTH" << std::endl;
                std::memset(byte_buffer, '\0', ATTRIBUTE_FIXED_LENGTH);
            }

            // Increment the Pointer of byte_buffer
            byte_buffer += ATTRIBUTE_FIXED_LENGTH;

        } */
    }

    // Functions for Deserializing Records
    /**
    * Deserializes `size` bytes from the buffer, `buf`, and
    * stores the record in `record`.
    */
   // FIXME: Deserialization Function needs to be reimplemented after changes to the fixed_len_write
    void fixed_len_read(void *buf, int size, Record *record) {
        char* byte_buffer = static_cast<char*>(buf);
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

            // ********* Note: Remember to deallocate these strings later to avoid memory leaks ***********
        }
    }
   
    // Variable Length Serialization & Deserialization



    // Utility Functions
    void print_record(const Record& record) {
        for (const auto& value : record) {
            std::cout << "\"" << value << "\" ";
        }
        std::cout << std::endl;
    }

    // Function to clean up dynamically allocated Records
    void cleanup_record(Record& record) {
        for (auto& value : record) {
            delete[] value; // Free the dynamically allocated c-strings
        }
        record.clear(); // Clear the vector for good measure
    }

}