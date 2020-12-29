# Copyright (C) 2017-present, Yeolar

cmake_minimum_required(VERSION 3.6)

# Define project
project(crystal VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-scripts)

include(GNUInstallDirs)
include(Functions)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(DoubleConversion REQUIRED)
find_package(Asio REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(GFlags REQUIRED)
# Faiss
find_package(Faiss REQUIRED)
find_package(BLAS REQUIRED)
find_package(OMP REQUIRED)
# RdKafka
find_package(RdKafka REQUIRED)

# Optional packages
find_package(GTest)

# Setup environment
option(BUILD_SHARED_LIBS "Build libraries as shared ones" OFF)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    # memcheck
if(${OpenMP_FOUND})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Link libraries
link_libraries(
  ${DOUBLE_CONVERSION_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${GFLAGS_LIBRARIES}
  ${FAISS_LIBRARY}
  ${BLAS_LIBRARIES}
  ${RDKAFKA_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
)
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  link_libraries(stdc++fs)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_PREFIX_PATH}/include
  ${DOUBLE_CONVERSION_INCLUDE_DIR}
  ${GFLAGS_INCLUDE_DIR}
)
if(OMP_FOUND)
  link_libraries(${OMP_LIBRARY})
endif()

# Check and setup configurations
include(CheckCXXSourceCompiles REQUIRED)
include(CheckFunctionExists REQUIRED)

check_cxx_source_compiles("
  #include <string.h>
  #include <errno.h>
  int main() {
    char buf[1024];
    buf[0] = 0;
    int ret = strerror_r(ENOMEM, buf, sizeof(buf));
    return ret;
  }"
  CRYSTAL_HAVE_XSI_STRERROR_R)

configure_file(
  ${PROJECT_SOURCE_DIR}/crystal/crystal-config.h.in
  ${PROJECT_SOURCE_DIR}/crystal/crystal-config.h
)

# Work on all sub directories
build_directory(crystal _base)
build_directory(crystal/dataframe)
build_directory(crystal/foundation)
build_directory(crystal/foundation/http)
build_directory(crystal/graph)
build_directory(crystal/math)
build_directory(crystal/memory)
build_directory(crystal/operator)
build_directory(crystal/operator/generic)
build_directory(crystal/operator/search)
build_directory(crystal/query)
build_directory(crystal/serializer)
build_directory(crystal/serializer/record)
build_directory(crystal/storage/builder)
build_directory(crystal/storage/index)
build_directory(crystal/storage/index/bitmap)
build_directory(crystal/storage/index/vector)
build_directory(crystal/storage/kv)
build_directory(crystal/storage/table)
build_directory(crystal/strategy)
build_directory(crystal/type)

add_library(crystal
  $<TARGET_OBJECTS:crystal_dataframe>
  $<TARGET_OBJECTS:crystal_foundation>
  $<TARGET_OBJECTS:crystal_foundation_http>
  $<TARGET_OBJECTS:crystal_graph>
  $<TARGET_OBJECTS:crystal_memory>
  $<TARGET_OBJECTS:crystal_operator_generic>
  $<TARGET_OBJECTS:crystal_operator_search>
  $<TARGET_OBJECTS:crystal_query>
  $<TARGET_OBJECTS:crystal_serializer>
  $<TARGET_OBJECTS:crystal_serializer_record>
  $<TARGET_OBJECTS:crystal_storage_builder>
  $<TARGET_OBJECTS:crystal_storage_index>
  $<TARGET_OBJECTS:crystal_storage_index_bitmap>
  $<TARGET_OBJECTS:crystal_storage_index_vector>
  $<TARGET_OBJECTS:crystal_storage_kv>
  $<TARGET_OBJECTS:crystal_storage_table>
  $<TARGET_OBJECTS:crystal_strategy>
  $<TARGET_OBJECTS:crystal_type>
)

install(TARGETS crystal
  EXPORT crystal-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Binary
add_subdirectory(crystal/foundation/http/examples)
add_subdirectory(crystal/service)
add_subdirectory(crystal/tools)

# Test
if(GTEST_FOUND)
  enable_testing()
  add_subdirectory(crystal/dataframe/test)
  add_subdirectory(crystal/memory/test)
  add_subdirectory(crystal/operator/test)
  add_subdirectory(crystal/operator/generic/test)
  add_subdirectory(crystal/operator/search/test)
  add_subdirectory(crystal/serializer/test)
  add_subdirectory(crystal/serializer/record/test)
  add_subdirectory(crystal/storage/builder/test)
  add_subdirectory(crystal/storage/index/test)
  add_subdirectory(crystal/storage/index/bitmap/test)
  add_subdirectory(crystal/storage/kv/test)
  add_subdirectory(crystal/storage/table/test)
  add_subdirectory(crystal/type/test)
endif()

# CmakeConfig
include(CMakePackageConfigHelpers)

install(
  EXPORT crystal-targets
  NAMESPACE crystal::
  FILE crystal-targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/crystal
)

configure_package_config_file(
  ${CMAKE_MODULE_PATH}/crystal-config.cmake.in
  crystal-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/crystal
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/crystal-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/crystal
)

# Pack
set(CPACK_SYSTEM_NAME "amd64")
find_program(DPKG_CMD dpkg)
if(DPKG_CMD)
  execute_process(
    COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE CPACK_SYSTEM_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(InstallRequiredSystemLibraries)
if(DPKG_CMD)
  set(CPACK_GENERATOR "DEB;TXZ")
else()
  set(CPACK_GENERATOR "TXZ")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "yeolar@qq.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Crystal Data OP library.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libdouble-conversion-dev,libgflags-dev,libopenblas-dev,python-numpy")
include(CPack)

