#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <string>
#include <vector>
#include <map>
#include <fstream>

struct Posting {
    int doc_id;
    std::vector<int> positions;
    
    Posting(int id) : doc_id(id) {}
};

struct DocumentMeta {
    int doc_id;
    std::string title;
    std::string source;
    int length;
};

class InvertedIndex {
private:
    std::map<std::string, std::vector<Posting>> index;
    std::map<int, DocumentMeta> documents;
    
public:
    void build_from_file(const std::string& filename);
    
    void add_document(int doc_id, const std::string& title,
                     const std::string& source,
                     const std::vector<std::string>& terms);
    
    std::vector<int> get_postings(const std::string& term) const;
    
    const std::vector<Posting>* get_postings_with_positions(const std::string& term) const;
    
    void save_to_file(const std::string& filename) const;
    
    void load_from_file(const std::string& filename);
    
    void print_statistics() const;
    
    const DocumentMeta* get_document_meta(int doc_id) const;
    
    size_t get_index_size() const { return index.size(); }
    size_t get_documents_count() const { return documents.size(); }
};

#endif
