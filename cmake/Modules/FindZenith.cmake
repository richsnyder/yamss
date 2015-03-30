# ==============================================================================
#
# Find the Zenith interprocess communication library
#
# ------------------------------------------------------------------------------
#
# Variables defined by this module include:
#   Zenith_FOUND            If false, do not try to use Zenith
#   Zenith_INCLUDE_DIRS     Where to find the header files needed to use Zenith
#   Zenith_LIBRARIES        The libraries needed to use Zenith
#   Zenith_VERSION          The version of the Zenith library
# Also defined, but not for general use, are:
#   Zenith_INCLUDE_DIR      Where to find the Zenith header files
#   Zenith_LIBRARY          Where to find the Zenith library
#
# ------------------------------------------------------------------------------
#
# Copyright (c) 2014, Richard Snyder
#
# Use, modification, and distribution is subject to the BSD License.
# See the accompanying file LICENSE.md for the full license.
# Terms can also be found at http://creativecommons.org/licenses/BSD/.
#
# ==============================================================================

IF(Zenith_FIND_QUIETLY)
  SET(_FIND_PACKAGE_ARG ${_FIND_PACKAGE_ARGS} QUIET)
ENDIF()
IF(Zenith_FIND_REQUIRED)
  SET(_FIND_PACKAGE_ARG ${_FIND_PACKAGE_ARGS} REQUIRED)
ENDIF()

FIND_PACKAGE(CZMQ ${_FIND_PACKAGE_ARGS})
FIND_PACKAGE(Thrift ${_FIND_PACKAGE_ARGS})

FIND_PATH(Zenith_INCLUDE_DIR zenith/version.hpp)
FIND_LIBRARY(Zenith_LIBRARY NAMES zenith)

IF(Zenith_INCLUDE_DIR AND Zenith_LIBRARY)
  ENABLE_LANGUAGE(CXX)
  GET_FILENAME_COMPONENT(libdir ${Zenith_LIBRARY} DIRECTORY)
  LIST(APPEND flags "-DINCLUDE_DIRECTORIES=${Zenith_INCLUDE_DIR}")
  LIST(APPEND flags "-DLINK_DIRECTORIES=${libdir}")
  LIST(APPEND flags "-DLINK_LIBRARIES=zenith")
  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp "
#include <iostream>
#include <zenith/about.hpp>
int main() {
  std::cout << zenith::about::version_string();
  return 0;
}")
  TRY_RUN(
      run_result compile_result
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp
      CMAKE_FLAGS ${flags}
      RUN_OUTPUT_VARIABLE Zenith_VERSION)
  FILE(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/conftest.cpp)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Zenith
    FOUND_VAR Zenith_FOUND
    REQUIRED_VARS
        Zenith_LIBRARY
        Zenith_INCLUDE_DIR
        CZMQ_LIBRARIES
        CZMQ_INCLUDE_DIRS
        Thrift_LIBRARIES
        Thrift_INCLUDE_DIRS
    VERSION_VAR Zenith_VERSION)

IF(Zenith_FOUND)
  SET(Zenith_INCLUDE_DIRS
      ${Zenith_INCLUDE_DIR}
      ${CZMQ_INCLUDE_DIRS}
      ${Thrift_INCLUDE_DIRS})
  SET(Zenith_LIBRARIES
      ${Zenith_LIBRARY}
      ${CZMQ_LIBRARIES}
      ${Thrift_LIBRARIES})
ENDIF(Zenith_FOUND)

MARK_AS_ADVANCED(Zenith_INCLUDE_DIR Zenith_LIBRARY)
