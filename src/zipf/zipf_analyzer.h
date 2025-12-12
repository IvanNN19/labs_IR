#ifndef ZIPF_ANALYZER_H
#define ZIPF_ANALYZER_H

#include <string>
#include <map>
#include <vector>

class ZipfAnalyzer {
private:
    std::map<std::string, int> word_frequencies;
    size_t total_words = 0;
    
public:
    void analyze_corpus(const std::string& input_file);
    
    void save_statistics(const std::string& output_file);
    
    void print_statistics() const;
    
    void print_top_words(int n = 50) const;
};

#endif
