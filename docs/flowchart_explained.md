# Подробное пояснение шагов алгоритмов и места реализации в коде

Ниже приведены два алгоритма из проекта «Кадровый учёт на малом предприятии» с подробным описанием каждого шага и указанием, где именно он реализован в кодовой базе.

---

## Алгоритм 1. Загрузка данных при старте программы

Точка входа: `src/main.cpp` — вызов `manager.loadAll()` в `main()`.

```12:25:src/main.cpp
std::string detectDataDirectory() {
    const std::vector<std::string> candidates = {
        "data", "../data", "../../data", "../../../data"
    };
    for (const auto& candidate : candidates) {
        std::ifstream test(candidate + "/employees.txt");
        if (test.good()) {
            return candidate;
        }
    }
    return "data";
}
```

```520:528:src/main.cpp
const std::string dataDir = detectDataDirectory();
DataManager manager(dataDir);
manager.loadAll();
```

### Шаги внутри `DataManager::loadAll()`
Файл: `src/DataManager.cpp`, метод `void DataManager::loadAll()`.

```51:68:src/DataManager.cpp
void DataManager::loadAll() {
    employees.clear();
    departments.clear();
    positions.clear();
    employeeIndex.clear();
    departmentIndex.clear();
    positionIndex.clear();
    undoStack.clear();

    nextEmployeeId = 1;
    nextDepartmentId = 1;
    nextPositionId = 1;

    loadPositions();
    loadDepartments();
    loadEmployees();
    loadHireDates();
}
```

- Очистка контейнеров и индексов: реализовано прямо в `loadAll()`.
- Сброс счётчиков `next*Id`: реализовано прямо в `loadAll()`.
- Последовательная загрузка сущностей: вызовы `loadPositions()`, `loadDepartments()`, `loadEmployees()`, `loadHireDates()`.

#### Загрузка должностей
Файл: `src/DataManager.cpp`, метод `void loadPositions()`.

```710:739:src/DataManager.cpp
void DataManager::loadPositions() {
    std::ifstream file(positionsPath);
    if (!file.is_open()) {
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') { continue; }
        const auto parts = split(line, ';');
        if (parts.size() < 3) { /* валидация */ continue; }
        const int id = std::stoi(parts[0]);
        const std::string name = parts[1];
        const int hours = std::stoi(parts[2]);
        positions.add(Position(id, name, hours));
        positionIndex[id] = positions.size() - 1;
        nextPositionId = std::max(nextPositionId, id + 1);
    }
}
```

#### Загрузка отделов
Файл: `src/DataManager.cpp`, метод `void loadDepartments()`.

```668:708:src/DataManager.cpp
void DataManager::loadDepartments() {
    std::ifstream file(departmentsPath);
    if (!file.is_open()) { return; }
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') { continue; }
        const auto parts = split(line, ';');
        if (parts.size() < 3) { continue; }
        const int id = std::stoi(parts[0]);
        const std::string name = parts[1];
        const int managerId = std::stoi(parts[2]);
        Department department(id, name, managerId);
        if (parts.size() >= 4 && !parts[3].empty()) {
            const auto employeeTokens = split(parts[3], ',');
            for (const auto& token : employeeTokens) {
                if (!token.empty()) { department.addEmployee(std::stoi(token)); }
            }
        }
        departments.add(department);
        departmentIndex[id] = departments.size() - 1;
        nextDepartmentId = std::max(nextDepartmentId, id + 1);
    }
}
```

#### Загрузка сотрудников
Файл: `src/DataManager.cpp`, метод `void loadEmployees()`.

```605:666:src/DataManager.cpp
void DataManager::loadEmployees() {
    std::ifstream file(employeesPath);
    if (!file.is_open()) { return; }
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') { continue; }
        const auto parts = split(line, ';');
        if (parts.size() < 6) { continue; }
        const int id = std::stoi(parts[0]);
        const std::string type = parts[1];
        const std::string firstName = parts[2];
        const std::string lastName = parts[3];
        const int departmentId = std::stoi(parts[4]);
        const double salary = std::stod(parts[5]);
        std::shared_ptr<Employee> employeePtr;
        if (type == "WORKER") {
            const int positionId = std::stoi(parts[6]);
            const double bonus = std::stod(parts[7]);
            employeePtr = std::make_shared<Worker>(id, firstName, lastName, departmentId, salary, positionId, bonus);
        } else if (type == "MANAGER") {
            std::vector<int> subordinates;
            if (parts.size() >= 7 && !parts[6].empty()) {
                const auto tokens = split(parts[6], ',');
                for (const auto& t : tokens) { subordinates.push_back(std::stoi(t)); }
            }
            employeePtr = std::make_shared<Manager>(id, firstName, lastName, departmentId, salary, subordinates);
        } else { continue; }
        EmployeeRecord record{employeePtr, HireDate(id, 1, 1, 2000, 9, 0)};
        employees.add(record);
        registerEmployee(record);
        ensureDepartmentContainsEmployee(departmentId, id);
        nextEmployeeId = std::max(nextEmployeeId, id + 1);
    }
}
```

#### Загрузка дат найма
Файл: `src/DataManager.cpp`, метод `void loadHireDates()`.

```741:775:src/DataManager.cpp
void DataManager::loadHireDates() {
    std::ifstream file(hiresPath);
    if (!file.is_open()) { return; }
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') { continue; }
        const auto parts = split(line, ';');
        if (parts.size() < 6) { continue; }
        const int employeeId = std::stoi(parts[0]);
        const int day = std::stoi(parts[1]);
        const int month = std::stoi(parts[2]);
        const int year = std::stoi(parts[3]);
        const int hour = std::stoi(parts[4]);
        const int minute = std::stoi(parts[5]);
        auto* record = findEmployeeRecord(employeeId);
        if (record) {
            record->hireDate = HireDate(employeeId, day, month, year, hour, minute);
        }
    }
}
```

---

## Алгоритм 2. Добавление нового работника

Точка входа из меню: `src/main.cpp` → `addEmployeeMenu()` вызывает `DataManager::addWorker` или `DataManager::addManager`.

```135:181:src/main.cpp
void addEmployeeMenu(DataManager& manager) {
    // считывание firstName, lastName, departmentId, salary
    // если Worker: positionId, bonus, hireDate → addWorker(...)
    // если Manager: subordinates, hireDate → addManager(...)
}
```

### Ветка «Работник» — `DataManager::addWorker`
Файл: `src/DataManager.cpp`.

```101:130:src/DataManager.cpp
int DataManager::addWorker(const std::string& firstName,
                           const std::string& lastName,
                           int departmentId,
                           double salary,
                           int positionId,
                           double bonus,
                           const HireDate& hireDate) {
    const int newId = generateEmployeeId();
    if (!findDepartment(departmentId)) { throw std::runtime_error("Указанный отдел не существует"); }
    if (!findPosition(positionId)) { throw std::runtime_error("Указанная должность не существует"); }
    auto worker = std::make_shared<Worker>(newId, firstName, lastName, departmentId, salary, positionId, bonus);
    EmployeeRecord record{worker, hireDate};
    record.hireDate.setEmployeeId(newId);
    employees.add(record);
    registerEmployee(record);
    ensureDepartmentContainsEmployee(departmentId, newId);
    if (!undoInProgress) {
        pushUndo([this, newId]() { removeEmployee(newId); }, "Отмена добавления сотрудника");
    }
    return newId;
}
```

- Генерация нового ID: `generateEmployeeId()` (см. ниже).
- Проверка существования отдела/должности: `findDepartment`, `findPosition`.
- Создание `Worker`, формирование `EmployeeRecord`, привязка к отделу.
- Регистрация в индексах: `registerEmployee`.
- Подготовка команды Undo: `pushUndo` c `removeEmployee(newId)`.

### Ветка «Менеджер» — `DataManager::addManager`
Файл: `src/DataManager.cpp`.

```132:163:src/DataManager.cpp
int DataManager::addManager(...){
    const int newId = generateEmployeeId();
    if (!findDepartment(departmentId)) { throw ... }
    std::vector<int> validSubordinates; // фильтрация существующих сотрудников
    auto manager = std::make_shared<Manager>(...);
    EmployeeRecord record{manager, hireDate};
    record.hireDate.setEmployeeId(newId);
    employees.add(record);
    registerEmployee(record);
    ensureDepartmentContainsEmployee(departmentId, newId);
    if (!undoInProgress) {
        pushUndo([this, newId]() { removeEmployee(newId); }, "Отмена добавления менеджера");
    }
    return newId;
}
```

### Вспомогательные функции и где они реализованы

- Генерация идентификаторов: `generateEmployeeId`, `generateDepartmentId`, `generatePositionId` — `src/DataManager.cpp`.

```967:977:src/DataManager.cpp
int DataManager::generateEmployeeId() { return nextEmployeeId++; }
int DataManager::generateDepartmentId() { return nextDepartmentId++; }
int DataManager::generatePositionId() { return nextPositionId++; }
```

- Поиск сущностей: `findEmployeeRecord`, `findDepartment`, `findPosition` — `src/DataManager.cpp`.

```889:903:src/DataManager.cpp
EmployeeRecord* DataManager::findEmployeeRecord(int employeeId);
const EmployeeRecord* DataManager::findEmployeeRecord(int employeeId) const;
```

```905:919:src/DataManager.cpp
Department* DataManager::findDepartment(int departmentId);
const Department* DataManager::findDepartment(int departmentId) const;
```

```921:935:src/DataManager.cpp
Position* DataManager::findPosition(int positionId);
const Position* DataManager::findPosition(int positionId) const;
```

- Регистрация сотрудника в индексах: `registerEmployee` — `src/DataManager.cpp`.

```937:941:src/DataManager.cpp
void DataManager::registerEmployee(const EmployeeRecord& record) {
    const std::size_t index = employees.size() - 1;
    employeeIndex[record.employee->getEmployeeId()] = index;
}
```

- Привязка сотрудника к отделу: `ensureDepartmentContainsEmployee` — `src/DataManager.cpp`.

```946:952:src/DataManager.cpp
void DataManager::ensureDepartmentContainsEmployee(int departmentId, int employeeId) {
    if (auto* department = findDepartment(departmentId)) {
        if (!department->hasEmployee(employeeId)) {
            department->addEmployee(employeeId);
        }
    }
}
```

- Механизм Undo: `pushUndo`, `undoLastAction`, `canUndo`, `getLastUndoDescription` — `src/DataManager.cpp`.

```960:965:src/DataManager.cpp
void DataManager::pushUndo(std::function<void()> undoAction, const std::string& description) {
    if (undoInProgress) { return; }
    undoStack.push_back({std::move(undoAction), description});
}
```

```581:596:src/DataManager.cpp
bool DataManager::canUndo() const { return !undoStack.empty(); }
bool DataManager::undoLastAction() { /* снимает команду, выполняет undo */ }
```

---

## Где хранятся пути к файлам данных

- В конструкторе `DataManager` (`src/DataManager.cpp`) формируются `employeesPath`, `departmentsPath`, `positionsPath`, `hiresPath`.

```44:49:src/DataManager.cpp
DataManager::DataManager(const std::string& dataDirectory) {
    employeesPath = combinePath(dataDirectory, "employees.txt");
    departmentsPath = combinePath(dataDirectory, "departments.txt");
    positionsPath = combinePath(dataDirectory, "positions.txt");
    hiresPath = combinePath(dataDirectory, "hires.txt");
}
```

## Где находятся определения сущностей

- `include/Worker.h` — класс `Worker` (должность, премия)
- `include/Manager.h` — класс `Manager` (список подчинённых)
- `include/HireDate.h` — класс `HireDate` (дата и время приёма)
- `include/DataManager.h` — интерфейс менеджера данных, объявления всех упомянутых методов

---

Этого файла достаточно, чтобы:
- сделать блок-схемы и подписать каждый шаг ссылкой на конкретный метод;
- быстро находить реализацию соответствующего шага в `src/DataManager.cpp` и `src/main.cpp`.


