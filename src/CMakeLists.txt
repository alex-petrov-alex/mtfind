cmake_minimum_required(VERSION 3.29)

add_executable(mtfind
        ${INCLUDE_DIR}/MtFind.h
        ${INCLUDE_DIR}/ThreadPool.h
        main.cpp
        MtFind.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})