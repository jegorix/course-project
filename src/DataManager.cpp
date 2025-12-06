#include "DataManager.h"

#include <algorithm>  // std::sort
#include <cstddef>    // std::size_t, std::ptrdiff_t
#include <fstream>    // std::ifstream, std::ofstream
#include <functional> // std::function
#include <sstream>    // std::istringstream, std::ostringstream
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <utility>    // std::move
#include <vector>     // std::vector
#include <cctype>     // std::isspace
#include <iomanip>    // std::setfill, std::setw
#include <sys/stat.h> // mkdir
#include <filesystem> // std::filesystem (C++17)
namespace fs = std::filesystem;

// Реализация двух ключевых алгоритмов:
// 1) Формирование рейтинга отделов по эффективности затрат.
// 2) Вычисление средней заработной платы по отделу.

// Новый алгоритм: формирование рейтинга отделов по эффективности затрат.
// Эффективность определяется как отношение количества сотрудников отдела
// к суммарным затратам на их заработную плату.
//
// Чем выше коэффициент, тем более «эффективно» используются затраты на персонал.
//
// Предполагается, что DataManager предоставляет доступ к коллекциям:
//   - departments  — контейнер всех отделов;
//   - employees    — контейнер всех сотрудников, у каждого есть departmentId и salary.
//
// Результат — вектор пар (идентификатор отдела, коэффициент эффективности),
// отсортированный по убыванию эффективности.
std::vector<std::pair<int, double>> DataManager::buildDepartmentsEfficiencyRating() const
{
    std::vector<std::pair<int, double>> rating;

    // Шаг 1. Обход всех отделов
    for (const auto& dept : departments) {
        int departmentId = dept.getDepartmentId();

        double totalSalary = 0.0;
        std::size_t count  = 0;

        // Шаг 2. Сбор информации о сотрудниках отдела
        for (const auto& record : employees) {
            if (!record.employee) {
                continue;
            }
            if (record.employee->getDepartmentId() != departmentId) {
                continue;
            }

            totalSalary += record.employee->getSalary();
            ++count;
        }

        // Шаг 3. Расчёт коэффициента эффективности
        double efficiency = 0.0;
        if (count > 0 && totalSalary > 0.0) {
            efficiency = static_cast<double>(count) / totalSalary;
        }

        rating.emplace_back(departmentId, efficiency);
    }

    // Шаг 4. Сортировка отделов по убыванию эффективности затрат
    std::sort(
        rating.begin(),
        rating.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second > rhs.second;
        }
    );

    return rating;
}

double DataManager::calculateAverageSalaryForDepartment(int departmentId) const
{
    // Шаг 2. Проверить существование отдела
    const Department* department = findDepartment(departmentId);
    if (!department) {
        return 0.0; // отдела нет — нет данных
    }

    // Шаг 3. Инициализация суммирующих переменных
    double sumSalary = 0.0;
    std::size_t count = 0;

    // Шаг 4–5. Цикл по всем сотрудникам
    for (const auto& record : employees) {
        if (!record.employee) {
            continue;
        }
        if (record.employee->getDepartmentId() != departmentId) {
            continue;
        }

        double salary = record.employee->getSalary();
        sumSalary += salary;
        ++count;
    }

    // Шаг 6. Если сотрудников нет — вернуть 0.0
    if (count == 0) {
        return 0.0;
    }

    // Шаг 7–8. Вычислить и вернуть среднюю зарплату
    return sumSalary / static_cast<double>(count);
}

// Простая реализация регистрации команды отмены
void DataManager::pushUndo(std::function<void()> undoAction,
                           const std::string& description)
{
    undoStack.push_back(UndoCommand{std::move(undoAction), description});
}

// Вспомогательные функции для работы со строками
namespace {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        return tokens;
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

// Конструктор
DataManager::DataManager(const std::string& dataDirectory) {
    employeesPath = dataDirectory + "/employees.txt";
    departmentsPath = dataDirectory + "/departments.txt";
    positionsPath = dataDirectory + "/positions.txt";
    hiresPath = dataDirectory + "/hires.txt";
}

// Загрузка всех данных
void DataManager::loadAll() {
    employees.clear();
    departments.clear();
    positions.clear();
    employeeIndex.clear();
    departmentIndex.clear();
    positionIndex.clear();
    undoStack.clear();
    undoInProgress = false;

    nextEmployeeId = 1;
    nextDepartmentId = 1;
    nextPositionId = 1;

    try {
        loadPositions();
        loadDepartments();
        loadEmployees();
        loadHireDates();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка загрузки данных: " + std::string(e.what()));
    }
}

// Сохранение всех данных
void DataManager::saveAll() const {
    try {
        savePositions();
        saveDepartments();
        saveEmployees();
        saveHireDates();
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка сохранения данных: " + std::string(e.what()));
    }
}

// Получение коллекций
const RecordCollection<EmployeeRecord>& DataManager::getEmployees() const {
    return employees;
}

RecordCollection<EmployeeRecord>& DataManager::getEmployees() {
    return employees;
}

const RecordCollection<Department>& DataManager::getDepartments() const {
    return departments;
}

RecordCollection<Department>& DataManager::getDepartments() {
    return departments;
}

const RecordCollection<Position>& DataManager::getPositions() const {
    return positions;
}

RecordCollection<Position>& DataManager::getPositions() {
    return positions;
}

// Загрузка должностей
void DataManager::loadPositions() {
    std::ifstream file(positionsPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 3) continue;

        try {
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            int hours = std::stoi(parts[2]);

            Position pos(id, name, hours);
            positions.add(pos);
            positionIndex[id] = positions.size() - 1;
            nextPositionId = std::max(nextPositionId, id + 1);
        } catch (...) {
            continue;
        }
    }
}

// Загрузка отделов
void DataManager::loadDepartments() {
    std::ifstream file(departmentsPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 3) continue;

        try {
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            int managerId = std::stoi(parts[2]);

            Department dept(id, name, managerId);
            if (parts.size() >= 4 && !parts[3].empty()) {
                auto employeeIds = split(parts[3], ',');
                for (const auto& empIdStr : employeeIds) {
                    if (!empIdStr.empty()) {
                        dept.addEmployee(std::stoi(empIdStr));
                    }
                }
            }

            departments.add(dept);
            departmentIndex[id] = departments.size() - 1;
            nextDepartmentId = std::max(nextDepartmentId, id + 1);
        } catch (...) {
            continue;
        }
    }
}

// Загрузка сотрудников
void DataManager::loadEmployees() {
    std::ifstream file(employeesPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 6) continue;

        try {
            int id = std::stoi(parts[0]);
            std::string type = toLower(parts[1]);
            std::string firstName = parts[2];
            std::string lastName = parts[3];
            int departmentId = std::stoi(parts[4]);
            double salary = std::stod(parts[5]);

            std::shared_ptr<Employee> employee;
            if (type == "manager") {
                std::vector<int> subordinates;
                if (parts.size() >= 7 && !parts[6].empty()) {
                    auto subIds = split(parts[6], ',');
                    for (const auto& subIdStr : subIds) {
                        if (!subIdStr.empty()) {
                            subordinates.push_back(std::stoi(subIdStr));
                        }
                    }
                }
                employee = std::make_shared<Manager>(id, firstName, lastName, departmentId, salary, subordinates);
            } else if (type == "worker") {
                int positionId = (parts.size() >= 7) ? std::stoi(parts[6]) : 0;
                double bonus = (parts.size() >= 8) ? std::stod(parts[7]) : 0.0;
                employee = std::make_shared<Worker>(id, firstName, lastName, departmentId, salary, positionId, bonus);
            } else {
                continue;
            }

            HireDate hireDate; // Будет обновлено в loadHireDates
            EmployeeRecord record{employee, hireDate};
            employees.add(record);
            registerEmployee(record);
            ensureDepartmentContainsEmployee(departmentId, id);
            nextEmployeeId = std::max(nextEmployeeId, id + 1);
        } catch (...) {
            continue;
        }
    }
}

// Загрузка дат найма
void DataManager::loadHireDates() {
    std::ifstream file(hiresPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 6) continue;

        try {
            int employeeId = std::stoi(parts[0]);
            int day = std::stoi(parts[1]);
            int month = std::stoi(parts[2]);
            int year = std::stoi(parts[3]);
            int hour = std::stoi(parts[4]);
            int minute = std::stoi(parts[5]);

            EmployeeRecord* record = findEmployeeRecord(employeeId);
            if (record) {
                record->hireDate = HireDate(employeeId, day, month, year, hour, minute);
            }
        } catch (...) {
            continue;
        }
    }
}

// Сохранение должностей
void DataManager::savePositions() const {
    // Создать директорию если её нет
    try {
        std::string dir = positionsPath.substr(0, positionsPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }
    
    std::ofstream file(positionsPath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для сохранения должностей");
    }

    for (const auto& pos : positions) {
        file << pos.getPositionId() << ";"
             << pos.getPositionName() << ";"
             << pos.getWorkHoursPerWeek() << "\n";
    }
}

// Сохранение отделов
void DataManager::saveDepartments() const {
    // Создать директорию если её нет
    try {
        std::string dir = departmentsPath.substr(0, departmentsPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }
    
    std::ofstream file(departmentsPath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для сохранения отделов");
    }

    for (const auto& dept : departments) {
        file << dept.getDepartmentId() << ";"
             << dept.getDepartmentName() << ";"
             << dept.getManagerId() << ";";

        auto empIds = dept.getEmployees();
        for (size_t i = 0; i < empIds.size(); ++i) {
            if (i > 0) file << ",";
            file << empIds[i];
        }
        file << "\n";
    }
}

// Сохранение сотрудников
void DataManager::saveEmployees() const {
    // Создать директорию если её нет
    try {
        std::string dir = employeesPath.substr(0, employeesPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }
    
    std::ofstream file(employeesPath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для сохранения сотрудников");
    }

    for (const auto& record : employees) {
        if (!record.employee) continue;

        file << record.employee->getEmployeeId() << ";";

        if (auto manager = std::dynamic_pointer_cast<Manager>(record.employee)) {
            file << "MANAGER;" << manager->getFirstName() << ";" << manager->getLastName() << ";"
                 << manager->getDepartmentId() << ";" << manager->getSalary() << ";";
            auto subs = manager->getSubordinates();
            for (size_t i = 0; i < subs.size(); ++i) {
                if (i > 0) file << ",";
                file << subs[i];
            }
        } else if (auto worker = std::dynamic_pointer_cast<Worker>(record.employee)) {
            file << "WORKER;" << worker->getFirstName() << ";" << worker->getLastName() << ";"
                 << worker->getDepartmentId() << ";" << worker->getSalary() << ";"
                 << worker->getPositionId() << ";" << worker->getBonus();
        }
        file << "\n";
    }
}

// Сохранение дат найма
void DataManager::saveHireDates() const {
    // Создать директорию если её нет
    try {
        std::string dir = hiresPath.substr(0, hiresPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }
    
    std::ofstream file(hiresPath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для сохранения дат найма");
    }

    for (const auto& record : employees) {
        if (!record.employee) continue;
        file << record.employee->getEmployeeId() << ";"
             << record.hireDate.getDay() << ";"
             << record.hireDate.getMonth() << ";"
             << record.hireDate.getYear() << ";"
             << record.hireDate.getHour() << ";"
             << record.hireDate.getMinute() << "\n";
    }
}

// Генерация ID
int DataManager::generateEmployeeId() {
    return nextEmployeeId++;
}

int DataManager::generateDepartmentId() {
    return nextDepartmentId++;
}

int DataManager::generatePositionId() {
    return nextPositionId++;
}

// Поиск записей
EmployeeRecord* DataManager::findEmployeeRecord(int employeeId) {
    auto it = employeeIndex.find(employeeId);
    if (it == employeeIndex.end()) return nullptr;
    if (it->second >= employees.size()) return nullptr;
    return &employees[it->second];
}

const EmployeeRecord* DataManager::findEmployeeRecord(int employeeId) const {
    auto it = employeeIndex.find(employeeId);
    if (it == employeeIndex.end()) return nullptr;
    if (it->second >= employees.size()) return nullptr;
    return &employees[it->second];
}

Department* DataManager::findDepartment(int departmentId) {
    auto it = departmentIndex.find(departmentId);
    if (it == departmentIndex.end()) return nullptr;
    if (it->second >= departments.size()) return nullptr;
    return &departments[it->second];
}

const Department* DataManager::findDepartment(int departmentId) const {
    auto it = departmentIndex.find(departmentId);
    if (it == departmentIndex.end()) return nullptr;
    if (it->second >= departments.size()) return nullptr;
    return &departments[it->second];
}

Position* DataManager::findPosition(int positionId) {
    auto it = positionIndex.find(positionId);
    if (it == positionIndex.end()) return nullptr;
    if (it->second >= positions.size()) return nullptr;
    return &positions[it->second];
}

const Position* DataManager::findPosition(int positionId) const {
    auto it = positionIndex.find(positionId);
    if (it == positionIndex.end()) return nullptr;
    if (it->second >= positions.size()) return nullptr;
    return &positions[it->second];
}

// Регистрация сотрудников
void DataManager::registerEmployee(const EmployeeRecord& record) {
    if (!record.employee) return;
    int id = record.employee->getEmployeeId();
    employeeIndex[id] = employees.size() - 1;
}

void DataManager::unregisterEmployee(int employeeId) {
    employeeIndex.erase(employeeId);
    // Пересчет индексов
    for (size_t i = 0; i < employees.size(); ++i) {
        if (employees[i].employee) {
            employeeIndex[employees[i].employee->getEmployeeId()] = i;
        }
    }
}

// Работа с отделами
void DataManager::ensureDepartmentContainsEmployee(int departmentId, int employeeId) {
    Department* dept = findDepartment(departmentId);
    if (dept && !dept->hasEmployee(employeeId)) {
        dept->addEmployee(employeeId);
    }
}

void DataManager::removeEmployeeFromDepartment(int departmentId, int employeeId) {
    Department* dept = findDepartment(departmentId);
    if (dept) {
        dept->removeEmployee(employeeId);
    }
}

// Клонирование сотрудника
std::shared_ptr<Employee> DataManager::cloneEmployee(const Employee& source) const {
    if (auto manager = dynamic_cast<const Manager*>(&source)) {
        return std::make_shared<Manager>(*manager);
    } else if (auto worker = dynamic_cast<const Worker*>(&source)) {
        return std::make_shared<Worker>(*worker);
    }
    return std::make_shared<Employee>(source);
}

// Операции над сотрудниками
int DataManager::addWorker(const std::string& firstName, const std::string& lastName,
                           int departmentId, double salary, int positionId,
                           double bonus, const HireDate& hireDate) {
    if (!findDepartment(departmentId)) {
        throw std::runtime_error("Отдел не найден");
    }
    if (!findPosition(positionId)) {
        throw std::runtime_error("Должность не найдена");
    }

    int id = generateEmployeeId();
    auto worker = std::make_shared<Worker>(id, firstName, lastName, departmentId, salary, positionId, bonus);
    EmployeeRecord record{worker, hireDate};
    employees.add(record);
    registerEmployee(record);
    ensureDepartmentContainsEmployee(departmentId, id);

    if (!undoInProgress) {
        pushUndo([this, id]() {
            removeEmployee(id);
        }, "Добавление работника");
    }

    return id;
}

int DataManager::addManager(const std::string& firstName, const std::string& lastName,
                            int departmentId, double salary,
                            const std::vector<int>& subordinateIds, const HireDate& hireDate) {
    if (!findDepartment(departmentId)) {
        throw std::runtime_error("Отдел не найден");
    }

    int id = generateEmployeeId();
    auto manager = std::make_shared<Manager>(id, firstName, lastName, departmentId, salary, subordinateIds);
    EmployeeRecord record{manager, hireDate};
    employees.add(record);
    registerEmployee(record);
    ensureDepartmentContainsEmployee(departmentId, id);

    if (!undoInProgress) {
        pushUndo([this, id]() {
            removeEmployee(id);
        }, "Добавление руководителя");
    }

    return id;
}

bool DataManager::removeEmployee(int employeeId) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) {
        return false;
    }

    int deptId = record->employee->getDepartmentId();
    auto employeeCopy = cloneEmployee(*record->employee);
    HireDate hireDateCopy = record->hireDate;

    // Удаление из отдела
    removeEmployeeFromDepartment(deptId, employeeId);

    // Удаление из контейнера
    size_t index = employeeIndex[employeeId];
    employees.data().erase(employees.data().begin() + static_cast<ptrdiff_t>(index));
    unregisterEmployee(employeeId);

    if (!undoInProgress) {
        pushUndo([this, employeeCopy, hireDateCopy, deptId]() {
            EmployeeRecord restored{employeeCopy, hireDateCopy};
            employees.add(restored);
            registerEmployee(restored);
            ensureDepartmentContainsEmployee(deptId, employeeCopy->getEmployeeId());
        }, "Удаление сотрудника");
    }

    return true;
}

bool DataManager::updateEmployeeDepartment(int employeeId, int newDepartmentId) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) return false;
    if (!findDepartment(newDepartmentId)) return false;

    int oldDeptId = record->employee->getDepartmentId();
    record->employee->setDepartmentId(newDepartmentId);
    removeEmployeeFromDepartment(oldDeptId, employeeId);
    ensureDepartmentContainsEmployee(newDepartmentId, employeeId);

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldDeptId]() {
            updateEmployeeDepartment(employeeId, oldDeptId);
        }, "Изменение отдела сотрудника");
    }

    return true;
}

bool DataManager::updateEmployeeSalary(int employeeId, double newSalary) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) return false;

    double oldSalary = record->employee->getSalary();
    record->employee->setSalary(newSalary);

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldSalary]() {
            updateEmployeeSalary(employeeId, oldSalary);
        }, "Изменение зарплаты");
    }

    return true;
}

bool DataManager::updateEmployeePosition(int employeeId, int newPositionId) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) return false;

    auto worker = std::dynamic_pointer_cast<Worker>(record->employee);
    if (!worker) return false;
    if (!findPosition(newPositionId)) return false;

    int oldPositionId = worker->getPositionId();
    worker->setPositionId(newPositionId);

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldPositionId]() {
            updateEmployeePosition(employeeId, oldPositionId);
        }, "Изменение должности");
    }

    return true;
}

bool DataManager::updateEmployeeBonus(int employeeId, double newBonus) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) return false;

    auto worker = std::dynamic_pointer_cast<Worker>(record->employee);
    if (!worker) return false;

    double oldBonus = worker->getBonus();
    worker->setBonus(newBonus);

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldBonus]() {
            updateEmployeeBonus(employeeId, oldBonus);
        }, "Изменение премии");
    }

    return true;
}

bool DataManager::updateEmployeeName(int employeeId, const std::string& newFirstName, const std::string& newLastName) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) return false;

    std::string oldFirstName = record->employee->getFirstName();
    std::string oldLastName = record->employee->getLastName();
    record->employee->setFirstName(newFirstName);
    record->employee->setLastName(newLastName);

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldFirstName, oldLastName]() {
            updateEmployeeName(employeeId, oldFirstName, oldLastName);
        }, "Изменение имени сотрудника");
    }

    return true;
}

bool DataManager::updateEmployeeHireDate(int employeeId, const HireDate& newHireDate) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record) return false;

    HireDate oldHireDate = record->hireDate;
    record->hireDate = newHireDate;

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldHireDate]() {
            updateEmployeeHireDate(employeeId, oldHireDate);
        }, "Изменение даты найма");
    }

    return true;
}

// Операции над отделами
int DataManager::addDepartment(const std::string& name, int managerId) {
    int id = generateDepartmentId();
    Department dept(id, name, managerId);
    departments.add(dept);
    departmentIndex[id] = departments.size() - 1;

    if (!undoInProgress) {
        pushUndo([this, id]() {
            removeDepartment(id);
        }, "Добавление отдела");
    }

    return id;
}

bool DataManager::removeDepartment(int departmentId) {
    Department* dept = findDepartment(departmentId);
    if (!dept) return false;

    // Проверка наличия сотрудников
    bool hasEmployees = false;
    for (const auto& record : employees) {
        if (record.employee && record.employee->getDepartmentId() == departmentId) {
            hasEmployees = true;
            break;
        }
    }

    if (hasEmployees) return false;

    Department removedDept = *dept;
    size_t index = departmentIndex[departmentId];
    departments.data().erase(departments.data().begin() + static_cast<ptrdiff_t>(index));
    departmentIndex.erase(departmentId);

    // Корректировка индексов
    for (auto& pair : departmentIndex) {
        if (pair.second > index) {
            --pair.second;
        }
    }

    if (!undoInProgress) {
        pushUndo([this, removedDept]() {
            departments.add(removedDept);
            size_t newIndex = departments.size() - 1;
            departmentIndex[removedDept.getDepartmentId()] = newIndex;
        }, "Удаление отдела");
    }

    return true;
}

bool DataManager::updateDepartmentName(int departmentId, const std::string& newName) {
    Department* dept = findDepartment(departmentId);
    if (!dept) return false;

    std::string oldName = dept->getDepartmentName();
    dept->setDepartmentName(newName);

    if (!undoInProgress) {
        pushUndo([this, departmentId, oldName]() {
            updateDepartmentName(departmentId, oldName);
        }, "Изменение названия отдела");
    }

    return true;
}

bool DataManager::updateDepartmentManager(int departmentId, int managerId) {
    Department* dept = findDepartment(departmentId);
    if (!dept) return false;

    int oldManagerId = dept->getManagerId();
    dept->setManagerId(managerId);

    if (!undoInProgress) {
        pushUndo([this, departmentId, oldManagerId]() {
            updateDepartmentManager(departmentId, oldManagerId);
        }, "Изменение руководителя отдела");
    }

    return true;
}

// Операции над должностями
int DataManager::addPosition(const std::string& name, int hoursPerWeek) {
    int id = generatePositionId();
    Position pos(id, name, hoursPerWeek);
    positions.add(pos);
    positionIndex[id] = positions.size() - 1;

    if (!undoInProgress) {
        pushUndo([this, id]() {
            removePosition(id);
        }, "Добавление должности");
    }

    return id;
}

bool DataManager::removePosition(int positionId) {
    Position* pos = findPosition(positionId);
    if (!pos) return false;

    // Проверка использования
    for (const auto& record : employees) {
        if (record.employee) {
            auto worker = std::dynamic_pointer_cast<Worker>(record.employee);
            if (worker && worker->getPositionId() == positionId) {
                return false; // Должность используется
            }
        }
    }

    Position removedPos = *pos;
    size_t index = positionIndex[positionId];
    positions.data().erase(positions.data().begin() + static_cast<ptrdiff_t>(index));
    positionIndex.erase(positionId);

    // Корректировка индексов
    for (auto& pair : positionIndex) {
        if (pair.second > index) {
            --pair.second;
        }
    }

    if (!undoInProgress) {
        pushUndo([this, removedPos]() {
            positions.add(removedPos);
            size_t newIndex = positions.size() - 1;
            positionIndex[removedPos.getPositionId()] = newIndex;
        }, "Удаление должности");
    }

    return true;
}

bool DataManager::updatePositionName(int positionId, const std::string& newName) {
    Position* pos = findPosition(positionId);
    if (!pos) return false;

    std::string oldName = pos->getPositionName();
    pos->setPositionName(newName);

    if (!undoInProgress) {
        pushUndo([this, positionId, oldName]() {
            updatePositionName(positionId, oldName);
        }, "Изменение названия должности");
    }

    return true;
}

bool DataManager::updatePositionHours(int positionId, int hoursPerWeek) {
    Position* pos = findPosition(positionId);
    if (!pos) return false;

    int oldHours = pos->getWorkHoursPerWeek();
    pos->setWorkHoursPerWeek(hoursPerWeek);

    if (!undoInProgress) {
        pushUndo([this, positionId, oldHours]() {
            updatePositionHours(positionId, oldHours);
        }, "Изменение часов работы");
    }

    return true;
}

// Поиск сотрудников
std::vector<EmployeeRecord> DataManager::findEmployees(const EmployeeSearchFilter& filter) const {
    std::vector<EmployeeRecord> result;

    for (const auto& record : employees) {
        if (!record.employee) continue;

        // Фильтр по отделу
        if (filter.useDepartmentId) {
            if (record.employee->getDepartmentId() != filter.departmentId) continue;
        }

        // Фильтр по зарплате
        if (filter.useMinSalary) {
            if (record.employee->getSalary() < filter.minSalary) continue;
        }
        if (filter.useMaxSalary) {
            if (record.employee->getSalary() > filter.maxSalary) continue;
        }

        // Фильтр по должности
        if (filter.usePositionId) {
            auto worker = std::dynamic_pointer_cast<const Worker>(record.employee);
            if (!worker || worker->getPositionId() != filter.positionId) continue;
        }

        // Фильтр по имени
        if (filter.useNameFragment) {
            std::string fullName = toLower(record.employee->getFullName());
            std::string fragment = toLower(filter.nameFragment);
            if (fullName.find(fragment) == std::string::npos) continue;
        }

        result.push_back(record);
    }

    return result;
}

// Сортировка сотрудников
std::vector<EmployeeRecord> DataManager::sortEmployees(EmployeeSortKey key, bool descending) const {
    std::vector<EmployeeRecord> result;
    for (const auto& record : employees) {
        if (record.employee) {
            result.push_back(record);
        }
    }

    std::sort(result.begin(), result.end(), [key, descending](const EmployeeRecord& a, const EmployeeRecord& b) {
        if (!a.employee || !b.employee) return false;

        int cmp = 0;
        switch (key) {
            case EmployeeSortKey::ById:
                cmp = (a.employee->getEmployeeId() < b.employee->getEmployeeId()) ? -1 : 1;
                break;
            case EmployeeSortKey::ByDepartment:
                cmp = (a.employee->getDepartmentId() < b.employee->getDepartmentId()) ? -1 : 1;
                break;
            case EmployeeSortKey::BySalary:
                cmp = (a.employee->getSalary() < b.employee->getSalary()) ? -1 : 1;
                break;
            case EmployeeSortKey::ByLastName:
                cmp = a.employee->getLastName().compare(b.employee->getLastName());
                break;
            case EmployeeSortKey::ByHireDate:
                if (a.hireDate.getYear() != b.hireDate.getYear()) {
                    cmp = (a.hireDate.getYear() < b.hireDate.getYear()) ? -1 : 1;
                } else if (a.hireDate.getMonth() != b.hireDate.getMonth()) {
                    cmp = (a.hireDate.getMonth() < b.hireDate.getMonth()) ? -1 : 1;
                } else if (a.hireDate.getDay() != b.hireDate.getDay()) {
                    cmp = (a.hireDate.getDay() < b.hireDate.getDay()) ? -1 : 1;
                }
                break;
        }
        return descending ? (cmp > 0) : (cmp < 0);
    });

    return result;
}

// Отмена действий
bool DataManager::canUndo() const {
    return !undoStack.empty();
}

bool DataManager::undoLastAction() {
    if (undoStack.empty()) return false;

    UndoCommand cmd = undoStack.back();
    undoStack.pop_back();

    undoInProgress = true;
    try {
        cmd.undo();
    } catch (...) {
        undoInProgress = false;
        throw;
    }
    undoInProgress = false;

    return true;
}

std::string DataManager::getLastUndoDescription() const {
    if (undoStack.empty()) return "";
    return undoStack.back().description;
}

void DataManager::restoreEmployeeFromCopy(int employeeId, std::shared_ptr<Employee> employeeCopy, const HireDate& hireDateCopy) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record) return;
    
    // Сохраняем старый отдел
    int oldDeptId = record->employee->getDepartmentId();
    
    // Восстанавливаем сотрудника из копии
    record->employee = employeeCopy;
    record->hireDate = hireDateCopy;
    
    // Обновляем отдел, если он изменился
    int newDeptId = employeeCopy->getDepartmentId();
    if (oldDeptId != newDeptId) {
        removeEmployeeFromDepartment(oldDeptId, employeeId);
        ensureDepartmentContainsEmployee(newDeptId, employeeId);
    }
    
    // Восстанавливаем подчиненных для менеджера
    if (auto oldManager = std::dynamic_pointer_cast<Manager>(employeeCopy)) {
        if (auto currentManager = std::dynamic_pointer_cast<Manager>(record->employee)) {
            // Очищаем текущих подчиненных
            auto currentSubs = currentManager->getSubordinates();
            for (int subId : currentSubs) {
                currentManager->removeSubordinate(subId);
            }
            // Восстанавливаем старых подчиненных
            auto oldSubs = oldManager->getSubordinates();
            for (int subId : oldSubs) {
                currentManager->addSubordinate(subId);
            }
        }
    }
}

// Сброс данных
void DataManager::clearAllEmployees() {
    if (undoInProgress) return;
    
    // Сохранить копии всех записей для отмены
    std::vector<EmployeeRecord> backup;
    for (const auto& record : employees) {
        if (record.employee) {
            backup.push_back(record);
        }
    }
    
    employees.clear();
    employeeIndex.clear();
    nextEmployeeId = 1;
    
    // Очистить сотрудников из всех отделов
    for (auto& dept : departments.data()) {
        dept.clearEmployees();
    }
    
    if (!backup.empty()) {
        pushUndo([this, backup]() {
            for (const auto& record : backup) {
                if (record.employee) {
                    employees.add(record);
                    registerEmployee(record);
                    ensureDepartmentContainsEmployee(record.employee->getDepartmentId(), record.employee->getEmployeeId());
                    nextEmployeeId = std::max(nextEmployeeId, record.employee->getEmployeeId() + 1);
                }
            }
        }, "Сброс всех сотрудников");
    }
}

void DataManager::clearAllDepartments() {
    if (undoInProgress) return;
    
    // Сохранить копии всех отделов для отмены
    std::vector<Department> backup;
    for (const auto& dept : departments) {
        backup.push_back(dept);
    }
    
    departments.clear();
    departmentIndex.clear();
    nextDepartmentId = 1;
    
    if (!backup.empty()) {
        pushUndo([this, backup]() {
            for (const auto& dept : backup) {
                departments.add(dept);
                departmentIndex[dept.getDepartmentId()] = departments.size() - 1;
                nextDepartmentId = std::max(nextDepartmentId, dept.getDepartmentId() + 1);
            }
        }, "Сброс всех отделов");
    }
}

void DataManager::clearAllPositions() {
    if (undoInProgress) return;
    
    // Сохранить копии всех должностей для отмены
    std::vector<Position> backup;
    for (const auto& pos : positions) {
        backup.push_back(pos);
    }
    
    positions.clear();
    positionIndex.clear();
    nextPositionId = 1;
    
    if (!backup.empty()) {
        pushUndo([this, backup]() {
            for (const auto& pos : backup) {
                positions.add(pos);
                positionIndex[pos.getPositionId()] = positions.size() - 1;
                nextPositionId = std::max(nextPositionId, pos.getPositionId() + 1);
            }
        }, "Сброс всех должностей");
    }
}



