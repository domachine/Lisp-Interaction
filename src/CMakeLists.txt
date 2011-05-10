
set(SRC main.cpp
  lisp.cpp
  object.cpp
  function.cpp cxx_function.cpp
  utils.cpp
  logging.cpp # numbers.cpp
  number.cpp)

add_executable(lisp-test ${SRC})
target_link_libraries(lisp-test ${Boost_LIBRARIES})
