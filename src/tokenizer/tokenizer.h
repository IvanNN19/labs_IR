#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "common/utils.h"
#include <vector>
#include <string>
#include <map>

class Tokenizer {
private:
    struct Statistics {
        int documents_processed = 0;
        size_t total_tokens = 0;
        size_t unique_tokens = 0;
    } stats;
    
    std::map<std::string, int> token_frequencies;
    
    bool is_token_char(unsigned char c) const;
    
public:
    std::vector<std::string> tokenize(const std::string& text);
    
    void tokenize_corpus(const std::string& input_file, const std::string& output_file);
    
    void save_vocabulary(const std::string& vocab_file, int top_n = 10000);
    
    void print_statistics() const;
};

#endif
