cmake_minimum_required(VERSION 3.16)
project(HRManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Включение автоматической обработки MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Исходные файлы
set(SOURCES
    src/Person.cpp
    src/Employee.cpp
    src/Worker.cpp
    src/Manager.cpp
    src/Department.cpp
    src/Position.cpp
    src/HireDate.cpp
    src/DataManager.cpp
    src/MainWindow.cpp
    src/main.cpp
)

# Заголовочные файлы
set(HEADERS
    include/Person.h
    include/Employee.h
    include/Worker.h
    include/Manager.h
    include/Department.h
    include/Position.h
    include/HireDate.h
    include/DataManager.h
    include/RecordCollection.h
    include/MainWindow.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключение Qt6
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
)

# Включение директорий
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

