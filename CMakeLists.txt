cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.40 COMPONENTS regex)

include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
