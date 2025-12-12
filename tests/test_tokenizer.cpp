#include "tokenizer/tokenizer.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Тестирование Tokenizer..." << std::endl;
    
    Tokenizer tokenizer;
    
    auto tokens = tokenizer.tokenize("Привет мир! Hello world!");
    assert(tokens.size() == 4);
    
    tokens = tokenizer.tokenize("BMW X5 2023");
    assert(tokens.size() >= 2);
    
    std::cout << "Все тесты пройдены!" << std::endl;
    return 0;
}
