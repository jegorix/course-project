#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * Базовое исключение приложения.
 * Наследуется от std::runtime_error, чтобы его можно было перехватывать
 * как стандартное исключение.
 */
class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * Ошибка валидации пользовательских данных.
 */
class ValidationException : public AppException {
public:
    explicit ValidationException(const std::string& message)
        : AppException(message) {}
};

/**
 * Обращение к несуществующей сущности (сотрудник, отдел, должность и т.д.).
 */
class EntityNotFoundException : public AppException {
public:
    explicit EntityNotFoundException(const std::string& message)
        : AppException(message) {}

    EntityNotFoundException(const std::string& entityName, int id)
        : AppException(entityName + " с id=" + std::to_string(id) + " не найдена") {}
};

/**
 * Нарушение бизнес-правил (например, попытка удалить отдел с сотрудниками).
 */
class OperationNotAllowedException : public AppException {
public:
    explicit OperationNotAllowedException(const std::string& message)
        : AppException(message) {}
};

/**
 * Проблемы доступа к файлам (загрузка/сохранение).
 */
class FileAccessException : public AppException {
public:
    explicit FileAccessException(const std::string& message)
        : AppException(message) {}
};

/**
 * Ошибка формата/парсинга данных.
 */
class DataFormatException : public AppException {
public:
    explicit DataFormatException(const std::string& message)
        : AppException(message) {}
};

/**
 * Ошибка выхода итератора за пределы коллекции.
 */
class CollectionBoundsException : public AppException {
public:
    explicit CollectionBoundsException(const std::string& message)
        : AppException(message) {}
};

#endif // EXCEPTIONS_H
