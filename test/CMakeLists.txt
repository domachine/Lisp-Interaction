
include_directories(../src)

add_executable(main_test main.cpp)

target_link_libraries(main_test lisp-interaction ${Boost_LIBRARIES})

add_test(CoreTestSuite main_test)
