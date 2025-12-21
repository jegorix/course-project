#include "Manager.h"
#include <iostream>
#include <algorithm>

Manager::Manager() : Employee(), positionId(0), subordinates() {}

Manager::Manager(int employeeId, const std::string& firstName,
                 const std::string& lastName, int departmentId,
                 double salary, int positionId, const std::vector<int>& subordinates)
    : Employee(employeeId, firstName, lastName, departmentId, salary),
      positionId(positionId),
      subordinates(subordinates) {}

Manager::~Manager() {}

int Manager::getPositionId() const {
    return positionId;
}

void Manager::setPositionId(int positionId) {
    this->positionId = positionId;
}

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
              << "Должность: " << positionId << ", "
              << "Подчиненных: " << getSubordinatesCount() << std::endl;
}

double Manager::calculateTotalIncome() const {
    return getSalary();
}
