# UML Диаграмма классов - Кадровый учет на малом предприятии

## Нотация UML для всех классов

### 1. Класс Person (Абстрактный базовый класс)

```
┌─────────────────────────────────────┐
│          <<abstract>>               │
│            Person                   │
├─────────────────────────────────────┤
│ # firstName : string                │
│ # lastName : string                 │
├─────────────────────────────────────┤
│ + Person()                          │
│ + Person(firstName, lastName)       │
│ + ~Person() {abstract}              │
│ + getFirstName() : string           │
│ + getLastName() : string            │
│ + getFullName() : string            │
│ + setFirstName(firstName) : void    │
│ + setLastName(lastName) : void      │
│ + displayInfo() {abstract} : void   │
└─────────────────────────────────────┘
```

**Видимость:**
- `#` = protected
- `+` = public
- `-` = private

---

### 2. Класс Employee (Наследует от Person)

```
┌─────────────────────────────────────────────────┐
│                  Employee                       │
│               (extends Person)                  │
├─────────────────────────────────────────────────┤
│ # employeeId : int                              │
│ # departmentId : int                            │
│ # salary : double                               │
├─────────────────────────────────────────────────┤
│ + Employee()                                    │
│ + Employee(employeeId, firstName, lastName,     │
│           departmentId, salary)                 │
│ + ~Employee() {virtual}                         │
│ + getEmployeeId() : int                         │
│ + getDepartmentId() : int                       │
│ + getSalary() : double                          │
│ + setEmployeeId(id) : void                      │
│ + setDepartmentId(id) : void                    │
│ + setSalary(salary) : void                      │
│ + displayInfo() {virtual} : void                │
│ + calculateTotalIncome() {virtual} : double     │
└─────────────────────────────────────────────────┘
```

**Наследование:** Person ◄── Employee

---

### 3. Класс Manager (Наследует от Employee)

```
┌─────────────────────────────────────────────────┐
│                    Manager                      │
│              (extends Employee)                 │
├─────────────────────────────────────────────────┤
│ - subordinates : vector<int>                    │
├─────────────────────────────────────────────────┤
│ + Manager()                                     │
│ + Manager(...)                                  │
│ + ~Manager()                                    │
│ + addSubordinate(id) : void                     │
│ + removeSubordinate(id) : void                  │
│ + getSubordinates() : vector<int>               │
│ + getSubordinatesCount() : int                  │
│ + hasSubordinate(id) : bool                     │
│ + displayInfo() {override} : void               │
│ + calculateTotalIncome() {override} : double    │
└─────────────────────────────────────────────────┘
```

**Наследование:** Employee ◄── Manager

---

### 4. Класс Worker (Наследует от Employee)

```
┌─────────────────────────────────────────────────┐
│                    Worker                       │
│              (extends Employee)                 │
├─────────────────────────────────────────────────┤
│ - positionId : int                              │
│ - bonus : double                                │
├─────────────────────────────────────────────────┤
│ + Worker()                                      │
│ + Worker(...)                                   │
│ + ~Worker()                                     │
│ + getPositionId() : int                         │
│ + getBonus() : double                           │
│ + setPositionId(id) : void                      │
│ + setBonus(bonus) : void                        │
│ + displayInfo() {override} : void               │
│ + calculateTotalIncome() {override} : double    │
└─────────────────────────────────────────────────┘
```

**Наследование:** Employee ◄── Worker

---

### 5. Класс Department

```
┌─────────────────────────────────────────────────┐
│                  Department                     │
├─────────────────────────────────────────────────┤
│ - departmentId : int                            │
│ - departmentName : string                       │
│ - managerId : int                               │
│ - employees : vector<int>                       │
├─────────────────────────────────────────────────┤
│ + Department()                                  │
│ + Department(id, name, managerId)               │
│ + ~Department()                                 │
│ + getDepartmentId() : int                       │
│ + getDepartmentName() : string                  │
│ + getManagerId() : int                          │
│ + getEmployees() : vector<int>                  │
│ + getEmployeesCount() : int                     │
│ + setDepartmentId(id) : void                    │
│ + setDepartmentName(name) : void                │
│ + setManagerId(id) : void                       │
│ + addEmployee(id) : void                        │
│ + removeEmployee(id) : void                     │
│ + hasEmployee(id) : bool                        │
│ + displayInfo() : void                          │
└─────────────────────────────────────────────────┘
```

---

### 6. Класс Position

```
┌─────────────────────────────────────────────────┐
│                   Position                      │
├─────────────────────────────────────────────────┤
│ - positionId : int                              │
│ - positionName : string                         │
│ - workHoursPerWeek : int                        │
├─────────────────────────────────────────────────┤
│ + Position()                                    │
│ + Position(id, name, hours)                     │
│ + ~Position()                                   │
│ + getPositionId() : int                         │
│ + getPositionName() : string                    │
│ + getWorkHoursPerWeek() : int                   │
│ + setPositionId(id) : void                      │
│ + setPositionName(name) : void                  │
│ + setWorkHoursPerWeek(hours) : void             │
│ + displayInfo() : void                          │
└─────────────────────────────────────────────────┘
```

---

### 7. Класс HireDate

```
┌─────────────────────────────────────────────────┐
│                   HireDate                      │
├─────────────────────────────────────────────────┤
│ - employeeId : int                              │
│ - day : int                                     │
│ - month : int                                   │
│ - year : int                                    │
│ - hour : int                                    │
│ - minute : int                                  │
├─────────────────────────────────────────────────┤
│ + HireDate()                                    │
│ + HireDate(employeeId, day, month, year,        │
│           hour, minute)                         │
│ + ~HireDate()                                   │
│ + getEmployeeId() : int                         │
│ + getDay() : int                                │
│ + getMonth() : int                              │
│ + getYear() : int                               │
│ + getHour() : int                               │
│ + getMinute() : int                             │
│ + setEmployeeId(id) : void                      │
│ + setDay(day) : void                            │
│ + setMonth(month) : void                        │
│ + setYear(year) : void                          │
│ + setHour(hour) : void                          │
│ + setMinute(minute) : void                      │
│ + getDateString() : string                      │
│ + getTimeString() : string                      │
│ + getFullDateTimeString() : string              │
│ + displayInfo() : void                          │
└─────────────────────────────────────────────────┘
```

---

## Диаграмма отношений

### Иерархия наследования:

```
                    Person
                     ╱  ╲
                    ╱    ╲
                extends extends
                  ╱        ╲
                 ╱          ╲
            Employee      (нет других классов)
          ╱        ╲
         ╱          ╲
    extends      extends
       ╱            ╲
      ╱              ╲
   Manager         Worker
```

### Ассоциации:

```
Employee ──┬── (1) works in (0..*) ──> Department
            │
            ├── (1) has (0..*) ─────────> HireDate
            │
            └── (для Manager)
                (1) manages (0..*) ──> Employee

Worker ──> (1) has (0..1) ──> Position
```

### Множественность:

- `1` - ровно один
- `0..1` - ноль или один (опционально)
- `0..*` - ноль или более (множество)
- `1..*` - один или более

---

## Символы UML

- `◄──` - Наследование (extends/inherits)
- `──>` - Ассоциация
- `⟷` - Двунаправленная ассоциация
- `╱` - Линия наследования
- `<<abstract>>` - Абстрактный класс
- `{virtual}` - Виртуальный метод
- `{override}` - Переопределенный метод
- `#` - Protected (защищенный)
- `+` - Public (публичный)
- `-` - Private (приватный)

