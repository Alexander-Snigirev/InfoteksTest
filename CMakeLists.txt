cmake_minimum_required(VERSION 3.10)
project(MyLoggingLibrary VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(LIB_SOURCES 
    src/filesink.cpp
    src/log_manager.cpp
    src/socketsink.cpp
)

add_library(mylog ${LIB_SOURCES})
target_include_directories(mylog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mylog PRIVATE Threads::Threads)


add_executable(demo_file example_app/demo_file.cpp)
target_include_directories(mylog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(demo_file PRIVATE mylog Threads::Threads)

add_executable(tests_logger tests/test_logger.cpp)
target_include_directories(mylog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tests_logger PRIVATE mylog Threads::Threads stdc++fs)



install(TARGETS mylog
        EXPORT MyLoggingLibraryTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY include/mylog DESTINATION include)
