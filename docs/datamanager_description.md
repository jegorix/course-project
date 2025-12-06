## Описание контейнера DataManager

### Общая идея

`DataManager` — центральный класс‑контейнер и менеджер данных приложения.  
Он хранит все сущности (сотрудники, отделы, должности, даты найма) и выполняет операции над ними, а также отвечает за загрузку/сохранение данных, поиск, сортировку, аналитику и отмену действий.

### Что хранится внутри DataManager

- **Пути к файлам данных**
  - `employeesPath`
  - `departmentsPath`
  - `positionsPath`
  - `hiresPath`

- **Основные коллекции (через RecordCollection)**
  - `RecordCollection<EmployeeRecord> employees;`
  - `RecordCollection<Department> departments;`
  - `RecordCollection<Position> positions;`

- **Используемые контейнеры в проекте**
  - Собственный обобщённый контейнер `RecordCollection<T>`:
    - внутри опирается на `std::vector<T>`;
    - предоставляет итератор, методы `add`, `findIf`, `removeIf`, `insertionSort`, `forEach`, доступ к данным через `data()` и `operator[]`.
  - Стандартные контейнеры:
    - `std::vector<int>` — для списков сотрудников в `Department` и подчинённых в `Manager`;
    - `std::vector<UndoCommand>` — стек команд отмены в `DataManager`;
    - `std::unordered_map<int, std::size_t>` — индексы сотрудников, отделов и должностей (`employeeIndex`, `departmentIndex`, `positionIndex`);
    - `std::vector<T>` внутри `RecordCollection<T>` как основное хранилище элементов.

- **Хеш‑таблицы индексов по идентификатору**
  - `employeeIndex : std::unordered_map<int, std::size_t>`
  - `departmentIndex : std::unordered_map<int, std::size_t>`
  - `positionIndex : std::unordered_map<int, std::size_t>`

- **Механизм отмены действий**
  - `std::vector<UndoCommand> undoStack;`
  - `bool undoInProgress = false;`
  - Вложенная структура `UndoCommand`:
    - `std::function<void()> undo;`
    - `std::string description;`

- **Счётчики для генерации новых идентификаторов**
  - `nextEmployeeId`
  - `nextDepartmentId`
  - `nextPositionId`

### Основные группы методов DataManager

- **Загрузка и сохранение данных**
  - `loadAll()`, `saveAll()`
  - Приватные методы: `loadEmployees()`, `loadDepartments()`, `loadPositions()`, `loadHireDates()`,  
    `saveEmployees()`, `saveDepartments()`, `savePositions()`, `saveHireDates()`.

- **Доступ к коллекциям**
  - `getEmployees()`, `getDepartments()`, `getPositions()`  
    (константные и неконстантные версии, возвращающие `RecordCollection<...>&`).

- **Операции над сотрудниками**
  - Добавление:
    - `addWorker(...)`
    - `addManager(...)`
  - Удаление:
    - `removeEmployee(int employeeId)`
  - Обновление:
    - `updateEmployeeDepartment(...)`
    - `updateEmployeeSalary(...)`
    - `updateEmployeePosition(...)`
    - `updateEmployeeBonus(...)`
    - `updateEmployeeName(...)`
    - `updateEmployeeHireDate(...)`

- **Операции над отделами**
  - `addDepartment(const std::string& name, int managerId)`
  - `removeDepartment(int departmentId)`
  - `updateDepartmentName(int departmentId, const std::string& newName)`
  - `updateDepartmentManager(int departmentId, int managerId)`

- **Операции над должностями**
  - `addPosition(const std::string& name, int hoursPerWeek)`
  - `removePosition(int positionId)`
  - `updatePositionName(int positionId, const std::string& newName)`
  - `updatePositionHours(int positionId, int hoursPerWeek)`

- **Поиск и сортировка сотрудников**
  - `findEmployees(const EmployeeSearchFilter& filter) const`
  - `sortEmployees(EmployeeSortKey key, bool descending = false) const`

- **Аналитика**
  - `calculateAverageSalaryForDepartment(int departmentId) const` — средняя зарплата по отделу.
  - В реализации `.cpp` дополнительно: формирование рейтинга отделов по эффективности затрат.

- **Отмена действий**
  - `canUndo() const`
  - `undoLastAction()`
  - `getLastUndoDescription() const`
  - Приватный метод `pushUndo(std::function<void()> undoAction, const std::string& description)`
    регистрирует команду отмены.

### Роль DataManager в архитектуре проекта

- **Единая точка входа к данным**  
  Интерфейс пользователя и остальная логика приложения работают не напрямую с отдельными
  контейнерами, а через `DataManager`.

- **Обеспечение целостности данных**  
  Класс следит за согласованностью связей:
  - сотрудник ↔ отдел (`departmentId`);
  - сотрудник ↔ должность (`positionId`);
  - сотрудник ↔ дата найма (`HireDate`).

- **Расширенная функциональность**  
  Помимо простого CRUD, `DataManager` реализует:
  - гибкий поиск и сортировку сотрудников;
  - аналитику по зарплатам и эффективности отделов;
  - механизм отмены последних операций (undo‑стек).

Таким образом, `DataManager` выступает как контейнер верхнего уровня и ядро бизнес‑логики системы кадрового учета.


