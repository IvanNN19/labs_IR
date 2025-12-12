#include "zipf/zipf_analyzer.h"
#include "common/utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cmath>

void ZipfAnalyzer::analyze_corpus(const std::string& input_file) {
    std::cout << "Анализ закона Ципфа..." << std::endl;
    
    std::ifstream in(input_file);
    if (!in.is_open()) {
        std::cerr << "Ошибка открытия файла: " << input_file << std::endl;
        return;
    }
    
    std::string line;
    int docs_processed = 0;
    
    while (std::getline(in, line)) {
        auto parts = utils::split(line, '|');
        if (parts.size() < 4) continue;
        
        std::string stems_str = parts[3];
        auto stems = utils::split(stems_str, ' ');
        
        for (const auto& stem : stems) {
            word_frequencies[stem]++;
            total_words++;
        }
        
        docs_processed++;
        if (docs_processed % 1000 == 0) {
            std::cout << "\rОбработано документов: " << docs_processed << std::flush;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Анализ завершён" << std::endl;
    
    in.close();
}

void ZipfAnalyzer::save_statistics(const std::string& output_file) {
    std::cout << "Сохранение статистики..." << std::endl;
    
    std::vector<std::pair<std::string, int>> sorted_words(
        word_frequencies.begin(), word_frequencies.end()
    );
    
    std::sort(sorted_words.begin(), sorted_words.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка создания файла: " << output_file << std::endl;
        return;
    }
    
    out << "# Статистика Ципфа\n";
    out << "# Всего слов: " << total_words << "\n";
    out << "# Уникальных слов: " << word_frequencies.size() << "\n";
    out << "# Формат: ранг слово частота ранг*частота\n\n";
    
    int rank = 1;
    for (const auto& [word, freq] : sorted_words) {
        int product = rank * freq;
        out << rank << " " << word << " " << freq << " " << product << "\n";
        rank++;
    }
    
    out.close();
    std::cout << "Статистика сохранена: " << output_file << std::endl;
}

void ZipfAnalyzer::print_statistics() const {
    std::cout << "\nСТАТИСТИКА ЗАКОНА ЦИПФА:" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Всего слов: " << total_words << std::endl;
    std::cout << "Уникальных слов: " << word_frequencies.size() << std::endl;
    
    int hapax = 0;
    for (const auto& [word, freq] : word_frequencies) {
        if (freq == 1) hapax++;
    }
    
    double hapax_percent = (static_cast<double>(hapax) / word_frequencies.size()) * 100.0;
    std::cout << "Hapax legomena: " << hapax << " (" << hapax_percent << "%)" << std::endl;
    
    std::vector<std::pair<std::string, int>> sorted_words(
        word_frequencies.begin(), word_frequencies.end()
    );
    
    std::sort(sorted_words.begin(), sorted_words.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::cout << "\nТоп-10 слов (проверка закона Ципфа):" << std::endl;
    std::cout << std::left << std::setw(6) << "Ранг" 
              << std::setw(20) << "Слово" 
              << std::setw(10) << "Частота" 
              << std::setw(15) << "Ранг*Частота" << std::endl;
    std::cout << std::string(51, '-') << std::endl;
    
    std::vector<int> products;
    for (int i = 0; i < 10 && i < static_cast<int>(sorted_words.size()); ++i) {
        int rank = i + 1;
        const auto& [word, freq] = sorted_words[i];
        int product = rank * freq;
        products.push_back(product);
        
        std::cout << std::left << std::setw(6) << rank
                  << std::setw(20) << word
                  << std::setw(10) << freq
                  << std::setw(15) << product << std::endl;
    }
    
    if (!products.empty()) {
        double avg = 0;
        for (int p : products) avg += p;
        avg /= products.size();
        
        double variance = 0;
        for (int p : products) {
            variance += (p - avg) * (p - avg);
        }
        variance /= products.size();
        double std_dev = std::sqrt(variance);
        
        std::cout << "\nСреднее (ранг * частота): " << avg << std::endl;
        std::cout << "Стандартное отклонение: " << std_dev << std::endl;
        std::cout << "Коэффициент вариации: " << (std_dev / avg) * 100.0 << "%" << std::endl;
    }
    
    std::cout << "==============================\n" << std::endl;
}

void ZipfAnalyzer::print_top_words(int n) const {
    std::vector<std::pair<std::string, int>> sorted_words(
        word_frequencies.begin(), word_frequencies.end()
    );
    
    std::sort(sorted_words.begin(), sorted_words.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::cout << "\nТоп-" << n << " самых частых слов:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    for (int i = 0; i < n && i < static_cast<int>(sorted_words.size()); ++i) {
        const auto& [word, freq] = sorted_words[i];
        std::cout << std::setw(3) << (i + 1) << ". " 
                  << std::setw(25) << word 
                  << std::setw(10) << freq << std::endl;
    }
}
