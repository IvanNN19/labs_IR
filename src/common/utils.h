#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <map>

namespace utils {

std::vector<std::string> split(const std::string& str, char delimiter);

std::string trim(const std::string& str);

std::string to_lower(const std::string& str);

bool is_cyrillic(char c);

bool is_letter(char c);

struct Document {
    int doc_id;
    std::string source;
    std::string title;
    std::string text;
    std::string url;
};

std::vector<Document> read_json_corpus(const std::string& filename);

void write_documents_txt(const std::vector<Document>& docs, const std::string& filename);

std::vector<Document> read_documents_txt(const std::string& filename);

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_time).count();
    }
    
    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
    }
};

} 

#endif
