#ifndef STEMMER_H
#define STEMMER_H

#include <string>
#include <vector>
#include <map>
#include <set>

class PorterStemmerRu {
private:
    std::set<std::string> vowels;
    
    std::vector<std::string> perfectiveground;
    std::vector<std::string> reflexive;
    std::vector<std::string> adjective;
    std::vector<std::string> participle;
    std::vector<std::string> verb;
    std::vector<std::string> noun;
    std::vector<std::string> superlative;
    std::vector<std::string> derivational;
    
    bool ends_with(const std::string& str, const std::string& suffix) const;
    bool remove_ending(std::string& word, const std::vector<std::string>& endings);
    size_t find_rv(const std::string& word) const;
    
public:
    PorterStemmerRu();
    std::string stem(const std::string& word);
};

class Stemmer {
private:
    PorterStemmerRu stemmer_ru;
    
    struct Statistics {
        int documents_processed = 0;
        size_t tokens_stemmed = 0;
        size_t unique_before = 0;
        size_t unique_after = 0;
    } stats;
    
    std::map<std::string, int> stem_frequencies;
    
    std::string stem_english(const std::string& word);
    
public:
    std::string stem_token(const std::string& token);
    
    void stem_corpus(const std::string& input_file, const std::string& output_file);
    
    void save_vocabulary(const std::string& vocab_file, int top_n = 10000);
    
    void print_statistics() const;
};

#endif
