# ArgParser — Handy C++ CLI Argument Parser

**ArgParser** — лёгковесная и удобная C++ библиотека для разбора аргументов командной строки: строк, целых чисел и булевых флагов.  
Идеальна для небольших CLI-утилит и pet-проектов, благодаря интуитивному API и гибкой настройке.

---

##  Основные возможности

- Парсинг аргументов форматов `--param=value`, `-p=value`, а также позиционных аргументов без флагов.
- Поддержка строк, целых чисел, булевых флагов.
- Богатый конфигурируемый API через цепочки методов:
  - `AddStringArgument(...)` — добавить строковый аргумент.
  - `AddIntArgument(...)` — добавить целочисленный аргумент.
  - `AddFlag(...)` — добавить флаг.
  - `AddHelp(...)` — добавить вспомогательную информацию.
  - `.Default(value)` — указать значение по умолчанию.
  - `.MultiValue([min_сount])` — разрешить несколько значений (с необязательным минимальным количеством).
  - `.StoreValue(variable)` / `.StoreValues(container)` — сохранить результат парсинга в переменную или контейнер.
  - `.Positional()` — обозначить позиционный аргумент.
  - `.Help()` и `.HelpDescription()` — генерировать справку/описание автоматически.
- Полностью покрыто тестами Google Test (`tests/argparser_test.cpp`), обеспечивая надёжность и удобство рефакторинга.

---

##  API — примеры использования

```cpp
ArgParser parser("MyApp");

// Строковый аргумент с обязательным значением
parser.AddStringArgument("input")
      .StoreValue(inputPath);

// Опциональный числовой аргумент с дефолтом
parser.AddIntArgument("threads")
      .Default(4)
      .StoreValue(numThreads);

// Позиционный аргумент (повторяемый минимум 1 раз)
std::vector<int> ids;
parser.AddIntArgument("ids")
      .MultiValue(1)
      .Positional()
      .StoreValues(ids);

// Обработка `--help` автоматически
parser.AddHelp('h', "help", "Show help message");

if (!parser.Parse(args)) {
    std::cerr << parser.HelpDescription();
    return 1;
}
if (parser.Help()) {
    std::cout << parser.HelpDescription();
    return 0;
}