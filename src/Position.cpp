#include "Position.h"
#include <iostream>
#include <sstream>

Position::Position() : positionId(0), positionName(""), workHoursPerWeek(0) {}

Position::Position(int positionId, const std::string& positionName, int workHoursPerWeek)
    : positionId(positionId), positionName(positionName),
      workHoursPerWeek(workHoursPerWeek) {}

Position::~Position() {}

int Position::getPositionId() const {
    return positionId;
}

std::string Position::getPositionName() const {
    return positionName;
}

int Position::getWorkHoursPerWeek() const {
    return workHoursPerWeek;
}

void Position::setPositionId(int positionId) {
    this->positionId = positionId;
}

void Position::setPositionName(const std::string& positionName) {
    this->positionName = positionName;
}

void Position::setWorkHoursPerWeek(int workHoursPerWeek) {
    this->workHoursPerWeek = workHoursPerWeek;
}

void Position::displayInfo() const {
    std::cout << "Должность ID: " << positionId << ", "
              << "Название: " << positionName << ", "
              << "Часов в неделю: " << workHoursPerWeek << std::endl;
}

