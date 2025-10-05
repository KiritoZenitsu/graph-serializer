cmake_minimum_required(VERSION 3.10)
project(GraphSerializer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки компиляции
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/serializer.cpp
    src/deserializer.cpp
)

# Заголовочные файлы
include_directories(include)

# Создание исполняемого файла
add_executable(run ${SOURCES})
