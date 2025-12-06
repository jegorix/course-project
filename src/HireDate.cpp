#include "HireDate.h"
#include <iostream>
#include <sstream>
#include <iomanip>

HireDate::HireDate() : employeeId(0), day(1), month(1), year(2000), hour(0), minute(0) {}

HireDate::HireDate(int employeeId, int day, int month, int year, int hour, int minute)
    : employeeId(employeeId), day(day), month(month), year(year),
      hour(hour), minute(minute) {}

HireDate::~HireDate() {}

int HireDate::getEmployeeId() const {
    return employeeId;
}

int HireDate::getDay() const {
    return day;
}

int HireDate::getMonth() const {
    return month;
}

int HireDate::getYear() const {
    return year;
}

int HireDate::getHour() const {
    return hour;
}

int HireDate::getMinute() const {
    return minute;
}

void HireDate::setEmployeeId(int employeeId) {
    this->employeeId = employeeId;
}

void HireDate::setDay(int day) {
    this->day = day;
}

void HireDate::setMonth(int month) {
    this->month = month;
}

void HireDate::setYear(int year) {
    this->year = year;
}

void HireDate::setHour(int hour) {
    this->hour = hour;
}

void HireDate::setMinute(int minute) {
    this->minute = minute;
}

std::string HireDate::getDateString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "."
        << std::setw(2) << month << "." << year;
    return oss.str();
}

std::string HireDate::getTimeString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setw(2) << minute;
    return oss.str();
}

std::string HireDate::getFullDateTimeString() const {
    return getDateString() + " " + getTimeString();
}

void HireDate::displayInfo() const {
    std::cout << "Сотрудник ID: " << employeeId << ", "
              << "Дата приема: " << getFullDateTimeString() << std::endl;
}

