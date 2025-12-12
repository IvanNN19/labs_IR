#!/bin/bash

echo "Сборка поисковой системы..."

mkdir -p build
cd build

cmake ..

make -j$(nproc)

echo "Сборка завершена!"
echo ""
echo "Собранные программы:"
ls -lh crawler tokenizer stemmer zipf_analyzer build_index bool_search
