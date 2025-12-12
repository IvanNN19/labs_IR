#include "crawler/crawler.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Использование: " << argv[0] 
                 << " <input_json> <output_txt>" << std::endl;
        std::cout << "Пример: ./crawler data/raw/wikipedia_cars.json data/processed/corpus.txt" 
                 << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    Crawler crawler;
    
    crawler.load_corpus_from_json(input_file);
    
    crawler.crawl();
    
    crawler.save_results(output_file);
    
    crawler.print_statistics();
    
    return 0;
}
