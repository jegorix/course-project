#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Department.h"
#include "HireDate.h"
#include "Manager.h"
#include "Position.h"
#include "RecordCollection.h"
#include "Worker.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Структура для хранения информации о сотруднике и дате приема.
 */
struct EmployeeRecord {
    std::shared_ptr<Employee> employee;
    HireDate hireDate;
};

/**
 * Структура фильтров для поиска сотрудников.
 */
struct EmployeeSearchFilter {
    bool useDepartmentId = false;
    int departmentId = 0;
    bool usePositionId = false;
    int positionId = 0;
    bool useMinSalary = false;
    double minSalary = 0.0;
    bool useMaxSalary = false;
    double maxSalary = 0.0;
    bool useNameFragment = false;
    std::string nameFragment;
};

/**
 * Ключ сортировки списка сотрудников.
 */
enum class EmployeeSortKey {
    ById,
    ByDepartment,
    BySalary,
    ByLastName,
    ByHireDate
};

/**
 * Менеджер данных: отвечает за загрузку/сохранение, операции над сущностями,
 * управление отменой действий и предоставление информации приложению.
 */
class DataManager {
public:
    explicit DataManager(const std::string& dataDirectory);

    void loadAll();
    void saveAll() const;

    // Получение данных
    const RecordCollection<EmployeeRecord>& getEmployees() const;
    RecordCollection<EmployeeRecord>& getEmployees();

    const RecordCollection<Department>& getDepartments() const;
    RecordCollection<Department>& getDepartments();

    const RecordCollection<Position>& getPositions() const;
    RecordCollection<Position>& getPositions();

    // Операции над сотрудниками
    int addWorker(const std::string& firstName,
                  const std::string& lastName,
                  int departmentId,
                  double salary,
                  int positionId,
                  double bonus,
                  const HireDate& hireDate);

    int addManager(const std::string& firstName,
                   const std::string& lastName,
                   int departmentId,
                   double salary,
                   const std::vector<int>& subordinateIds,
                   const HireDate& hireDate);

    bool removeEmployee(int employeeId);

    bool updateEmployeeDepartment(int employeeId, int newDepartmentId);
    bool updateEmployeeSalary(int employeeId, double newSalary);
    bool updateEmployeePosition(int employeeId, int newPositionId);
    bool updateEmployeeBonus(int employeeId, double newBonus);
    bool updateEmployeeName(int employeeId, const std::string& newFirstName, const std::string& newLastName);
    bool updateEmployeeHireDate(int employeeId, const HireDate& newHireDate);

    // Операции над отделами
    int addDepartment(const std::string& name, int managerId);
    bool removeDepartment(int departmentId);
    bool updateDepartmentName(int departmentId, const std::string& newName);
    bool updateDepartmentManager(int departmentId, int managerId);

    // Операции над должностями
    int addPosition(const std::string& name, int hoursPerWeek);
    bool removePosition(int positionId);
    bool updatePositionName(int positionId, const std::string& newName);
    bool updatePositionHours(int positionId, int hoursPerWeek);

    // Поиск и сортировка
    std::vector<EmployeeRecord> findEmployees(const EmployeeSearchFilter& filter) const;
    std::vector<EmployeeRecord> sortEmployees(EmployeeSortKey key, bool descending = false) const;

    // Аналитика
    double calculateAverageSalaryForDepartment(int departmentId) const;
    std::vector<std::pair<int, double>> buildDepartmentsEfficiencyRating() const;

    // Поиск записей (для UI)
    EmployeeRecord* findEmployeeRecord(int employeeId);
    const EmployeeRecord* findEmployeeRecord(int employeeId) const;
    Department* findDepartment(int departmentId);
    const Department* findDepartment(int departmentId) const;
    Position* findPosition(int positionId);
    const Position* findPosition(int positionId) const;

    // Отмена
    bool canUndo() const;
    bool undoLastAction();
    std::string getLastUndoDescription() const;
    void pushUndo(std::function<void()> undoAction, const std::string& description);
    
    // Вспомогательный метод для восстановления сотрудника из копии (для undo)
    void restoreEmployeeFromCopy(int employeeId, std::shared_ptr<Employee> employeeCopy, const HireDate& hireDateCopy);
    std::shared_ptr<Employee> cloneEmployee(const Employee& source) const;

    // Сброс данных
    void clearAllEmployees();
    void clearAllDepartments();
    void clearAllPositions();

private:
    struct UndoCommand {
        std::function<void()> undo;
        std::string description;
    };

    std::string employeesPath;
    std::string departmentsPath;
    std::string positionsPath;
    std::string hiresPath;

    RecordCollection<EmployeeRecord> employees;
    RecordCollection<Department> departments;
    RecordCollection<Position> positions;

    std::unordered_map<int, std::size_t> employeeIndex;
    std::unordered_map<int, std::size_t> departmentIndex;
    std::unordered_map<int, std::size_t> positionIndex;

    std::vector<UndoCommand> undoStack;
    bool undoInProgress = false;

    int nextEmployeeId = 1;
    int nextDepartmentId = 1;
    int nextPositionId = 1;

    // Вспомогательные методы
    void loadEmployees();
    void loadDepartments();
    void loadPositions();
    void loadHireDates();

    void saveEmployees() const;
    void saveDepartments() const;
    void savePositions() const;
    void saveHireDates() const;

    void registerEmployee(const EmployeeRecord& record);
    void unregisterEmployee(int employeeId);

    void ensureDepartmentContainsEmployee(int departmentId, int employeeId);
    void removeEmployeeFromDepartment(int departmentId, int employeeId);

    int generateEmployeeId();
    int generateDepartmentId();
    int generatePositionId();
};

#endif // DATAMANAGER_H

