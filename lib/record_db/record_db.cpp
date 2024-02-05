#include "record_db.h"
#include <vector>
#include <cstring>
#include <iostream> // Logging & Printing Error: std::cerr


namespace dbms::record_codec {
    
    // Fixed Length Serialization & Deserialization

    // Functions for Serializing Records

    /**
     *  Compute the number of bytes required to serialize record
    */

    // Lambda Expression Implementation to be used if local heavy usage
    auto fixed_len_sizeof_lambda = [] (Record *record) {
        // sizeof(char); record->size(); ATTRIBUTE_FIXED_LENGTH;
        return sizeof(char) * record->size() * ATTRIBUTE_FIXED_LENGTH;
    };

    int fixed_len_sizeof(Record *record) {
        return sizeof(char) * record->size() * ATTRIBUTE_FIXED_LENGTH;
    }

    /**
    * Serialize the record to a byte array to be stored in buf.
    */
    void fixed_len_write(Record *record, void *buf) {
        // @Parameter 2: void type *buf casted to char* type
        char* byte_buffer = static_cast<char*>(buf);
        
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

        }
    }

    // Functions for Deserializing Records
    /**
    * Deserializes `size` bytes from the buffer, `buf`, and
    * stores the record in `record`.
    */
    void fixed_len_read(void *buf, int size, Record *record);

    
    // Variable Length Serialization & Deserialization

}


int main() {

    using namespace dbms::record_codec;

    Record record_1 = {"Aaron", "123", "apaul9@rochester.edu"};

    // Create a byte_buffer to pass into the serialization function
    // Pass the address of the pointer
    int buffer_size = fixed_len_sizeof(&record_1);
    char* new_buffer = new char[buffer_size];

    fixed_len_write(&record_1, new_buffer);

    // Deallocate
    delete[] new_buffer;
    for (auto& entry: record_1) {
        delete[] entry;
    }
    record_1.clear();

    return 0;

}