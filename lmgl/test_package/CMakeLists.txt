cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)

find_package(lmlib REQUIRED)

add_executable(test_lmlib_package test_lmlib_package.cpp)
target_link_libraries(test_lmlib_package lmlib)
