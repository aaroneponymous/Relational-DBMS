#include "page_layout_db.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring> // For std::strcpy
#include <iostream>

using namespace dbms::page;
using namespace dbms::record_codec;

typedef std::vector<Record*> Records;

void read_csv(const std::string& filename, Page *page, std::vector<int> &record_id) {
    std::ifstream file(filename);
    std::string line;
    int i = 1;
    while (std::getline(file, line)) {
        std::stringstream line_stream(line);
        std::string cell;
        Record record;

        while (std::getline(line_stream, cell, ',')) {
            char* cell_str = new char[cell.length() + 1];
            std::strcpy(cell_str, cell.c_str());
            record.push_back(cell_str);
            // FIXME: Memory Leak here?
            // [x]: Fixed this was the dynamic allocation I was not deleting
            // delete[] cell_str;
        }
        fixed_len_sizeof(&record);
        int record_entry = add_fixed_len_page(page, &record);
        if (record_entry != -1) record_id.push_back(record_entry);
        int* slot_dir = get_slot_directory(page);
        // std::cout << "Count[" << i << "] : Record Entry[" << record_entry << "]" << ": Offset Value: " << slot_dir[record_entry] << " : ";
        // i++;
        // print_record(record);
        cleanup_record(record);
    }
}

void cleanup_records(Records& records) {
    for (auto& record : records) {
        for (auto& cell : *record) {
            delete[] cell; // Free the dynamically allocated strings
        }
    }
}

int main() {


    // BUG: Undefined Behaviour at INT MAX
    // write_fixed_len_pages(filename, out_filename, 2147483646);
    std::string csv_file_name = "people-100.csv";
    std::string output_file_name = "page_output"; 

    write_fixed_len_pages(csv_file_name, output_file_name, 100000);
    read_fixed_len_pages(output_file_name, 100000);



/*  std::string filename = "people-100.csv";
    Records records_read = read_csv(filename);
    std::vector<int> record_id;
    Record record_write; */

    // auto start = std::chrono::high_resolution_clock::now();

    // std::cout << "Total Number of Records: " << page_record_capacity(5000) <<
    //             "\n\n\n" << std::endl;

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> elapsed = end - start;

    // std::cout << "Time taken: " << elapsed.count() << " milliseconds\n";
    
    // std::vector<int> record_id;
    // Page* page_1 = new Page;
    // init_fixed_len_page(page_1, 5402, 52);
    // std::string filename = "people-reverse.csv";
    // read_csv(filename, page_1, record_id);

    // Record record_1 = {"dbd12","Shelby","Terrell","Male","1945-10-26"};
    // int x = page_1->slot_size_ - 3;
    // for (auto entry_id: record_id)
    // {
    //     write_fixed_len_page(page_1, x, &record_1);
    //     x--;

    // }


    // std::cout << "\n\nPrinting Records After Retrieval: \n" << std::endl;
    // int i = 1;

    // for (auto entry_id: record_id)
    // {
    //     Record record_deserialized;
    //     std::cout << "Count[" << i << "] : Record Entry[" << record_id[i - 1] << "]" << ":";
    //     i++;
    //     read_fixed_len_page(page_1, entry_id, &record_deserialized);
    //     print_record(record_deserialized);
    //     cleanup_record(record_deserialized);
    // }


    // Allocated Page* page_1
/*     Page* page_1 = new Page;
    init_fixed_len_page(page_1, 5402, 102);
    int* slot_dir = get_slot_directory(page_1);

    Record record_1 = {"dbd12","Shelby","Terrell","Male","1945-10-26"};

    Record record_2 = {"dbd13","Phillip","Summers","Female","1910-03-24"};

    int slot_r1 = add_fixed_len_page(page_1, &record_1);
    int slot_r2 = add_fixed_len_page(page_1, &record_2);


    Record record_1_deserialized, record_2_deserialized;
    read_fixed_len_page(page_1, slot_r1, &record_1_deserialized);
    read_fixed_len_page(page_1, slot_r2, &record_2_deserialized);

    // Print the deserialized records
    std::cout << "Deserialized Record 1: ";
    print_record(record_1_deserialized);

    std::cout << "Deserialized Record 2: ";
    print_record(record_2_deserialized);

    cleanup_record(record_1_deserialized);
    cleanup_record(record_2_deserialized);  */


    /* int* slot_dir = dbms::page::get_slot_directory(page_1);


    Record record_1 = {"mbaig2", "Marzook", "Riverview"};
    Record record_2 = {"apaul9", "Aaron", "Innovation"};

    // int slot_r1 = add_fixed_len_page(page_1, )

    // Space for the new record
    int required_space = ATTRIBUTE_FIXED_LENGTH * record_1.size();
    
    // Serialize the records into the reinterpret_cast<char*>(page_1->data_)
    fixed_len_write(&record_1, reinterpret_cast<char*>(page_1->data_));
    slot_dir[page_1->slot_size_- 1];
    slot_dir[page_1->slot_size_- 2] = required_space;
    slot_dir[page_1->slot_size_- 3] = 0;
    fixed_len_write(&record_2, reinterpret_cast<char*>(page_1->data_) + required_space);
    slot_dir[page_1->slot_size_- 1];
    slot_dir[page_1->slot_size_- 2] += required_space;
    slot_dir[page_1->slot_size_- 4] = required_space;
    std::cout << slot_dir[page_1->slot_size_- 4] << std::endl;


    // Create empty records for deserialization
    Record record_1_deserialized, record_2_deserialized;

    // Deserialize the records from the reinterpret_cast<char*>(page_1->data_)
    fixed_len_read(reinterpret_cast<char*>(page_1->data_) + slot_dir[page_1->slot_size_- 3], required_space, &record_1_deserialized);
    fixed_len_read(reinterpret_cast<char*>(page_1->data_) + slot_dir[page_1->slot_size_- 4], required_space, &record_2_deserialized);

    // Print the deserialized records
    std::cout << "Deserialized Record 1: ";
    print_record(record_1_deserialized);

    std::cout << "Deserialized Record 2: ";
    print_record(record_2_deserialized);

    cleanup_record(record_1_deserialized);
    cleanup_record(record_2_deserialized); */



    // Example usage
    // put a const_cast<Record*>
/*     for (const auto& record : records_read) {
    record_id.push_back(add_fixed_len_page(&page_1, record));
    }

    read_fixed_len_page(&page_1, 10, &record_write);
    print_record(record_write);

    cleanup_records(records_read);
    cleanup_record(record_write);
    record_id.clear(); */

    
    // std::memset(reinterpret_cast<char*>(page_1->data_), 0, page_1->page_size_);
    // delete[] reinterpret_cast<char*>(page_1->data_);
    // delete page_1;



    // Don't forget to clean up dynamically allocated memory

    return 0;
}

