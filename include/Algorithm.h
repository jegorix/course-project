#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "DataManager.h"

#include <utility>
#include <vector>

/**
 * Класс, инкапсулирующий логику поиска, сортировки и аналитики.
 */
class Algorithm {
public:
    Algorithm() = default;

    struct DepartmentPayrollStat {
        int departmentId = 0;
        double baseSalary = 0.0;
        double bonus = 0.0;
        double total = 0.0;
        double bonusShare = 0.0; // bonus / baseSalary
    };

    // Поиск сотрудников по заданному фильтру.
    std::vector<EmployeeRecord> findEmployees(const RecordCollection<EmployeeRecord>& employees,
                                              const EmployeeSearchFilter& filter) const;

    // Сортировка сотрудников по ключу.
    std::vector<EmployeeRecord> sortEmployees(const RecordCollection<EmployeeRecord>& employees,
                                              EmployeeSortKey key,
                                              bool descending) const;

    // Средняя зарплата по конкретному отделу.
    double calculateAverageSalaryForDepartment(const RecordCollection<EmployeeRecord>& employees,
                                               int departmentId,
                                               const Department* department) const;

    // Рейтинг отделов по эффективности.
    std::vector<std::pair<int, double>> buildDepartmentsEfficiencyRating(
        const RecordCollection<EmployeeRecord>& employees,
        const RecordCollection<Department>& departments) const;

    // Статистика ФОТ по отделам.
    std::vector<DepartmentPayrollStat> buildDepartmentsPayrollStats(
        const RecordCollection<EmployeeRecord>& employees,
        const RecordCollection<Department>& departments) const;

    // Поиск ссылок на записи в менеджере данных.
    EmployeeRecord* findEmployeeRecord(DataManager& manager, int employeeId) const;
    const EmployeeRecord* findEmployeeRecord(const DataManager& manager, int employeeId) const;
    Department* findDepartment(DataManager& manager, int departmentId) const;
    const Department* findDepartment(const DataManager& manager, int departmentId) const;
    Position* findPosition(DataManager& manager, int positionId) const;
    const Position* findPosition(const DataManager& manager, int positionId) const;
};

#endif // ALGORITHM_H
