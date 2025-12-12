#include "search/bool_search.h"
#include <iostream>
#include <unistd.h>

void print_usage(const char* program_name) {
    std::cout << "Использование:" << std::endl;
    std::cout << "  Интерактивный режим: " << program_name << " <index_file>" << std::endl;
    std::cout << "  Одиночный запрос:    echo 'запрос' | " << program_name << " <index_file>" << std::endl;
    std::cout << "  С аргументом:        " << program_name << " <index_file> <запрос>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    std::string index_file = argv[1];
    
    InvertedIndex index;
    std::cout << "Загрузка индекса..." << std::endl;
    index.load_from_file(index_file);
    index.print_statistics();
    
    BoolSearch search(index);
    
    if (argc >= 3) {
        std::string query;
        for (int i = 2; i < argc; ++i) {
            if (i > 2) query += " ";
            query += argv[i];
        }
        
        auto result = search.execute_query(query);
        
        std::cout << "\nЗапрос: " << query << std::endl;
        std::cout << "Найдено: " << result.total_found << std::endl;
        std::cout << "Время: " << result.search_time_ms << " мс" << std::endl;
        
        int limit = std::min(10, result.total_found);
        if (limit > 0) {
            std::cout << "\nТоп-" << limit << ":" << std::endl;
            for (int i = 0; i < limit; ++i) {
                auto* meta = index.get_document_meta(result.doc_ids[i]);
                if (meta) {
                    std::cout << (i+1) << ". [" << meta->source << "] "
                             << meta->title << std::endl;
                }
            }
        }
        
        return 0;
    }
    
    bool is_pipe = !isatty(fileno(stdin));
    
    if (!is_pipe) {
        std::cout << "\nБУЛЕВ ПОИСК (интерактивный режим)" << std::endl;
        std::cout << "Введите запрос (или 'exit' для выхода):" << std::endl;
        std::cout << "Примеры: toyota, bmw AND x5, audi OR mercedes" << std::endl;
        std::cout << "==============================\n" << std::endl;
    }
    
    std::string query;
    int queries_processed = 0;
    
    while (std::getline(std::cin, query)) {
        if (query.empty()) {
            if (is_pipe) break;
            continue;
        }
        
        if (query == "exit" || query == "quit") {
            break;
        }
        
        auto result = search.execute_query(query);
        
        if (!is_pipe) {
            std::cout << "\nЗапрос: " << query << std::endl;
        }
        
        std::cout << "Найдено: " << result.total_found << std::endl;
        std::cout << "Время: " << result.search_time_ms << " мс" << std::endl;
        
        int limit = std::min(10, result.total_found);
        if (limit > 0) {
            std::cout << "\nТоп-" << limit << ":" << std::endl;
            for (int i = 0; i < limit; ++i) {
                auto* meta = index.get_document_meta(result.doc_ids[i]);
                if (meta) {
                    std::cout << (i+1) << ". [" << meta->source << "] "
                             << meta->title << std::endl;
                }
            }
        }
        std::cout << std::endl;
        
        queries_processed++;
        
        if (is_pipe && queries_processed > 0) {
            break;
        }
        
        if (!is_pipe) {
            std::cout << "> ";
            std::cout.flush();
        }
    }
    
    if (!is_pipe) {
        std::cout << "\nДо свидания! Обработано запросов: " << queries_processed << std::endl;
    }
    return 0;
}
