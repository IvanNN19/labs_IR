#ifndef BOOL_SEARCH_H
#define BOOL_SEARCH_H

#include "index/inverted_index.h"
#include <string>
#include <vector>
#include <set>

enum class BoolOperator {
    AND,
    OR,
    NOT
};

struct SearchResult {
    std::vector<int> doc_ids;
    int total_found;
    double search_time_ms;
};

class BoolSearch {
private:
    InvertedIndex& index;
    
    std::set<int> intersect(const std::set<int>& s1, const std::set<int>& s2);
    std::set<int> union_sets(const std::set<int>& s1, const std::set<int>& s2);
    std::set<int> difference(const std::set<int>& s1, const std::set<int>& s2);
    
public:
    BoolSearch(InvertedIndex& idx) : index(idx) {}
    
    SearchResult search_term(const std::string& term);
    SearchResult search_query(const std::vector<std::string>& terms,
                             const std::vector<BoolOperator>& operators);
    SearchResult execute_query(const std::string& query);
};

#endif
