
set(LIBSRC
  lisp.cpp
  object.cpp
  function.cpp cxx_function.cpp
  utils.cpp
  # logging.cpp # numbers.cpp
  number.cpp)

set(EXESRC main.cpp)

add_library(lisp-interaction ${LIBSRC})

add_executable(lisp-test ${EXESRC})
target_link_libraries(lisp-test lisp-interaction ${Boost_LIBRARIES})
