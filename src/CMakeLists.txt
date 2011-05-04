
set(SRC main.cpp
  lisp.cpp
  object.cpp
  function.cpp cxx_function.cpp
  utils.cpp
  logging.cpp numbers.cpp)

add_executable(lisp-test ${SRC})
