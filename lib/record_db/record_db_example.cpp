#include "record_db.h"
#include <iostream>
#include <vector>

using namespace dbms::record_codec;

int main() {

    // Example records
    Record record_1 = {"mbaig2", "Marzook", "Riverview"};
    Record record_2 = {"apaul9", "Aaron", "Innovation"};

    std::cout << fixed_len_sizeof(&record_1) << std::endl;;
    std::cout << fixed_len_sizeof(&record_2) << std::endl;;


    // Calculate the buffer size needed for the records
    int buffer_size = ATTRIBUTE_FIXED_LENGTH * (record_1.size() + record_2.size());
    std::cout << "Buffer Size: " << buffer_size << std::endl;

    // Allocate a buffer to hold both records
    char* buffer = new char[buffer_size];
    
    // Serialize the records into the buffer
    fixed_len_write(&record_1, buffer);
    fixed_len_write(&record_2, buffer + (record_1.size() * ATTRIBUTE_FIXED_LENGTH));

    // Create empty records for deserialization
    Record record_1_deserialized, record_2_deserialized;

    // Deserialize the records from the buffer
    fixed_len_read(buffer, buffer_size/2, &record_1_deserialized);
    fixed_len_read(buffer + (record_1.size() * ATTRIBUTE_FIXED_LENGTH), buffer_size/2, &record_2_deserialized);

    // Print the deserialized records
    std::cout << "Deserialized Record 1: ";
    print_record(record_1_deserialized);

    std::cout << "Deserialized Record 2: ";
    print_record(record_2_deserialized);

    // Clean up
    delete[] buffer;
    cleanup_record(record_1_deserialized);
    cleanup_record(record_2_deserialized);

    return 0;
}
