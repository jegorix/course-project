#include "Employee.h"
#include <iostream>
#include <sstream>

Employee::Employee() : Person(), employeeId(0), departmentId(0), salary(0.0) {}

Employee::Employee(int employeeId, const std::string& firstName,
                   const std::string& lastName, int departmentId, double salary)
    : Person(firstName, lastName), employeeId(employeeId),
      departmentId(departmentId), salary(salary) {}

Employee::~Employee() {}

int Employee::getEmployeeId() const {
    return employeeId;
}

int Employee::getDepartmentId() const {
    return departmentId;
}

double Employee::getSalary() const {
    return salary;
}

void Employee::setEmployeeId(int employeeId) {
    this->employeeId = employeeId;
}

void Employee::setDepartmentId(int departmentId) {
    this->departmentId = departmentId;
}

void Employee::setSalary(double salary) {
    this->salary = salary;
}

void Employee::displayInfo() const {
    std::cout << "ID: " << employeeId << ", "
              << "Имя: " << getFullName() << ", "
              << "Отдел: " << departmentId << ", "
              << "Оклад: " << salary << std::endl;
}

double Employee::calculateTotalIncome() const {
    return salary;
}

