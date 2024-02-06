#include "record_db.h"
#include <iostream>
#include <vector>

using namespace dbms::record_codec;

int main() {

    // Example records
    Record record_1 = {"Marzook\0\0", "Riverview", "mbaig2"};
    Record record_2 = {"Aaron\0\0\0\0", "Inno\0\0\0\0\0", "apaul9"};

    std::cout << fixed_len_sizeof(&record_1) << std::endl;;
    std::cout << fixed_len_sizeof(&record_2) << std::endl;;


    // Calculate the buffer size needed for the records
    int bufferSize = fixed_len_sizeof(&record_1) + fixed_len_sizeof(&record_2);

    // Allocate a buffer to hold both records
    char* buffer = new char[bufferSize];
    
    // Serialize the records into the buffer
    fixed_len_write(&record_1, buffer);
    fixed_len_write(&record_2, buffer + fixed_len_sizeof(&record_1));

    // Create empty records for deserialization
    Record record_1_deserialized, record_2_deserialized;

    // Deserialize the records from the buffer
    fixed_len_read(buffer, fixed_len_sizeof(&record_1), &record_1_deserialized);
    fixed_len_read(buffer + fixed_len_sizeof(&record_1), fixed_len_sizeof(&record_2), &record_2_deserialized);

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
