cmake_minimum_required(VERSION 3.10)
project(mdr)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)
find_package(PortMidi REQUIRED)

pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(PORTMIDI portmidi)

add_subdirectory(source)