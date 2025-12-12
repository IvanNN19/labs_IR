#include "index/inverted_index.h"
#include "common/utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

void InvertedIndex::build_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    
    std::string line;
    int docs_processed = 0;
    
    std::cout << "Построение индекса..." << std::endl;
    
    while (std::getline(file, line)) {
        auto parts = utils::split(line, '|');
        if (parts.size() < 4) continue;
        
        int doc_id = std::stoi(parts[0]);
        std::string source = parts[1];
        std::string title = parts[2];
        std::string terms_str = parts[3];
        
        auto terms = utils::split(terms_str, ' ');
        
        add_document(doc_id, title, source, terms);
        
        docs_processed++;
        if (docs_processed % 1000 == 0) {
            std::cout << "\rОбработано: " << docs_processed << std::flush;
        }
    }
    
    std::cout << "\nИндекс построен: " << docs_processed << " документов" << std::endl;
    file.close();
}

void InvertedIndex::add_document(int doc_id, const std::string& title,
                                 const std::string& source,
                                 const std::vector<std::string>& terms) {
    DocumentMeta meta;
    meta.doc_id = doc_id;
    meta.title = title;
    meta.source = source;
    meta.length = terms.size();
    documents[doc_id] = meta;
    
    for (size_t position = 0; position < terms.size(); ++position) {
        const std::string& term = terms[position];
        
        auto& postings = index[term];
        
        if (postings.empty() || postings.back().doc_id != doc_id) {
            postings.push_back(Posting(doc_id));
        }
        
        postings.back().positions.push_back(position);
    }
}

std::vector<int> InvertedIndex::get_postings(const std::string& term) const {
    auto it = index.find(term);
    if (it == index.end()) {
        return std::vector<int>();
    }
    
    std::vector<int> doc_ids;
    for (const auto& posting : it->second) {
        doc_ids.push_back(posting.doc_id);
    }
    return doc_ids;
}

const std::vector<Posting>* InvertedIndex::get_postings_with_positions(const std::string& term) const {
    auto it = index.find(term);
    if (it == index.end()) {
        return nullptr;
    }
    return &(it->second);
}

void InvertedIndex::save_to_file(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка создания файла: " << filename << std::endl;
        return;
    }
    
    size_t terms_count = index.size();
    file.write(reinterpret_cast<const char*>(&terms_count), sizeof(terms_count));
    
    for (const auto& entry : index) {
        const std::string& term = entry.first;
        const auto& postings = entry.second;
        
        size_t term_len = term.length();
        file.write(reinterpret_cast<const char*>(&term_len), sizeof(term_len));
        file.write(term.c_str(), term_len);
        
        size_t postings_count = postings.size();
        file.write(reinterpret_cast<const char*>(&postings_count), sizeof(postings_count));
        
        for (const auto& posting : postings) {
            file.write(reinterpret_cast<const char*>(&posting.doc_id), sizeof(posting.doc_id));
            
            size_t positions_count = posting.positions.size();
            file.write(reinterpret_cast<const char*>(&positions_count), sizeof(positions_count));
            file.write(reinterpret_cast<char*>(const_cast<int*>(posting.positions.data())),
                      positions_count * sizeof(int));
        }
    }
    
    size_t docs_count = documents.size();
    file.write(reinterpret_cast<const char*>(&docs_count), sizeof(docs_count));
    
    for (const auto& entry : documents) {
        const auto& meta = entry.second;
        file.write(reinterpret_cast<const char*>(&meta.doc_id), sizeof(meta.doc_id));
        file.write(reinterpret_cast<const char*>(&meta.length), sizeof(meta.length));
        
        size_t title_len = meta.title.length();
        file.write(reinterpret_cast<const char*>(&title_len), sizeof(title_len));
        file.write(meta.title.c_str(), title_len);
        
        size_t source_len = meta.source.length();
        file.write(reinterpret_cast<const char*>(&source_len), sizeof(source_len));
        file.write(meta.source.c_str(), source_len);
    }
    
    file.close();
    std::cout << "Индекс сохранён: " << filename << std::endl;
}

void InvertedIndex::load_from_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия: " << filename << std::endl;
        return;
    }
    
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (file_size < 8) {
        std::cerr << "Файл индекса повреждён (слишком мал)" << std::endl;
        file.close();
        return;
    }
    
    std::cout << "📂 Загрузка индекса (" << file_size << " байт)..." << std::endl;
    
    size_t terms_count;
    file.read(reinterpret_cast<char*>(&terms_count), sizeof(terms_count));
    
    if (terms_count > 10000000) {
        std::cerr << "Ошибка: слишком много терминов (" << terms_count << ")" << std::endl;
        std::cerr << "Индекс повреждён. Пересоздайте его." << std::endl;
        file.close();
        return;
    }
    
    std::cout << "Загрузка " << terms_count << " терминов..." << std::endl;
    
    for (size_t i = 0; i < terms_count; ++i) {
        size_t term_len;
        file.read(reinterpret_cast<char*>(&term_len), sizeof(term_len));
        
        if (term_len > 1000) {
            std::cerr << "Ошибка: слишком длинный термин (" << term_len << ")" << std::endl;
            file.close();
            return;
        }
        
        std::string term(term_len, '\0');
        file.read(&term[0], term_len);
        
        size_t postings_count;
        file.read(reinterpret_cast<char*>(&postings_count), sizeof(postings_count));
        
        if (postings_count > 1000000) {
            std::cerr << "Ошибка: слишком много постингов для '" << term << "' (" << postings_count << ")" << std::endl;
            file.close();
            return;
        }
        
        std::vector<Posting> postings;
        for (size_t j = 0; j < postings_count; ++j) {
            int doc_id;
            file.read(reinterpret_cast<char*>(&doc_id), sizeof(doc_id));
            
            Posting posting(doc_id);
            
            size_t positions_count;
            file.read(reinterpret_cast<char*>(&positions_count), sizeof(positions_count));
            
            if (positions_count > 100000) {
                std::cerr << "Ошибка: слишком много позиций (" << positions_count << ")" << std::endl;
                file.close();
                return;
            }
            
            posting.positions.resize(positions_count);
            file.read(reinterpret_cast<char*>(posting.positions.data()),
                     positions_count * sizeof(int));
            
            postings.push_back(posting);
        }
        
        index[term] = postings;
        
        if ((i + 1) % 10000 == 0) {
            std::cout << "\rЗагружено терминов: " << (i + 1) << " / " << terms_count << std::flush;
        }
    }
    
    std::cout << std::endl;
    
    size_t docs_count;
    file.read(reinterpret_cast<char*>(&docs_count), sizeof(docs_count));
    
    if (docs_count > 10000000) {
        std::cerr << "Ошибка: слишком много документов (" << docs_count << ")" << std::endl;
        file.close();
        return;
    }
    
    std::cout << "Загрузка метаданных " << docs_count << " документов..." << std::endl;
    
    for (size_t i = 0; i < docs_count; ++i) {
        DocumentMeta meta;
        file.read(reinterpret_cast<char*>(&meta.doc_id), sizeof(meta.doc_id));
        file.read(reinterpret_cast<char*>(&meta.length), sizeof(meta.length));
        
        size_t title_len;
        file.read(reinterpret_cast<char*>(&title_len), sizeof(title_len));
        
        if (title_len > 1000) {
            std::cerr << "Ошибка: слишком длинный заголовок (" << title_len << ")" << std::endl;
            file.close();
            return;
        }
        
        meta.title.resize(title_len);
        file.read(&meta.title[0], title_len);
        
        size_t source_len;
        file.read(reinterpret_cast<char*>(&source_len), sizeof(source_len));
        
        if (source_len > 100) {
            std::cerr << "Ошибка: слишком длинное имя источника (" << source_len << ")" << std::endl;
            file.close();
            return;
        }
        
        meta.source.resize(source_len);
        file.read(&meta.source[0], source_len);
        
        documents[meta.doc_id] = meta;
        
        if ((i + 1) % 10000 == 0) {
            std::cout << "\rЗагружено документов: " << (i + 1) << " / " << docs_count << std::flush;
        }
    }
    
    std::cout << std::endl;
    
    file.close();
    
    if (file.fail() && !file.eof()) {
        std::cerr << "Ошибка чтения файла" << std::endl;
        index.clear();
        documents.clear();
        return;
    }
    
    std::cout << "Индекс загружен успешно" << std::endl;
}

void InvertedIndex::print_statistics() const {
    size_t total_postings = 0;
    for (const auto& entry : index) {
        total_postings += entry.second.size();
    }
    double avg_postings = index.size() > 0 ? static_cast<double>(total_postings) / index.size() : 0;
    
    std::cout << "\nСТАТИСТИКА ИНДЕКСА:" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Документов: " << documents.size() << std::endl;
    std::cout << "Уникальных термов: " << index.size() << std::endl;
    std::cout << "Средняя длина постинг-листа: " << avg_postings << std::endl;
    std::cout << "==============================\n" << std::endl;
}

const DocumentMeta* InvertedIndex::get_document_meta(int doc_id) const {
    auto it = documents.find(doc_id);
    if (it == documents.end()) {
        return nullptr;
    }
    return &(it->second);
}
