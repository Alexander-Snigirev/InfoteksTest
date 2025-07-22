# Библиотека mylog для записи логов в файл

### Инструкция по запуску

1. Склонируйте репозиторий
2. В рабочем каталоге создайте пустую папку build
```mkdir build && cd build```
3. Проведите сборку
```cmake ..``` - статическая сборка
```cmake .. -DBUILD_SHARED_LIBS=ON``` - динамическая сборка
4. Используйте утилиту make для создания исполняемого файла
```make```
5. Запустите файл demo
```./demo <filename> <Base importance(LOW/MEDIUM/HIGH)>```


Документация:

## Назначение
Библиотека предоставляет систему логирования с:
- Записью в файл (`FileSink`)
- Отправкой по сети (`SocketSink`)
- Фильтрацией по уровням важности
- Потокобезопасностью

## Быстрый старт
```cpp
#include <mylog/log_manager.h>

int main() {
    LogManager logger("app.log", Importance::MEDIUM);
    logger.log("Запуск системы", Importance::HIGH);
    return 0;
}

