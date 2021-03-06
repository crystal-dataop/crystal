# Finds libdouble-conversion.
#
# This module defines:
# DOUBLE_CONVERSION_INCLUDE_DIR
# DOUBLE_CONVERSION_LIBRARY
#

if(DOUBLE_CONVERSION_INCLUDE_DIR)
  # Already in cache, be silent
  set(DOUBLE_CONVERSION_FIND_QUIETLY TRUE)
endif()

find_path(DOUBLE_CONVERSION_INCLUDE_DIR
    NAMES
      double-conversion/double-conversion.h
    PATHS
      /usr/include
      /usr/local/include)
find_library(DOUBLE_CONVERSION_LIBRARY NAMES double-conversion)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    DOUBLE_CONVERSION DEFAULT_MSG
    DOUBLE_CONVERSION_LIBRARY DOUBLE_CONVERSION_INCLUDE_DIR)

mark_as_advanced(DOUBLE_CONVERSION_INCLUDE_DIR DOUBLE_CONVERSION_LIBRARY)
