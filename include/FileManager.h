#ifndef FILEMANAGER_H
#define FILEMANAGER_H

class DataManager;

/**
 * Класс для работы с файловым вводом/выводом данных приложения.
 * Отвечает за загрузку и сохранение коллекций сотрудников, отделов,
 * должностей и дат найма.
 */
class FileManager {
public:
    FileManager() = default;

    void loadAll(DataManager& manager) const;
    void saveAll(const DataManager& manager) const;

private:
    void loadPositions(DataManager& manager) const;
    void loadDepartments(DataManager& manager) const;
    void loadEmployees(DataManager& manager) const;
    void loadHireDates(DataManager& manager) const;

    void savePositions(const DataManager& manager) const;
    void saveDepartments(const DataManager& manager) const;
    void saveEmployees(const DataManager& manager) const;
    void saveHireDates(const DataManager& manager) const;
};

#endif // FILEMANAGER_H
