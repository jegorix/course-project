#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMenuBar>
#include <QTabWidget>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QDateEdit>
#include <QTimeEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QListWidget>
#include <QColor>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    try {
        manager = std::make_unique<DataManager>("data");
        manager->loadAll();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось загрузить данные: %1").arg(e.what()));
    }
    setupUI();
    refreshAll();
}
void MainWindow::addEmployee() {
        QDialog dialog(this);
        dialog.setWindowTitle("Добавить сотрудника");
        QFormLayout layout(&dialog);

        QComboBox *typeCombo = new QComboBox(&dialog);
        typeCombo->addItems({"Работник", "Руководитель"});
        layout.addRow("Тип:", typeCombo);

        QLineEdit *firstNameEdit = new QLineEdit(&dialog);
        QLineEdit *lastNameEdit = new QLineEdit(&dialog);
        layout.addRow("Имя:", firstNameEdit);
        layout.addRow("Фамилия:", lastNameEdit);

        QComboBox *deptCombo = new QComboBox(&dialog);
        for (const auto& dept : manager->getDepartments()) {
            deptCombo->addItem(QString::fromStdString(dept.getDepartmentName()), dept.getDepartmentId());
        }
        layout.addRow("Отдел:", deptCombo);

        QDoubleSpinBox *salarySpin = new QDoubleSpinBox(&dialog);
        salarySpin->setMinimum(0);
        salarySpin->setMaximum(10000000);
        salarySpin->setValue(50000);
        layout.addRow("Зарплата:", salarySpin);

        QComboBox *positionCombo = new QComboBox(&dialog);
        positionCombo->addItem("Не выбрано", 0);
        for (const auto& pos : manager->getPositions()) {
            positionCombo->addItem(QString::fromStdString(pos.getPositionName()), pos.getPositionId());
        }
        layout.addRow("Должность:", positionCombo);

        QDoubleSpinBox *bonusSpin = new QDoubleSpinBox(&dialog);
        bonusSpin->setMinimum(0);
        bonusSpin->setMaximum(1000000);
        QWidget *bonusWidget = new QWidget(&dialog);
        QHBoxLayout *bonusLayout = new QHBoxLayout(bonusWidget);
        bonusLayout->setContentsMargins(0, 0, 0, 0);
        bonusLayout->addWidget(bonusSpin);
        layout.addRow("Премия:", bonusWidget);

        QListWidget *subordinatesList = new QListWidget(&dialog);
        subordinatesList->setSelectionMode(QAbstractItemView::MultiSelection);
        QComboBox *addSubCombo = new QComboBox(&dialog);
        addSubCombo->addItem("Выберите работника...", 0);
        for (const auto& empRecord : manager->getEmployees()) {
            if (empRecord.employee && std::dynamic_pointer_cast<Worker>(empRecord.employee)) {
                addSubCombo->addItem(QString::fromStdString(empRecord.employee->getFullName()),
                                    empRecord.employee->getEmployeeId());
            }
        }
        QPushButton *addSubBtn = new QPushButton("Добавить подчиненного", &dialog);
        connect(addSubBtn, &QPushButton::clicked, [addSubCombo, subordinatesList]() {
            int empId = addSubCombo->currentData().toInt();
            if (empId > 0) {
                bool exists = false;
                for (int i = 0; i < subordinatesList->count(); ++i) {
                    if (subordinatesList->item(i)->data(Qt::UserRole).toInt() == empId) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    QListWidgetItem *item = new QListWidgetItem(addSubCombo->currentText());
                    item->setData(Qt::UserRole, empId);
                    subordinatesList->addItem(item);
                }
            }
        });
        QPushButton *removeSubBtn = new QPushButton("Удалить выбранного", &dialog);
        connect(removeSubBtn, &QPushButton::clicked, [subordinatesList]() {
            QList<QListWidgetItem*> items = subordinatesList->selectedItems();
            for (auto* item : items) {
                delete subordinatesList->takeItem(subordinatesList->row(item));
            }
        });
        
        QHBoxLayout *subLayout = new QHBoxLayout;
        subLayout->addWidget(addSubCombo);
        subLayout->addWidget(addSubBtn);
        subLayout->addWidget(removeSubBtn);
        QVBoxLayout *subVLayout = new QVBoxLayout;
        subVLayout->addLayout(subLayout);
        subVLayout->addWidget(subordinatesList);
        QWidget *subWidget = new QWidget(&dialog);
        subWidget->setLayout(subVLayout);
        layout.addRow("Подчиненные (для руководителя):", subWidget);

        QDateEdit *dateEdit = new QDateEdit(QDate::currentDate(), &dialog);
        QTimeEdit *timeEdit = new QTimeEdit(QTime::currentTime(), &dialog);
        layout.addRow("Дата найма:", dateEdit);
        layout.addRow("Время найма:", timeEdit);
        
        // Показывать/скрывать поля в зависимости от типа
        connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [positionCombo, bonusWidget, subWidget](int index) {
            bool isWorker = (index == 0);
            positionCombo->setVisible(isWorker);
            bonusWidget->setVisible(isWorker);
            subWidget->setVisible(!isWorker);
        });
        positionCombo->setVisible(true);
        bonusWidget->setVisible(true);
        subWidget->setVisible(false);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (firstNameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым");
                return;
            }
            if (lastNameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Фамилия не может быть пустой");
                return;
            }
            if (deptCombo->count() == 0) {
                QMessageBox::warning(this, "Ошибка", "Нет доступных отделов. Сначала создайте отдел.");
                return;
            }
            if (salarySpin->value() <= 0) {
                QMessageBox::warning(this, "Ошибка", "Зарплата должна быть больше нуля");
                return;
            }

            try {
                int deptId = deptCombo->currentData().toInt();
                QDate date = dateEdit->date();
                QTime time = timeEdit->time();
                HireDate hireDate(0, date.day(), date.month(), date.year(), time.hour(), time.minute());

                if (typeCombo->currentText() == "Работник") {
                    int positionId = positionCombo->currentData().toInt();
                    if (positionId == 0) {
                        QMessageBox::warning(this, "Ошибка", "Выберите должность");
                        return;
                    }
                    manager->addWorker(firstNameEdit->text().trimmed().toStdString(),
                                      lastNameEdit->text().trimmed().toStdString(),
                                      deptId, salarySpin->value(),
                                      positionId, bonusSpin->value(), hireDate);
                } else {
                    // Собрать список подчиненных
                    std::vector<int> subordinateIds;
                    for (int i = 0; i < subordinatesList->count(); ++i) {
                        subordinateIds.push_back(subordinatesList->item(i)->data(Qt::UserRole).toInt());
                    }
                    manager->addManager(firstNameEdit->text().trimmed().toStdString(),
                                       lastNameEdit->text().trimmed().toStdString(),
                                       deptId, salarySpin->value(),
                                       subordinateIds, hireDate);
                }
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Сотрудник добавлен");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::editEmployee() {
        int row = employeesTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите сотрудника для редактирования");
            return;
        }

        int employeeId = employeesTable->item(row, 0)->text().toInt();
        auto* record = manager->findEmployeeRecord(employeeId);
        if (!record || !record->employee) return;

        QDialog dialog(this);
        dialog.setWindowTitle("Редактировать сотрудника");
        QFormLayout layout(&dialog);

        QLabel *idLabel = new QLabel(QString::number(employeeId), &dialog);
        layout.addRow("ID:", idLabel);

        QLineEdit *firstNameEdit = new QLineEdit(QString::fromStdString(record->employee->getFirstName()), &dialog);
        QLineEdit *lastNameEdit = new QLineEdit(QString::fromStdString(record->employee->getLastName()), &dialog);
        layout.addRow("Имя:", firstNameEdit);
        layout.addRow("Фамилия:", lastNameEdit);

        QComboBox *deptCombo = new QComboBox(&dialog);
        for (const auto& dept : manager->getDepartments()) {
            deptCombo->addItem(QString::fromStdString(dept.getDepartmentName()), dept.getDepartmentId());
            if (dept.getDepartmentId() == record->employee->getDepartmentId()) {
                deptCombo->setCurrentIndex(deptCombo->count() - 1);
            }
        }
        layout.addRow("Отдел:", deptCombo);

        QDoubleSpinBox *salarySpin = new QDoubleSpinBox(&dialog);
        salarySpin->setMinimum(0);
        salarySpin->setMaximum(10000000);
        salarySpin->setValue(record->employee->getSalary());
        layout.addRow("Зарплата:", salarySpin);

        QComboBox *positionCombo = nullptr;
        QDoubleSpinBox *bonusSpin = nullptr;
        QListWidget *subordinatesList = nullptr;
        bool isWorker = false;
        bool isManager = false;
        
        if (auto worker = std::dynamic_pointer_cast<Worker>(record->employee)) {
            isWorker = true;
            positionCombo = new QComboBox(&dialog);
            for (const auto& pos : manager->getPositions()) {
                positionCombo->addItem(QString::fromStdString(pos.getPositionName()), pos.getPositionId());
                if (pos.getPositionId() == worker->getPositionId()) {
                    positionCombo->setCurrentIndex(positionCombo->count() - 1);
                }
            }
            layout.addRow("Должность:", positionCombo);

            bonusSpin = new QDoubleSpinBox(&dialog);
            bonusSpin->setMinimum(0);
            bonusSpin->setMaximum(1000000);
            bonusSpin->setValue(worker->getBonus());
            layout.addRow("Премия:", bonusSpin);
        } else if (auto managerPtr = std::dynamic_pointer_cast<Manager>(record->employee)) {
            isManager = true;
            subordinatesList = new QListWidget(&dialog);
            subordinatesList->setSelectionMode(QAbstractItemView::MultiSelection);
            
            // Добавить всех работников в список
            QComboBox *addSubCombo = new QComboBox(&dialog);
            addSubCombo->addItem("Выберите работника...", 0);
            for (const auto& empRecord : manager->getEmployees()) {
                if (empRecord.employee && 
                    std::dynamic_pointer_cast<Worker>(empRecord.employee) &&
                    empRecord.employee->getEmployeeId() != employeeId) {
                    addSubCombo->addItem(QString::fromStdString(empRecord.employee->getFullName()),
                                        empRecord.employee->getEmployeeId());
                }
            }
            
            // Показать текущих подчиненных
            auto subs = managerPtr->getSubordinates();
            for (int subId : subs) {
                for (const auto& empRecord : manager->getEmployees()) {
                    if (empRecord.employee && empRecord.employee->getEmployeeId() == subId) {
                        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(empRecord.employee->getFullName()));
                        item->setData(Qt::UserRole, subId);
                        subordinatesList->addItem(item);
                        break;
                    }
                }
            }
            
            QPushButton *addSubBtn = new QPushButton("Добавить подчиненного", &dialog);
            connect(addSubBtn, &QPushButton::clicked, [addSubCombo, subordinatesList]() {
                int empId = addSubCombo->currentData().toInt();
                if (empId > 0) {
                    bool exists = false;
                    for (int i = 0; i < subordinatesList->count(); ++i) {
                        if (subordinatesList->item(i)->data(Qt::UserRole).toInt() == empId) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        QListWidgetItem *item = new QListWidgetItem(addSubCombo->currentText());
                        item->setData(Qt::UserRole, empId);
                        subordinatesList->addItem(item);
                    }
                }
            });
            
            QPushButton *removeSubBtn = new QPushButton("Удалить выбранного", &dialog);
            connect(removeSubBtn, &QPushButton::clicked, [subordinatesList]() {
                QList<QListWidgetItem*> items = subordinatesList->selectedItems();
                for (auto* item : items) {
                    delete subordinatesList->takeItem(subordinatesList->row(item));
                }
            });
            
            QHBoxLayout *subLayout = new QHBoxLayout;
            subLayout->addWidget(addSubCombo);
            subLayout->addWidget(addSubBtn);
            subLayout->addWidget(removeSubBtn);
            
            QVBoxLayout *subVLayout = new QVBoxLayout;
            subVLayout->addLayout(subLayout);
            subVLayout->addWidget(subordinatesList);
            
            QWidget *subWidget = new QWidget(&dialog);
            subWidget->setLayout(subVLayout);
            layout.addRow("Подчиненные:", subWidget);
        }

        QDateEdit *dateEdit = new QDateEdit(&dialog);
        dateEdit->setDate(QDate(record->hireDate.getYear(), record->hireDate.getMonth(), record->hireDate.getDay()));
        QTimeEdit *timeEdit = new QTimeEdit(&dialog);
        timeEdit->setTime(QTime(record->hireDate.getHour(), record->hireDate.getMinute()));
        layout.addRow("Дата найма:", dateEdit);
        layout.addRow("Время найма:", timeEdit);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (firstNameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым");
                return;
            }
            if (lastNameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Фамилия не может быть пустой");
                return;
            }
            if (salarySpin->value() <= 0) {
                QMessageBox::warning(this, "Ошибка", "Зарплата должна быть больше нуля");
                return;
            }

            try {
                manager->updateEmployeeName(employeeId, firstNameEdit->text().trimmed().toStdString(), 
                                            lastNameEdit->text().trimmed().toStdString());
                manager->updateEmployeeDepartment(employeeId, deptCombo->currentData().toInt());
                manager->updateEmployeeSalary(employeeId, salarySpin->value());
                
                // Обновление даты найма
                QDate date = dateEdit->date();
                QTime time = timeEdit->time();
                HireDate newHireDate(employeeId, date.day(), date.month(), date.year(), time.hour(), time.minute());
                manager->updateEmployeeHireDate(employeeId, newHireDate);
                
                if (isWorker && positionCombo) {
                    manager->updateEmployeePosition(employeeId, positionCombo->currentData().toInt());
                    if (bonusSpin) {
                        manager->updateEmployeeBonus(employeeId, bonusSpin->value());
                    }
                } else if (isManager && subordinatesList) {
                    // Обновление подчиненных менеджера
                    auto managerPtr = std::dynamic_pointer_cast<Manager>(record->employee);
                    if (managerPtr) {
                        // Удалить всех текущих подчиненных
                        auto currentSubs = managerPtr->getSubordinates();
                        for (int subId : currentSubs) {
                            managerPtr->removeSubordinate(subId);
                        }
                        // Добавить новых
                        for (int i = 0; i < subordinatesList->count(); ++i) {
                            int subId = subordinatesList->item(i)->data(Qt::UserRole).toInt();
                            managerPtr->addSubordinate(subId);
                        }
                    }
                }
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Сотрудник обновлен");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::removeEmployee() {
        int row = employeesTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите сотрудника для удаления");
            return;
        }

        int id = employeesTable->item(row, 0)->text().toInt();
        if (QMessageBox::question(this, "Подтверждение", "Удалить сотрудника?") == QMessageBox::Yes) {
            try {
                if (manager->removeEmployee(id)) {
                    manager->saveAll();
                    refreshAll();
                    QMessageBox::information(this, "Успех", "Сотрудник удален");
                } else {
                    QMessageBox::warning(this, "Ошибка", "Не удалось удалить сотрудника");
                }
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::resetEmployees() {
    if (QMessageBox::question(this, "Подтверждение", 
        "Вы уверены, что хотите удалить всех сотрудников? Это действие можно отменить.") == QMessageBox::Yes) {
        try {
            manager->clearAllEmployees();
            manager->saveAll();
            refreshAll();
            QMessageBox::information(this, "Успех", "Все сотрудники удалены");
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
        }
    }
}
void MainWindow::addDepartment() {
        QDialog dialog(this);
        dialog.setWindowTitle("Добавить отдел");
        QFormLayout layout(&dialog);

        QLineEdit *nameEdit = new QLineEdit(&dialog);
        QComboBox *managerCombo = new QComboBox(&dialog);
        managerCombo->addItem("Не выбран", 0);
        for (const auto& record : manager->getEmployees()) {
            if (record.employee && std::dynamic_pointer_cast<Manager>(record.employee)) {
                managerCombo->addItem(QString::fromStdString(record.employee->getFullName()),
                                     record.employee->getEmployeeId());
            }
        }
        layout.addRow("Название:", nameEdit);
        layout.addRow("Руководитель:", managerCombo);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (nameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Название отдела не может быть пустым");
                return;
            }
            
            try {
                int managerId = managerCombo->currentData().toInt();
                manager->addDepartment(nameEdit->text().trimmed().toStdString(), managerId);
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Отдел добавлен");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::editDepartment() {
        int row = departmentsTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите отдел для редактирования");
            return;
        }

        int deptId = departmentsTable->item(row, 0)->text().toInt();
        Department* dept = manager->findDepartment(deptId);
        if (!dept) return;

        QDialog dialog(this);
        dialog.setWindowTitle("Редактировать отдел");
        QFormLayout layout(&dialog);

        QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(dept->getDepartmentName()), &dialog);
        QComboBox *managerCombo = new QComboBox(&dialog);
        managerCombo->addItem("Не выбран", 0);
            for (const auto& record : manager->getEmployees()) {
                if (record.employee && std::dynamic_pointer_cast<Manager>(record.employee)) {
                    managerCombo->addItem(QString::fromStdString(record.employee->getFullName()),
                                     record.employee->getEmployeeId());
                    if (record.employee->getEmployeeId() == dept->getManagerId()) {
                        managerCombo->setCurrentIndex(managerCombo->count() - 1);
                    }
                }
            }
        layout.addRow("Название:", nameEdit);
        layout.addRow("Руководитель:", managerCombo);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (nameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Название отдела не может быть пустым");
                return;
            }
            
            try {
                manager->updateDepartmentName(deptId, nameEdit->text().trimmed().toStdString());
                manager->updateDepartmentManager(deptId, managerCombo->currentData().toInt());
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Отдел обновлен");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::removeDepartment() {
        int row = departmentsTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите отдел для удаления");
            return;
        }

        int id = departmentsTable->item(row, 0)->text().toInt();
        if (QMessageBox::question(this, "Подтверждение", "Удалить отдел?") == QMessageBox::Yes) {
            try {
                if (manager->removeDepartment(id)) {
                    manager->saveAll();
                    refreshAll();
                    QMessageBox::information(this, "Успех", "Отдел удален");
                } else {
                    QMessageBox::warning(this, "Ошибка", "Не удалось удалить отдел. Возможно, в нем есть сотрудники.");
                }
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::resetDepartments() {
    if (QMessageBox::question(this, "Подтверждение", 
        "Вы уверены, что хотите удалить все отделы? Это действие можно отменить.") == QMessageBox::Yes) {
        try {
            manager->clearAllDepartments();
            manager->saveAll();
            refreshAll();
            QMessageBox::information(this, "Успех", "Все отделы удалены");
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
        }
    }
}

void MainWindow::addPosition() {
        QDialog dialog(this);
        dialog.setWindowTitle("Добавить должность");
        QFormLayout layout(&dialog);

        QLineEdit *nameEdit = new QLineEdit(&dialog);
        QSpinBox *hoursSpin = new QSpinBox(&dialog);
        hoursSpin->setMinimum(1);
        hoursSpin->setMaximum(168);
        hoursSpin->setValue(40);
        layout.addRow("Название:", nameEdit);
        layout.addRow("Часов в неделю:", hoursSpin);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (nameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Название должности не может быть пустым");
                return;
            }
            if (hoursSpin->value() <= 0 || hoursSpin->value() > 168) {
                QMessageBox::warning(this, "Ошибка", "Количество часов должно быть от 1 до 168");
                return;
            }
            
            try {
                manager->addPosition(nameEdit->text().trimmed().toStdString(), hoursSpin->value());
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Должность добавлена");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::editPosition() {
        int row = positionsTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите должность для редактирования");
            return;
        }

        int posId = positionsTable->item(row, 0)->text().toInt();
        Position* pos = manager->findPosition(posId);
        if (!pos) return;

        QDialog dialog(this);
        dialog.setWindowTitle("Редактировать должность");
        QFormLayout layout(&dialog);

        QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(pos->getPositionName()), &dialog);
        QSpinBox *hoursSpin = new QSpinBox(&dialog);
        hoursSpin->setMinimum(1);
        hoursSpin->setMaximum(168);
        hoursSpin->setValue(pos->getWorkHoursPerWeek());
        layout.addRow("Название:", nameEdit);
        layout.addRow("Часов в неделю:", hoursSpin);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            // Валидация
            if (nameEdit->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Название должности не может быть пустым");
                return;
            }
            if (hoursSpin->value() <= 0 || hoursSpin->value() > 168) {
                QMessageBox::warning(this, "Ошибка", "Количество часов должно быть от 1 до 168");
                return;
            }
            
            try {
                manager->updatePositionName(posId, nameEdit->text().trimmed().toStdString());
                manager->updatePositionHours(posId, hoursSpin->value());
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Должность обновлена");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::removePosition() {
        int row = positionsTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите должность для удаления");
            return;
        }

        int id = positionsTable->item(row, 0)->text().toInt();
        if (QMessageBox::question(this, "Подтверждение", "Удалить должность?") == QMessageBox::Yes) {
            try {
                if (manager->removePosition(id)) {
                    manager->saveAll();
                    refreshAll();
                    QMessageBox::information(this, "Успех", "Должность удалена");
                } else {
                    QMessageBox::warning(this, "Ошибка", "Не удалось удалить должность. Возможно, она используется.");
                }
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    }

void MainWindow::resetPositions() {
    if (QMessageBox::question(this, "Подтверждение", 
        "Вы уверены, что хотите удалить все должности? Это действие можно отменить.") == QMessageBox::Yes) {
        try {
            manager->clearAllPositions();
            manager->saveAll();
            refreshAll();
            QMessageBox::information(this, "Успех", "Все должности удалены");
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
        }
    }
}
void MainWindow::searchEmployees() {
        QDialog dialog(this);
        dialog.setWindowTitle("Поиск сотрудников");
        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

        QGroupBox *filterGroup = new QGroupBox("Критерии поиска", &dialog);
        QFormLayout *layout = new QFormLayout(filterGroup);

        QCheckBox *useDept = new QCheckBox(&dialog);
        QComboBox *deptCombo = new QComboBox(&dialog);
        deptCombo->addItem("Все отделы", 0);
        for (const auto& dept : manager->getDepartments()) {
            deptCombo->addItem(QString::fromStdString(dept.getDepartmentName()), dept.getDepartmentId());
        }
        deptCombo->setEnabled(false);
        connect(useDept, &QCheckBox::toggled, deptCombo, &QComboBox::setEnabled);
        layout->addRow("По отделу:", useDept);
        layout->addRow("", deptCombo);

        QCheckBox *useMinSalary = new QCheckBox(&dialog);
        QCheckBox *useMaxSalary = new QCheckBox(&dialog);
        QDoubleSpinBox *minSalary = new QDoubleSpinBox(&dialog);
        QDoubleSpinBox *maxSalary = new QDoubleSpinBox(&dialog);
        minSalary->setMinimum(0);
        maxSalary->setMinimum(0);
        minSalary->setEnabled(false);
        maxSalary->setEnabled(false);
        connect(useMinSalary, &QCheckBox::toggled, minSalary, &QDoubleSpinBox::setEnabled);
        connect(useMaxSalary, &QCheckBox::toggled, maxSalary, &QDoubleSpinBox::setEnabled);
        layout->addRow("Минимальная зарплата:", useMinSalary);
        layout->addRow("", minSalary);
        layout->addRow("Максимальная зарплата:", useMaxSalary);
        layout->addRow("", maxSalary);

        QCheckBox *useName = new QCheckBox(&dialog);
        QLineEdit *nameEdit = new QLineEdit(&dialog);
        nameEdit->setEnabled(false);
        connect(useName, &QCheckBox::toggled, nameEdit, &QLineEdit::setEnabled);
        layout->addRow("По имени:", useName);
        layout->addRow("Фрагмент:", nameEdit);

        mainLayout->addWidget(filterGroup);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        mainLayout->addWidget(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            EmployeeSearchFilter filter;
            if (useDept->isChecked() && deptCombo->currentData().toInt() > 0) {
                filter.useDepartmentId = true;
                filter.departmentId = deptCombo->currentData().toInt();
            }
            if (useMinSalary->isChecked()) {
                filter.useMinSalary = true;
                filter.minSalary = minSalary->value();
            }
            if (useMaxSalary->isChecked()) {
                filter.useMaxSalary = true;
                filter.maxSalary = maxSalary->value();
            }
            if (useName->isChecked()) {
                filter.useNameFragment = true;
                filter.nameFragment = nameEdit->text().toStdString();
            }

            auto results = manager->findEmployees(filter);
            showSearchResults(results);
        }
    }

void MainWindow::showSearchResults(const std::vector<EmployeeRecord>& results) {
        QDialog dialog(this);
        dialog.setWindowTitle("Результаты поиска");
        dialog.resize(800, 600);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        QTableWidget *table = new QTableWidget(&dialog);
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"ID", "ФИО", "Отдел", "Зарплата", "Тип"});
        table->setRowCount(static_cast<int>(results.size()));

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& record = results[i];
            if (!record.employee) continue;

            QString deptName = "Неизвестно";
            for (const auto& dept : manager->getDepartments()) {
                if (dept.getDepartmentId() == record.employee->getDepartmentId()) {
                    deptName = QString::fromStdString(dept.getDepartmentName());
                    break;
                }
            }

            table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(record.employee->getEmployeeId())));
            table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(record.employee->getFullName())));
            table->setItem(static_cast<int>(i), 2, new QTableWidgetItem(deptName));
            QTableWidgetItem *salaryItem = new QTableWidgetItem("$" + QString::number(record.employee->getSalary(), 'f', 2));
            salaryItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(static_cast<int>(i), 3, salaryItem);
            
            QString type = "Работник";
            if (std::dynamic_pointer_cast<Manager>(record.employee)) {
                type = "Руководитель";
            }
            table->setItem(static_cast<int>(i), 4, new QTableWidgetItem(type));
        }

        table->setAlternatingRowColors(true);
        table->setStyleSheet(
            "QTableWidget { "
            "    gridline-color: #b0b0b0; "
            "    background-color: #ffffff; "
            "    color: #000000; "
            "} "
            "QTableWidget::item { "
            "    padding: 6px; "
            "    color: #000000; "
            "    background-color: #ffffff; "
            "} "
            "QTableWidget::item:alternate { "
            "    background-color: #f5f5f5; "
            "    color: #000000; "
            "} "
            "QTableWidget::item:selected { "
            "    background-color: #2196F3; "
            "    color: #ffffff; "
            "} "
            "QHeaderView::section { "
            "    background-color: #2c3e50; "
            "    color: #ffffff; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    font-size: 11pt; "
            "    border: 1px solid #1a252f; "
            "} "
            "QHeaderView::section:hover { "
            "    background-color: #34495e; "
            "}"
        );
        table->resizeColumnsToContents();
        layout->addWidget(table);

        QDialogButtonBox buttons(QDialogButtonBox::Ok);
        layout->addWidget(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

        dialog.exec();
    }

void MainWindow::sortEmployees() {
        QDialog dialog(this);
        dialog.setWindowTitle("Сортировка сотрудников");
        QFormLayout layout(&dialog);

        QComboBox *keyCombo = new QComboBox(&dialog);
        keyCombo->addItems({"По ID", "По отделу", "По зарплате", "По фамилии", "По дате найма"});
        
        QCheckBox *descCheck = new QCheckBox("По убыванию", &dialog);
        layout.addRow("Ключ сортировки:", keyCombo);
        layout.addRow("", descCheck);

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            EmployeeSortKey key = static_cast<EmployeeSortKey>(keyCombo->currentIndex());
            auto sorted = manager->sortEmployees(key, descCheck->isChecked());
            showSortedResults(sorted);
        }
    }

void MainWindow::showSortedResults(const std::vector<EmployeeRecord>& results) {
        QDialog dialog(this);
        dialog.setWindowTitle("Отсортированные сотрудники");
        dialog.resize(800, 600);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        QTableWidget *table = new QTableWidget(&dialog);
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"ID", "ФИО", "Отдел", "Зарплата", "Тип"});
        table->setRowCount(static_cast<int>(results.size()));

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& record = results[i];
            if (!record.employee) continue;

            QString deptName = "Неизвестно";
            for (const auto& dept : manager->getDepartments()) {
                if (dept.getDepartmentId() == record.employee->getDepartmentId()) {
                    deptName = QString::fromStdString(dept.getDepartmentName());
                    break;
                }
            }

            table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(record.employee->getEmployeeId())));
            table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(record.employee->getFullName())));
            table->setItem(static_cast<int>(i), 2, new QTableWidgetItem(deptName));
            QTableWidgetItem *salaryItem = new QTableWidgetItem("$" + QString::number(record.employee->getSalary(), 'f', 2));
            salaryItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(static_cast<int>(i), 3, salaryItem);
            
            QString type = "Работник";
            if (std::dynamic_pointer_cast<Manager>(record.employee)) {
                type = "Руководитель";
            }
            table->setItem(static_cast<int>(i), 4, new QTableWidgetItem(type));
        }

        table->setAlternatingRowColors(true);
        table->setStyleSheet(
            "QTableWidget { "
            "    gridline-color: #b0b0b0; "
            "    background-color: #ffffff; "
            "    color: #000000; "
            "} "
            "QTableWidget::item { "
            "    padding: 6px; "
            "    color: #000000; "
            "    background-color: #ffffff; "
            "} "
            "QTableWidget::item:alternate { "
            "    background-color: #f5f5f5; "
            "    color: #000000; "
            "} "
            "QTableWidget::item:selected { "
            "    background-color: #2196F3; "
            "    color: #ffffff; "
            "} "
            "QHeaderView::section { "
            "    background-color: #2c3e50; "
            "    color: #ffffff; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    font-size: 11pt; "
            "    border: 1px solid #1a252f; "
            "} "
            "QHeaderView::section:hover { "
            "    background-color: #34495e; "
            "}"
        );
        table->resizeColumnsToContents();
        layout->addWidget(table);

        QDialogButtonBox buttons(QDialogButtonBox::Ok);
        layout->addWidget(&buttons);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

        dialog.exec();
    }

void MainWindow::showAverageSalary() {
        QDialog dialog(this);
        dialog.setWindowTitle("Средняя зарплата по отделу");
        QFormLayout layout(&dialog);

        QComboBox *deptCombo = new QComboBox(&dialog);
        for (const auto& dept : manager->getDepartments()) {
            deptCombo->addItem(QString::fromStdString(dept.getDepartmentName()), dept.getDepartmentId());
        }
        layout.addRow("Отдел:", deptCombo);

        QLabel *resultLabel = new QLabel("", &dialog);
        layout.addRow("Результат:", resultLabel);

        QPushButton *calcBtn = new QPushButton("Рассчитать", &dialog);
        connect(calcBtn, &QPushButton::clicked, [this, deptCombo, resultLabel]() {
            try {
                int deptId = deptCombo->currentData().toInt();
                double avg = manager->calculateAverageSalaryForDepartment(deptId);
                resultLabel->setText(QString("$%1").arg(avg, 0, 'f', 2));
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        });
        layout.addRow("", calcBtn);

        QDialogButtonBox buttons(QDialogButtonBox::Close);
        layout.addRow(&buttons);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        dialog.exec();
    }

void MainWindow::showEfficiencyRating() {
        try {
            auto rating = manager->buildDepartmentsEfficiencyRating();
            QDialog dialog(this);
            dialog.setWindowTitle("Рейтинг отделов по эффективности");
            dialog.resize(600, 400);
            QVBoxLayout *layout = new QVBoxLayout(&dialog);

            QTextEdit *textEdit = new QTextEdit(&dialog);
            textEdit->setReadOnly(true);
            QString text = "<h2>Рейтинг отделов по эффективности затрат</h2><table border='1'><tr><th>Место</th><th>Отдел</th><th>Эффективность</th></tr>";
            
            int place = 1;
            for (const auto& pair : rating) {
                QString deptName = "Неизвестно";
                for (const auto& dept : manager->getDepartments()) {
                    if (dept.getDepartmentId() == pair.first) {
                        deptName = QString::fromStdString(dept.getDepartmentName());
                        break;
                    }
                }
                text += QString("<tr><td>%1</td><td>%2 (ID: %3)</td><td>%4</td></tr>")
                    .arg(place++).arg(deptName).arg(pair.first).arg(pair.second, 0, 'f', 6);
            }
            text += "</table>";
            textEdit->setHtml(text);
            layout->addWidget(textEdit);

            QDialogButtonBox buttons(QDialogButtonBox::Ok);
            layout->addWidget(&buttons);
            connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

            dialog.exec();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
        }
    }

void MainWindow::undoLastAction() {
        try {
            if (manager->canUndo()) {
                manager->undoLastAction();
                manager->saveAll();
                refreshAll();
                QMessageBox::information(this, "Успех", "Действие отменено");
            } else {
                QMessageBox::information(this, "Информация", "Нет действий для отмены");
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
        }
    }
void MainWindow::refreshAll() {
        // Обновление таблицы сотрудников
        employeesTable->setRowCount(0);
        for (const auto& record : manager->getEmployees()) {
            if (!record.employee) continue;
            int row = employeesTable->rowCount();
            employeesTable->insertRow(row);
            
            QString deptName = "Неизвестно";
            for (const auto& dept : manager->getDepartments()) {
                if (dept.getDepartmentId() == record.employee->getDepartmentId()) {
                    deptName = QString::fromStdString(dept.getDepartmentName());
                    break;
                }
            }

            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(record.employee->getEmployeeId()));
            idItem->setTextAlignment(Qt::AlignCenter);
            employeesTable->setItem(row, 0, idItem);
            
            employeesTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(record.employee->getFullName())));
            employeesTable->setItem(row, 2, new QTableWidgetItem(deptName));
            
            QTableWidgetItem *salaryItem = new QTableWidgetItem("$" + QString::number(record.employee->getSalary(), 'f', 2));
            salaryItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            employeesTable->setItem(row, 3, salaryItem);
            
            QString type = "Работник";
            QColor typeColor = QColor(76, 175, 80);
            if (std::dynamic_pointer_cast<Manager>(record.employee)) {
                type = "Руководитель";
                typeColor = QColor(33, 150, 243);
            }
            QTableWidgetItem *typeItem = new QTableWidgetItem(type);
            typeItem->setTextAlignment(Qt::AlignCenter);
            typeItem->setForeground(typeColor);
            employeesTable->setItem(row, 4, typeItem);
        }
        employeesTable->resizeColumnsToContents();
        employeesTable->setColumnWidth(1, 200); // ФИО шире
        employeesTable->setColumnWidth(2, 150); // Отдел

        // Обновление таблицы отделов
        departmentsTable->setRowCount(0);
        for (const auto& dept : manager->getDepartments()) {
            int row = departmentsTable->rowCount();
            departmentsTable->insertRow(row);
            
            QString managerName = "Не назначен";
            for (const auto& record : manager->getEmployees()) {
                if (record.employee && record.employee->getEmployeeId() == dept.getManagerId()) {
                    managerName = QString::fromStdString(record.employee->getFullName());
                    break;
                }
            }
            
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(dept.getDepartmentId()));
            idItem->setTextAlignment(Qt::AlignCenter);
            departmentsTable->setItem(row, 0, idItem);
            
            departmentsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(dept.getDepartmentName())));
            departmentsTable->setItem(row, 2, new QTableWidgetItem(managerName));
            
            QTableWidgetItem *countItem = new QTableWidgetItem(QString::number(dept.getEmployeesCount()));
            countItem->setTextAlignment(Qt::AlignCenter);
            departmentsTable->setItem(row, 3, countItem);
        }
        departmentsTable->resizeColumnsToContents();
        departmentsTable->setColumnWidth(1, 200); // Название шире

        // Обновление таблицы должностей
        positionsTable->setRowCount(0);
        for (const auto& pos : manager->getPositions()) {
            int row = positionsTable->rowCount();
            positionsTable->insertRow(row);
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(pos.getPositionId()));
            idItem->setTextAlignment(Qt::AlignCenter);
            positionsTable->setItem(row, 0, idItem);
            
            positionsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pos.getPositionName())));
            
            QTableWidgetItem *hoursItem = new QTableWidgetItem(QString::number(pos.getWorkHoursPerWeek()) + " ч/нед");
            hoursItem->setTextAlignment(Qt::AlignCenter);
            positionsTable->setItem(row, 2, hoursItem);
        }
        positionsTable->resizeColumnsToContents();
        positionsTable->setColumnWidth(1, 200); // Название шире
    }

void MainWindow::setupUI() {
        setWindowTitle("Система кадрового учета");
        setMinimumSize(1200, 700);

        QWidget *central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // Меню
        QMenuBar *menu = menuBar();
        
        QMenu *fileMenu = menu->addMenu("Файл");
        QAction *saveAction = fileMenu->addAction("Сохранить");
        connect(saveAction, &QAction::triggered, [this]() {
            try {
                manager->saveAll();
                QMessageBox::information(this, "Успех", "Данные сохранены");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        });

        QMenu *editMenu = menu->addMenu("Правка");
        QAction *undoAction = editMenu->addAction("Отменить");
        connect(undoAction, &QAction::triggered, this, &MainWindow::undoLastAction);

        QMenu *analyticsMenu = menu->addMenu("Аналитика");
        QAction *avgAction = analyticsMenu->addAction("Средняя зарплата по отделу");
        QAction *ratingAction = analyticsMenu->addAction("Рейтинг отделов");
        connect(avgAction, &QAction::triggered, this, &MainWindow::showAverageSalary);
        connect(ratingAction, &QAction::triggered, this, &MainWindow::showEfficiencyRating);

        QMenu *toolsMenu = menu->addMenu("Инструменты");
        QAction *searchAction = toolsMenu->addAction("Поиск сотрудников");
        QAction *sortAction = toolsMenu->addAction("Сортировка сотрудников");
        connect(searchAction, &QAction::triggered, this, &MainWindow::searchEmployees);
        connect(sortAction, &QAction::triggered, this, &MainWindow::sortEmployees);

        // Кнопки для сотрудников
        QGroupBox *empGroup = new QGroupBox("Сотрудники", this);
        QHBoxLayout *empLayout = new QHBoxLayout(empGroup);
        QPushButton *addEmpBtn = new QPushButton("Добавить");
        QPushButton *editEmpBtn = new QPushButton("Редактировать");
        QPushButton *removeEmpBtn = new QPushButton("Удалить");
        QPushButton *resetEmpBtn = new QPushButton("Сбросить все");
        resetEmpBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 6px; font-weight: bold; }");
        connect(addEmpBtn, &QPushButton::clicked, this, &MainWindow::addEmployee);
        connect(editEmpBtn, &QPushButton::clicked, this, &MainWindow::editEmployee);
        connect(removeEmpBtn, &QPushButton::clicked, this, &MainWindow::removeEmployee);
        connect(resetEmpBtn, &QPushButton::clicked, this, &MainWindow::resetEmployees);
        empLayout->addWidget(addEmpBtn);
        empLayout->addWidget(editEmpBtn);
        empLayout->addWidget(removeEmpBtn);
        empLayout->addWidget(resetEmpBtn);
        empLayout->addStretch();

        // Кнопки для отделов
        QGroupBox *deptGroup = new QGroupBox("Отделы", this);
        QHBoxLayout *deptLayout = new QHBoxLayout(deptGroup);
        QPushButton *addDeptBtn = new QPushButton("Добавить");
        QPushButton *editDeptBtn = new QPushButton("Редактировать");
        QPushButton *removeDeptBtn = new QPushButton("Удалить");
        QPushButton *resetDeptBtn = new QPushButton("Сбросить все");
        resetDeptBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 6px; font-weight: bold; }");
        connect(addDeptBtn, &QPushButton::clicked, this, &MainWindow::addDepartment);
        connect(editDeptBtn, &QPushButton::clicked, this, &MainWindow::editDepartment);
        connect(removeDeptBtn, &QPushButton::clicked, this, &MainWindow::removeDepartment);
        connect(resetDeptBtn, &QPushButton::clicked, this, &MainWindow::resetDepartments);
        deptLayout->addWidget(addDeptBtn);
        deptLayout->addWidget(editDeptBtn);
        deptLayout->addWidget(removeDeptBtn);
        deptLayout->addWidget(resetDeptBtn);
        deptLayout->addStretch();

        // Кнопки для должностей
        QGroupBox *posGroup = new QGroupBox("Должности", this);
        QHBoxLayout *posLayout = new QHBoxLayout(posGroup);
        QPushButton *addPosBtn = new QPushButton("Добавить");
        QPushButton *editPosBtn = new QPushButton("Редактировать");
        QPushButton *removePosBtn = new QPushButton("Удалить");
        QPushButton *resetPosBtn = new QPushButton("Сбросить все");
        resetPosBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 6px; font-weight: bold; }");
        connect(addPosBtn, &QPushButton::clicked, this, &MainWindow::addPosition);
        connect(editPosBtn, &QPushButton::clicked, this, &MainWindow::editPosition);
        connect(removePosBtn, &QPushButton::clicked, this, &MainWindow::removePosition);
        connect(resetPosBtn, &QPushButton::clicked, this, &MainWindow::resetPositions);
        posLayout->addWidget(addPosBtn);
        posLayout->addWidget(editPosBtn);
        posLayout->addWidget(removePosBtn);
        posLayout->addWidget(resetPosBtn);
        posLayout->addStretch();

        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->addWidget(empGroup);
        btnLayout->addWidget(deptGroup);
        btnLayout->addWidget(posGroup);
        mainLayout->addLayout(btnLayout);

        // Кнопки для поиска, сортировки и аналитики
        QGroupBox *toolsGroup = new QGroupBox("Поиск, сортировка и аналитика", this);
        QHBoxLayout *toolsLayout = new QHBoxLayout(toolsGroup);
        QPushButton *searchBtn = new QPushButton("🔍 Поиск сотрудников", this);
        QPushButton *sortBtn = new QPushButton("📊 Сортировка", this);
        QPushButton *avgSalaryBtn = new QPushButton("💰 Средняя зарплата", this);
        QPushButton *efficiencyBtn = new QPushButton("📈 Рейтинг отделов", this);
        QPushButton *undoBtn = new QPushButton("↶ Отменить", this);
        searchBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #45a049; }");
        sortBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #0b7dda; }");
        avgSalaryBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #e68900; }");
        efficiencyBtn->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #7b1fa2; }");
        undoBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #da190b; }");
        connect(searchBtn, &QPushButton::clicked, this, &MainWindow::searchEmployees);
        connect(sortBtn, &QPushButton::clicked, this, &MainWindow::sortEmployees);
        connect(avgSalaryBtn, &QPushButton::clicked, this, &MainWindow::showAverageSalary);
        connect(efficiencyBtn, &QPushButton::clicked, this, &MainWindow::showEfficiencyRating);
        connect(undoBtn, &QPushButton::clicked, this, &MainWindow::undoLastAction);
        toolsLayout->addWidget(searchBtn);
        toolsLayout->addWidget(sortBtn);
        toolsLayout->addWidget(avgSalaryBtn);
        toolsLayout->addWidget(efficiencyBtn);
        toolsLayout->addWidget(undoBtn);
        toolsLayout->addStretch();
        mainLayout->addWidget(toolsGroup);

        // Таблицы
        QTabWidget *tabs = new QTabWidget;
        
        employeesTable = new QTableWidget;
        employeesTable->setColumnCount(5);
        employeesTable->setHorizontalHeaderLabels({"ID", "ФИО", "Отдел", "Зарплата", "Тип"});
        employeesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        employeesTable->setSelectionMode(QAbstractItemView::SingleSelection);
        employeesTable->setAlternatingRowColors(true);
        employeesTable->setStyleSheet(
            "QTableWidget { "
            "    gridline-color: #b0b0b0; "
            "    background-color: #ffffff; "
            "    color: #000000; "
            "} "
            "QTableWidget::item { "
            "    padding: 6px; "
            "    color: #000000; "
            "    background-color: #ffffff; "
            "} "
            "QTableWidget::item:alternate { "
            "    background-color: #f5f5f5; "
            "    color: #000000; "
            "} "
            "QTableWidget::item:selected { "
            "    background-color: #2196F3; "
            "    color: #ffffff; "
            "} "
            "QHeaderView::section { "
            "    background-color: #2c3e50; "
            "    color: #ffffff; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    font-size: 11pt; "
            "    border: 1px solid #1a252f; "
            "} "
            "QHeaderView::section:hover { "
            "    background-color: #34495e; "
            "}"
        );
        employeesTable->horizontalHeader()->setStretchLastSection(true);
        employeesTable->setSortingEnabled(true);
        tabs->addTab(employeesTable, "Сотрудники");

        departmentsTable = new QTableWidget;
        departmentsTable->setColumnCount(4);
        departmentsTable->setHorizontalHeaderLabels({"ID", "Название", "Руководитель", "Сотрудников"});
        departmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        departmentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        departmentsTable->setAlternatingRowColors(true);
        departmentsTable->setStyleSheet(
            "QTableWidget { "
            "    gridline-color: #b0b0b0; "
            "    background-color: #ffffff; "
            "    color: #000000; "
            "} "
            "QTableWidget::item { "
            "    padding: 6px; "
            "    color: #000000; "
            "    background-color: #ffffff; "
            "} "
            "QTableWidget::item:alternate { "
            "    background-color: #f5f5f5; "
            "    color: #000000; "
            "} "
            "QTableWidget::item:selected { "
            "    background-color: #2196F3; "
            "    color: #ffffff; "
            "} "
            "QHeaderView::section { "
            "    background-color: #2c3e50; "
            "    color: #ffffff; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    font-size: 11pt; "
            "    border: 1px solid #1a252f; "
            "} "
            "QHeaderView::section:hover { "
            "    background-color: #34495e; "
            "}"
        );
        departmentsTable->horizontalHeader()->setStretchLastSection(true);
        departmentsTable->setSortingEnabled(true);
        tabs->addTab(departmentsTable, "Отделы");

        positionsTable = new QTableWidget;
        positionsTable->setColumnCount(3);
        positionsTable->setHorizontalHeaderLabels({"ID", "Название", "Часов/неделю"});
        positionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        positionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        positionsTable->setAlternatingRowColors(true);
        positionsTable->setStyleSheet(
            "QTableWidget { "
            "    gridline-color: #b0b0b0; "
            "    background-color: #ffffff; "
            "    color: #000000; "
            "} "
            "QTableWidget::item { "
            "    padding: 6px; "
            "    color: #000000; "
            "    background-color: #ffffff; "
            "} "
            "QTableWidget::item:alternate { "
            "    background-color: #f5f5f5; "
            "    color: #000000; "
            "} "
            "QTableWidget::item:selected { "
            "    background-color: #2196F3; "
            "    color: #ffffff; "
            "} "
            "QHeaderView::section { "
            "    background-color: #2c3e50; "
            "    color: #ffffff; "
            "    padding: 10px; "
            "    font-weight: bold; "
            "    font-size: 11pt; "
            "    border: 1px solid #1a252f; "
            "} "
            "QHeaderView::section:hover { "
            "    background-color: #34495e; "
            "}"
        );
        positionsTable->horizontalHeader()->setStretchLastSection(true);
        positionsTable->setSortingEnabled(true);
        tabs->addTab(positionsTable, "Должности");

        mainLayout->addWidget(tabs);
    }
