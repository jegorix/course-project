#include "FileManager.h"

#include "DataManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

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

void FileManager::loadAll(DataManager& manager) const {
    loadPositions(manager);
    loadDepartments(manager);
    loadEmployees(manager);
    loadHireDates(manager);
}

void FileManager::saveAll(const DataManager& manager) const {
    savePositions(manager);
    saveDepartments(manager);
    saveEmployees(manager);
    saveHireDates(manager);
}

void FileManager::loadPositions(DataManager& manager) const {
    std::ifstream file(manager.positionsPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 3) {
            throw DataFormatException("Некорректная строка в файле должностей: " + line);
        }

        try {
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            int hours = std::stoi(parts[2]);

            Position pos(id, name, hours);
            manager.positions.add(pos);
            manager.positionIndex[id] = manager.positions.size() - 1;
            manager.nextPositionId = std::max(manager.nextPositionId, id + 1);
        } catch (const std::exception&) {
            throw DataFormatException("Некорректная строка в файле должностей: " + line);
        }
    }
}

void FileManager::loadDepartments(DataManager& manager) const {
    std::ifstream file(manager.departmentsPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 3) {
            throw DataFormatException("Некорректная строка в файле отделов: " + line);
        }

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

            manager.departments.add(dept);
            manager.departmentIndex[id] = manager.departments.size() - 1;
            manager.nextDepartmentId = std::max(manager.nextDepartmentId, id + 1);
        } catch (const std::exception&) {
            throw DataFormatException("Некорректная строка в файле отделов: " + line);
        }
    }
}

void FileManager::loadEmployees(DataManager& manager) const {
    std::ifstream file(manager.employeesPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 6) {
            throw DataFormatException("Некорректная строка в файле сотрудников: " + line);
        }

        try {
            int id = std::stoi(parts[0]);
            std::string type = toLower(parts[1]);
            std::string firstName = parts[2];
            std::string lastName = parts[3];
            int departmentId = std::stoi(parts[4]);
            double salary = std::stod(parts[5]);

            std::shared_ptr<Employee> employee;
            int positionId = 0;
            std::string subordinatesStr;
            if (type == "manager") {
                if (parts.size() >= 8) {
                    if (!parts[6].empty()) {
                        positionId = std::stoi(parts[6]);
                    }
                    subordinatesStr = parts[7];
                } else if (parts.size() >= 7) {
                    subordinatesStr = parts[6];
                }

                std::vector<int> subordinates;
                if (!subordinatesStr.empty()) {
                    auto subIds = split(subordinatesStr, ',');
                    for (const auto& subIdStr : subIds) {
                        if (!subIdStr.empty()) {
                            subordinates.push_back(std::stoi(subIdStr));
                        }
                    }
                }
                employee = std::make_shared<Manager>(id, firstName, lastName, departmentId, salary, positionId, subordinates);
            } else if (type == "worker") {
                int positionId = (parts.size() >= 7) ? std::stoi(parts[6]) : 0;
                double bonus = (parts.size() >= 8) ? std::stod(parts[7]) : 0.0;
                employee = std::make_shared<Worker>(id, firstName, lastName, departmentId, salary, positionId, bonus);
            } else {
                continue;
            }

            HireDate hireDate; // Будет обновлено в loadHireDates
            EmployeeRecord record{employee, hireDate};
            manager.employees.add(record);
            manager.registerEmployee(record);
            manager.ensureDepartmentContainsEmployee(departmentId, id);
            manager.nextEmployeeId = std::max(manager.nextEmployeeId, id + 1);
        } catch (const std::exception&) {
            throw DataFormatException("Некорректная строка в файле сотрудников: " + line);
        }
    }
}

void FileManager::loadHireDates(DataManager& manager) const {
    std::ifstream file(manager.hiresPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto parts = split(line, ';');
        if (parts.size() < 6) {
            throw DataFormatException("Некорректная строка в файле дат найма: " + line);
        }

        try {
            int employeeId = std::stoi(parts[0]);
            int day = std::stoi(parts[1]);
            int month = std::stoi(parts[2]);
            int year = std::stoi(parts[3]);
            int hour = std::stoi(parts[4]);
            int minute = std::stoi(parts[5]);

            HireDate parsed(employeeId, day, month, year, hour, minute);
            ensureValidHireDate(parsed);

            EmployeeRecord* record = manager.findEmployeeRecord(employeeId);
            if (record) {
                record->hireDate = parsed;
            }
        } catch (const std::exception&) {
            throw DataFormatException("Некорректная строка в файле дат найма: " + line);
        }
    }
}

void FileManager::savePositions(const DataManager& manager) const {
    try {
        std::string dir = manager.positionsPath.substr(0, manager.positionsPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }

    std::ofstream file(manager.positionsPath);
    if (!file.is_open()) {
        throw FileAccessException("Не удалось открыть файл для сохранения должностей");
    }

    for (const auto& pos : manager.positions) {
        file << pos.getPositionId() << ";"
             << pos.getPositionName() << ";"
             << pos.getWorkHoursPerWeek() << "\n";
    }
}

void FileManager::saveDepartments(const DataManager& manager) const {
    try {
        std::string dir = manager.departmentsPath.substr(0, manager.departmentsPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }

    std::ofstream file(manager.departmentsPath);
    if (!file.is_open()) {
        throw FileAccessException("Не удалось открыть файл для сохранения отделов");
    }

    for (const auto& dept : manager.departments) {
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

void FileManager::saveEmployees(const DataManager& manager) const {
    try {
        std::string dir = manager.employeesPath.substr(0, manager.employeesPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }

    std::ofstream file(manager.employeesPath);
    if (!file.is_open()) {
        throw FileAccessException("Не удалось открыть файл для сохранения сотрудников");
    }

    for (const auto& record : manager.employees) {
        if (!record.employee) continue;

        file << record.employee->getEmployeeId() << ";";

        if (auto managerPtr = std::dynamic_pointer_cast<Manager>(record.employee)) {
            file << "MANAGER;" << managerPtr->getFirstName() << ";" << managerPtr->getLastName() << ";"
                 << managerPtr->getDepartmentId() << ";" << managerPtr->getSalary() << ";"
                 << managerPtr->getPositionId() << ";";
            auto subs = managerPtr->getSubordinates();
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

void FileManager::saveHireDates(const DataManager& manager) const {
    try {
        std::string dir = manager.hiresPath.substr(0, manager.hiresPath.find_last_of("/\\"));
        if (!dir.empty()) {
            fs::create_directories(dir);
        }
    } catch (...) {
        // Игнорируем ошибки создания директории
    }

    std::ofstream file(manager.hiresPath);
    if (!file.is_open()) {
        throw FileAccessException("Не удалось открыть файл для сохранения дат найма");
    }

    for (const auto& record : manager.employees) {
        if (!record.employee) continue;
        file << record.employee->getEmployeeId() << ";"
             << record.hireDate.getDay() << ";"
             << record.hireDate.getMonth() << ";"
             << record.hireDate.getYear() << ";"
             << record.hireDate.getHour() << ";"
             << record.hireDate.getMinute() << "\n";
    }
}
