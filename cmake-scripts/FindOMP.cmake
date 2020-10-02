# Finds libomp.
#
# This module defines:
# OMP_INCLUDE_DIR
# OMP_LIBRARY
#

if(APPLE)

    if(OMP_INCLUDE_DIR)
      # Already in cache, be silent
      set(OMP_FIND_QUIETLY TRUE)
    endif()

    find_path(OMP_INCLUDE_DIR
        NAMES
          omp.h
        PATHS
          /usr/include
          /usr/local/include)
    find_library(OMP_LIBRARY NAMES omp)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        OMP DEFAULT_MSG
        OMP_LIBRARY OMP_INCLUDE_DIR)

    mark_as_advanced(OMP_INCLUDE_DIR OMP_LIBRARY)

else()

    find_package(OpenMP REQUIRED)

endif()
