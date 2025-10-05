# Graph Serializer

Решение задачи по сериализации графов для курса "Проектирование больших систем на C++".

## Сборка
```
g++ -std=c++17 -O3 -I include src/*.cpp -o run.exe
```
## Использование

**Сериализация:**
```
run.exe -s -i input.tsv -o graph.bin
```

**Десериализация:**
```
run.exe -d -i graph.bin -o output.tsv
```
