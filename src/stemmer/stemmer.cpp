#include "stemmer/stemmer.h"
#include "common/utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>

PorterStemmerRu::PorterStemmerRu() {
    vowels = {"а", "е", "и", "о", "у", "ы", "э", "ю", "я"};
    
    perfectiveground = {"в", "вши", "вшись"};
    reflexive = {"ся", "сь"};
    adjective = {"ее", "ие", "ые", "ое", "ими", "ыми", "ей", "ий", "ый", "ой",
                 "ем", "им", "ым", "ом", "его", "ого", "ему", "ому", "их", "ых",
                 "ую", "юю", "ая", "яя", "ою", "ею"};
    participle = {"ивш", "ывш", "ующ"};
    verb = {"ла", "на", "ете", "йте", "ли", "й", "л", "ем", "н", "ло", "но",
            "ет", "ют", "ны", "ть", "ешь", "нно"};
    noun = {"а", "ев", "ов", "ие", "ье", "е", "иями", "ями", "ами", "еи", "ии",
            "и", "ией", "ей", "ой", "ий", "й", "иям", "ям", "ием", "ем", "ам",
            "ом", "о", "у", "ах", "иях", "ях", "ы", "ь", "ию", "ью", "ю", "ия",
            "ья", "я"};
    superlative = {"ейш", "ейше"};
    derivational = {"ост", "ость"};
}

bool PorterStemmerRu::ends_with(const std::string& str, const std::string& suffix) const {
    if (suffix.length() > str.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool PorterStemmerRu::remove_ending(std::string& word, const std::vector<std::string>& endings) {
    std::vector<std::string> sorted_endings = endings;
    std::sort(sorted_endings.begin(), sorted_endings.end(),
              [](const std::string& a, const std::string& b) {
                  return a.length() > b.length();
              });
    
    for (const auto& ending : sorted_endings) {
        if (ends_with(word, ending)) {
            word = word.substr(0, word.length() - ending.length());
            return true;
        }
    }
    return false;
}

size_t PorterStemmerRu::find_rv(const std::string& word) const {
    for (size_t i = 0; i < word.length(); ++i) {
        std::string ch(1, word[i]);
        if (vowels.find(ch) != vowels.end()) {
            return i + 1;
        }
    }
    return word.length();
}

std::string PorterStemmerRu::stem(const std::string& word) {
    if (word.length() < 3) return word;
    
    std::string w = utils::to_lower(word);
    size_t rv_pos = find_rv(w);
    
    if (rv_pos >= w.length()) return w;
    
    std::string rv = w.substr(rv_pos);
    std::string stem_word = w.substr(0, rv_pos);
    
    if (remove_ending(rv, perfectiveground)) {
        return stem_word + rv;
    }
    
    remove_ending(rv, reflexive);
    
    bool removed = remove_ending(rv, adjective);
    if (removed) {
        remove_ending(rv, participle);
    } else {
        removed = remove_ending(rv, verb);
        if (!removed) {
            remove_ending(rv, noun);
        }
    }
    
    if (ends_with(rv, "и")) {
        rv = rv.substr(0, rv.length() - 1);
    }
    
    remove_ending(rv, derivational);
    
    if (ends_with(rv, "ь")) {
        rv = rv.substr(0, rv.length() - 1);
    }
    
    if (ends_with(rv, "нн")) {
        rv = rv.substr(0, rv.length() - 1);
    }
    
    return stem_word + rv;
}

std::string Stemmer::stem_english(const std::string& word) {
    std::string w = utils::to_lower(word);
    
    std::vector<std::string> suffixes = {"ing", "ed", "es", "s", "er", "ly"};
    
    for (const auto& suffix : suffixes) {
        if (w.length() > suffix.length() + 2 && 
            w.substr(w.length() - suffix.length()) == suffix) {
            return w.substr(0, w.length() - suffix.length());
        }
    }
    
    return w;
}

std::string Stemmer::stem_token(const std::string& token) {
    bool is_cyrillic = false;
    for (char c : token) {
        if (utils::is_cyrillic(c)) {
            is_cyrillic = true;
            break;
        }
    }
    
    if (is_cyrillic) {
        return stemmer_ru.stem(token);
    } else {
        return stem_english(token);
    }
}

void Stemmer::stem_corpus(const std::string& input_file, const std::string& output_file) {
    std::cout << "Начинаем стемминг..." << std::endl;
    
    utils::Timer timer;
    
    std::ifstream in(input_file);
    std::ofstream out(output_file);
    
    if (!in.is_open() || !out.is_open()) {
        std::cerr << "Ошибка открытия файлов" << std::endl;
        return;
    }
    
    std::set<std::string> unique_tokens;
    std::set<std::string> unique_stems;
    
    std::string line;
    while (std::getline(in, line)) {
        auto parts = utils::split(line, '|');
        if (parts.size() < 4) continue;
        
        std::string doc_id = parts[0];
        std::string source = parts[1];
        std::string title = parts[2];
        std::string tokens_str = parts[3];
        
        auto tokens = utils::split(tokens_str, ' ');
        std::vector<std::string> stems;
        
        for (const auto& token : tokens) {
            unique_tokens.insert(token);
            
            std::string stem = stem_token(token);
            stems.push_back(stem);
            
            unique_stems.insert(stem);
            stem_frequencies[stem]++;
        }
        
        stats.documents_processed++;
        stats.tokens_stemmed += tokens.size();
        
        out << doc_id << "|" << source << "|" << title << "|";
        for (size_t i = 0; i < stems.size(); ++i) {
            out << stems[i];
            if (i < stems.size() - 1) out << " ";
        }
        out << "\n";
        
        if (stats.documents_processed % 1000 == 0) {
            std::cout << "\rОбработано документов: " << stats.documents_processed << std::flush;
        }
    }
    
    std::cout << std::endl;
    
    stats.unique_before = unique_tokens.size();
    stats.unique_after = unique_stems.size();
    
    in.close();
    out.close();
    
    std::cout << "Стемминг завершён за " << timer.elapsed_ms() / 1000.0 << " сек" << std::endl;
}

void Stemmer::save_vocabulary(const std::string& vocab_file, int top_n) {
    std::cout << "Сохранение словаря стемов..." << std::endl;
    
    std::vector<std::pair<std::string, int>> sorted_stems(
        stem_frequencies.begin(), stem_frequencies.end()
    );
    
    std::sort(sorted_stems.begin(), sorted_stems.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::ofstream out(vocab_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка создания файла: " << vocab_file << std::endl;
        return;
    }
    
    out << "# Словарь стемов (топ-" << top_n << ")\n";
    out << "# Формат: стем частота\n";
    
    int count = 0;
    for (const auto& [stem, freq] : sorted_stems) {
        if (count++ >= top_n) break;
        out << stem << " " << freq << "\n";
    }
    
    out.close();
    std::cout << "Словарь сохранён: " << vocab_file << std::endl;
}

void Stemmer::print_statistics() const {
    double reduction = stats.unique_before > 0
        ? (1.0 - static_cast<double>(stats.unique_after) / stats.unique_before) * 100.0
        : 0;
    
    std::cout << "\nСТАТИСТИКА СТЕММИНГА:" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Документов обработано: " << stats.documents_processed << std::endl;
    std::cout << "Токенов обработано: " << stats.tokens_stemmed << std::endl;
    std::cout << "Уникальных токенов до: " << stats.unique_before << std::endl;
    std::cout << "Уникальных стемов после: " << stats.unique_after << std::endl;
    std::cout << "Сокращение словаря: " << reduction << "%" << std::endl;
    std::cout << "==============================\n" << std::endl;
}
