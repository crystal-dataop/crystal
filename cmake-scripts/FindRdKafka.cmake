# Finds librdkafka.
#
# This module defines:
# RDKAFKA_INCLUDE_DIR
# RDKAFKA_LIBRARY
#

if(RDKAFKA_INCLUDE_DIR)
  # Already in cache, be silent
  set(RDKAFKA_FIND_QUIETLY TRUE)
endif()

find_path(RDKAFKA_INCLUDE_DIR
    NAMES
      librdkafka/rdkafkacpp.h
    PATHS
      /usr/include
      /usr/local/include)
find_library(RDKAFKA_LIBRARY NAMES rdkafka)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    RDKAFKA DEFAULT_MSG
    RDKAFKA_LIBRARY RDKAFKA_INCLUDE_DIR)

mark_as_advanced(RDKAFKA_INCLUDE_DIR RDKAFKA_LIBRARY)
