#include "search/bool_search.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>

SearchResult BoolSearch::search_term(const std::string& term) {
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchResult result;
    result.doc_ids = index.get_postings(term);
    result.total_found = result.doc_ids.size();
    
    auto end = std::chrono::high_resolution_clock::now();
    result.search_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return result;
}

std::set<int> BoolSearch::intersect(const std::set<int>& s1, const std::set<int>& s2) {
    std::set<int> result;
    std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                         std::inserter(result, result.begin()));
    return result;
}

std::set<int> BoolSearch::union_sets(const std::set<int>& s1, const std::set<int>& s2) {
    std::set<int> result;
    std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
                  std::inserter(result, result.begin()));
    return result;
}

std::set<int> BoolSearch::difference(const std::set<int>& s1, const std::set<int>& s2) {
    std::set<int> result;
    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
                       std::inserter(result, result.begin()));
    return result;
}

SearchResult BoolSearch::search_query(const std::vector<std::string>& terms,
                                     const std::vector<BoolOperator>& operators) {
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchResult result;
    
    if (terms.empty()) {
        result.total_found = 0;
        result.search_time_ms = 0;
        return result;
    }
    
    auto doc_ids = index.get_postings(terms[0]);
    std::set<int> result_set(doc_ids.begin(), doc_ids.end());
    
    for (size_t i = 0; i < operators.size() && i + 1 < terms.size(); ++i) {
        auto next_docs = index.get_postings(terms[i + 1]);
        std::set<int> next_set(next_docs.begin(), next_docs.end());
        
        switch (operators[i]) {
            case BoolOperator::AND:
                result_set = intersect(result_set, next_set);
                break;
            case BoolOperator::OR:
                result_set = union_sets(result_set, next_set);
                break;
            case BoolOperator::NOT:
                result_set = difference(result_set, next_set);
                break;
        }
    }
    
    result.doc_ids = std::vector<int>(result_set.begin(), result_set.end());
    result.total_found = result.doc_ids.size();
    
    auto end = std::chrono::high_resolution_clock::now();
    result.search_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return result;
}

SearchResult BoolSearch::execute_query(const std::string& query) {
    std::istringstream iss(query);
    std::vector<std::string> terms;
    std::vector<BoolOperator> operators;
    
    std::string token;
    while (iss >> token) {
        if (token == "AND") {
            operators.push_back(BoolOperator::AND);
        } else if (token == "OR") {
            operators.push_back(BoolOperator::OR);
        } else if (token == "NOT") {
            operators.push_back(BoolOperator::NOT);
        } else {
            terms.push_back(token);
        }
    }
    
    return search_query(terms, operators);
}
