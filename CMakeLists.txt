cmake_minimum_required(VERSION 3.1)

project(pdfb2d CXX)

set(CMAKE_CXX_STANDARD 11)

set(APP_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Application directory")

set(B2D_DIR "${APP_DIR}/../b2d" CACHE PATH "Location of 'b2d'")

set(B2D_BUILD_STATIC TRUE)
include("${B2D_DIR}/CMakeLists.txt")

set(APP_LIBS -Llib -lPDFNetC ${B2D_LIBS})
set(APP_CFLAGS ${B2D_CFLAGS})

add_executable(pdfb2d pdfb2d.cpp)
target_include_directories(pdfb2d BEFORE PRIVATE "include/pdfnet" ${B2D_INCLUDE_DIR})
target_compile_options(pdfb2d PRIVATE ${APP_CFLAGS})
target_link_libraries(pdfb2d ${APP_LIBS})
add_custom_command(TARGET pdfb2d POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${APP_DIR}/lib ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
