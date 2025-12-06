#ifndef WORKER_H
#define WORKER_H

#include "Employee.h"

/**
 * Класс для представления рядового сотрудника
 * Третий уровень иерархии наследования (наследует от Employee)
 */
class Worker : public Employee {
private:
    int positionId;  // Номер должности
    double bonus;    // Премия

public:
    // Конструкторы
    Worker();
    Worker(int employeeId, const std::string& firstName, 
           const std::string& lastName, int departmentId, 
           double salary, int positionId, double bonus = 0.0);
    
    // Деструктор
    ~Worker();

    // Геттеры
    int getPositionId() const;
    double getBonus() const;

    // Сеттеры
    void setPositionId(int positionId);
    void setBonus(double bonus);

    // Виртуальные методы
    virtual void displayInfo() const override;
    virtual double calculateTotalIncome() const override;
};

#endif // WORKER_H

