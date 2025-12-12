#include "common/utils.h"
#include <iostream>
#include <chrono>
#include <locale>
#include <codecvt>

namespace utils {

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    }
    return result;
}

bool is_cyrillic(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 0xC0 && uc <= 0xFF);
}

bool is_letter(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalpha(uc) || is_cyrillic(c);
}

std::vector<Document> read_json_corpus(const std::string& filename) {
    std::vector<Document> documents;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return documents;
    }
    
    file.imbue(std::locale("en_US.UTF-8"));
    
    std::string line;
    std::string current_json;
    bool in_object = false;
    int brace_count = 0;
    
    while (std::getline(file, line)) {
        for (char c : line) {
            if (c == '{') {
                brace_count++;
                in_object = true;
            } else if (c == '}') {
                brace_count--;
            }
        }
        
        if (in_object) {
            current_json += line + "\n";
        }
        
        if (in_object && brace_count == 0) {
            Document doc;
            doc.doc_id = documents.size();
            
            std::istringstream iss(current_json);
            std::string json_line;
            
            while (std::getline(iss, json_line)) {
                json_line = trim(json_line);
                
                if (json_line.find("\"source\"") != std::string::npos) {
                    size_t start = json_line.find(":") + 1;
                    size_t first_quote = json_line.find("\"", start);
                    size_t second_quote = json_line.find("\"", first_quote + 1);
                    if (first_quote != std::string::npos && second_quote != std::string::npos) {
                        doc.source = json_line.substr(first_quote + 1, second_quote - first_quote - 1);
                    }
                }
                else if (json_line.find("\"title\"") != std::string::npos) {
                    size_t start = json_line.find(":") + 1;
                    size_t first_quote = json_line.find("\"", start);
                    size_t second_quote = json_line.find("\"", first_quote + 1);
                    if (first_quote != std::string::npos && second_quote != std::string::npos) {
                        doc.title = json_line.substr(first_quote + 1, second_quote - first_quote - 1);
                    }
                }
                else if (json_line.find("\"content\"") != std::string::npos || 
                         json_line.find("\"text\"") != std::string::npos) {
                    size_t start = json_line.find(":") + 1;
                    size_t first_quote = json_line.find("\"", start);
                    
                    if (first_quote != std::string::npos) {
                        std::string text_content;
                        size_t second_quote = json_line.find("\"", first_quote + 1);
                        
                        if (second_quote != std::string::npos) {
                            text_content = json_line.substr(first_quote + 1, second_quote - first_quote - 1);
                        }
                        
                        doc.text = text_content;
                    }
                }
                else if (json_line.find("\"url\"") != std::string::npos) {
                    size_t start = json_line.find(":") + 1;
                    size_t first_quote = json_line.find("\"", start);
                    size_t second_quote = json_line.find("\"", first_quote + 1);
                    if (first_quote != std::string::npos && second_quote != std::string::npos) {
                        doc.url = json_line.substr(first_quote + 1, second_quote - first_quote - 1);
                    }
                }
            }
            
            if (!doc.title.empty() && !doc.text.empty()) {
                documents.push_back(doc);
            }
            
            current_json.clear();
            in_object = false;
            brace_count = 0;
        }
    }
    
    file.close();
    
    std::cout << "Прочитано " << documents.size() << " документов из JSON" << std::endl;
    return documents;
}

void write_documents_txt(const std::vector<Document>& docs, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка создания файла: " << filename << std::endl;
        return;
    }
    
    for (const auto& doc : docs) {
        std::string clean_text = doc.text;
        std::replace(clean_text.begin(), clean_text.end(), '|', ' ');
        
        std::string clean_title = doc.title;
        std::replace(clean_title.begin(), clean_title.end(), '|', ' ');
        
        file << doc.doc_id << "|" 
             << doc.source << "|" 
             << clean_title << "|" 
             << doc.url << "|" 
             << clean_text << "\n";
    }
    
    file.close();
    std::cout << "Сохранено " << docs.size() << " документов в " << filename << std::endl;
}

std::vector<Document> read_documents_txt(const std::string& filename) {
    std::vector<Document> documents;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return documents;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t pos1 = line.find('|');
        if (pos1 == std::string::npos) continue;
        
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos2 == std::string::npos) continue;
        
        size_t pos3 = line.find('|', pos2 + 1);
        if (pos3 == std::string::npos) continue;
        
        size_t pos4 = line.find('|', pos3 + 1);
        if (pos4 == std::string::npos) continue;
        
        Document doc;
        try {
            doc.doc_id = std::stoi(line.substr(0, pos1));
            doc.source = line.substr(pos1 + 1, pos2 - pos1 - 1);
            doc.title = line.substr(pos2 + 1, pos3 - pos2 - 1);
            doc.url = line.substr(pos3 + 1, pos4 - pos3 - 1);
            doc.text = line.substr(pos4 + 1);
            
            documents.push_back(doc);
        } catch (...) {
            std::cerr << "Ошибка парсинга строки " << documents.size() << std::endl;
            continue;
        }
    }
    
    file.close();
    return documents;
}

}
