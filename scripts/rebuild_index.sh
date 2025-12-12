set -e
echo "ПЕРЕСОЗДАНИЕ ИНДЕКСА"
echo "======================="

DATA_RAW="data/raw"
DATA_PROC="data/processed"
BUILD="build"

echo "Очистка старых файлов..."
rm -f $DATA_PROC/corpus.txt
rm -f $DATA_PROC/tokens.txt
rm -f $DATA_PROC/stems.txt
rm -f $DATA_PROC/index.bin
rm -f $DATA_PROC/*.txt

if [ ! -f "$DATA_RAW/wikipedia_cars.json" ] && [ ! -f "$DATA_RAW/avito_cars.json" ]; then
    echo "Нет исходных данных!"
    exit 1
fi

echo ""
echo "1/5: Обработка корпуса..."
if [ -f "$DATA_RAW/wikipedia_cars.json" ]; then
    echo "  Обработка Wikipedia..."
    $BUILD/crawler "$DATA_RAW/wikipedia_cars.json" "$DATA_PROC/corpus_wiki.txt"
    cat "$DATA_PROC/corpus_wiki.txt" > "$DATA_PROC/corpus.txt"
fi

if [ -f "$DATA_RAW/avito_cars.json" ]; then
    echo "  Обработка Avito..."
    $BUILD/crawler "$DATA_RAW/avito_cars.json" "$DATA_PROC/corpus_avito.txt"
    cat "$DATA_PROC/corpus_avito.txt" >> "$DATA_PROC/corpus.txt"
    rm "$DATA_PROC/corpus_avito.txt"
fi

echo "  Всего документов: $(wc -l < $DATA_PROC/corpus.txt)"

echo ""
echo "2/5: Токенизация..."
$BUILD/tokenizer "$DATA_PROC/corpus.txt" "$DATA_PROC/tokens.txt"

echo ""
echo "3/5: Стемминг..."
$BUILD/stemmer "$DATA_PROC/tokens.txt" "$DATA_PROC/stems.txt"

echo ""
echo "Проверка stems.txt:"
echo "  Размер: $(ls -lh $DATA_PROC/stems.txt | awk '{print $5}')"
echo "  Строк: $(wc -l < $DATA_PROC/stems.txt)"
echo "  Первая строка:"
head -n 1 "$DATA_PROC/stems.txt"

echo ""
echo "4/5: Анализ Ципфа..."
$BUILD/zipf_analyzer "$DATA_PROC/stems.txt"

echo ""
echo "5/5: Построение индекса..."
$BUILD/build_index "$DATA_PROC/stems.txt" "$DATA_PROC/index.bin"

echo ""
echo "Проверка index.bin:"
ls -lh "$DATA_PROC/index.bin"

echo ""
echo "Тестовый поиск (toyota):"
echo "toyota" | $BUILD/bool_search "$DATA_PROC/index.bin" | head -n 15

echo ""
echo "======================="
echo "ИНДЕКС ПЕРЕСОЗДАН!"
echo "======================="
