# Семантические тесты

Тесты для проверки корректности заполнения метаинформации в таблицах.

## Структура

```
tests/semantic/
├── run_tests.py          # Скрипт запуска тестов
├── 01_simple_class/      # Тест: простой класс
│   ├── input.scala       # Входной файл
│   └── expected/         # Ожидаемый результат
│       └── SimpleClass/
│           ├── class_info.csv
│           ├── fields.csv
│           └── methods.csv
├── 02_fields/            # Тест: поля класса
├── 03_methods/           # Тест: методы
├── 04_local_vars/        # Тест: локальные переменные
├── 05_inheritance/       # Тест: наследование
└── 06_scopes/            # Тест: вложенные скоупы и for
```

## Запуск

```bash
python tests/semantic/run_tests.py
```

## Добавление нового теста

1. Создать директорию `NN_test_name/`
2. Создать `input.scala` с тестовым кодом
3. Создать `expected/` с ожидаемыми CSV файлами

## Форматы CSV

### class_info.csv
```
Modifiers,Name,Parent,FieldCount,MethodCount,ConstantCount
```

### fields.csv
```
Modifiers,ValVar,Name,Type,IsInit,HasValue
```

### methods.csv
```
Modifiers,Name,ArgTypes,ReturnType,LocalVarCount
```

### local_vars/*.csv
```
ValVar,Name,Type,IsInit,HasValue,Number,ScopeId,ScopeType
```

## Тесты

| # | Название | Проверяет |
|---|----------|-----------|
| 01 | simple_class | Базовый класс с методом |
| 02 | fields | val/var, модификаторы, инициализация |
| 03 | methods | Аргументы, возвращаемые типы, модификаторы |
| 04 | local_vars | Локальные переменные в методе |
| 05 | inheritance | Наследование классов, Parent |
| 06 | scopes | Вложенные блоки, for-циклы, scopeId |
