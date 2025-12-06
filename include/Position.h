#ifndef POSITION_H
#define POSITION_H

#include <string>

/**
 * Класс для представления должности
 */
class Position {
private:
    int positionId;         // Номер должности
    std::string positionName; // Название должности
    int workHoursPerWeek;   // Количество рабочих часов в неделю

public:
    // Конструкторы
    Position();
    Position(int positionId, const std::string& positionName, int workHoursPerWeek);
    
    // Деструктор
    ~Position();

    // Геттеры
    int getPositionId() const;
    std::string getPositionName() const;
    int getWorkHoursPerWeek() const;

    // Сеттеры
    void setPositionId(int positionId);
    void setPositionName(const std::string& positionName);
    void setWorkHoursPerWeek(int workHoursPerWeek);

    // Метод вывода информации
    void displayInfo() const;
};

#endif // POSITION_H

