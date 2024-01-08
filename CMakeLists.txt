project(mdr)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(source) 