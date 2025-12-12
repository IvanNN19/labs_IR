#ifndef CRAWLER_H
#define CRAWLER_H

#include "common/utils.h"
#include <vector>
#include <string>

class Crawler {
private:
    std::vector<utils::Document> documents;
    
    struct Statistics {
        int total_docs = 0;
        int crawled = 0;
        int failed = 0;
        double elapsed_time_ms = 0;
    } stats;
    
    bool validate_document(const utils::Document& doc);
    
public:
    void load_corpus_from_json(const std::string& filename);
    
    void crawl();
    
    void save_results(const std::string& output_file);
    
    void print_statistics() const;
    
    const std::vector<utils::Document>& get_documents() const { return documents; }
};

#endif
