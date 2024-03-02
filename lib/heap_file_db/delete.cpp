#include <iostream>
#include <sstream>
#include <algorithm> // Include algorithm header for std::replace_if
#include <cctype> 
#include "heap_file_db.h"

using namespace dbms::heap_file;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <heapfile> <page_size> <record_id>" << std::endl;
        return 1;
    }

    const char* heapfile = argv[1];
    int page_size = std::stoi(argv[2]);

    RecordID record_id;

    // Parse the record ID from the command line argument
    std::string record_id_arg = argv[3];
    std::replace_if(record_id_arg.begin(), record_id_arg.end(), [](char c) { return !std::isdigit(c); }, ' ');
    std::istringstream iss(record_id_arg);
    if (!(iss >> record_id.page_id_ >> record_id.slot_)) {
        std::cerr << "Error parsing record ID" << std::endl;
        return 1;
    }

    bool success = delete_record(heapfile, record_id, page_size);
    if (success) {
        std::cout << "Record deleted successfully." << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to delete record." << std::endl;
        return 1;
    }
}
