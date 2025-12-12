#!/bin/bash

echo "Запуск тестов..."

cd build

ctest --output-on-failure

echo "Тесты завершены!"
