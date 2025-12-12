#include "tokenizer/tokenizer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Использование: " << argv[0] 
                 << " <input_corpus> <output_tokens>" << std::endl;
        std::cout << "Пример: ./tokenizer data/processed/corpus.txt data/processed/tokens.txt" 
                 << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::string vocab_file = "data/processed/vocabulary.txt";
    
    Tokenizer tokenizer;
    tokenizer.tokenize_corpus(input_file, output_file);
    tokenizer.save_vocabulary(vocab_file);
    tokenizer.print_statistics();
    
    return 0;
}
