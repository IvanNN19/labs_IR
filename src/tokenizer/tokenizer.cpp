#include "tokenizer/tokenizer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <set>

bool Tokenizer::is_token_char(unsigned char c) const {
    return std::isalnum(c) || c == '-' || c == '\'' || 
           (c >= 0xC0 && c <= 0xFF);
}

std::vector<std::string> Tokenizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        
        if (is_token_char(c)) {
            current_token += std::tolower(c);
        } else {
            if (!current_token.empty() && current_token.length() >= 2) {
                tokens.push_back(current_token);
            }
            current_token.clear();
        }
    }
    
    if (!current_token.empty() && current_token.length() >= 2) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

void Tokenizer::tokenize_corpus(const std::string& input_file, const std::string& output_file) {
    std::cout << "Начинаем токенизацию..." << std::endl;
    
    utils::Timer timer;
    
    auto documents = utils::read_documents_txt(input_file);
    
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка создания файла: " << output_file << std::endl;
        return;
    }
    
    std::set<std::string> unique_tokens_set;
    
    for (const auto& doc : documents) {
        auto tokens = tokenize(doc.text);
        
        stats.documents_processed++;
        stats.total_tokens += tokens.size();
        
        for (const auto& token : tokens) {
            token_frequencies[token]++;
            unique_tokens_set.insert(token);
        }
        
        out << doc.doc_id << "|" << doc.source << "|" << doc.title << "|";
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            out << tokens[i];
            if (i < tokens.size() - 1) out << " ";
        }
        out << "\n";
        
        if (stats.documents_processed % 1000 == 0) {
            std::cout << "\rОбработано документов: " << stats.documents_processed 
                     << " / " << documents.size() << std::flush;
        }
    }
    
    std::cout << std::endl;
    
    stats.unique_tokens = unique_tokens_set.size();
    
    out.close();
    
    std::cout << "Токенизация завершена за " << timer.elapsed_ms() / 1000.0 << " сек" << std::endl;
}

void Tokenizer::save_vocabulary(const std::string& vocab_file, int top_n) {
    std::cout << "Сохранение словаря..." << std::endl;
    
    std::vector<std::pair<std::string, int>> sorted_tokens(
        token_frequencies.begin(), token_frequencies.end()
    );
    
    std::sort(sorted_tokens.begin(), sorted_tokens.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::ofstream out(vocab_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка создания файла: " << vocab_file << std::endl;
        return;
    }
    
    out << "# Словарь токенов (топ-" << top_n << ")\n";
    out << "# Формат: токен частота\n";
    
    int count = 0;
    for (const auto& [token, freq] : sorted_tokens) {
        if (count++ >= top_n) break;
        out << token << " " << freq << "\n";
    }
    
    out.close();
    std::cout << "Словарь сохранён: " << vocab_file << std::endl;
}

void Tokenizer::print_statistics() const {
    double avg_tokens = stats.documents_processed > 0 
        ? static_cast<double>(stats.total_tokens) / stats.documents_processed 
        : 0;
    
    std::cout << "\nСТАТИСТИКА ТОКЕНИЗАЦИИ:" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Документов обработано: " << stats.documents_processed << std::endl;
    std::cout << "Всего токенов: " << stats.total_tokens << std::endl;
    std::cout << "Уникальных токенов: " << stats.unique_tokens << std::endl;
    std::cout << "Средняя длина документа: " << avg_tokens << " токенов" << std::endl;
    std::cout << "==============================\n" << std::endl;
}
