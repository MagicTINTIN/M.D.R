cmake_minimum_required(VERSION 3.10)
project(mdr)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(source) 