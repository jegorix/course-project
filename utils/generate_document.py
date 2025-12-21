from pathlib import Path

from docx import Document
from docx.shared import Pt, Cm
from docx.enum.text import WD_LINE_SPACING

NBSP = "\u00A0"  # неразрывный пробел


def set_font(run, name: str, bold: bool = False):
    run.font.name = name
    run.font.size = Pt(14)
    run.font.bold = bold


def make_header_paragraph(doc: Document, text: str):
    p = doc.add_paragraph()
    pf = p.paragraph_format
    pf.left_indent = None
    pf.first_line_indent = None
    pf.line_spacing_rule = WD_LINE_SPACING.SINGLE
    pf.space_after = Pt(0)
    r = p.add_run(text)
    set_font(r, "Times New Roman")
    return p


def make_normal_paragraph(doc: Document, text: str):
    """Обычный абзац с красной строкой 1.25 см, TNR 14."""
    p = doc.add_paragraph()
    pf = p.paragraph_format
    pf.left_indent = None
    pf.first_line_indent = Cm(1.25)
    pf.line_spacing_rule = WD_LINE_SPACING.SINGLE
    pf.space_after = Pt(0)
    r = p.add_run(text)
    set_font(r, "Times New Roman")
    return p


def make_class_heading(doc: Document, kind: str, name: str):
    """
    Заголовок: "Класс Name:" или "Структура Name:"
    'Класс/Структура' — Times New Roman, жирный
    Name: — Courier New
    """
    p = doc.add_paragraph()
    pf = p.paragraph_format
    pf.left_indent = None
    pf.first_line_indent = Cm(1.25)
    pf.line_spacing_rule = WD_LINE_SPACING.SINGLE
    pf.space_after = Pt(0)

    r1 = p.add_run(f"{kind} ")
    set_font(r1, "Times New Roman", bold=True)

    r2 = p.add_run(f"{name}:")
    set_font(r2, "Courier New")

    return p


def add_bullet_block(doc: Document, title: str, items: list[dict]):
    """
    items: список словарей вида {"code": "...", "desc": "..."}.
    code — шрифт кода (Courier New), desc — Times New Roman.
    """
    p_title = make_normal_paragraph(doc, "")
    p_title.clear()
    r = p_title.add_run(title)
    set_font(r, "Times New Roman", bold=True)

    if not items:
        return

    n = len(items)
    for idx, item in enumerate(items):
        last = (idx == n - 1)
        p = doc.add_paragraph()
        pf = p.paragraph_format

        # ВАЖНО: убираем красную строку принудительно
        pf.left_indent = Cm(2.5)
        pf.first_line_indent = Cm(0)
        pf.line_spacing_rule = WD_LINE_SPACING.SINGLE
        pf.space_after = Pt(0)

        code = item.get("code", "").strip()
        desc = (item.get("desc", "") or "").strip()

        # Маркер "– " (с NBSP), Times New Roman
        r0 = p.add_run("–" + NBSP)
        set_font(r0, "Times New Roman")

        # Код — Courier New
        if code:
            r_code = p.add_run(code)
            set_font(r_code, "Courier New")

        ending = "." if last else ";"

        # Описание — Times New Roman
        if desc:
            r_sep = p.add_run(" – ")
            set_font(r_sep, "Times New Roman")

            r_desc = p.add_run(desc + ending)
            set_font(r_desc, "Times New Roman")
        else:
            r_end = p.add_run(ending)
            set_font(r_end, "Times New Roman")


def build_sections() -> list[dict]:
    """
    Формат элемента:
    {
        "kind": "Класс" или "Структура",
        "name": "Имя",
        "desc": "Описание",
        "fields": [{"code": "...", "desc": "..."}, ...],
        "methods": [{"code": "...", "desc": "..."}, ...],
    }
    """
    sections: list[dict] = []

    # === Основные классы предметной области ===

    sections.append({
        "kind": "Класс",
        "name": "Person",
        "desc": "Базовый класс для представления человека. Содержит общее для всех сущностей имя и фамилию.",
        "fields": [
            {"code": "std::string firstName", "desc": "имя"},
            {"code": "std::string lastName", "desc": "фамилия"},
        ],
        "methods": [
            {"code": "Person()", "desc": "конструктор по умолчанию"},
            {"code": "Person(const std::string& firstName, const std::string& lastName)", "desc": "конструктор с параметрами"},
            {"code": "virtual ~Person()", "desc": "виртуальный деструктор"},
            {"code": "std::string getFirstName() const", "desc": "получение имени"},
            {"code": "std::string getLastName() const", "desc": "получение фамилии"},
            {"code": "std::string getFullName() const", "desc": "получение полного имени"},
            {"code": "void setFirstName(const std::string& firstName)", "desc": "установка имени"},
            {"code": "void setLastName(const std::string& lastName)", "desc": "установка фамилии"},
            {"code": "virtual void displayInfo() const = 0", "desc": "виртуальный вывод информации"},
        ],
    })

    # 2. Employee
    sections.append({
        "kind": "Класс",
        "name": "Employee",
        "desc": "Производный от Person, описывает сотрудника предприятия.",
        "fields": [
            {"code": "int employeeId", "desc": "идентификатор сотрудника"},
            {"code": "int departmentId", "desc": "идентификатор отдела"},
            {"code": "double salary", "desc": "должностной оклад"},
        ],
        "methods": [
            {"code": "Employee()", "desc": "конструктор по умолчанию"},
            {"code": "Employee(int employeeId, const std::string& firstName, const std::string& lastName, int departmentId, double salary)", "desc": "конструктор с параметрами"},
            {"code": "virtual ~Employee()", "desc": "деструктор"},
            {"code": "int getEmployeeId() const", "desc": "получение id"},
            {"code": "int getDepartmentId() const", "desc": "получение id отдела"},
            {"code": "double getSalary() const", "desc": "получение оклада"},
            {"code": "void setEmployeeId(int employeeId)", "desc": "установка id"},
            {"code": "void setDepartmentId(int departmentId)", "desc": "установка отдела"},
            {"code": "void setSalary(double salary)", "desc": "установка оклада"},
            {"code": "virtual void displayInfo() const override", "desc": "вывод информации"},
            {"code": "virtual double calculateTotalIncome() const", "desc": "расчет итогового дохода"},
        ],
    })

    # 3. Worker
    sections.append({
        "kind": "Класс",
        "name": "Worker",
        "desc": "Производный от Employee, представляет рядового сотрудника.",
        "fields": [
            {"code": "int positionId", "desc": "идентификатор должности"},
            {"code": "double bonus", "desc": "размер премии"},
        ],
        "methods": [
            {"code": "Worker()", "desc": "конструктор по умолчанию"},
            {"code": "Worker(int employeeId, const std::string& firstName, const std::string& lastName, int departmentId, double salary, int positionId, double bonus = 0.0)", "desc": "конструктор с параметрами"},
            {"code": "~Worker()", "desc": "деструктор"},
            {"code": "int getPositionId() const", "desc": "получение id должности"},
            {"code": "double getBonus() const", "desc": "получение премии"},
            {"code": "void setPositionId(int positionId)", "desc": "установка должности"},
            {"code": "void setBonus(double bonus)", "desc": "установка премии"},
            {"code": "void displayInfo() const override", "desc": "вывод информации"},
            {"code": "double calculateTotalIncome() const override", "desc": "расчет дохода с учетом премии"},
        ],
    })

    # 4. Manager
    sections.append({
        "kind": "Класс",
        "name": "Manager",
        "desc": "Производный от Employee, описывает руководителя отдела.",
        "fields": [
            {"code": "std::vector<int> subordinates", "desc": "список id подчиненных"},
        ],
        "methods": [
            {"code": "Manager()", "desc": "конструктор по умолчанию"},
            {"code": "Manager(int employeeId, const std::string& firstName, const std::string& lastName, int departmentId, double salary, const std::vector<int>& subordinates)", "desc": "конструктор с параметрами"},
            {"code": "~Manager()", "desc": "деструктор"},
            {"code": "void addSubordinate(int subordinateId)", "desc": "добавление подчиненного"},
            {"code": "void removeSubordinate(int subordinateId)", "desc": "удаление подчиненного"},
            {"code": "std::vector<int> getSubordinates() const", "desc": "получение списка подчиненных"},
            {"code": "int getSubordinatesCount() const", "desc": "количество подчиненных"},
            {"code": "bool hasSubordinate(int subordinateId) const", "desc": "проверка наличия подчиненного"},
            {"code": "void displayInfo() const override", "desc": "вывод информации"},
            {"code": "double calculateTotalIncome() const override", "desc": "расчет дохода (оклад, без премий работников)"},
        ],
    })

    # 5. Department
    sections.append({
        "kind": "Класс",
        "name": "Department",
        "desc": "Описывает отдел предприятия и его сотрудников.",
        "fields": [
            {"code": "int departmentId", "desc": "идентификатор отдела"},
            {"code": "std::string departmentName", "desc": "название отдела"},
            {"code": "int managerId", "desc": "идентификатор руководителя"},
            {"code": "std::vector<int> employees", "desc": "список id сотрудников отдела"},
        ],
        "methods": [
            {"code": "Department()", "desc": "конструктор по умолчанию"},
            {"code": "Department(int departmentId, const std::string& departmentName, int managerId)", "desc": "конструктор с параметрами"},
            {"code": "~Department()", "desc": "деструктор"},
            {"code": "int getDepartmentId() const", "desc": "получение id"},
            {"code": "std::string getDepartmentName() const", "desc": "получение названия"},
            {"code": "int getManagerId() const", "desc": "получение id руководителя"},
            {"code": "std::vector<int> getEmployees() const", "desc": "получение списка сотрудников"},
            {"code": "int getEmployeesCount() const", "desc": "количество сотрудников"},
            {"code": "void setDepartmentId(int departmentId)", "desc": "установка id"},
            {"code": "void setDepartmentName(const std::string& departmentName)", "desc": "установка названия"},
            {"code": "void setManagerId(int managerId)", "desc": "установка руководителя"},
            {"code": "void addEmployee(int employeeId)", "desc": "добавление сотрудника"},
            {"code": "void removeEmployee(int employeeId)", "desc": "удаление сотрудника"},
            {"code": "bool hasEmployee(int employeeId) const", "desc": "проверка наличия сотрудника"},
            {"code": "void clearEmployees()", "desc": "очистка списка"},
            {"code": "void displayInfo() const", "desc": "вывод информации"},
        ],
    })

    # 6. Position
    sections.append({
        "kind": "Класс",
        "name": "Position",
        "desc": "Представляет должность с графиком работы.",
        "fields": [
            {"code": "int positionId", "desc": "идентификатор должности"},
            {"code": "std::string positionName", "desc": "название должности"},
            {"code": "int workHoursPerWeek", "desc": "недельная норма часов"},
        ],
        "methods": [
            {"code": "Position()", "desc": "конструктор по умолчанию"},
            {"code": "Position(int positionId, const std::string& positionName, int workHoursPerWeek)", "desc": "конструктор с параметрами"},
            {"code": "~Position()", "desc": "деструктор"},
            {"code": "int getPositionId() const", "desc": "получение id"},
            {"code": "std::string getPositionName() const", "desc": "получение названия"},
            {"code": "int getWorkHoursPerWeek() const", "desc": "получение нормы часов"},
            {"code": "void setPositionId(int positionId)", "desc": "установка id"},
            {"code": "void setPositionName(const std::string& positionName)", "desc": "установка названия"},
            {"code": "void setWorkHoursPerWeek(int workHoursPerWeek)", "desc": "установка нормы часов"},
            {"code": "void displayInfo() const", "desc": "вывод информации"},
        ],
    })

    # 7. HireDate
    sections.append({
        "kind": "Класс",
        "name": "HireDate",
        "desc": "Хранит дату и время приема сотрудника на работу.",
        "fields": [
            {"code": "int employeeId", "desc": "идентификатор сотрудника"},
            {"code": "int day", "desc": "день"},
            {"code": "int month", "desc": "месяц"},
            {"code": "int year", "desc": "год"},
            {"code": "int hour", "desc": "час"},
            {"code": "int minute", "desc": "минуты"},
        ],
        "methods": [
            {"code": "HireDate()", "desc": "конструктор по умолчанию"},
            {"code": "HireDate(int employeeId, int day, int month, int year, int hour, int minute)", "desc": "конструктор с параметрами"},
            {"code": "~HireDate()", "desc": "деструктор"},
            {"code": "int getEmployeeId() const", "desc": "получение id"},
            {"code": "int getDay() const", "desc": "получение дня"},
            {"code": "int getMonth() const", "desc": "получение месяца"},
            {"code": "int getYear() const", "desc": "получение года"},
            {"code": "int getHour() const", "desc": "получение часа"},
            {"code": "int getMinute() const", "desc": "получение минут"},
            {"code": "void setEmployeeId(int employeeId)", "desc": "установка id"},
            {"code": "void setDay(int day)", "desc": "установка дня"},
            {"code": "void setMonth(int month)", "desc": "установка месяца"},
            {"code": "void setYear(int year)", "desc": "установка года"},
            {"code": "void setHour(int hour)", "desc": "установка часа"},
            {"code": "void setMinute(int minute)", "desc": "установка минут"},
            {"code": "std::string getDateString() const", "desc": "форматированная дата"},
            {"code": "std::string getTimeString() const", "desc": "форматированное время"},
            {"code": "std::string getFullDateTimeString() const", "desc": "дата и время"},
            {"code": "void displayInfo() const", "desc": "вывод информации"},
        ],
    })

    # 8. RecordCollection<T>
    sections.append({
        "kind": "Класс",
        "name": "RecordCollection<T>",
        "desc": "Шаблонный контейнер с пользовательским итератором, обеспечивает хранение записей и базовые алгоритмы.",
        "fields": [
            {"code": "std::vector<T> records", "desc": "внутреннее хранилище"},
        ],
        "methods": [
            {"code": "RecordCollection()", "desc": "конструктор по умолчанию"},
            {"code": "explicit RecordCollection(std::vector<T> values)", "desc": "конструктор из набора данных"},
            {"code": "Iterator begin()", "desc": "итератор на начало с проверкой границ"},
            {"code": "Iterator end()", "desc": "итератор на конец с проверкой границ"},
            {"code": "std::size_t size() const", "desc": "размер коллекции"},
            {"code": "bool empty() const", "desc": "проверка пустоты"},
            {"code": "void reserve(std::size_t capacity)", "desc": "резервирование памяти"},
            {"code": "void add(const T& value)", "desc": "добавление записи (копированием)"},
            {"code": "void add(T&& value)", "desc": "добавление записи (перемещением)"},
            {"code": "T& at(std::size_t index)", "desc": "доступ по индексу с проверкой"},
            {"code": "T& operator[](std::size_t index)", "desc": "доступ по индексу без проверки"},
            {"code": "void clear()", "desc": "очистка коллекции"},
            {"code": "template <typename Predicate> Iterator findIf(Predicate predicate)", "desc": "поиск по условию"},
            {"code": "template <typename Predicate> void removeIf(Predicate predicate)", "desc": "удаление по условию"},
            {"code": "template <typename Compare> void insertionSort(Compare compare)", "desc": "сортировка вставками"},
            {"code": "template <typename Function> void forEach(Function fn)", "desc": "обход записей"},
            {"code": "std::vector<T>& data()", "desc": "прямой доступ к данным"},
            {"code": "const std::vector<T>& data() const", "desc": "прямой доступ к данным (const)"},
        ],
    })

    # 9. DataManager
    sections.append({
        "kind": "Класс",
        "name": "DataManager",
        "desc": "Менеджер данных приложения: загрузка/сохранение, CRUD-операции, индексация и управление undo.",
        "fields": [
            {"code": "std::string employeesPath, departmentsPath, positionsPath, hiresPath", "desc": "пути к файлам данных"},
            {"code": "RecordCollection<EmployeeRecord> employees", "desc": "коллекция сотрудников с датами найма"},
            {"code": "RecordCollection<Department> departments", "desc": "коллекция отделов"},
            {"code": "RecordCollection<Position> positions", "desc": "коллекция должностей"},
            {"code": "std::unordered_map<int, std::size_t> employeeIndex", "desc": "индекс для быстрого доступа к сотрудникам"},
            {"code": "std::unordered_map<int, std::size_t> departmentIndex", "desc": "индекс для быстрого доступа к отделам"},
            {"code": "std::unordered_map<int, std::size_t> positionIndex", "desc": "индекс для быстрого доступа к должностям"},
            {"code": "std::vector<UndoCommand> undoStack", "desc": "стек команд отмены"},
            {"code": "int nextEmployeeId, nextDepartmentId, nextPositionId", "desc": "генераторы идентификаторов"},
            {"code": "bool undoInProgress", "desc": "флаг выполнения undo"},
        ],
        "methods": [
            {"code": "explicit DataManager(const std::string& dataDirectory)", "desc": "конструктор с указанием каталога данных"},
            {"code": "void loadAll()", "desc": "загрузка всех файлов"},
            {"code": "void saveAll() const", "desc": "сохранение всех файлов"},
            {"code": "int addWorker(...)", "desc": "добавление работника"},
            {"code": "int addManager(...)", "desc": "добавление руководителя"},
            {"code": "bool removeEmployee(int employeeId)", "desc": "удаление сотрудника"},
            {"code": "bool updateEmployeeDepartment/Salary/Position/Bonus/Name/HireDate(...)", "desc": "обновление полей сотрудника"},
            {"code": "int addDepartment(...)", "desc": "добавление отдела"},
            {"code": "bool removeDepartment(...)", "desc": "удаление отдела"},
            {"code": "bool updateDepartmentName(...)", "desc": "обновление названия отдела"},
            {"code": "bool updateDepartmentManager(...)", "desc": "обновление руководителя отдела"},
            {"code": "int addPosition(...)", "desc": "добавление должности"},
            {"code": "bool removePosition(...)", "desc": "удаление должности"},
            {"code": "bool updatePositionName/Hours(...)", "desc": "обновление параметров должности"},
            {"code": "EmployeeRecord*/const EmployeeRecord* findEmployeeRecord(int employeeId)", "desc": "поиск записи сотрудника"},
            {"code": "Department*/const Department* findDepartment(int departmentId)", "desc": "поиск отдела"},
            {"code": "Position*/const Position* findPosition(int positionId)", "desc": "поиск должности"},
            {"code": "bool canUndo() const", "desc": "проверка возможности undo"},
            {"code": "bool undoLastAction()", "desc": "выполнение отмены последнего действия"},
            {"code": "void pushUndo(std::function<void()> undoAction, const std::string& description)", "desc": "добавление команды отмены"},
            {"code": "void clearAllEmployees()/clearAllDepartments()/clearAllPositions()", "desc": "очистка данных"},
            {"code": "std::shared_ptr<Employee> cloneEmployee(const Employee& source) const", "desc": "копирование сотрудника"},
            {"code": "void restoreEmployeeFromCopy(int employeeId, std::shared_ptr<Employee> employeeCopy, const HireDate& hireDateCopy)", "desc": "восстановление записи"},
        ],
    })

    # 10. FileManager
    sections.append({
        "kind": "Класс",
        "name": "FileManager",
        "desc": "Сервис для загрузки и сохранения данных (сотрудники, отделы, должности, даты найма) в текстовые файлы.",
        "fields": [],
        "methods": [
            {"code": "FileManager()", "desc": "конструктор по умолчанию"},
            {"code": "void loadAll(DataManager& manager) const", "desc": "загрузка всех коллекций в DataManager"},
            {"code": "void saveAll(const DataManager& manager) const", "desc": "сохранение всех коллекций из DataManager"},
            {"code": "void loadPositions/DataManager/Employees/HireDates(...)", "desc": "загрузка отдельных сущностей"},
            {"code": "void savePositions/Departments/Employees/HireDates(...)", "desc": "сохранение отдельных сущностей"},
        ],
    })

    # 11. Algorithm
    sections.append({
        "kind": "Класс",
        "name": "Algorithm",
        "desc": "Сервис поиска, сортировки и аналитики по коллекциям сотрудников и отделов.",
        "fields": [],
        "methods": [
            {"code": "Algorithm()", "desc": "конструктор по умолчанию"},
            {"code": "std::vector<EmployeeRecord> findEmployees(const RecordCollection<EmployeeRecord>&, const EmployeeSearchFilter&) const", "desc": "поиск по фильтрам"},
            {"code": "std::vector<EmployeeRecord> sortEmployees(const RecordCollection<EmployeeRecord>&, EmployeeSortKey, bool descending) const", "desc": "сортировка сотрудников"},
            {"code": "double calculateAverageSalaryForDepartment(const RecordCollection<EmployeeRecord>&, int departmentId, const Department*) const", "desc": "средняя зарплата по отделу"},
            {"code": "std::vector<std::pair<int, double>> buildDepartmentsEfficiencyRating(const RecordCollection<EmployeeRecord>&, const RecordCollection<Department>&) const", "desc": "рейтинг эффективности отделов"},
        ],
    })

    # === Классы интерфейса (GUI) ===

    # 12. MainWindow (GUI)
    sections.append({
        "kind": "Класс",
        "name": "MainWindow",
        "desc": (
            "Главное окно приложения, наследуется от QMainWindow в Qt и управляет "
            "всем взаимодействием с пользователем."
        ),
        "fields": [
            {"code": "std::unique_ptr<DataManager> manager", "desc": "управляющий объект данных"},
            {"code": "std::unique_ptr<Algorithm> algorithm", "desc": "сервис поиска/сортировки/аналитики"},
            {"code": "QTableWidget* employeesTable", "desc": "таблица сотрудников"},
            {"code": "QTableWidget* departmentsTable", "desc": "таблица отделов"},
            {"code": "QTableWidget* positionsTable", "desc": "таблица должностей"},
        ],
        "methods": [
            {"code": "MainWindow(QWidget* parent = nullptr)", "desc": "конструктор, настраивает окно и подключает слоты"},
            {"code": "void setupUI()", "desc": "построение меню, таблиц и кнопок"},
            {"code": "void refreshAll()", "desc": "обновление всех таблиц после изменений"},
            {"code": "void addEmployee()", "desc": "управление записями сотрудников: добавление"},
            {"code": "void editEmployee()", "desc": "управление записями сотрудников: редактирование"},
            {"code": "void removeEmployee()", "desc": "управление записями сотрудников: удаление"},
            {"code": "void resetEmployees()", "desc": "управление записями сотрудников: сброс/очистка"},
            {"code": "void addDepartment()", "desc": "управление отделами: добавление"},
            {"code": "void editDepartment()", "desc": "управление отделами: редактирование"},
            {"code": "void removeDepartment()", "desc": "управление отделами: удаление"},
            {"code": "void resetDepartments()", "desc": "управление отделами: сброс/очистка"},
            {"code": "void addPosition()", "desc": "управление должностями: добавление"},
            {"code": "void editPosition()", "desc": "управление должностями: редактирование"},
            {"code": "void removePosition()", "desc": "управление должностями: удаление"},
            {"code": "void resetPositions()", "desc": "управление должностями: сброс/очистка"},
            {"code": "void searchEmployees()", "desc": "поиск сотрудников по фильтрам (через Algorithm)"},
            {"code": "void sortEmployees()", "desc": "сортировка сотрудников по выбранному ключу (через Algorithm)"},
            {"code": "void showSearchResults(const std::vector<EmployeeRecord>& results)", "desc": "вывод результатов поиска"},
            {"code": "void showSortedResults(const std::vector<EmployeeRecord>& results)", "desc": "вывод отсортированных данных"},
            {"code": "void showAverageSalary()", "desc": "показ средней зарплаты по отделу (через Algorithm)"},
            {"code": "void showEfficiencyRating()", "desc": "показ рейтинга эффективности отделов (через Algorithm)"},
            {"code": "void undoLastAction()", "desc": "отмена последнего действия через стек undo в DataManager"},
        ],
    })

    # === Классы исключений ===
    sections.append({
        "kind": "Класс",
        "name": "AppException",
        "desc": (
            "Базовое исключение приложения (наследуется от std::runtime_error). "
            "Используется для обозначения ошибок валидации, отсутствия сущностей, "
            "нарушения бизнес-правил, проблем файлового доступа, формата данных и "
            "выхода итератора за пределы коллекции."
        ),
        "fields": [
            {"code": "(нет)", "desc": "дополнительных полей не содержит"},
        ],
        "methods": [
            {"code": "explicit AppException(const std::string& message)", "desc": "создать исключение с сообщением"},
            {"code": "virtual ~AppException()", "desc": "деструктор"},
        ],
    })

    sections.append({
        "kind": "Класс",
        "name": "ValidationException",
        "desc": "Исключение ошибок валидации входных данных.",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit ValidationException(const std::string& message)", "desc": "создать исключение валидации"}],
    })

    sections.append({
        "kind": "Класс",
        "name": "EntityNotFoundException",
        "desc": "Исключение, сигнализирующее об отсутствии сущности.",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit EntityNotFoundException(const std::string& message)", "desc": "создать исключение \"не найдено\""}],
    })

    sections.append({
        "kind": "Класс",
        "name": "OperationNotAllowedException",
        "desc": "Исключение нарушения бизнес-правил/запрещённой операции.",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit OperationNotAllowedException(const std::string& message)", "desc": "создать исключение запрета операции"}],
    })

    sections.append({
        "kind": "Класс",
        "name": "FileAccessException",
        "desc": "Исключение проблем файлового доступа/ввода-вывода.",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit FileAccessException(const std::string& message)", "desc": "создать исключение файлового доступа"}],
    })

    sections.append({
        "kind": "Класс",
        "name": "DataFormatException",
        "desc": "Исключение ошибок формата данных (некорректная структура/парсинг).",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit DataFormatException(const std::string& message)", "desc": "создать исключение формата данных"}],
    })

    sections.append({
        "kind": "Класс",
        "name": "CollectionBoundsException",
        "desc": "Исключение выхода итератора/индекса за пределы коллекции.",
        "fields": [{"code": "(нет)", "desc": "наследует AppException"}],
        "methods": [{"code": "explicit CollectionBoundsException(const std::string& message)", "desc": "создать исключение границ коллекции"}],
    })

    return sections


def build_docx(output_path: str):
    doc = Document()

    make_header_paragraph(doc, "2.3 Описание классов")
    make_normal_paragraph(doc, "Основные классы предметной области:")

    sections = build_sections()

    gui_title_inserted = False
    exceptions_title_inserted = False
    service_title_inserted = False
    data_title_inserted = False
    file_title_inserted = False
    algo_title_inserted = False

    for sec in sections:
        name = sec.get("name")

        if (not service_title_inserted) and name == "RecordCollection<T>":
            make_normal_paragraph(doc, "Служебные классы (контейнеры):")
            service_title_inserted = True

        if (not data_title_inserted) and name == "DataManager":
            make_normal_paragraph(doc, "Классы управления данными:")
            data_title_inserted = True

        if (not file_title_inserted) and name == "FileManager":
            make_normal_paragraph(doc, "Классы для работы с файлами:")
            file_title_inserted = True

        if (not algo_title_inserted) and name == "Algorithm":
            make_normal_paragraph(doc, "Классы алгоритмов и аналитики:")
            algo_title_inserted = True

        if (not gui_title_inserted) and name == "MainWindow":
            make_normal_paragraph(doc, "Классы интерфейса (GUI):")
            gui_title_inserted = True

        if (not exceptions_title_inserted) and name == "AppException":
            make_normal_paragraph(doc, "Классы исключений:")
            exceptions_title_inserted = True

        make_class_heading(doc, sec["kind"], sec["name"])
        make_normal_paragraph(doc, sec["desc"])
        add_bullet_block(doc, "Поля:", sec["fields"])
        add_bullet_block(doc, "Методы:", sec["methods"])
        doc.add_paragraph("")  # пустая строка между классами

    doc.save(output_path)


if __name__ == "__main__":
    utils_dir = Path(__file__).resolve().parent
    output_file = utils_dir / "Раздел_2.3_описание_классов.docx"
    build_docx(str(output_file))
    print(f"Готово: {output_file}")
