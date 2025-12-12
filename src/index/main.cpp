#include "index/inverted_index.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Использование: " << argv[0] << " <input_stems> <output_index>" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    InvertedIndex index;
    
    index.build_from_file(input_file);
    index.print_statistics();
    index.save_to_file(output_file);
    
    return 0;
}
