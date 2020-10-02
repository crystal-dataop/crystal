# Find faiss
#
#  FAISS_INCLUDE_DIR - where to find faiss/*.
#  FAISS_LIBRARY     - List of libraries when using faiss.
#  FAISS_FOUND       - True if faiss found.

if(FAISS_INCLUDE_DIR)
  # Already in cache, be silent
  set(FAISS_FIND_QUIETLY TRUE)
endif()

find_path(FAISS_INCLUDE_DIR faiss/Index.h)
find_library(FAISS_LIBRARY NAMES libfaiss.a)

# handle the QUIETLY and REQUIRED arguments and set FAISS_FOUND to TRUE 
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FAISS DEFAULT_MSG
    FAISS_LIBRARY FAISS_INCLUDE_DIR)

mark_as_advanced(FAISS_LIBRARY FAISS_INCLUDE_DIR)
