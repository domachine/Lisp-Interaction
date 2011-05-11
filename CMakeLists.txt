cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.40 COMPONENTS regex
                                   unit_test_framework
				   test_exec_monitor
				   prg_exec_monitor)

enable_testing()

include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(test)
