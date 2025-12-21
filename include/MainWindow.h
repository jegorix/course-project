#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DataManager.h"
#include "Algorithm.h"

#include <QMainWindow>
#include <QTableWidget>
#include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    // Сотрудники
    void addEmployee();
    void editEmployee();
    void removeEmployee();
    void resetEmployees();

    // Отделы
    void addDepartment();
    void editDepartment();
    void removeDepartment();
    void resetDepartments();

    // Должности
    void addPosition();
    void editPosition();
    void removePosition();
    void resetPositions();

    // Поиск и сортировка
    void searchEmployees();
    void sortEmployees();
    void showSearchResults(const std::vector<EmployeeRecord>& results);
    void showSortedResults(const std::vector<EmployeeRecord>& results);

    // Аналитика
    void showAverageSalary();
    void showEfficiencyRating();
    void showPayrollTop();

    // Отмена
    void undoLastAction();

private:
    void setupUI();
    void refreshAll();

    std::unique_ptr<DataManager> manager;
    std::unique_ptr<Algorithm> algorithm;
    QTableWidget *employeesTable;
    QTableWidget *departmentsTable;
    QTableWidget *positionsTable;
};

#endif // MAINWINDOW_H
