cmake_minimum_required(VERSION 3.10)

project(TodoApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Specify source files
set(SOURCE_FILES
    src/main.cpp
    src/CreateNewForm.cpp
    src/DeleteForm.cpp
    src/FormDefinition.cpp # Include FormDefinition.cpp
    src/SelectAndUseForm.cpp # Include SelectAndUseForm.cpp
    src/Entry.cpp # Include Entry.cpp
    src/AddEntry.cpp # Include AddEntry.cpp
    src/EditEntry.cpp # Include EditEntry.cpp
    src/ViewEntry.cpp # Include ViewEntry.cpp
    src/DeleteEntry.cpp # Include DeleteEntry.cpp
    src/SaveAsCSV.cpp # Include SaveAsCSV.cpp
    src/SaveAsJSON.cpp # Include SaveAsJSON.cpp
    src/SaveAsSQL.cpp # Include SaveAsSQL.cpp
)

add_executable(TodoApp ${SOURCE_FILES})

# Specify include directories
target_include_directories(TodoApp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Forms
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_main test/test_main.cpp src/CreateNewForm.cpp test/test_CreateNewForm.cpp)
target_link_libraries(test_main gtest_main)

target_include_directories(test_main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

include(GoogleTest)
gtest_discover_tests(test_main)
