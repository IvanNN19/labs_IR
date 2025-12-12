#include "stemmer/stemmer.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Тестирование Stemmer..." << std::endl;
    
    Stemmer stemmer;
    
    std::string stem = stemmer.stem_token("автомобиль");
    assert(!stem.empty());
    
    stem = stemmer.stem_token("машины");
    assert(!stem.empty());
    
    std::cout << "Все тесты пройдены!" << std::endl;
    return 0;
}
