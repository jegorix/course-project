#ifndef MANAGER_H
#define MANAGER_H

#include "Employee.h"
#include <vector>

/**
 * Класс для представления руководителя отдела
 * Третий уровень иерархии наследования (наследует от Employee)
 */
class Manager : public Employee {
private:
    int positionId = 0;        // Номер должности руководителя
    std::vector<int> subordinates;  // Номера подчиненных сотрудников

public:
    // Конструкторы
    Manager();
    Manager(int employeeId, const std::string& firstName, 
            const std::string& lastName, int departmentId, 
            double salary, int positionId, const std::vector<int>& subordinates);
    
    // Деструктор
    ~Manager();

    // Должность
    int getPositionId() const;
    void setPositionId(int positionId);

    // Методы для работы с подчиненными
    void addSubordinate(int subordinateId);
    void removeSubordinate(int subordinateId);
    std::vector<int> getSubordinates() const;
    int getSubordinatesCount() const;
    bool hasSubordinate(int subordinateId) const;

    // Виртуальные методы
    virtual void displayInfo() const override;
    virtual double calculateTotalIncome() const override;
};

#endif // MANAGER_H
