#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
#include <vector>

/**
 * Класс для представления отдела предприятия
 */
class Department {
private:
    int departmentId;           // Номер отдела
    std::string departmentName; // Название отдела
    int managerId;              // Номер руководителя отдела
    std::vector<int> employees; // Номера сотрудников отдела

public:
    // Конструкторы
    Department();
    Department(int departmentId, const std::string& departmentName, int managerId);
    
    // Деструктор
    ~Department();

    // Геттеры
    int getDepartmentId() const;
    std::string getDepartmentName() const;
    int getManagerId() const;
    std::vector<int> getEmployees() const;
    int getEmployeesCount() const;

    // Сеттеры
    void setDepartmentId(int departmentId);
    void setDepartmentName(const std::string& departmentName);
    void setManagerId(int managerId);

    // Методы для работы с сотрудниками
    void addEmployee(int employeeId);
    void removeEmployee(int employeeId);
    bool hasEmployee(int employeeId) const;
    void clearEmployees();

    // Метод вывода информации
    void displayInfo() const;
};

#endif // DEPARTMENT_H

