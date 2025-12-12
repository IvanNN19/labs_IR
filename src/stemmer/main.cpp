#include "stemmer/stemmer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Использование: " << argv[0] 
                 << " <input_tokens> <output_stems>" << std::endl;
        std::cout << "Пример: ./stemmer data/processed/tokens.txt data/processed/stems.txt" 
                 << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::string vocab_file = "data/processed/stem_vocabulary.txt";
    
    Stemmer stemmer;
    
    stemmer.stem_corpus(input_file, output_file);
    
    stemmer.save_vocabulary(vocab_file);
    
    stemmer.print_statistics();
    
    return 0;
}
