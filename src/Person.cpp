#include "Person.h"

Person::Person() : firstName(""), lastName("") {}

Person::Person(const std::string& firstName, const std::string& lastName)
    : firstName(firstName), lastName(lastName) {}

Person::~Person() {}

std::string Person::getFirstName() const {
    return firstName;
}

std::string Person::getLastName() const {
    return lastName;
}

std::string Person::getFullName() const {
    return lastName + " " + firstName;
}

void Person::setFirstName(const std::string& firstName) {
    this->firstName = firstName;
}

void Person::setLastName(const std::string& lastName) {
    this->lastName = lastName;
}

