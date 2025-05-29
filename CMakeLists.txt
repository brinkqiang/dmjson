cmake_minimum_required(VERSION 3.21)

PROJECT(dmjson)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

ModuleImportAll("thirdparty")

InterfaceImport("dmjson" "include" "")

if(PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "dmjson;dmtest")
endif()

AddInstall("dmjson" "${CMAKE_CURRENT_SOURCE_DIR}/include")
