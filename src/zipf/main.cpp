#include "zipf/zipf_analyzer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Использование: " << argv[0] 
                 << " <input_stems>" << std::endl;
        std::cout << "Пример: ./zipf_analyzer data/processed/stems.txt" 
                 << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = "data/processed/zipf_statistics.txt";
    
    ZipfAnalyzer analyzer;
    
    analyzer.analyze_corpus(input_file);

    analyzer.save_statistics(output_file);
    
    analyzer.print_statistics();
    analyzer.print_top_words(50);
    
    return 0;
}
