#include "Worker.h"
#include <iostream>

Worker::Worker() : Employee(), positionId(0), bonus(0.0) {}

Worker::Worker(int employeeId, const std::string& firstName,
               const std::string& lastName, int departmentId,
               double salary, int positionId, double bonus)
    : Employee(employeeId, firstName, lastName, departmentId, salary),
      positionId(positionId), bonus(bonus) {}

Worker::~Worker() {}

int Worker::getPositionId() const {
    return positionId;
}

double Worker::getBonus() const {
    return bonus;
}

void Worker::setPositionId(int positionId) {
    this->positionId = positionId;
}

void Worker::setBonus(double bonus) {
    this->bonus = bonus;
}

void Worker::displayInfo() const {
    std::cout << "Работник - ID: " << getEmployeeId() << ", "
              << "Имя: " << getFullName() << ", "
              << "Отдел: " << getDepartmentId() << ", "
              << "Оклад: " << getSalary() << ", "
              << "Должность: " << positionId << ", "
              << "Премия: " << bonus << std::endl;
}

double Worker::calculateTotalIncome() const {
    return getSalary() + bonus;
}

