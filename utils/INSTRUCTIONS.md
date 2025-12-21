# Инструкция по запуску проекта

## Требования

1. **C++ компилятор** (GCC, Clang или MSVC)
2. **CMake** версии 3.16 или выше
3. **Qt6** (Core и Widgets модули)

### Установка Qt6

#### macOS (через Homebrew):
```bash
brew install qt@6
```

#### Linux (Ubuntu/Debian):
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev cmake build-essential
```

#### Windows:
Скачайте и установите Qt6 с официального сайта: https://www.qt.io/download

## Сборка проекта

### Шаг 1: Откройте терминал в корневой папке проекта

```bash
cd /Users/macbook/Desktop/курсовая_25.11.25
```

### Шаг 2: Создайте папку для сборки

```bash
mkdir build
cd build
```

### Шаг 3: Запустите CMake

#### macOS/Linux:
```bash
cmake ..
```

Если Qt6 не найден автоматически, укажите путь:
```bash
cmake .. -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6  # для macOS с Homebrew
# или
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/qt6  # для Linux
```

#### Windows:
```bash
cmake .. -G "MinGW Makefiles"
# или для Visual Studio:
cmake .. -G "Visual Studio 16 2019"
```

### Шаг 4: Соберите проект

#### macOS/Linux:
```bash
make
```

#### Windows (MinGW):
```bash
mingw32-make
```

#### Windows (Visual Studio):
```bash
cmake --build . --config Release
```

## Запуск программы

### macOS/Linux:
```bash
./HRManagementSystem
```

### Windows:
```bash
HRManagementSystem.exe
```

## Возможные проблемы и решения

### Проблема: CMake не находит Qt6

**Решение:**
Укажите путь к Qt6 явно:
```bash
cmake .. -DCMAKE_PREFIX_PATH=/путь/к/qt6
```

На macOS с Homebrew:
```bash
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
```

### Проблема: Ошибка компиляции

**Решение:**
Убедитесь, что:
- Установлен C++17 компилятор
- Qt6 установлен корректно
- Все заголовочные файлы на месте в папке `include/`

### Проблема: Программа не запускается (Qt библиотеки не найдены)

**macOS:**
```bash
export DYLD_LIBRARY_PATH=/opt/homebrew/opt/qt@6/lib:$DYLD_LIBRARY_PATH
./HRManagementSystem
```

**Linux:**
```bash
export LD_LIBRARY_PATH=/usr/lib/qt6/lib:$LD_LIBRARY_PATH
./HRManagementSystem
```

**Windows:**
Убедитесь, что DLL файлы Qt6 находятся в PATH или в папке с exe файлом.

## Структура после сборки

После успешной сборки в папке `build/` будут:
- Исполняемый файл `HRManagementSystem` (или `HRManagementSystem.exe`)
- Объектные файлы (.o или .obj)
- CMake кэш файлы

## Быстрый старт (одной командой)

```bash
mkdir -p build && cd build && cmake .. && make && ./HRManagementSystem
```

## Проверка установки Qt6

Проверить, что Qt6 установлен:
```bash
qmake6 --version
# или
cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=EXIST
```

