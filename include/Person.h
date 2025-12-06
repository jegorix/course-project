#ifndef PERSON_H
#define PERSON_H

#include <string>

/**
 * Базовый класс для представления человека
 * Первый уровень иерархии наследования
 */
class Person {
protected:
    std::string firstName;    // Имя
    std::string lastName;     // Фамилия

public:
    // Конструкторы
    Person();
    Person(const std::string& firstName, const std::string& lastName);
    
    // Деструктор
    virtual ~Person();

    // Геттеры
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getFullName() const;

    // Сеттеры
    void setFirstName(const std::string& firstName);
    void setLastName(const std::string& lastName);

    // Виртуальные методы
    virtual void displayInfo() const = 0;
};

#endif // PERSON_H

