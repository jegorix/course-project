#ifndef HIREDATE_H
#define HIREDATE_H

#include <string>

/**
 * Класс для представления даты приема на работу
 */
class HireDate {
private:
    int employeeId;    // Номер сотрудника
    int day;           // День
    int month;         // Месяц
    int year;          // Год
    int hour;          // Часы
    int minute;        // Минуты

public:
    // Конструкторы
    HireDate();
    HireDate(int employeeId, int day, int month, int year, int hour, int minute);
    
    // Деструктор
    ~HireDate();

    // Геттеры
    int getEmployeeId() const;
    int getDay() const;
    int getMonth() const;
    int getYear() const;
    int getHour() const;
    int getMinute() const;

    // Сеттеры
    void setEmployeeId(int employeeId);
    void setDay(int day);
    void setMonth(int month);
    void setYear(int year);
    void setHour(int hour);
    void setMinute(int minute);

    // Методы форматирования
    std::string getDateString() const;
    std::string getTimeString() const;
    std::string getFullDateTimeString() const;

    // Метод вывода информации
    void displayInfo() const;
};

#endif // HIREDATE_H

