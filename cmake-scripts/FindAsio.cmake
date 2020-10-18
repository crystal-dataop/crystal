# Finds asio.
#
# This module defines:
# ASIO_INCLUDE_DIR
# ASIO_LIBRARY
#

if(ASIO_INCLUDE_DIR)
  # Already in cache, be silent
  set(ASIO_FIND_QUIETLY TRUE)
endif()

find_path(ASIO_INCLUDE_DIR
    NAMES
      asio.hpp
    PATHS
      /usr/include
      /usr/local/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASIO DEFAULT_MSG ASIO_INCLUDE_DIR)

mark_as_advanced(ASIO_INCLUDE_DIR)
