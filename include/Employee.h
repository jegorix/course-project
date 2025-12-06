#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Person.h"

/**
 * Класс для представления сотрудника предприятия
 * Второй уровень иерархии наследования (наследует от Person)
 */
class Employee : public Person {
protected:
    int employeeId;      // Номер сотрудника
    int departmentId;    // Номер отдела
    double salary;       // Оклад

public:
    // Конструкторы
    Employee();
    Employee(int employeeId, const std::string& firstName, 
             const std::string& lastName, int departmentId, double salary);
    
    // Деструктор
    virtual ~Employee();

    // Геттеры
    int getEmployeeId() const;
    int getDepartmentId() const;
    double getSalary() const;

    // Сеттеры
    void setEmployeeId(int employeeId);
    void setDepartmentId(int departmentId);
    void setSalary(double salary);

    // Виртуальные методы
    virtual void displayInfo() const override;
    virtual double calculateTotalIncome() const;
};

#endif // EMPLOYEE_H

