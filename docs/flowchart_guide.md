# Гайд по созданию блок-схем алгоритмов

## Обозначения типов блоков

- **Овал** `([текст])` — начало/конец алгоритма
- **Прямоугольник** `[текст]` — процесс/действие
- **Ромб** `{текст}` — условие/проверка
- **Параллелограмм** `[/текст/]` — ввод/вывод данных

---

## АЛГОРИТМ 1: Загрузка данных при старте программы

### Блоки и их содержимое:

#### Блок 1 (Овал - Начало)
```
Старт
loadAll()
```
**Связи:**
- → Блок 2 (Определить рабочий каталог)

#### Блок 2 (Прямоугольник - Процесс)
```
Определить рабочий каталог
dataDirectory = detectDataDirectory()
```
**Связи:**
- → Блок 3 (Сформировать пути к файлам)

#### Блок 3 (Прямоугольник - Процесс)
```
Сформировать пути к файлам:
employeesPath = combinePath(dataDir, "employees.txt")
departmentsPath = combinePath(dataDir, "departments.txt")
positionsPath = combinePath(dataDir, "positions.txt")
hiresPath = combinePath(dataDir, "hires.txt")
```
**Связи:**
- → Блок 4 (Очистить контейнеры)

#### Блок 4 (Прямоугольник - Процесс)
```
Очистить контейнеры:
employees.clear()
departments.clear()
positions.clear()
```
**Связи:**
- → Блок 5 (Очистить индексы)

#### Блок 5 (Прямоугольник - Процесс)
```
Очистить индексы:
employeeIndex.clear()
departmentIndex.clear()
positionIndex.clear()
```
**Связи:**
- → Блок 6 (Сбросить счетчики)

#### Блок 6 (Прямоугольник - Процесс)
```
Сбросить счетчики:
nextEmployeeId = 1
nextDepartmentId = 1
nextPositionId = 1
```
**Связи:**
- → Блок 7 (Очистить стек undo)

#### Блок 7 (Прямоугольник - Процесс)
```
Очистить стек undo:
undoStack.clear()
undoInProgress = false
```
**Связи:**
- → Блок 8 (Открыть файл positions.txt)

#### Блок 8 (Прямоугольник - Процесс)
```
Открыть файл positions.txt
ifstream file(positionsPath)
```
**Связи:**
- → Блок 9 (Файл открыт?)

#### Блок 9 (Ромб - Условие)
```
Файл открыт?
file.is_open()
```
**Связи:**
- **ДА** → Блок 11 (Построчно читать positions.txt)
- **НЕТ** → Блок 10 (Пропустить загрузку должностей)

#### Блок 10 (Прямоугольник - Процесс)
```
Пропустить загрузку должностей
```
**Связи:**
- → Блок 18 (Открыть файл departments.txt)

#### Блок 11 (Прямоугольник - Процесс)
```
Построчно читать positions.txt
while (getline(file, line))
```
**Связи:**
- → Блок 12 (Есть еще строка?)

#### Блок 12 (Ромб - Условие)
```
Есть еще строка?
!file.eof()
```
**Связи:**
- **ДА** → Блок 13 (Разделить строку по ';')
- **НЕТ** → Блок 17 (Закрыть positions.txt)

#### Блок 13 (Прямоугольник - Процесс)
```
Разделить строку по ';':
parts = split(line, ';')
positionId = stoi(parts[0])
positionName = parts[1]
workHours = stoi(parts[2])
```
**Связи:**
- → Блок 14 (Создать объект Position)

#### Блок 14 (Прямоугольник - Процесс)
```
Создать объект Position:
Position pos(positionId, positionName, workHours)
```
**Связи:**
- → Блок 15 (Добавить в контейнер)

#### Блок 15 (Прямоугольник - Процесс)
```
Добавить в контейнер:
positions.add(pos)
```
**Связи:**
- → Блок 16 (Обновить индекс)

#### Блок 16 (Прямоугольник - Процесс)
```
Обновить индекс:
positionIndex[positionId] = index
```
**Связи:**
- → Блок 12 (Есть еще строка? - вернуться к циклу)

#### Блок 17 (Прямоугольник - Процесс)
```
Закрыть positions.txt
file.close()
```
**Связи:**
- → Блок 18 (Открыть файл departments.txt)

#### Блок 18 (Прямоугольник - Процесс)
```
Открыть файл departments.txt
ifstream file(departmentsPath)
```
**Связи:**
- → Блок 19 (Файл открыт?)

#### Блок 19 (Ромб - Условие)
```
Файл открыт?
file.is_open()
```
**Связи:**
- **ДА** → Блок 21 (Построчно читать departments.txt)
- **НЕТ** → Блок 20 (Пропустить загрузку отделов)

#### Блок 20 (Прямоугольник - Процесс)
```
Пропустить загрузку отделов
```
**Связи:**
- → Блок 29 (Открыть файл employees.txt)

#### Блок 21 (Прямоугольник - Процесс)
```
Построчно читать departments.txt
while (getline(file, line))
```
**Связи:**
- → Блок 22 (Есть еще строка?)

#### Блок 22 (Ромб - Условие)
```
Есть еще строка?
!file.eof()
```
**Связи:**
- **ДА** → Блок 23 (Разделить строку по ';')
- **НЕТ** → Блок 28 (Закрыть departments.txt)

#### Блок 23 (Прямоугольник - Процесс)
```
Разделить строку по ';':
parts = split(line, ';')
departmentId = stoi(parts[0])
departmentName = parts[1]
managerId = stoi(parts[2])
employeeIds = parseEmployeeList(parts[3])
```
**Связи:**
- → Блок 24 (Создать объект Department)

#### Блок 24 (Прямоугольник - Процесс)
```
Создать объект Department:
Department dept(departmentId, departmentName, managerId)
```
**Связи:**
- → Блок 25 (Восстановить список сотрудников)

#### Блок 25 (Прямоугольник - Процесс)
```
Восстановить список сотрудников:
for (id in employeeIds) {
    dept.addEmployee(id)
}
```
**Связи:**
- → Блок 26 (Добавить в контейнер)

#### Блок 26 (Прямоугольник - Процесс)
```
Добавить в контейнер:
departments.add(dept)
```
**Связи:**
- → Блок 27 (Обновить индекс)

#### Блок 27 (Прямоугольник - Процесс)
```
Обновить индекс:
departmentIndex[departmentId] = index
```
**Связи:**
- → Блок 22 (Есть еще строка? - вернуться к циклу)

#### Блок 28 (Прямоугольник - Процесс)
```
Закрыть departments.txt
file.close()
```
**Связи:**
- → Блок 29 (Открыть файл employees.txt)

#### Блок 29 (Прямоугольник - Процесс)
```
Открыть файл employees.txt
ifstream file(employeesPath)
```
**Связи:**
- → Блок 30 (Файл открыт?)

#### Блок 30 (Ромб - Условие)
```
Файл открыт?
file.is_open()
```
**Связи:**
- **ДА** → Блок 32 (Построчно читать employees.txt)
- **НЕТ** → Блок 31 (Пропустить загрузку сотрудников)

#### Блок 31 (Прямоугольник - Процесс)
```
Пропустить загрузку сотрудников
```
**Связи:**
- → Блок 44 (Открыть файл hires.txt)

#### Блок 32 (Прямоугольник - Процесс)
```
Построчно читать employees.txt
while (getline(file, line))
```
**Связи:**
- → Блок 33 (Есть еще строка?)

#### Блок 33 (Ромб - Условие)
```
Есть еще строка?
!file.eof()
```
**Связи:**
- **ДА** → Блок 34 (Разделить строку по ';')
- **НЕТ** → Блок 43 (Закрыть employees.txt)

#### Блок 34 (Прямоугольник - Процесс)
```
Разделить строку по ';':
parts = split(line, ';')
employeeId = stoi(parts[0])
type = parts[1]
firstName = parts[2]
lastName = parts[3]
departmentId = stoi(parts[4])
salary = stod(parts[5])
```
**Связи:**
- → Блок 35 (Тип = WORKER?)

#### Блок 35 (Ромб - Условие)
```
Тип = WORKER?
type == "WORKER"
```
**Связи:**
- **ДА** → Блок 36 (Создать Worker)
- **НЕТ** → Блок 37 (Создать Manager)

#### Блок 36 (Прямоугольник - Процесс)
```
Создать Worker:
positionId = stoi(parts[6])
bonus = stod(parts[7])
worker = make_shared<Worker>(
    employeeId, firstName, lastName,
    departmentId, salary, positionId, bonus
)
```
**Связи:**
- → Блок 38 (Создать EmployeeRecord)

#### Блок 37 (Прямоугольник - Процесс)
```
Создать Manager:
subordinateIds = parseSubordinates(parts[6])
manager = make_shared<Manager>(
    employeeId, firstName, lastName,
    departmentId, salary, subordinateIds
)
```
**Связи:**
- → Блок 38 (Создать EmployeeRecord)

#### Блок 38 (Прямоугольник - Процесс)
```
Создать EmployeeRecord:
EmployeeRecord record
record.employee = worker/manager
record.hireDate = HireDate(0, 1, 1, 2000, 9, 0)
```
**Связи:**
- → Блок 39 (Добавить в контейнер)

#### Блок 39 (Прямоугольник - Процесс)
```
Добавить в контейнер:
employees.add(record)
```
**Связи:**
- → Блок 40 (Зарегистрировать в индексах)

#### Блок 40 (Прямоугольник - Процесс)
```
Зарегистрировать в индексах:
registerEmployee(record)
employeeIndex[employeeId] = index
```
**Связи:**
- → Блок 41 (Обновить счетчик)

#### Блок 41 (Прямоугольник - Процесс)
```
Обновить счетчик:
nextEmployeeId = max(nextEmployeeId, employeeId + 1)
```
**Связи:**
- → Блок 42 (Привязать к отделу)

#### Блок 42 (Прямоугольник - Процесс)
```
Привязать к отделу:
ensureDepartmentContainsEmployee(
    departmentId, employeeId
)
```
**Связи:**
- → Блок 33 (Есть еще строка? - вернуться к циклу)

#### Блок 43 (Прямоугольник - Процесс)
```
Закрыть employees.txt
file.close()
```
**Связи:**
- → Блок 44 (Открыть файл hires.txt)

#### Блок 44 (Прямоугольник - Процесс)
```
Открыть файл hires.txt
ifstream file(hiresPath)
```
**Связи:**
- → Блок 45 (Файл открыт?)

#### Блок 45 (Ромб - Условие)
```
Файл открыт?
file.is_open()
```
**Связи:**
- **ДА** → Блок 47 (Построчно читать hires.txt)
- **НЕТ** → Блок 46 (Пропустить загрузку дат найма)

#### Блок 46 (Прямоугольник - Процесс)
```
Пропустить загрузку дат найма
```
**Связи:**
- → Блок 55 (Готово - Конец)

#### Блок 47 (Прямоугольник - Процесс)
```
Построчно читать hires.txt
while (getline(file, line))
```
**Связи:**
- → Блок 48 (Есть еще строка?)

#### Блок 48 (Ромб - Условие)
```
Есть еще строка?
!file.eof()
```
**Связи:**
- **ДА** → Блок 49 (Разделить строку по ';')
- **НЕТ** → Блок 54 (Закрыть hires.txt)

#### Блок 49 (Прямоугольник - Процесс)
```
Разделить строку по ';':
parts = split(line, ';')
employeeId = stoi(parts[0])
day = stoi(parts[1])
month = stoi(parts[2])
year = stoi(parts[3])
hour = stoi(parts[4])
minute = stoi(parts[5])
```
**Связи:**
- → Блок 50 (Найти EmployeeRecord)

#### Блок 50 (Прямоугольник - Процесс)
```
Найти EmployeeRecord:
record = findEmployeeRecord(employeeId)
```
**Связи:**
- → Блок 51 (Запись найдена?)

#### Блок 51 (Ромб - Условие)
```
Запись найдена?
record != nullptr
```
**Связи:**
- **ДА** → Блок 52 (Обновить HireDate)
- **НЕТ** → Блок 53 (Пропустить запись)

#### Блок 52 (Прямоугольник - Процесс)
```
Обновить HireDate:
record.hireDate = HireDate(
    employeeId, day, month, year, hour, minute
)
```
**Связи:**
- → Блок 48 (Есть еще строка? - вернуться к циклу)

#### Блок 53 (Прямоугольник - Процесс)
```
Пропустить запись
```
**Связи:**
- → Блок 48 (Есть еще строка? - вернуться к циклу)

#### Блок 54 (Прямоугольник - Процесс)
```
Закрыть hires.txt
file.close()
```
**Связи:**
- → Блок 55 (Готово - Конец)

#### Блок 55 (Овал - Конец)
```
Готово
Данные загружены
```

### Схема связей между блоками:

```
1 → 2 → 3 → 4 → 5 → 6 → 7 → 8 → 9
                                    ↓
9 →|да|→ 11 → 12 →|да|→ 13 → 14 → 15 → 16 → 12
    |нет|→ 10 → 18
                                    ↓
12 →|нет|→ 17 → 18 → 19 →|да|→ 21 → 22 →|да|→ 23 → 24 → 25 → 26 → 27 → 22
                          |нет|→ 20 → 29
                                    ↓
22 →|нет|→ 28 → 29 → 30 →|да|→ 32 → 33 →|да|→ 34 → 35
                          |нет|→ 31 → 44
                                    ↓
35 →|да|→ 36 → 38
    |нет|→ 37 → 38
                                    ↓
38 → 39 → 40 → 41 → 42 → 33
                                    ↓
33 →|нет|→ 43 → 44 → 45 →|да|→ 47 → 48 →|да|→ 49 → 50 → 51
                          |нет|→ 46 → 55
                                    ↓
51 →|да|→ 52 → 48
    |нет|→ 53 → 48
                                    ↓
48 →|нет|→ 54 → 55
```

---

## АЛГОРИТМ 2: Добавление нового работника

### Блоки и их содержимое:

#### Блок 1 (Овал - Начало)
```
Старт
addWorker()
```
**Связи:**
- → Блок 2 (Ввод данных)

#### Блок 2 (Параллелограмм - Ввод)
```
Ввод данных:
firstName, lastName
departmentId, positionId
salary, bonus
hireDate (day, month, year, hour, minute)
```
**Связи:**
- → Блок 3 (Сгенерировать ID)

#### Блок 3 (Прямоугольник - Процесс)
```
Сгенерировать ID:
newId = generateEmployeeId()
```
**Связи:**
- → Блок 4 (Отдел существует?)

#### Блок 4 (Ромб - Условие)
```
Отдел существует?
findDepartment(departmentId) != nullptr
```
**Связи:**
- **ДА** → Блок 7 (Должность существует?)
- **НЕТ** → Блок 5 (Ошибка: Отдел не существует)

#### Блок 5 (Параллелограмм - Вывод)
```
Ошибка: Отдел не существует
```
**Связи:**
- → Блок 6 (Конец - Операция прекращена)

#### Блок 6 (Овал - Конец)
```
Конец
Операция прекращена
```

#### Блок 7 (Ромб - Условие)
```
Должность существует?
findPosition(positionId) != nullptr
```
**Связи:**
- **ДА** → Блок 9 (Создать объект Worker)
- **НЕТ** → Блок 8 (Ошибка: Должность не существует)

#### Блок 8 (Параллелограмм - Вывод)
```
Ошибка: Должность не существует
```
**Связи:**
- → Блок 6 (Конец - Операция прекращена)

#### Блок 9 (Прямоугольник - Процесс)
```
Создать объект Worker:
worker = make_shared<Worker>(
    newId, firstName, lastName,
    departmentId, salary, positionId, bonus
)
```
**Связи:**
- → Блок 10 (Создать EmployeeRecord)

#### Блок 10 (Прямоугольник - Процесс)
```
Создать EmployeeRecord:
EmployeeRecord record
record.employee = worker
record.hireDate = hireDate
```
**Связи:**
- → Блок 11 (Установить employeeId в HireDate)

#### Блок 11 (Прямоугольник - Процесс)
```
Установить employeeId в HireDate:
record.hireDate.setEmployeeId(newId)
```
**Связи:**
- → Блок 12 (Добавить в контейнер)

#### Блок 12 (Прямоугольник - Процесс)
```
Добавить в контейнер:
employees.add(record)
```
**Связи:**
- → Блок 13 (Зарегистрировать сотрудника)

#### Блок 13 (Прямоугольник - Процесс)
```
Зарегистрировать сотрудника:
registerEmployee(record)
```
**Связи:**
- → Блок 14 (Обновить индекс)

#### Блок 14 (Прямоугольник - Процесс)
```
Обновить индекс:
employeeIndex[newId] = index
```
**Связи:**
- → Блок 15 (Привязать к отделу)

#### Блок 15 (Прямоугольник - Процесс)
```
Привязать к отделу:
ensureDepartmentContainsEmployee(
    departmentId, newId
)
```
**Связи:**
- → Блок 16 (undoInProgress == false?)

#### Блок 16 (Ромб - Условие)
```
undoInProgress == false?
!undoInProgress
```
**Связи:**
- **ДА** → Блок 17 (Создать undo-команду)
- **НЕТ** → Блок 18 (Пропустить undo)

#### Блок 17 (Прямоугольник - Процесс)
```
Создать undo-команду:
pushUndo([this, newId]() {
    removeEmployee(newId)
}, "Отмена добавления сотрудника")
```
**Связи:**
- → Блок 19 (Работник успешно добавлен)

#### Блок 18 (Прямоугольник - Процесс)
```
Пропустить undo
```
**Связи:**
- → Блок 19 (Работник успешно добавлен)

#### Блок 19 (Параллелограмм - Вывод)
```
Работник успешно добавлен
ID: newId
```
**Связи:**
- → Блок 20 (Конец - return newId)

#### Блок 20 (Овал - Конец)
```
Конец
return newId
```

### Схема связей между блоками:

```
1 → 2 → 3 → 4
              ↓
4 →|да|→ 7 →|да|→ 9 → 10 → 11 → 12 → 13 → 14 → 15 → 16
    |нет|→ 5 → 6                                    ↓
                                                    16 →|да|→ 17 → 19 → 20
7 →|нет|→ 8 → 6                                    |нет|→ 18 → 19 → 20
```

---

## Инструкция по созданию блок-схемы

### Шаг 1: Выберите инструмент
- **Draw.io** (https://app.diagrams.net/) - бесплатный онлайн редактор
- **Lucidchart** - профессиональный инструмент
- **Visio** - Microsoft Office
- **Бумага и карандаш** - для ручного рисования

### Шаг 2: Создайте блоки
1. Для каждого блока из списка выше:
   - Выберите нужный тип (овал, прямоугольник, ромб, параллелограмм)
   - Вставьте текст из соответствующего блока
   - Пронумеруйте блок (например, "Блок 1", "Блок 2")

### Шаг 3: Соедините блоки стрелками
- Следуйте схеме связей
- Для условий (ромбы) подпишите стрелки: "да" и "нет"
- Для циклов используйте возвратные стрелки

### Шаг 4: Проверьте логику
- Убедитесь, что все блоки соединены
- Проверьте, что циклы имеют выход
- Убедитесь, что все пути ведут к конечному блоку

### Пример для Draw.io:

1. Откройте https://app.diagrams.net/
2. Создайте новую диаграмму
3. В панели слева выберите:
   - **Овал** для начала/конца
   - **Прямоугольник** для процессов
   - **Ромб** для условий
   - **Параллелограмм** для ввода/вывода
4. Перетащите нужные фигуры на холст
5. Дважды кликните на фигуру, чтобы вставить текст
6. Используйте инструмент "Соединитель" для связывания блоков
7. Для условий подпишите стрелки: правой кнопкой → "Редактировать метку" → введите "да" или "нет"

### Советы:

- **Группировка**: Сложные процессы можно разбить на подпроцессы
- **Цвета**: Используйте разные цвета для разных типов блоков
- **Размещение**: Располагайте блоки слева направо и сверху вниз
- **Компактность**: Не делайте блок-схему слишком большой, разбейте на части при необходимости

---

## Готовые тексты для копирования

### Алгоритм 1 - Все блоки подряд:

```
Блок 1 (Овал): Старт loadAll()
Блок 2 (Прямоугольник): Определить рабочий каталог dataDirectory = detectDataDirectory()
Блок 3 (Прямоугольник): Сформировать пути к файлам: employeesPath = combinePath(dataDir, "employees.txt") departmentsPath = combinePath(dataDir, "departments.txt") positionsPath = combinePath(dataDir, "positions.txt") hiresPath = combinePath(dataDir, "hires.txt")
Блок 4 (Прямоугольник): Очистить контейнеры: employees.clear() departments.clear() positions.clear()
Блок 5 (Прямоугольник): Очистить индексы: employeeIndex.clear() departmentIndex.clear() positionIndex.clear()
Блок 6 (Прямоугольник): Сбросить счетчики: nextEmployeeId = 1 nextDepartmentId = 1 nextPositionId = 1
Блок 7 (Прямоугольник): Очистить стек undo: undoStack.clear() undoInProgress = false
Блок 8 (Прямоугольник): Открыть файл positions.txt ifstream file(positionsPath)
Блок 9 (Ромб): Файл открыт? file.is_open()
Блок 10 (Прямоугольник): Пропустить загрузку должностей
Блок 11 (Прямоугольник): Построчно читать positions.txt while (getline(file, line))
Блок 12 (Ромб): Есть еще строка? !file.eof()
Блок 13 (Прямоугольник): Разделить строку по ';': parts = split(line, ';') positionId = stoi(parts[0]) positionName = parts[1] workHours = stoi(parts[2])
Блок 14 (Прямоугольник): Создать объект Position: Position pos(positionId, positionName, workHours)
Блок 15 (Прямоугольник): Добавить в контейнер: positions.add(pos)
Блок 16 (Прямоугольник): Обновить индекс: positionIndex[positionId] = index
Блок 17 (Прямоугольник): Закрыть positions.txt file.close()
Блок 18 (Прямоугольник): Открыть файл departments.txt ifstream file(departmentsPath)
Блок 19 (Ромб): Файл открыт? file.is_open()
Блок 20 (Прямоугольник): Пропустить загрузку отделов
Блок 21 (Прямоугольник): Построчно читать departments.txt while (getline(file, line))
Блок 22 (Ромб): Есть еще строка? !file.eof()
Блок 23 (Прямоугольник): Разделить строку по ';': parts = split(line, ';') departmentId = stoi(parts[0]) departmentName = parts[1] managerId = stoi(parts[2]) employeeIds = parseEmployeeList(parts[3])
Блок 24 (Прямоугольник): Создать объект Department: Department dept(departmentId, departmentName, managerId)
Блок 25 (Прямоугольник): Восстановить список сотрудников: for (id in employeeIds) { dept.addEmployee(id) }
Блок 26 (Прямоугольник): Добавить в контейнер: departments.add(dept)
Блок 27 (Прямоугольник): Обновить индекс: departmentIndex[departmentId] = index
Блок 28 (Прямоугольник): Закрыть departments.txt file.close()
Блок 29 (Прямоугольник): Открыть файл employees.txt ifstream file(employeesPath)
Блок 30 (Ромб): Файл открыт? file.is_open()
Блок 31 (Прямоугольник): Пропустить загрузку сотрудников
Блок 32 (Прямоугольник): Построчно читать employees.txt while (getline(file, line))
Блок 33 (Ромб): Есть еще строка? !file.eof()
Блок 34 (Прямоугольник): Разделить строку по ';': parts = split(line, ';') employeeId = stoi(parts[0]) type = parts[1] firstName = parts[2] lastName = parts[3] departmentId = stoi(parts[4]) salary = stod(parts[5])
Блок 35 (Ромб): Тип = WORKER? type == "WORKER"
Блок 36 (Прямоугольник): Создать Worker: positionId = stoi(parts[6]) bonus = stod(parts[7]) worker = make_shared<Worker>(employeeId, firstName, lastName, departmentId, salary, positionId, bonus)
Блок 37 (Прямоугольник): Создать Manager: subordinateIds = parseSubordinates(parts[6]) manager = make_shared<Manager>(employeeId, firstName, lastName, departmentId, salary, subordinateIds)
Блок 38 (Прямоугольник): Создать EmployeeRecord: EmployeeRecord record record.employee = worker/manager record.hireDate = HireDate(0, 1, 1, 2000, 9, 0)
Блок 39 (Прямоугольник): Добавить в контейнер: employees.add(record)
Блок 40 (Прямоугольник): Зарегистрировать в индексах: registerEmployee(record) employeeIndex[employeeId] = index
Блок 41 (Прямоугольник): Обновить счетчик: nextEmployeeId = max(nextEmployeeId, employeeId + 1)
Блок 42 (Прямоугольник): Привязать к отделу: ensureDepartmentContainsEmployee(departmentId, employeeId)
Блок 43 (Прямоугольник): Закрыть employees.txt file.close()
Блок 44 (Прямоугольник): Открыть файл hires.txt ifstream file(hiresPath)
Блок 45 (Ромб): Файл открыт? file.is_open()
Блок 46 (Прямоугольник): Пропустить загрузку дат найма
Блок 47 (Прямоугольник): Построчно читать hires.txt while (getline(file, line))
Блок 48 (Ромб): Есть еще строка? !file.eof()
Блок 49 (Прямоугольник): Разделить строку по ';': parts = split(line, ';') employeeId = stoi(parts[0]) day = stoi(parts[1]) month = stoi(parts[2]) year = stoi(parts[3]) hour = stoi(parts[4]) minute = stoi(parts[5])
Блок 50 (Прямоугольник): Найти EmployeeRecord: record = findEmployeeRecord(employeeId)
Блок 51 (Ромб): Запись найдена? record != nullptr
Блок 52 (Прямоугольник): Обновить HireDate: record.hireDate = HireDate(employeeId, day, month, year, hour, minute)
Блок 53 (Прямоугольник): Пропустить запись
Блок 54 (Прямоугольник): Закрыть hires.txt file.close()
Блок 55 (Овал): Готово Данные загружены
```

### Алгоритм 2 - Все блоки подряд:

```
Блок 1 (Овал): Старт addWorker()
Блок 2 (Параллелограмм): Ввод данных: firstName, lastName departmentId, positionId salary, bonus hireDate (day, month, year, hour, minute)
Блок 3 (Прямоугольник): Сгенерировать ID: newId = generateEmployeeId()
Блок 4 (Ромб): Отдел существует? findDepartment(departmentId) != nullptr
Блок 5 (Параллелограмм): Ошибка: Отдел не существует
Блок 6 (Овал): Конец Операция прекращена
Блок 7 (Ромб): Должность существует? findPosition(positionId) != nullptr
Блок 8 (Параллелограмм): Ошибка: Должность не существует
Блок 9 (Прямоугольник): Создать объект Worker: worker = make_shared<Worker>(newId, firstName, lastName, departmentId, salary, positionId, bonus)
Блок 10 (Прямоугольник): Создать EmployeeRecord: EmployeeRecord record record.employee = worker record.hireDate = hireDate
Блок 11 (Прямоугольник): Установить employeeId в HireDate: record.hireDate.setEmployeeId(newId)
Блок 12 (Прямоугольник): Добавить в контейнер: employees.add(record)
Блок 13 (Прямоугольник): Зарегистрировать сотрудника: registerEmployee(record)
Блок 14 (Прямоугольник): Обновить индекс: employeeIndex[newId] = index
Блок 15 (Прямоугольник): Привязать к отделу: ensureDepartmentContainsEmployee(departmentId, newId)
Блок 16 (Ромб): undoInProgress == false? !undoInProgress
Блок 17 (Прямоугольник): Создать undo-команду: pushUndo([this, newId]() { removeEmployee(newId) }, "Отмена добавления сотрудника")
Блок 18 (Прямоугольник): Пропустить undo
Блок 19 (Параллелограмм): Работник успешно добавлен ID: newId
Блок 20 (Овал): Конец return newId
```

---

## Быстрый старт в Draw.io

1. Перейдите на https://app.diagrams.net/
2. Нажмите "Create New Diagram"
3. Выберите "Blank Diagram"
4. В левой панели найдите раздел "Flowchart"
5. Используйте:
   - **Terminator** (овал) - для начала/конца
   - **Process** (прямоугольник) - для процессов
   - **Decision** (ромб) - для условий
   - **Data** (параллелограмм) - для ввода/вывода
6. Перетащите фигуры на холст
7. Дважды кликните для редактирования текста
8. Используйте стрелки из панели для соединения блоков
9. Сохраните файл (Ctrl+S или Cmd+S)

Готово! Теперь у вас есть все необходимое для создания блок-схем.

