# ==============================================================================
#
# Find the Armadillo optimization library
#
# ------------------------------------------------------------------------------
#
# Variables defined by this module include:
#   Armadillo_FOUND            If false, do not try to use Armadillo
#   Armadillo_INCLUDE_DIRS     Where to find the header files needed to use Armadillo
#   Armadillo_LIBRARIES        The libraries needed to use Armadillo
#   Armadillo_VERSION          The version of the Armadillo library
# Also defined, but not for general use, are:
#   Armadillo_INCLUDE_DIR      Where to find the Armadillo header files
#   Armadillo_LIBRARY          Where to find the Armadillo library
#
# ==============================================================================

INCLUDE(FindPackageHandleStandardArgs)

IF(Armadillo_FIND_QUIETLY)
  SET(_FIND_PACKAGE_ARGS ${_FIND_PACKAGE_ARGS} QUIET)
ENDIF(Armadillo_FIND_QUIETLY)
IF(Armadillo_FIND_REQUIRED)
  SET(_FIND_PACKAGE_ARGS ${_FIND_PACKAGE_ARGS} REQUIRED)
ENDIF(Armadillo_FIND_REQUIRED)

FIND_PACKAGE(LAPACK ${_FIND_PACKAGE_ARGS})

FIND_PATH(Armadillo_INCLUDE_DIR armadillo_bits/arma_version.hpp)
FIND_LIBRARY(Armadillo_LIBRARY NAMES armadillo)

IF(Armadillo_INCLUDE_DIR AND Armadillo_LIBRARY)
  ENABLE_LANGUAGE(CXX)
  GET_FILENAME_COMPONENT(libdir ${Armadillo_LIBRARY} DIRECTORY)
  LIST(APPEND flags "-DINCLUDE_DIRECTORIES=${Armadillo_INCLUDE_DIR}")
  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp "
#include <armadillo>
#include <cstdio>
int main() {
  int major = ARMA_VERSION_MAJOR;
  int minor = ARMA_VERSION_MINOR;
  int patch = ARMA_VERSION_PATCH;
  printf(\"%d.%d.%d\", major, minor, patch);
  return 0;
}")
  TRY_RUN(
      run_result compile_result
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp
      CMAKE_FLAGS ${flags}
      RUN_OUTPUT_VARIABLE Armadillo_VERSION)
  FILE(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp)
ENDIF()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Armadillo
    FOUND_VAR Armadillo_FOUND
    REQUIRED_VARS
      Armadillo_INCLUDE_DIR
      Armadillo_LIBRARY
      LAPACK_LIBRARIES
    VERSION_VAR Armadillo_VERSION)

IF(Armadillo_FOUND)
  SET(Armadillo_INCLUDE_DIRS
    ${Armadillo_INCLUDE_DIR})
  SET(Armadillo_LIBRARIES
    ${Armadillo_LIBRARY}
    ${LAPACK_LIBRARIES})
ENDIF()

MARK_AS_ADVANCED(Armadillo_INCLUDE_DIR Armadillo_LIBRARY)
