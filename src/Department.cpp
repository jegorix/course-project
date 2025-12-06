#include "Department.h"
#include <iostream>
#include <algorithm>

Department::Department() : departmentId(0), departmentName(""), managerId(0), employees() {}

Department::Department(int departmentId, const std::string& departmentName, int managerId)
    : departmentId(departmentId), departmentName(departmentName),
      managerId(managerId), employees() {}

Department::~Department() {}

int Department::getDepartmentId() const {
    return departmentId;
}

std::string Department::getDepartmentName() const {
    return departmentName;
}

int Department::getManagerId() const {
    return managerId;
}

std::vector<int> Department::getEmployees() const {
    return employees;
}

int Department::getEmployeesCount() const {
    return static_cast<int>(employees.size());
}

void Department::setDepartmentId(int departmentId) {
    this->departmentId = departmentId;
}

void Department::setDepartmentName(const std::string& departmentName) {
    this->departmentName = departmentName;
}

void Department::setManagerId(int managerId) {
    this->managerId = managerId;
}

void Department::addEmployee(int employeeId) {
    if (!hasEmployee(employeeId)) {
        employees.push_back(employeeId);
    }
}

void Department::removeEmployee(int employeeId) {
    employees.erase(
        std::remove(employees.begin(), employees.end(), employeeId),
        employees.end()
    );
}

bool Department::hasEmployee(int employeeId) const {
    return std::find(employees.begin(), employees.end(), employeeId) != employees.end();
}

void Department::clearEmployees() {
    employees.clear();
}

void Department::displayInfo() const {
    std::cout << "Отдел ID: " << departmentId << ", "
              << "Название: " << departmentName << ", "
              << "Руководитель: " << managerId << ", "
              << "Сотрудников: " << getEmployeesCount() << std::endl;
}

