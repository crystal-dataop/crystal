# Copyright 2017-present Yeolar

add_executable(crystal-build
  crystal-build.cpp
)
target_link_libraries(crystal-build crystal)

add_executable(crystal-dump
  crystal-dump.cpp
)
target_link_libraries(crystal-dump crystal)

add_executable(crystal-query
  crystal-query.cpp
)
target_link_libraries(crystal-query crystal)

install(TARGETS crystal-build crystal-dump crystal-query DESTINATION ${CMAKE_INSTALL_BINDIR})
