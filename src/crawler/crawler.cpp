#include "crawler/crawler.h"
#include <iostream>
#include <algorithm>

void Crawler::load_corpus_from_json(const std::string& filename) {
    std::cout << "Загрузка корпуса из JSON..." << std::endl;
    
    auto docs = utils::read_json_corpus(filename);
    
    stats.total_docs = docs.size();
    documents = docs;
    
    std::cout << "Загружено " << stats.total_docs << " документов" << std::endl;
}

bool Crawler::validate_document(const utils::Document& doc) {
    if (doc.text.length() < 50) {
        return false;
    }
    
    if (doc.title.empty()) {
        return false;
    }
    
    return true;
}

void Crawler::crawl() {
    std::cout << "\nНачинаем обход документов..." << std::endl;
    
    utils::Timer timer;
    std::vector<utils::Document> valid_docs;
    
    for (const auto& doc : documents) {
        if (validate_document(doc)) {
            valid_docs.push_back(doc);
            stats.crawled++;
        } else {
            stats.failed++;
        }

        if ((stats.crawled + stats.failed) % 1000 == 0) {
            std::cout << "\rОбработано: " << (stats.crawled + stats.failed) 
                     << " / " << stats.total_docs << std::flush;
        }
    }
    
    std::cout << std::endl;
    
    documents = valid_docs;
    stats.elapsed_time_ms = timer.elapsed_ms();
    
    std::cout << "Обход завершён" << std::endl;
}

void Crawler::save_results(const std::string& output_file) {
    utils::write_documents_txt(documents, output_file);
}

void Crawler::print_statistics() const {
    std::cout << "\nСТАТИСТИКА ОБХОДА:" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Всего документов: " << stats.total_docs << std::endl;
    std::cout << "Обработано: " << stats.crawled << std::endl;
    std::cout << "Отклонено: " << stats.failed << std::endl;
    std::cout << "Время обхода: " << stats.elapsed_time_ms / 1000.0 << " сек" << std::endl;
    
    if (stats.elapsed_time_ms > 0) {
        double docs_per_sec = (stats.crawled * 1000.0) / stats.elapsed_time_ms;
        std::cout << "Скорость: " << docs_per_sec << " док/сек" << std::endl;
    }
    
    std::cout << "==============================\n" << std::endl;
}
