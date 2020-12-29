# Copyright 2017-present Yeolar

add_executable(crystal-searcher
  crystal-searcher.cpp
)
target_link_libraries(crystal-searcher crystal)

install(TARGETS crystal-searcher DESTINATION ${CMAKE_INSTALL_BINDIR})
