#include "Manager.h"
#include <iostream>
#include <algorithm>

Manager::Manager() : Employee(), subordinates() {}

Manager::Manager(int employeeId, const std::string& firstName,
                 const std::string& lastName, int departmentId,
                 double salary, const std::vector<int>& subordinates)
    : Employee(employeeId, firstName, lastName, departmentId, salary),
      subordinates(subordinates) {}

Manager::~Manager() {}

void Manager::addSubordinate(int subordinateId) {
    if (!hasSubordinate(subordinateId)) {
        subordinates.push_back(subordinateId);
    }
}

void Manager::removeSubordinate(int subordinateId) {
    subordinates.erase(
        std::remove(subordinates.begin(), subordinates.end(), subordinateId),
        subordinates.end()
    );
}

std::vector<int> Manager::getSubordinates() const {
    return subordinates;
}

int Manager::getSubordinatesCount() const {
    return static_cast<int>(subordinates.size());
}

bool Manager::hasSubordinate(int subordinateId) const {
    return std::find(subordinates.begin(), subordinates.end(), subordinateId) != subordinates.end();
}

void Manager::displayInfo() const {
    std::cout << "Руководитель - ID: " << getEmployeeId() << ", "
              << "Имя: " << getFullName() << ", "
              << "Отдел: " << getDepartmentId() << ", "
              << "Оклад: " << getSalary() << ", "
              << "Подчиненных: " << getSubordinatesCount() << std::endl;
}

double Manager::calculateTotalIncome() const {
    return getSalary();
}

