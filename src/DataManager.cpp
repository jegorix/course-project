#include "DataManager.h"
#include "FileManager.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

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

    void ensureNotEmpty(const std::string& value, const std::string& fieldName) {
        if (trim(value).empty()) {
            throw ValidationException("Поле \"" + fieldName + "\" не может быть пустым");
        }
    }

    void ensurePositive(double value, const std::string& fieldName) {
        if (value <= 0.0) {
            throw ValidationException("Поле \"" + fieldName + "\" должно быть положительным");
        }
    }

    void ensureValidWorkHours(int hours) {
        if (hours <= 0 || hours > 168) {
            throw ValidationException("Количество часов в неделю должно быть в диапазоне 1-168");
        }
    }

    void ensureValidHireDate(const HireDate& date) {
        if (date.getMonth() < 1 || date.getMonth() > 12) {
            throw ValidationException("Месяц найма должен быть в диапазоне 1-12");
        }
        if (date.getDay() < 1 || date.getDay() > 31) {
            throw ValidationException("День найма должен быть в диапазоне 1-31");
        }
        if (date.getHour() < 0 || date.getHour() > 23 || date.getMinute() < 0 || date.getMinute() > 59) {
            throw ValidationException("Время найма указано неверно");
        }
    }
}

// Конструктор
DataManager::DataManager(const std::string& dataDirectory)
    : fileManager(std::make_unique<FileManager>()) {
    employeesPath = dataDirectory + "/employees.txt";
    departmentsPath = dataDirectory + "/departments.txt";
    positionsPath = dataDirectory + "/positions.txt";
    hiresPath = dataDirectory + "/hires.txt";
}

DataManager::~DataManager() = default;

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
        if (!fileManager) {
            fileManager = std::make_unique<FileManager>();
        }
        fileManager->loadAll(*this);
    } catch (const AppException&) {
        throw;
    } catch (const std::exception& e) {
        throw AppException("Не удалось загрузить данные: " + std::string(e.what()));
    }
}

// Сохранение всех данных
void DataManager::saveAll() const {
    try {
        if (!fileManager) {
            // const method, но fileManager должен существовать; если его нет, это логическая ошибка
            throw AppException("FileManager не инициализирован");
        }
        fileManager->saveAll(*this);
    } catch (const AppException&) {
        throw;
    } catch (const std::exception& e) {
        throw AppException("Не удалось сохранить данные: " + std::string(e.what()));
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
    ensureNotEmpty(firstName, "Имя");
    ensureNotEmpty(lastName, "Фамилия");
    ensurePositive(salary, "Зарплата");
    ensureValidHireDate(hireDate);
    if (bonus < 0.0) {
        throw ValidationException("Премия не может быть отрицательной");
    }

    if (!findDepartment(departmentId)) {
        throw EntityNotFoundException("Отдел", departmentId);
    }
    if (!findPosition(positionId)) {
        throw EntityNotFoundException("Должность", positionId);
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
                            int departmentId, double salary, int positionId,
                            const std::vector<int>& subordinateIds, const HireDate& hireDate) {
    ensureNotEmpty(firstName, "Имя");
    ensureNotEmpty(lastName, "Фамилия");
    ensurePositive(salary, "Зарплата");
    ensureValidHireDate(hireDate);

    if (!findDepartment(departmentId)) {
        throw EntityNotFoundException("Отдел", departmentId);
    }
    if (!findPosition(positionId)) {
        throw EntityNotFoundException("Должность", positionId);
    }
    for (int subordinateId : subordinateIds) {
        auto* subordinateRecord = findEmployeeRecord(subordinateId);
        if (!subordinateRecord || !subordinateRecord->employee) {
            throw EntityNotFoundException("Подчиненный", subordinateId);
        }
        if (!std::dynamic_pointer_cast<Worker>(subordinateRecord->employee)) {
            throw OperationNotAllowedException("Подчиненным может быть только работник (id=" + std::to_string(subordinateId) + ")");
        }
    }

    int id = generateEmployeeId();
    auto manager = std::make_shared<Manager>(id, firstName, lastName, departmentId, salary, positionId, subordinateIds);
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
        throw EntityNotFoundException("Сотрудник", employeeId);
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
    if (!record || !record->employee) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }
    if (!findDepartment(newDepartmentId)) {
        throw EntityNotFoundException("Отдел", newDepartmentId);
    }

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
    if (!record || !record->employee) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }
    if (newSalary <= 0) {
        throw ValidationException("Зарплата должна быть больше нуля");
    }

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
    if (!record || !record->employee) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }

    if (!findPosition(newPositionId)) {
        throw EntityNotFoundException("Должность", newPositionId);
    }

    int oldPositionId = 0;
    if (auto worker = std::dynamic_pointer_cast<Worker>(record->employee)) {
        oldPositionId = worker->getPositionId();
        worker->setPositionId(newPositionId);
    } else if (auto managerPtr = std::dynamic_pointer_cast<Manager>(record->employee)) {
        oldPositionId = managerPtr->getPositionId();
        managerPtr->setPositionId(newPositionId);
    } else {
        throw ValidationException("Неизвестный тип сотрудника");
    }

    if (!undoInProgress) {
        pushUndo([this, employeeId, oldPositionId]() {
            updateEmployeePosition(employeeId, oldPositionId);
        }, "Изменение должности");
    }

    return true;
}

bool DataManager::updateEmployeeBonus(int employeeId, double newBonus) {
    EmployeeRecord* record = findEmployeeRecord(employeeId);
    if (!record || !record->employee) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }

    auto worker = std::dynamic_pointer_cast<Worker>(record->employee);
    if (!worker) {
        throw ValidationException("Сотрудник не является работником");
    }
    if (newBonus < 0) {
        throw ValidationException("Премия не может быть отрицательной");
    }

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
    if (!record || !record->employee) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }
    ensureNotEmpty(newFirstName, "Имя");
    ensureNotEmpty(newLastName, "Фамилия");

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
    if (!record) {
        throw EntityNotFoundException("Сотрудник", employeeId);
    }
    ensureValidHireDate(newHireDate);

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
    ensureNotEmpty(name, "Название отдела");
    if (managerId != 0) {
        auto* managerRecord = findEmployeeRecord(managerId);
        if (!managerRecord || !managerRecord->employee) {
            throw EntityNotFoundException("Руководитель отдела", managerId);
        }
        if (!std::dynamic_pointer_cast<Manager>(managerRecord->employee)) {
            throw OperationNotAllowedException("Руководителем отдела может быть только сотрудник типа \"Руководитель\"");
        }
    }
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
    if (!dept) {
        throw EntityNotFoundException("Отдел", departmentId);
    }

    // Проверка наличия сотрудников
    bool hasEmployees = false;
    for (const auto& record : employees) {
        if (record.employee && record.employee->getDepartmentId() == departmentId) {
            hasEmployees = true;
            break;
        }
    }

    if (hasEmployees) {
        throw OperationNotAllowedException("Отдел содержит сотрудников и не может быть удален");
    }

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
    if (!dept) {
        throw EntityNotFoundException("Отдел", departmentId);
    }
    ensureNotEmpty(newName, "Название отдела");

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
    if (!dept) {
        throw EntityNotFoundException("Отдел", departmentId);
    }

    if (managerId != 0) {
        auto* managerRecord = findEmployeeRecord(managerId);
        if (!managerRecord || !managerRecord->employee) {
            throw EntityNotFoundException("Руководитель отдела", managerId);
        }
        if (!std::dynamic_pointer_cast<Manager>(managerRecord->employee)) {
            throw OperationNotAllowedException("Руководителем отдела может быть только сотрудник типа \"Руководитель\"");
        }
    }

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
    ensureNotEmpty(name, "Название должности");
    ensureValidWorkHours(hoursPerWeek);
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
    if (!pos) {
        throw EntityNotFoundException("Должность", positionId);
    }

    // Проверка использования
    for (const auto& record : employees) {
        if (record.employee) {
            if (auto worker = std::dynamic_pointer_cast<Worker>(record.employee)) {
                if (worker->getPositionId() == positionId) {
                    throw OperationNotAllowedException("Должность используется сотрудниками");
                }
            } else if (auto managerPtr = std::dynamic_pointer_cast<Manager>(record.employee)) {
                if (managerPtr->getPositionId() == positionId) {
                    throw OperationNotAllowedException("Должность используется сотрудниками");
                }
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
    if (!pos) {
        throw EntityNotFoundException("Должность", positionId);
    }
    ensureNotEmpty(newName, "Название должности");

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
    if (!pos) {
        throw EntityNotFoundException("Должность", positionId);
    }
    ensureValidWorkHours(hoursPerWeek);

    int oldHours = pos->getWorkHoursPerWeek();
    pos->setWorkHoursPerWeek(hoursPerWeek);

    if (!undoInProgress) {
        pushUndo([this, positionId, oldHours]() {
            updatePositionHours(positionId, oldHours);
        }, "Изменение часов работы");
    }

    return true;
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
