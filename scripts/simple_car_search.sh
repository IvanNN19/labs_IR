RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' 

echo -e "${BLUE} ПРОСТОЙ ПОИСК АВТОМОБИЛЕЙ${NC}"
echo -e "${BLUE}==============================${NC}\n"

DATA_RAW="data/raw"
DATA_PROC="data/processed"
BUILD="build"

if [ ! -f "$BUILD/crawler" ]; then
    echo -e "${RED} Программы не собраны!${NC}"
    echo "Запустите: ./scripts/build.sh"
    exit 1
fi

if [ ! -f "$DATA_RAW/wikipedia_cars.json" ] && [ ! -f "$DATA_RAW/avito_cars.json" ]; then
    echo -e "${RED} Нет данных для поиска!${NC}"
    echo "Положите файлы в $DATA_RAW/:"
    echo "  - wikipedia_cars.json"
    echo "  - avito_cars.json"
    exit 1
fi

if [ ! -f "$DATA_PROC/index.bin" ]; then
    echo -e "${YELLOW}⚠️  Индекс не найден. Создаём...${NC}\n"
    
    echo -e "${GREEN}Шаг 1/5: Обработка корпуса...${NC}"
    
    if [ -f "$DATA_RAW/wikipedia_cars.json" ]; then
        $BUILD/crawler "$DATA_RAW/wikipedia_cars.json" "$DATA_PROC/corpus_wiki.txt"
        cat "$DATA_PROC/corpus_wiki.txt" > "$DATA_PROC/corpus.txt"
    fi
    
    if [ -f "$DATA_RAW/avito_cars.json" ]; then
        $BUILD/crawler "$DATA_RAW/avito_cars.json" "$DATA_PROC/corpus_avito.txt"
        cat "$DATA_PROC/corpus_avito.txt" >> "$DATA_PROC/corpus.txt"
        rm "$DATA_PROC/corpus_avito.txt"
    fi
    
    echo -e "\n${GREEN}Шаг 2/5: Токенизация...${NC}"
    $BUILD/tokenizer "$DATA_PROC/corpus.txt" "$DATA_PROC/tokens.txt"
    
    echo -e "\n${GREEN}Шаг 3/5: Стемминг...${NC}"
    $BUILD/stemmer "$DATA_PROC/tokens.txt" "$DATA_PROC/stems.txt"
    
    echo -e "\n${GREEN}Шаг 4/5: Анализ Ципфа...${NC}"
    $BUILD/zipf_analyzer "$DATA_PROC/stems.txt"
    
    echo -e "\n${GREEN}Шаг 5/5: Построение индекса...${NC}"
    $BUILD/build_index "$DATA_PROC/stems.txt" "$DATA_PROC/index.bin"
    
    echo -e "\n${GREEN} Индекс создан!${NC}\n"
fi

echo -e "${BLUE}Введите модель автомобиля для поиска:${NC}"
echo -e "${YELLOW}Примеры: toyota, bmw, camry, x5, lada, audi${NC}"
echo -e "${YELLOW}Операторы: toyota AND camry, bmw OR audi${NC}"
echo -e "${YELLOW}Введите 'exit' для выхода${NC}\n"

while true; do
    echo -n " > "
    read query
    
    if [ "$query" = "exit" ] || [ "$query" = "quit" ]; then
        echo -e "\n${GREEN}До свидания!${NC}"
        break
    fi
    
    if [ -z "$query" ]; then
        continue
    fi
    
    echo ""
    result=$(echo "$query" | $BUILD/bool_search "$DATA_PROC/index.bin" 2>&1)
    
    echo "$result" | while IFS= read -r line; do
        if [[ $line == *"Найдено:"* ]]; then
            echo -e "${GREEN}$line${NC}"
        elif [[ $line == *"Время:"* ]]; then
            echo -e "${BLUE}$line${NC}"
        elif [[ $line == *"[wikipedia]"* ]]; then
            echo -e "${YELLOW}$line${NC}"
        elif [[ $line == *"[avito]"* ]]; then
            echo -e "${BLUE}$line${NC}"
        else
            echo "$line"
        fi
    done
    
    echo ""
done
