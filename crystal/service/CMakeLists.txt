# Copyright 2017-present Yeolar

add_executable(crystal-httpserver
  crystal-httpserver.cpp
)
target_link_libraries(crystal-httpserver crystal)

install(TARGETS crystal-httpserver DESTINATION ${CMAKE_INSTALL_BINDIR})
