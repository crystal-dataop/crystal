# Define functions
include(GNUInstallDirs)

function(build_directory path)
  string(REPLACE "/" "_" name ${path}${ARGN})
  message(STATUS "Build target: ${name} @${path}")

  file(GLOB ${name}_SRCS ${path}/*.cpp ${path}/detail/*.cpp)
  file(GLOB ${name}_HDRS ${path}/*.h ${path}/*.hpp)
  file(GLOB ${name}_DETAIL_HDRS ${path}/detail/*.h ${path}/detail/*.hpp)
  if(${name}_SRCS)
    add_library(${name} OBJECT ${${name}_SRCS})
  endif()
  install(FILES ${${name}_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${path})
  install(FILES ${${name}_DETAIL_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${path}/detail)
endfunction()

# required GTest
function(test_sources)
  foreach(test_src ${ARGN})
    get_filename_component(test ${test_src} NAME_WE)
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} crystal)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
  endforeach()
endfunction()
