#include "Algorithm.h"

#include "RecordCollection.h"
#include "DataManager.h"
#include "Employee.h"
#include "Manager.h"
#include "Worker.h"
#include "Department.h"
#include "HireDate.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

namespace {
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

std::vector<EmployeeRecord> Algorithm::findEmployees(
    const RecordCollection<EmployeeRecord>& employees,
    const EmployeeSearchFilter& filter) const
{
    std::vector<EmployeeRecord> result;

    for (const auto& record : employees) {
        if (!record.employee) continue;

        // Фильтр по отделу
        if (filter.useDepartmentId && record.employee->getDepartmentId() != filter.departmentId) {
            continue;
        }

        // Фильтр по зарплате
        if (filter.useMinSalary && record.employee->getSalary() < filter.minSalary) {
            continue;
        }
        if (filter.useMaxSalary && record.employee->getSalary() > filter.maxSalary) {
            continue;
        }

        // Фильтр по должности
        if (filter.usePositionId) {
            bool positionMatched = false;
            if (auto worker = std::dynamic_pointer_cast<const Worker>(record.employee)) {
                positionMatched = (worker->getPositionId() == filter.positionId);
            } else if (auto managerPtr = std::dynamic_pointer_cast<const Manager>(record.employee)) {
                positionMatched = (managerPtr->getPositionId() == filter.positionId);
            }
            if (!positionMatched) continue;
        }

        // Фильтр по имени
        if (filter.useNameFragment) {
            std::string fullName = toLower(record.employee->getFullName());
            std::string fragment = toLower(filter.nameFragment);
            if (fullName.find(fragment) == std::string::npos) {
                continue;
            }
        }

        result.push_back(record);
    }

    return result;
}

std::vector<EmployeeRecord> Algorithm::sortEmployees(
    const RecordCollection<EmployeeRecord>& employees,
    EmployeeSortKey key,
    bool descending) const
{
    std::vector<EmployeeRecord> result;
    for (const auto& record : employees) {
        if (record.employee) {
            result.push_back(record);
        }
    }

    auto positionIdOf = [](const EmployeeRecord& rec) {
        if (!rec.employee) return -1;
        if (auto worker = std::dynamic_pointer_cast<const Worker>(rec.employee)) {
            return worker->getPositionId();
        }
        if (auto managerPtr = std::dynamic_pointer_cast<const Manager>(rec.employee)) {
            return managerPtr->getPositionId();
        }
        return -1;
    };

    std::sort(result.begin(), result.end(), [key, descending, positionIdOf](const EmployeeRecord& a, const EmployeeRecord& b) {
        if (!a.employee || !b.employee) return false;

        int cmp = 0;
        switch (key) {
            case EmployeeSortKey::ById:
                cmp = (a.employee->getEmployeeId() < b.employee->getEmployeeId()) ? -1 : 1;
                break;
            case EmployeeSortKey::ByDepartment:
                cmp = (a.employee->getDepartmentId() < b.employee->getDepartmentId()) ? -1 : 1;
                break;
            case EmployeeSortKey::ByPosition: {
                int posA = positionIdOf(a);
                int posB = positionIdOf(b);
                if (posA == posB) {
                    cmp = 0;
                } else {
                    cmp = (posA < posB) ? -1 : 1;
                }
                break;
            }
            case EmployeeSortKey::BySalary:
                cmp = (a.employee->getSalary() < b.employee->getSalary()) ? -1 : 1;
                break;
            case EmployeeSortKey::ByLastName:
                cmp = a.employee->getLastName().compare(b.employee->getLastName());
                break;
            case EmployeeSortKey::ByHireDate:
                if (a.hireDate.getYear() != b.hireDate.getYear()) {
                    cmp = (a.hireDate.getYear() < b.hireDate.getYear()) ? -1 : 1;
                } else if (a.hireDate.getMonth() != b.hireDate.getMonth()) {
                    cmp = (a.hireDate.getMonth() < b.hireDate.getMonth()) ? -1 : 1;
                } else if (a.hireDate.getDay() != b.hireDate.getDay()) {
                    cmp = (a.hireDate.getDay() < b.hireDate.getDay()) ? -1 : 1;
                }
                break;
        }
        return descending ? (cmp > 0) : (cmp < 0);
    });

    return result;
}

double Algorithm::calculateAverageSalaryForDepartment(
    const RecordCollection<EmployeeRecord>& employees,
    int departmentId,
    const Department* department) const
{
    if (!department) {
        return 0.0;
    }

    double sumSalary = 0.0;
    std::size_t count = 0;

    for (const auto& record : employees) {
        if (!record.employee) {
            continue;
        }
        if (record.employee->getDepartmentId() != departmentId) {
            continue;
        }

        sumSalary += record.employee->getSalary();
        ++count;
    }

    if (count == 0) {
        return 0.0;
    }

    return sumSalary / static_cast<double>(count);
}

std::vector<std::pair<int, double>> Algorithm::buildDepartmentsEfficiencyRating(
    const RecordCollection<EmployeeRecord>& employees,
    const RecordCollection<Department>& departments) const
{
    std::vector<std::pair<int, double>> rating;

    for (const auto& dept : departments) {
        int departmentId = dept.getDepartmentId();

        double totalSalary = 0.0;
        std::size_t count  = 0;

        for (const auto& record : employees) {
            if (!record.employee) {
                continue;
            }
            if (record.employee->getDepartmentId() != departmentId) {
                continue;
            }

            totalSalary += record.employee->getSalary();
            ++count;
        }

        double efficiency = 0.0;
        if (count > 0 && totalSalary > 0.0) {
            efficiency = static_cast<double>(count) / totalSalary;
        }

        rating.emplace_back(departmentId, efficiency);
    }

    std::sort(
        rating.begin(),
        rating.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second > rhs.second;
        }
    );

    return rating;
}

std::vector<Algorithm::DepartmentPayrollStat> Algorithm::buildDepartmentsPayrollStats(
    const RecordCollection<EmployeeRecord>& employees,
    const RecordCollection<Department>& departments) const
{
    std::vector<DepartmentPayrollStat> stats;

    for (const auto& dept : departments) {
        DepartmentPayrollStat stat;
        stat.departmentId = dept.getDepartmentId();

        for (const auto& record : employees) {
            if (!record.employee) {
                continue;
            }
            if (record.employee->getDepartmentId() != stat.departmentId) {
                continue;
            }

            stat.baseSalary += record.employee->getSalary();

            if (auto worker = std::dynamic_pointer_cast<Worker>(record.employee)) {
                stat.bonus += worker->getBonus();
            }
        }

        stat.total = stat.baseSalary + stat.bonus;
        stat.bonusShare = (stat.baseSalary > 1e-9) ? (stat.bonus / stat.baseSalary) : 0.0;
        stats.push_back(stat);
    }

    std::sort(
        stats.begin(),
        stats.end(),
        [](const DepartmentPayrollStat& a, const DepartmentPayrollStat& b) {
            return a.total > b.total;
        }
    );

    return stats;
}

EmployeeRecord* Algorithm::findEmployeeRecord(DataManager& manager, int employeeId) const {
    auto it = manager.employeeIndex.find(employeeId);
    if (it == manager.employeeIndex.end()) return nullptr;
    if (it->second >= manager.employees.size()) return nullptr;
    return &manager.employees[it->second];
}

const EmployeeRecord* Algorithm::findEmployeeRecord(const DataManager& manager, int employeeId) const {
    auto it = manager.employeeIndex.find(employeeId);
    if (it == manager.employeeIndex.end()) return nullptr;
    if (it->second >= manager.employees.size()) return nullptr;
    return &manager.employees[it->second];
}

Department* Algorithm::findDepartment(DataManager& manager, int departmentId) const {
    auto it = manager.departmentIndex.find(departmentId);
    if (it == manager.departmentIndex.end()) return nullptr;
    if (it->second >= manager.departments.size()) return nullptr;
    return &manager.departments[it->second];
}

const Department* Algorithm::findDepartment(const DataManager& manager, int departmentId) const {
    auto it = manager.departmentIndex.find(departmentId);
    if (it == manager.departmentIndex.end()) return nullptr;
    if (it->second >= manager.departments.size()) return nullptr;
    return &manager.departments[it->second];
}

Position* Algorithm::findPosition(DataManager& manager, int positionId) const {
    auto it = manager.positionIndex.find(positionId);
    if (it == manager.positionIndex.end()) return nullptr;
    if (it->second >= manager.positions.size()) return nullptr;
    return &manager.positions[it->second];
}

const Position* Algorithm::findPosition(const DataManager& manager, int positionId) const {
    auto it = manager.positionIndex.find(positionId);
    if (it == manager.positionIndex.end()) return nullptr;
    if (it->second >= manager.positions.size()) return nullptr;
    return &manager.positions[it->second];
}
