# Система кадрового учета на малом предприятии

## Описание

Программа для учета сотрудников, отделов и должностей на малом предприятии. Реализована на C++ с использованием Qt6 для графического интерфейса.

## Требования

- C++17 компилятор (GCC, Clang или MSVC)
- CMake 3.16 или выше
- Qt6 (Core, Widgets)

## Сборка проекта

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
```

### Windows

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"  # или "Visual Studio 16 2019"
cmake --build .
```

## Запуск

После сборки выполните:

```bash
./HRManagementSystem  # Linux/macOS
# или
HRManagementSystem.exe  # Windows
```

## Структура проекта

- `include/` - заголовочные файлы
- `src/` - исходные файлы
- `data/` - файлы данных (employees.txt, departments.txt, positions.txt, hires.txt)
- `docs/` - документация
- `diagram/` - диаграммы классов

## Основные возможности

- Учет сотрудников по отделам и должностям
- Добавление, удаление, редактирование сотрудников
- Управление отделами и должностями
- Поиск сотрудников по различным критериям
- Сортировка сотрудников
- Аналитика:
  - Средняя зарплата по отделу
  - Рейтинг отделов по эффективности затрат
- Отмена последних действий (Undo)

## Архитектура

### Иерархия классов

- **Person** (базовый класс)
  - **Employee** (сотрудник)
    - **Worker** (рядовой сотрудник)
    - **Manager** (руководитель)

### Основные классы

- `DataManager` - менеджер данных, центральный класс приложения
- `Department` - отдел предприятия
- `Position` - должность
- `HireDate` - дата приема на работу
- `RecordCollection<T>` - обобщенный контейнер для хранения данных

### Ключевые алгоритмы

1. **Рейтинг отделов по эффективности затрат** (`buildDepartmentsEfficiencyRating`)
   - Вычисляет коэффициент эффективности как отношение количества сотрудников к суммарным затратам на зарплату
   - Возвращает отсортированный список отделов

2. **Вычисление средней зарплаты по отделу** (`calculateAverageSalaryForDepartment`)
   - Суммирует зарплаты всех сотрудников отдела
   - Вычисляет среднее значение

## Формат данных

### employees.txt
```
ID;TYPE;FirstName;LastName;DepartmentId;Salary;PositionId/Bonus;Subordinates
```

### departments.txt
```
ID;Name;ManagerId;EmployeeIds
```

### positions.txt
```
ID;Name;HoursPerWeek
```

### hires.txt
```
EmployeeId;Day;Month;Year;Hour;Minute
```

## Автор

Новицкий Егор Юрьевич

