# Блок-схемы алгоритмов

## Руководство по использованию

### Что такое Mermaid?

Mermaid — это язык разметки для создания диаграмм и блок-схем, который поддерживается многими платформами:
- GitHub (автоматически отображает Mermaid диаграммы)
- GitLab
- VS Code (с расширением "Markdown Preview Mermaid Support")
- Онлайн редакторы: https://mermaid.live/

### Как просмотреть блок-схемы

1. **В VS Code:**
   - Установите расширение "Markdown Preview Mermaid Support"
   - Откройте этот файл и нажмите `Ctrl+Shift+V` (или `Cmd+Shift+V` на Mac) для предпросмотра

2. **Онлайн:**
   - Скопируйте код блок-схемы (между ```mermaid и ```)
   - Откройте https://mermaid.live/
   - Вставьте код в редактор
   - Диаграмма отобразится автоматически

3. **В GitHub/GitLab:**
   - Просто загрузите файл в репозиторий
   - Диаграммы отобразятся автоматически при просмотре файла

### Обозначения в блок-схемах

- **Овал** `([текст])` — начало/конец алгоритма
- **Прямоугольник** `[текст]` — процесс/действие
- **Ромб** `{текст}` — условие/проверка
- **Параллелограмм** `[/текст/]` — ввод/вывод данных
- **Стрелки** — направление выполнения алгоритма
  - `-->` — обычная стрелка
  - `-->|да|` — стрелка с меткой "да"
  - `-->|нет|` — стрелка с меткой "нет"

---

## Алгоритм 1. Загрузка данных при старте программы

```mermaid
flowchart TD
    Start([Старт: loadAll]) --> A[Определить рабочий каталог<br/>dataDirectory]
    A --> B[Сформировать пути к файлам:<br/>employeesPath, departmentsPath,<br/>positionsPath, hiresPath]
    B --> C[Очистить контейнеры:<br/>employees.clear,<br/>departments.clear,<br/>positions.clear]
    C --> D[Очистить индексы:<br/>employeeIndex.clear,<br/>departmentIndex.clear,<br/>positionIndex.clear]
    D --> E[Сбросить счетчики:<br/>nextEmployeeId = 1,<br/>nextDepartmentId = 1,<br/>nextPositionId = 1]
    E --> F[Очистить стек undo:<br/>undoStack.clear,<br/>undoInProgress = false]
    F --> G[Открыть positions.txt]
    G --> H{Файл<br/>открыт?}
    H -->|нет| I[Пропустить загрузку<br/>должностей]
    H -->|да| J[Построчно читать<br/>positions.txt]
    J --> K{Есть еще<br/>строка?}
    K -->|да| L[Разделить строку по ';'<br/>positionId, name, hours]
    L --> M[Создать объект Position]
    M --> N[Добавить в контейнер positions]
    N --> O[Обновить positionIndex]
    O --> K
    K -->|нет| P[Закрыть positions.txt]
    P --> Q[Открыть departments.txt]
    Q --> R{Файл<br/>открыт?}
    R -->|нет| S[Пропустить загрузку<br/>отделов]
    R -->|да| T[Построчно читать<br/>departments.txt]
    T --> U{Есть еще<br/>строка?}
    U -->|да| V[Разделить строку по ';'<br/>departmentId, name, managerId, employees]
    V --> W[Создать объект Department]
    W --> X[Восстановить список<br/>сотрудников отдела]
    X --> Y[Добавить в контейнер departments]
    Y --> Z[Обновить departmentIndex]
    Z --> U
    U -->|нет| AA[Закрыть departments.txt]
    AA --> AB[Открыть employees.txt]
    AB --> AC{Файл<br/>открыт?}
    AC -->|нет| AD[Пропустить загрузку<br/>сотрудников]
    AC -->|да| AE[Построчно читать<br/>employees.txt]
    AE --> AF{Есть еще<br/>строка?}
    AF -->|да| AG[Разделить строку по ';'<br/>employeeId, type, firstName,<br/>lastName, departmentId, salary, ...]
    AG --> AH{Тип =<br/>WORKER?}
    AH -->|да| AI[Создать объект Worker<br/>через make_shared]
    AH -->|нет| AJ[Создать объект Manager<br/>через make_shared]
    AI --> AK[Создать EmployeeRecord<br/>с временной HireDate]
    AJ --> AK
    AK --> AL[Добавить в контейнер employees]
    AL --> AM[Зарегистрировать в employeeIndex]
    AM --> AN[Обновить nextEmployeeId]
    AN --> AO[Привязать к отделу:<br/>ensureDepartmentContainsEmployee]
    AO --> AF
    AF -->|нет| AP[Закрыть employees.txt]
    AP --> AQ[Открыть hires.txt]
    AQ --> AR{Файл<br/>открыт?}
    AR -->|нет| AS[Пропустить загрузку<br/>дат найма]
    AR -->|да| AT[Построчно читать<br/>hires.txt]
    AT --> AU{Есть еще<br/>строка?}
    AU -->|да| AV[Разделить строку по ';'<br/>employeeId, day, month,<br/>year, hour, minute]
    AV --> AW[Найти EmployeeRecord<br/>по employeeId]
    AW --> AX{Запись<br/>найдена?}
    AX -->|да| AY[Обновить HireDate<br/>в EmployeeRecord]
    AX -->|нет| AZ[Пропустить запись]
    AY --> AU
    AZ --> AU
    AU -->|нет| BA[Закрыть hires.txt]
    BA --> BB([Готово: данные загружены])
    I --> Q
    S --> AB
    AD --> AQ
    AS --> BB
```

---

## Алгоритм 2. Добавление нового работника

```mermaid
flowchart TD
    Start([Старт: addWorker]) --> A[/Ввод данных:<br/>firstName, lastName,<br/>departmentId, positionId,<br/>salary, bonus, hireDate/]
    A --> B[Сгенерировать новый<br/>employeeId = generateEmployeeId]
    B --> C{Отдел существует?<br/>findDepartment}
    C -->|нет| D[/Вывод: Ошибка:<br/>Отдел не существует/]
    D --> E([Конец: операция<br/>прекращена])
    C -->|да| F{Должность существует?<br/>findPosition}
    F -->|нет| G[/Вывод: Ошибка:<br/>Должность не существует/]
    G --> E
    F -->|да| H[Создать объект Worker:<br/>Worker employeeId, firstName,<br/>lastName, departmentId,<br/>salary, positionId, bonus]
    H --> I[Создать EmployeeRecord:<br/>record = EmployeeRecord worker, hireDate]
    I --> J[Установить employeeId<br/>в record.hireDate]
    J --> K[Добавить запись в контейнер:<br/>employees.add record]
    K --> L[Зарегистрировать сотрудника:<br/>registerEmployee record]
    L --> M[Обновить employeeIndex]
    M --> N[Привязать к отделу:<br/>ensureDepartmentContainsEmployee<br/>departmentId, employeeId]
    N --> O{undoInProgress<br/>== false?}
    O -->|да| P[Создать undo-команду:<br/>pushUndo removeEmployee employeeId]
    O -->|нет| Q[Пропустить undo]
    P --> R[/Вывод: Работник<br/>успешно добавлен/]
    Q --> R
    R --> S([Конец: возврат<br/>employeeId])
```

---

## Альтернативный вариант (упрощенный)

Если предыдущие блок-схемы слишком детализированы, можно использовать упрощенные версии:

### Алгоритм 1. Загрузка данных (упрощенный)

```mermaid
flowchart TD
    Start([Старт]) --> A[Определить каталог data]
    A --> B[Сформировать пути к файлам]
    B --> C[Очистить контейнеры и индексы]
    C --> D[Загрузить должности:<br/>loadPositions]
    D --> E[Загрузить отделы:<br/>loadDepartments]
    E --> F[Загрузить сотрудников:<br/>loadEmployees]
    F --> G[Загрузить даты найма:<br/>loadHireDates]
    G --> End([Готово])
```

### Алгоритм 2. Добавление работника (упрощенный)

```mermaid
flowchart TD
    Start([Старт]) --> A[/Ввод данных<br/>о работнике/]
    A --> B{Отдел<br/>существует?}
    B -->|нет| C[/Ошибка:<br/>Отдел не найден/]
    C --> End1([Конец])
    B -->|да| D{Должность<br/>существует?}
    D -->|нет| E[/Ошибка:<br/>Должность не найдена/]
    E --> End1
    D -->|да| F[Сгенерировать ID]
    F --> G[Создать Worker]
    G --> H[Создать EmployeeRecord]
    H --> I[Добавить в контейнер]
    I --> J[Зарегистрировать в индексах]
    J --> K[Привязать к отделу]
    K --> L[Создать undo-команду]
    L --> M[/Сообщение:<br/>Работник добавлен/]
    M --> End2([Конец])
```

---

## Как экспортировать блок-схемы в изображения

### Способ 1: Mermaid Live Editor
1. Откройте https://mermaid.live/
2. Вставьте код блок-схемы
3. Нажмите "Actions" → "Download PNG" или "Download SVG"

### Способ 2: VS Code
1. Установите расширение "Markdown Preview Mermaid Support"
2. Откройте предпросмотр (Ctrl+Shift+V)
3. Используйте расширение "Markdown PDF" для экспорта в PDF с изображениями

### Способ 3: Командная строка (mermaid-cli)
```bash
npm install -g @mermaid-js/mermaid-cli
mmdc -i flowchart.mmd -o flowchart.png
```

---

## Примечания

- Блок-схемы созданы на основе алгоритмов из файла `algorithm_steps.txt`
- Детализированные версии показывают все шаги алгоритма
- Упрощенные версии показывают основные этапы для лучшей читаемости
- Выберите подходящий вариант в зависимости от требований документации


